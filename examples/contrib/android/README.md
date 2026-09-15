# Android fingerprint quality example

This example uses the system image picker and computes an
NFIQ 2 quality score through the reusable `NFIQ2Android` JNI library. The Java
application is under `examples/contrib/android/app`; the Android library module
is under `NFIQ2/NFIQ2Android`.

The app remembers the user's 500 PPI source setting across image selections and
restarts. It does not infer PPI from image dimensions or metadata. Images stay
on the device, and the manifest requests neither network nor storage
permissions.

## Build

Open `examples/contrib/android` in Android Studio or run the Gradle wrapper from
this directory. Required tools:

- JDK 17 or 21, Android SDK platform 36, and Build Tools 35.0.0.
- Android NDK 29.0.14206865 and SDK CMake 3.31.6.
- The repository's pinned submodules, initialized with
  `git submodule update --init --recursive`.

Create an untracked `local.properties` file:

```properties
sdk.dir=C\:/Users/YOU/AppData/Local/Android/Sdk
# Optional when NDK r29 is installed outside the SDK:
nfiq2.ndkPath=C\:/path/to/android-ndk-r29
```

Build the default external-model configuration:

```powershell
.\gradlew.bat :app:assembleDebug
```

The build compiles NFIQ2 and its pinned dependencies for arm64-v8a/API 21 with
`EMBED_RANDOM_FOREST_PARAMETERS=OFF`. It packages the repository's
`nist_plain_tir-ink.yaml` into the application assets without duplicating the
22 MB model in source control. The build normalizes the asset to LF line endings
and verifies its expected MD5 hash, so the packaged bytes are identical on
Windows, Linux, and macOS. Native output is stored in
`build-android-example/arm64-v8a-external`.

The embedded configuration remains available:

```powershell
.\gradlew.bat '-Pnfiq2.embedModel=true' :app:assembleDebug
```

Its native output is stored separately under
`build-android-example/arm64-v8a-embedded`.

The debug APK is
`app/build/outputs/apk/debug/app-debug.apk`. It supports ARM64 devices running
Android 9/API 28 or later.

## Native integration

The application depends on the `:nfiq2-android` Android library module. That
module builds `libnfiq2_jni.so`, packages it in an AAR, and exposes
`gov.nist.nfiq2.Nfiq2`:

```java
import android.content.Context;
import gov.nist.nfiq2.Nfiq2;

public final class FingerprintQuality {
    private FingerprintQuality() { }

    public static int score(Context context, byte[] grayscale,
            int width, int height) {
        Nfiq2.initialize(
            context.getAssets(),
            "nist_plain_tir-ink.yaml",
            "b4a1e7586b3be906f9770e4b77768038");
        return Nfiq2.score(grayscale, width, height, 500);
    }
}
```

Save this class as `FingerprintQuality.java` in the consuming Android module.
Call `score` on a worker thread with one grayscale byte per pixel.

For an external-model build, `initialize` loads and verifies the named Android
asset through `AAssetManager`. For an embedded build, the same call initializes
the compiled-in model and ignores the asset arguments. Initialization is
idempotent within the process, and scoring is serialized because the shared
model is not treated as thread-safe.

The JNI layer validates buffer length, dimensions, and 500 PPI before creating
`FingerprintImageData`. NFIQ2 exceptions are translated to Java exceptions.
The Java API can also be called directly from Kotlin.

## Image requirements

- Use one fingerprint scan captured at a known 500 PPI, preferably a lossless
  PNG.
- Camera photos, screenshots, resized scans, and ordinary photos are outside
  the intended NFIQ 2 input domain.
- Full-resolution pixels are converted to BT.601 grayscale. Only the preview may
  be reduced.
- Each dimension must be 32–4096 pixels, with at most four million pixels.

## Tests

```powershell
.\gradlew.bat :app:testDebugUnitTest :app:lintDebug
.\gradlew.bat :app:assembleDebugAndroidTest
# Requires an attached ARM64 device:
.\gradlew.bat :app:connectedDebugAndroidTest
# Run the same checks with the embedded model:
.\gradlew.bat '-Pnfiq2.embedModel=true' :app:connectedDebugAndroidTest
```

JVM tests cover grayscale conversion. The instrumentation test initializes the
selected model configuration, scores the repository's synthetic
`SFinGe_Test01.pgm` fixture twice with expected score 54, checks native input
validation, and verifies that the 500 PPI source setting survives Activity
recreation.
External-model runs also check that an invalid model hash reports an exception
and that initialization can then succeed with the correct hash.
