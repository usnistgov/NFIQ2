# NFIQ2 Android JNI library

This Android library module packages the NFIQ2 C++ API behind a small Java/JNI
interface. It is consumed by `examples/contrib/android` and produces an AAR containing
the Java class and ARM64 native library.

`gov.nist.nfiq2.Nfiq2.initialize` creates one process-wide algorithm instance.
External builds load a model from Android assets and verify its MD5 hash.
Embedded builds use the default NFIQ2 constructor. Call `initialize` before
`score`.

`gov.nist.nfiq2.Nfiq2.score` accepts one byte per grayscale pixel, width,
height, and PPI. The current binding accepts 500 PPI images from 32 to 4096
pixels per dimension and limits the buffer to four million pixels.

The module builds NFIQ2 from the repository superbuild. It is not published to a
Maven repository. See `examples/contrib/android/README.md` for setup and build
commands.
