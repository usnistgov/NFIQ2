package org.nfiq2.demo;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.core.app.ActivityScenario;
import androidx.test.platform.app.InstrumentationRegistry;
import android.content.Context;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.SystemClock;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.TextView;
import gov.nist.nfiq2.Nfiq2;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import static org.junit.Assert.*;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.atomic.AtomicBoolean;

/** Run on an ARM64 device using connectedDebugAndroidTest. */
@RunWith(AndroidJUnit4.class)
public class Nfiq2Test {
    private static final String MODEL_ASSET = "nist_plain_tir-ink.yaml";
    private static final String MODEL_HASH = "b4a1e7586b3be906f9770e4b77768038";

    @BeforeClass public static void initializeNfiq2() {
        Context context = InstrumentationRegistry.getInstrumentation().getTargetContext();
        if (BuildConfig.NFIQ2_EMBED_MODEL) {
            Nfiq2.initialize(null, null, null);
            return;
        }
        try {
            Nfiq2.initialize(context.getAssets(), MODEL_ASSET,
                "00000000000000000000000000000000");
            fail("Expected model hash validation");
        } catch (IllegalStateException expected) { }
        Nfiq2.initialize(context.getAssets(), MODEL_ASSET, MODEL_HASH);
    }

    @Test public void ppiConfirmationSurvivesActivityRecreation() {
        Context context = InstrumentationRegistry.getInstrumentation().getTargetContext();
        context.getSharedPreferences("fingerprint_quality_preferences", Context.MODE_PRIVATE)
            .edit().clear().commit();
        try (ActivityScenario<MainActivity> scenario = ActivityScenario.launch(MainActivity.class)) {
            scenario.onActivity(activity -> {
                CheckBox confirmation = activity.findViewById(R.id.confirm);
                assertFalse(confirmation.isChecked());
                confirmation.setChecked(true);
            });
            scenario.recreate();
            scenario.onActivity(activity ->
                assertTrue(((CheckBox) activity.findViewById(R.id.confirm)).isChecked()));
        } finally {
            context.getSharedPreferences("fingerprint_quality_preferences", Context.MODE_PRIVATE)
                .edit().clear().commit();
        }
    }

    @Test public void referenceFingerprintAndRepeatedCall() throws Exception {
        byte[] pixels = referencePixels();
        assertEquals(54, Nfiq2.score(pixels, 416, 560, 500));
        assertEquals(54, Nfiq2.score(pixels, 416, 560, 500));
    }

    private static byte[] referencePixels() throws Exception {
        byte[] data;
        try (InputStream input = InstrumentationRegistry.getInstrumentation().getContext().getAssets().open("SFinGe_Test01.pgm")) {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            byte[] buffer = new byte[8192];
            int read;
            while ((read = input.read(buffer)) != -1) output.write(buffer, 0, read);
            data = output.toByteArray();
        }
        // This repository fixture has a fixed P5 header, no comments or metadata.
        byte[] header = "P5\n416 560\n255\n".getBytes(StandardCharsets.US_ASCII);
        assertEquals(header.length + 416 * 560, data.length);
        for (int i = 0; i < header.length; i++) assertEquals(header[i], data[i]);
        byte[] pixels = new byte[416 * 560];
        System.arraycopy(data, header.length, pixels, 0, pixels.length);
        return pixels;
    }
    @Test public void invalidBufferRejectedBeforeNativeRead() {
        try {
            Nfiq2.score(new byte[1], 416, 560, 500);
            fail("Expected input validation");
        } catch (IllegalArgumentException expected) { }
    }
    @Test public void wrongPpiRejected() {
        try {
            Nfiq2.score(new byte[32 * 32], 32, 32, 1000);
            fail("Expected PPI validation");
        } catch (IllegalArgumentException expected) { }
    }

    @Test public void invalidDimensionsAndNullBufferRejected() {
        int[][] sizes = {{0, 32}, {-1, 32}, {31, 32}, {4097, 32},
            {2001, 2000}, {Integer.MAX_VALUE, Integer.MAX_VALUE}};
        for (int[] size : sizes) {
            try {
                Nfiq2.score(new byte[0], size[0], size[1], 500);
                fail("Expected dimension validation");
            } catch (IllegalArgumentException expected) { }
        }
        try {
            Nfiq2.score(null, 32, 32, 500);
            fail("Expected null-buffer validation");
        } catch (IllegalArgumentException expected) { }
    }

    @Test public void selectedPngSurvivesRecreationAndScoresAtOriginalSize() throws Exception {
        Context context = InstrumentationRegistry.getInstrumentation().getTargetContext();
        File image = File.createTempFile("fingerprint-", ".png", context.getCacheDir());
        byte[] gray = referencePixels();
        int[] colors = new int[gray.length];
        for (int i = 0; i < gray.length; i++) {
            int value = gray[i] & 255;
            colors[i] = 0xff000000 | (value << 16) | (value << 8) | value;
        }
        Bitmap bitmap = Bitmap.createBitmap(colors, 416, 560, Bitmap.Config.ARGB_8888);
        try (FileOutputStream output = new FileOutputStream(image)) {
            assertTrue(bitmap.compress(Bitmap.CompressFormat.PNG, 100, output));
        } finally {
            bitmap.recycle();
        }
        try (ActivityScenario<MainActivity> scenario = ActivityScenario.launch(MainActivity.class)) {
            scenario.onActivity(activity -> {
                ((CheckBox) activity.findViewById(R.id.confirm)).setChecked(true);
                activity.onActivityResult(10, Activity.RESULT_OK,
                    new Intent().setData(Uri.fromFile(image)));
            });
            waitForReady(scenario);
            scenario.recreate();
            waitForReady(scenario);
            scenario.onActivity(activity -> {
                assertEquals(activity.getString(R.string.dimensions, 416, 560),
                    ((TextView) activity.findViewById(R.id.dimensions)).getText().toString());
                ((Button) activity.findViewById(R.id.analyze)).performClick();
            });
            waitForReady(scenario);
            scenario.onActivity(activity -> assertEquals("54",
                ((TextView) activity.findViewById(R.id.score)).getText().toString()));
        } finally {
            assertTrue(image.delete());
            context.getSharedPreferences("fingerprint_quality_preferences", Context.MODE_PRIVATE)
                .edit().clear().commit();
        }
    }

    private static void waitForReady(ActivityScenario<MainActivity> scenario) {
        AtomicBoolean ready = new AtomicBoolean();
        long deadline = SystemClock.uptimeMillis() + 30000;
        do {
            scenario.onActivity(activity ->
                ready.set(activity.findViewById(R.id.analyze).isEnabled()));
            if (ready.get()) return;
            SystemClock.sleep(50);
        } while (SystemClock.uptimeMillis() < deadline);
        fail("Image preparation or analysis did not finish within 30 seconds");
    }
}
