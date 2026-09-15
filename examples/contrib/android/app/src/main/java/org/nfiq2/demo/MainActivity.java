package org.nfiq2.demo;

import android.app.Activity;
import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.ColorSpace;
import android.graphics.ImageDecoder;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.MediaStore;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import gov.nist.nfiq2.Nfiq2;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public final class MainActivity extends Activity {
    private static final int PICK_IMAGE = 10;
    private static final String PREFERENCES = "fingerprint_quality_preferences";
    private static final String PREF_SOURCE_IS_500_PPI = "source_is_500_ppi";
    private static final String MODEL_ASSET = "nist_plain_tir-ink.yaml";
    private static final String MODEL_HASH = "b4a1e7586b3be906f9770e4b77768038";
    private final ExecutorService worker = Executors.newSingleThreadExecutor();
    private Button choose, analyze;
    private CheckBox confirm;
    private TextView status, score, dimensions, placeholder;
    private ImageView preview;
    private ProgressBar progress;
    private boolean busy;
    private byte[] grayscale;
    private int width, height;
    private Uri selected;
    private SharedPreferences preferences;

    @SuppressWarnings("deprecation")
    @Override public void onCreate(Bundle state) {
        super.onCreate(state);
        setContentView(R.layout.activity_main);
        View root = findViewById(R.id.root);
        root.setOnApplyWindowInsetsListener((view, insets) -> {
            view.setPadding(insets.getSystemWindowInsetLeft(), insets.getSystemWindowInsetTop(),
                insets.getSystemWindowInsetRight(), insets.getSystemWindowInsetBottom());
            return insets;
        });
        if (Build.VERSION.SDK_INT >= 30) getWindow().setDecorFitsSystemWindows(false);
        root.requestApplyInsets();
        choose = findViewById(R.id.choose);
        analyze = findViewById(R.id.analyze);
        confirm = findViewById(R.id.confirm);
        status = findViewById(R.id.status);
        score = findViewById(R.id.score);
        dimensions = findViewById(R.id.dimensions);
        placeholder = findViewById(R.id.placeholder);
        preview = findViewById(R.id.preview);
        progress = findViewById(R.id.progress);
        preferences = getSharedPreferences(PREFERENCES, MODE_PRIVATE);
        confirm.setChecked(preferences.getBoolean(PREF_SOURCE_IS_500_PPI, false));
        choose.setOnClickListener(view -> pickImage());
        analyze.setOnClickListener(view -> analyze());
        confirm.setOnCheckedChangeListener((view, checked) -> {
            preferences.edit().putBoolean(PREF_SOURCE_IS_500_PPI, checked).apply();
            if (!busy && grayscale != null) {
                status.setText(checked ? R.string.loaded : R.string.loaded_needs_ppi);
            }
            updateButtons();
        });
        if (state != null && state.getString("image") != null) {
            loadImage(Uri.parse(state.getString("image")));
        }
    }

    @SuppressWarnings("deprecation")
    private void pickImage() {
        Intent intent;
        if (Build.VERSION.SDK_INT >= 33) {
            intent = new Intent(MediaStore.ACTION_PICK_IMAGES).setType("image/*");
        } else {
            intent = documentIntent();
        }
        try {
            startActivityForResult(intent, PICK_IMAGE);
        } catch (ActivityNotFoundException error) {
            try { startActivityForResult(documentIntent(), PICK_IMAGE); }
            catch (ActivityNotFoundException ignored) { status.setText(R.string.picker_error); }
        }
    }

    private Intent documentIntent() {
        return new Intent(Intent.ACTION_OPEN_DOCUMENT).setType("image/*")
            .addCategory(Intent.CATEGORY_OPENABLE)
            .addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION);
    }

    @SuppressWarnings("deprecation")
    @Override protected void onActivityResult(int request, int result, Intent data) {
        super.onActivityResult(request, result, data);
        if (request != PICK_IMAGE || result != RESULT_OK || data == null || data.getData() == null) return;
        Uri uri = data.getData();
        try { getContentResolver().takePersistableUriPermission(uri, Intent.FLAG_GRANT_READ_URI_PERMISSION); }
        catch (SecurityException ignored) { /* Some photo providers only grant temporary access. */ }
        loadImage(uri);
    }

    private void loadImage(Uri uri) {
        selected = uri;
        grayscale = null;
        preview.setImageDrawable(null);
        placeholder.setVisibility(View.VISIBLE);
        dimensions.setText("");
        score.setText(R.string.score_empty);
        setBusy(true, getString(R.string.loading));
        worker.execute(() -> {
            Bitmap bitmap = null;
            try {
                bitmap = ImageDecoder.decodeBitmap(ImageDecoder.createSource(getContentResolver(), uri), (decoder, info, source) -> {
                    int w = info.getSize().getWidth(), h = info.getSize().getHeight();
                    if (w < 32 || h < 32 || w > 4096 || h > 4096 || (long) w * h > 4000000)
                        throw new IllegalArgumentException(getString(R.string.size_error));
                    decoder.setAllocator(ImageDecoder.ALLOCATOR_SOFTWARE);
                    decoder.setTargetColorSpace(ColorSpace.get(ColorSpace.Named.SRGB));
                });
                int w = bitmap.getWidth(), h = bitmap.getHeight();
                if (w < 32 || h < 32 || w > 4096 || h > 4096 || (long) w * h > 4000000)
                    throw new IllegalArgumentException(getString(R.string.size_error));
                int[] pixels = new int[w * h];
                bitmap.getPixels(pixels, 0, w, 0, 0, w, h);
                byte[] gray = GrayPixels.fromArgb(pixels);
                // Only the preview is reduced. The full-resolution grayscale goes to JNI.
                float scale = Math.min(1f, 1000f / Math.max(w, h));
                Bitmap thumbnail = Bitmap.createScaledBitmap(bitmap, Math.max(1, Math.round(w * scale)),
                    Math.max(1, Math.round(h * scale)), true);
                if (thumbnail == bitmap) bitmap = null;
                runOnUiThread(() -> {
                    if (isDestroyed()) { thumbnail.recycle(); return; }
                    grayscale = gray;
                    width = w;
                    height = h;
                    preview.setImageBitmap(thumbnail);
                    placeholder.setVisibility(View.GONE);
                    dimensions.setText(getString(R.string.dimensions, w, h));
                    setBusy(false, getString(confirm.isChecked() ?
                        R.string.loaded : R.string.loaded_needs_ppi));
                });
            } catch (OutOfMemoryError error) {
                finishError(getString(R.string.memory_error));
            } catch (Exception error) {
                finishError(error instanceof IllegalArgumentException ? error.getMessage() : getString(R.string.image_error));
            } finally {
                if (bitmap != null) bitmap.recycle();
            }
        });
    }

    private void analyze() {
        if (busy || grayscale == null || !confirm.isChecked()) return;
        final byte[] image = grayscale;
        final int w = width, h = height;
        score.setText(R.string.score_empty);
        setBusy(true, getString(R.string.analyzing));
        worker.execute(() -> {
            long start = System.nanoTime();
            try {
                Nfiq2.initialize(getAssets(), MODEL_ASSET, MODEL_HASH);
                int result = Nfiq2.score(image, w, h, 500);
                if (result < 0 || result > 100) throw new IllegalStateException("Invalid quality score");
                double seconds = (System.nanoTime() - start) / 1_000_000_000.0;
                runOnUiThread(() -> {
                    if (isDestroyed()) return;
                    score.setText(getString(R.string.score_value, result));
                    setBusy(false, getString(R.string.complete, seconds));
                });
            } catch (LinkageError error) {
                finishError(getString(R.string.native_error));
            } catch (OutOfMemoryError error) {
                finishError(getString(R.string.memory_error));
            } catch (Exception error) {
                finishError(getString(R.string.analysis_error, error.getMessage()));
            }
        });
    }

    private void finishError(String message) {
        runOnUiThread(() -> { if (!isDestroyed()) setBusy(false, message); });
    }
    private void setBusy(boolean value, String message) {
        busy = value;
        progress.setVisibility(value ? View.VISIBLE : View.INVISIBLE);
        status.setText(message);
        updateButtons();
    }
    private void updateButtons() {
        choose.setEnabled(!busy);
        confirm.setEnabled(!busy);
        analyze.setEnabled(!busy && grayscale != null && confirm.isChecked());
    }
    @Override protected void onSaveInstanceState(Bundle state) {
        super.onSaveInstanceState(state);
        if (selected != null) state.putString("image", selected.toString());
    }
    @Override protected void onDestroy() {
        worker.shutdownNow();
        super.onDestroy();
    }
}
