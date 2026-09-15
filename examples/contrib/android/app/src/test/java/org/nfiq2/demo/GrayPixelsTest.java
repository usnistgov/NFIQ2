package org.nfiq2.demo;

import org.junit.Test;
import static org.junit.Assert.*;

public class GrayPixelsTest {
    @Test public void preservesEveryGrayByte() {
        int[] pixels = new int[256];
        for (int i = 0; i < 256; i++) pixels[i] = 0xff000000 | (i << 16) | (i << 8) | i;
        byte[] gray = GrayPixels.fromArgb(pixels);
        for (int i = 0; i < 256; i++) assertEquals(i, gray[i] & 255);
    }
    @Test public void compositesTransparencyOnWhite() {
        assertArrayEquals(new byte[] {(byte) 255, (byte) 255, (byte) 127, 0},
            GrayPixels.fromArgb(new int[] {0x00000000, 0x00ff0000, 0x80000000, 0xff000000}));
    }
    @Test public void usesLuminanceNotAverage() {
        assertArrayEquals(new byte[] {76, (byte) 150, 29},
            GrayPixels.fromArgb(new int[] {0xffff0000, 0xff00ff00, 0xff0000ff}));
    }
    @Test public void preservesLengthAndOrder() {
        assertEquals(0, GrayPixels.fromArgb(new int[0]).length);
        assertArrayEquals(new byte[] {0, (byte) 255, 0},
            GrayPixels.fromArgb(new int[] {0xff000000, 0xffffffff, 0xff000000}));
    }
}
