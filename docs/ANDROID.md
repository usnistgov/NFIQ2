# Android library build

Initialize the repository's pinned dependencies with
`git submodule update --init --recursive`. Install an Android NDK, CMake and
Ninja. The following PowerShell commands build the official libraries for
64-bit Android devices. Adjust the NDK path for your machine.

```powershell
$ndk = 'C:/path/to/android-ndk-r29'
$env:CMAKE_BUILD_PARALLEL_LEVEL = '4'
cmake -S . -B build-android-arm64 -G Ninja `
  "-DCMAKE_TOOLCHAIN_FILE=$ndk/build/cmake/android.toolchain.cmake" `
  -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-21 `
  -DCMAKE_BUILD_TYPE=Release -DBUILD_NFIQ2_CLI=OFF `
  -DEMBED_RANDOM_FOREST_PARAMETERS=OFF
cmake --build build-android-arm64 --parallel 4
```

The Android toolchain must set `ANDROID_ABI`, `ANDROID_PLATFORM`,
`CMAKE_TOOLCHAIN_FILE`, and `CMAKE_ANDROID_NDK`. Configuration stops early with
a clear error if any required value is missing. The selected ABI, API level,
C++ runtime, toolchain, and NDK are forwarded to every external project.
Android's installed OpenCV package directory is also passed to the NFIQ2
library projects so that it remains discoverable when they use standard CMake
package lookup.

The command above keeps the random forest parameters outside the library. An
Android application can package `NFIQ2/nist_plain_tir-ink.yaml` as an asset and
initialize `NFIQ2::Algorithm` with an Android `AAssetManager`, the asset name,
and the hash from `NFIQ2/nist_plain_tir-ink.txt`.

To compile the model into the library instead, set
`EMBED_RANDOM_FOREST_PARAMETERS=ON`. Applications compiling against an embedded
build must define `NFIQ2_EMBED_RANDOM_FOREST_PARAMETERS` and the matching
`NFIQ2_EMBEDDED_RANDOM_FOREST_PARAMETERS_FCT` value.

Outputs are in `build-android-arm64/install_staging/nfiq2`:

- `lib/libnfiq2.a`: official C++ API static library.
- `include/`: NFIQ2 public headers.
- `lib/libFRFXLL_static.a`: FingerJetFX static dependency.
- `sdk/native/`: OpenCV headers, static libraries and third-party dependencies.
- The build currently also emits the legacy C wrapper. Its static/shared form
  is not part of the Android CI contract because the project marks that API for
  removal. It is not a JNI wrapper or an AAR.

Link all required static dependencies when using `libnfiq2.a`; the archive does
not contain OpenCV or FingerJetFX. The build uses the NDK's default
`c++_static`. Choose a consistent C++ runtime for the application and all native
dependencies.

The existing GitHub Actions build matrices include an Android arm64-v8a/API 21
target on Ubuntu 24.04. It uses the runner's installed CMake, Ninja, and latest
NDK, builds with the CLI and embedded model disabled, and verifies the library
outputs and target architecture. Android does not run the desktop CLI
conformance test.
