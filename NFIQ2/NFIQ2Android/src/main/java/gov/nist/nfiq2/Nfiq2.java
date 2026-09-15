package gov.nist.nfiq2;

import android.content.res.AssetManager;

public final class Nfiq2 {
    static {
        System.loadLibrary("nfiq2_jni");
    }

    private Nfiq2() {
    }

    /** Initializes the process-wide NFIQ 2 model. Repeated calls are ignored. */
    public static native void initialize(
        AssetManager assets, String modelAsset, String modelHash);

    /** Returns the NFIQ 2 score for an 8-bit, 500 PPI grayscale image. */
    public static native int score(byte[] grayscale, int width, int height, int ppi);
}
