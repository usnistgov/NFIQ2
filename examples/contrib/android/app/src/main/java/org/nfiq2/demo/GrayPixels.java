package org.nfiq2.demo;

/** Integer BT.601 luminance; transparent pixels are composited onto white. */
final class GrayPixels {
    private GrayPixels() { }
    static byte[] fromArgb(int[] pixels) {
        byte[] gray = new byte[pixels.length];
        for (int i = 0; i < pixels.length; i++) {
            int argb = pixels[i];
            int alpha = argb >>> 24;
            int r = composite((argb >>> 16) & 255, alpha);
            int g = composite((argb >>> 8) & 255, alpha);
            int b = composite(argb & 255, alpha);
            gray[i] = (byte) ((4899 * r + 9617 * g + 1868 * b + 8192) >> 14);
        }
        return gray;
    }
    private static int composite(int value, int alpha) {
        return (value * alpha + 255 * (255 - alpha) + 127) / 255;
    }
}
