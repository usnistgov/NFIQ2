NFIQ 2
======

[![Linux Build Status (TravisCI)](https://travis-ci.org/usnistgov/NFIQ2.svg?branch=iso_wg3)](https://travis-ci.org/usnistgov/NFIQ2)
[![Windows Build Status (AppVeyor)](https://ci.appveyor.com/api/projects/status/0ilbxxilcyed409s/branch/iso_wg3?svg=true)](https://ci.appveyor.com/project/usnistgov/nfiq2/branch/iso_wg3)
[![Frequently Asked Questions](https://img.shields.io/badge/wiki-frequently%20asked%20questions-informational)](https://github.com/usnistgov/NFIQ2/wiki/Frequently-Asked-Questions)

--------------------------------------------------------------------------------

 **IMPORTANT**: This branch is being actively developed by the ISO/IEC JTC 1
SC 37 WG 3 Special Group towards ISO/IEC 29794-4 Recommendation 3.15. As a
result, the code in this branch is **unstable**, and at times, may not build or
produce expected results.

--------------------------------------------------------------------------------

Overview
--------
In 2004, the [National Institute of Standards and Technology (NIST)](https://www.nist.gov) developed the first open source and publicly available fingerprint quality assessment tool, [NIST Finger Image Quality (NFIQ)](https://www.nist.gov/services-resources/software/nist-biometric-image-software-nbis#NFIQ).
NFIQ 2 is a revision of that tool.
Advances in fingerprint technology since 2004 necessitated an update to NFIQ.
As such, development of NFIQ 2 was initiated in 2011 as collaboration between
NIST and Germany's [Federal Office for Information Security (BSI)](https://www.bsi.bund.de/)
and [Federal Criminal Police Office (BKA)](https://www.bka.de), as well as research and development entities [MITRE](https://www.mitre.org),
[Fraunhofer IGD](https://www.igd.fraunhofer.de/), [Hochschule Darmstadt (h_da)](https://h-da.de), and [Secunet](https://www.secunet.com).

NFIQ 2 provides a higher resolution quality score in the range of [0-100], adhering
to the international biometric sample quality standard [ISO/IEC 29794-1:2016](https://www.iso.org/standard/62782.html) (as opposed to the original NFIQ's 1-5),
lower computation complexity, and support for quality assessment on mobile platforms.

The major innovation of NFIQ was linking image quality to operational recognition performance.
This allowed quality values to be tightly defined and then numerically calibrated.
This, in turn, allowed for the standardization needed to support a worldwide deployment of fingerprint sensors with
universally interpretable image qualities.

NFIQ 2 is the basis for a revision of the
Technical Report [ISO/IEC TR 29794-4:2010](http://www.iso.org/iso/catalogue_detail.htm?csnumber=50911)
into an international standard.  Specifically, NFIQ quality features are being formally standardized as part of
[ISO/IEC 29794-4](http://www.iso.org/iso/catalogue_detail.htm?csnumber=62791) and
NFIQ source code serves as the reference implementation of the standard.

Operationally, NFIQ has increased the reliability, accuracy, and interoperability  of fingerprint recognition
systems by identifying the samples that are likely to cause recognition failure.

If you would like more information please read the [NFIQ 2 Report](https://www.nist.gov/document/nfiq2reportpdf).

Quick Build
-----------
**Linux and macOS:**
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

**Windows:**
```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE={VCPKG_DIR/scripts/buildsystems/vcpkg.cmake} -DCMAKE_CONFIGURATION_TYPES=Release -DCMAKE_BUILD_TYPE=Release -A {Win32 || x64}
cmake --build .
```
Notes:

VCPKG_DIR is the path to where VCPKG is installed.

-A takes either Win32 or x64 for 32-bit and 64-bit builds, respectfully. 

Cross Compilation
-----------------
The build process is based on [CMake](https://cmake.org/), which is available for all major platforms. For convenience, cross-compilation scripts are available in the root directory.

The build process is a two step process.

 1. Use CMake to generate the compiler makefiles (makefiles, project files etc)

    This step can be started by executing the correct script for your compiler:

     * Visual Studio
       * [`runCMake.ps1`](runCMake.ps1)
         * Only the `Release` configuration will be configured.
     * Everything else
       * [`runCMake.sh`](runCMake.sh)
         * Requires argument of `x32` or `x64`, depending on desired bitness.

    During the CMake step all the required dependencies that are not `git` submodules
    will be downloaded. Note that all dependencies will
    [eventually be submodules](https://github.com/usnistgov/NFIQ2/issues/32).

    - OpenCV
        * downloaded from [https://github.com/opencv/opencv/archive/`OPENCV_VERSION`.zip](https://github.com/opencv/opencv/archive/OPENCV_VERSION.zip)
        * the OpenCV version is controlled by the variable `${OPENCV_VERSION}`,
          which is defined in the top level CMake file (default is "4.4.0")
        * If you need to download the source ahead of time, place the zip file in the root directory and change the URL

 2. CMake will generate makefiles or project files in the directories listed below.
    After CMake generated the according make solution or files, NFIQ2 can be build from the dedicated `build` folder. All generated artifacts will be
    placed in a dedicated `dist` folder.

The build process has been tested for the following platforms:

| OS        | Bitness  | Compiler   | CMake Script command                      | Build Files (`./build/`) | Artifacts (`./dist/`)     |
|:---------:|:--------:|:----------:|:-----------------------------------------:|:------------------------:|:-------------------------:|
| Windows   | 32       | MSVC 2017  | `runCMake.ps1`                            | `Windows32/NFIQ2.sln`    | use release configuration |
| Windows   | 64       | MSVC 2017  | `runCMake.ps1`                            | `Windows64/NFIQ2.sln`    | use release configuration |
| Windows   | 32       | MSYS2/GCC  | `runCMake.sh x32`                         | `MinGw-x86_64/x32/`      | `MinGw-x86_64/x32/GCC/`   |
| Windows   | 64       | MSYS2/GCC  | `runCMake.sh x64`                         | `MinGw-x86_64/x64/`      | `MinGw-x86_64/x64/GCC/`   |
| Linux     | 32       | GCC        | `runCMake.sh x32`                         | `Linux-x86_64/x32/`      | `Linux-x86_64/x32/GCC/`   |
| Linux     | 64       | GCC        | `runCMake.sh x64`                         | `Linux-x86_64/x64/`      | `Linux-x86_64/x64/GCC/`   |
| macOS     | 32       | AppleClang | `runCMake.sh x32`                         | `Mac-x86/x32/`           | `Mac-x86/x32/Clang/`      |
| macOS     | 64       | AppleClang | `runCMake.sh x64`                         | `Mac-x86_64/x64/`        | `Mac-x86_64/x64/Clang/`   |
| Android P | 32       | Clang/GCC  | `runCMake.sh android-arm32 <path to NDK>` | `android-arm32/`         | `android-arm32/Clang/`    |
| Android P | 64       | Clang/GCC  | `runCMake.sh android-arm64 <path to NDK>` | `android-arm64/`         | `android-arm64/Clang/`    |

Due to the specifics of an Android build, a modified (GCC support) NDK needs to be used, which is not part of this NFIQ2 distribution.

OpenCV version
--------------

The tested and approved OpenCV version is 2.4.13.6. The source were also experimentally compiled with the OpenCV version 3.4.8 and 4.4.0.
The OpenCV version can be changed within by setting the CMake variable `OPENCV_VERSION`:
```bash
cmake -DOPENCV_VERSION="4.4.0" ..
```

Known Limitations
-----------------

 * **macOS**:
   * Xcode 10 and later does not support 32-bit applications. In order to build
   NFIQ2 for 32-bit macOS, use Xcode 9.4.x.

 * **All Platforms**:
   * The current NFIQ2 executable binary is only supported on 64-bit machines.

Communication
-------------
If you found a bug and can provide steps to reliably reproduce it, or if you
have a feature request, please
[open an issue](https://github.com/usnistgov/NFIQ2/issues). Other
questions may be addressed to the
[NIST project maintainers](mailto:nfiq2.development@nist.gov).

License
-------
NFIQ is released in the public domain. See the
[LICENSE](https://github.com/usnistgov/NFIQ2/blob/master/LICENSE.md)
for details.

