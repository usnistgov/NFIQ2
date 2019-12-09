NFIQ 2.0
========

[![Linux Build Status (TravisCI)](https://travis-ci.org/usnistgov/NFIQ2.svg?branch=iso_wg3)](https://travis-ci.org/usnistgov/NFIQ2)
[![Windows Build Status (AppVeyor)](https://ci.appveyor.com/api/projects/status/hb1kwdohdekvv8kf/branch/iso_wg3?svg=true)](https://ci.appveyor.com/project/gfiumara/nfiq2/branch/iso_wg3)


--------------------------------------------------------------------------------

 **IMPORTANT**: This branch is being actively developed by the ISO/IEC JTC 1 
SC 37 WG 3 Special Group towards ISO/IEC 29794-4 Recommendation 3.15. As a 
result, the code in this branch is **unstable**, and at times, may not build or 
produce expected results.

--------------------------------------------------------------------------------	

Overview
--------
NFIQ 2.0 is a revision of the open source NIST Finger Image Quality (NFIQ).
In 2004, NIST developed the first publicly available fingerprint quality assessment tool NFIQ.
Advances in fingerprint technology since 2004, necessitated an update to NFIQ. 
As such, development of NFIQ 2.0 was initiated in 2011 as collaboration between 
National Institute  of Standards and Technology (NIST) and  Federal Office for Information Security (BSI) 
and Federal Criminal Police Office (BKA) in Germany as well as research and development entities, MITRE, 
Fraunhofer IGD,  Hochschule Darmstadt (HAD)  and Secunet.  

NFIQ 2.0 provides a higher resolution quality score, in range of 0-100 according 
to the international biometric sample quality standard ISO/IEC 29794-1:2016 (as opposed to 1-5), 
lower computation complexity, as well as support for quality assessment in mobile platform.

The major innovation of NFIQ was linking image quality to operational recognition performance. 
This had several immediate benefits; it allowed quality values to be tightly defined and then numerically calibrated.
This, in turn, allowed for the standardization needed to support a worldwide deployment of fingerprint sensors with
universally interpretable image qualities. NFIQ 2.0 is the basis for a revision of the 
Technical Report [ISO/IEC 29794-4 Biometric sample quality -- Part 4:Finger image data:2010](http://www.iso.org/iso/catalogue_detail.htm?csnumber=50911) 
into an international standard.  Specifically, NFIQ quality features are being formally standardized as part of 
[ISO/IEC 29794-4 Biometric sample quality -- Part 4: Finger image data](http://www.iso.org/iso/catalogue_detail.htm?csnumber=62791) and 
NFIQ source code serves as the reference implementation of the standard.

Operationally, NFIQ has increased the reliability, accuracy, and interoperability  of fingerprint recognition 
systems by identifying the samples that are likely to cause recognition failure.

If you would like more information please read the [NFIQ 2.0 Report](https://www.nist.gov/sites/default/files/documents/2016/12/07/nfiq2_report.pdf).

Quick Build
-----------
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Cross Compilation
-----------------
The build process is based on [CMake](https://cmake.org/) which is available for all major platforms. For convenients, a shell script and a powershell 
script (for Windows MSVC) are available at the root folder.

The build process is a two step process.

1.) Use CMake to generate the compiler makefiles (makefiles, project files etc)

This step can be started by executing the correct script for your compiler:

 * Visual Studio
   * `runCMake.ps1`
 * Everything else
   * `runCMake.sh`
Requires as argument eihter x32 or x64 as argument for the script (see table below). Only the release configuration will be configured.

During the CMake step all the required dependencies that are not git submodules 
will be downloaded. Note that all dependencies will 
[eventually be submodules](https://github.com/usnistgov/NFIQ2/issues/32).

- OpenCV
    * downloaded from https://github.com/opencv/opencv/archive/`${OPENCV_VERSION}`.zip
    * the OpenCV version is controlled by the variable `${OPENCV_VERSION}`,
      which is defined in the top level CMake file (default is "2.4.13.6")
      

2.) CMAKE will generate the make files or project files according your compiler settings. The table below note the according directories.
    After CMake generated the according make solution or files, NFIQ2 can be build from the dedicated "build" folder. All generated artifacts will be 
    placed in a dedicated "dist" folder.

The build process has been tested for the following platforms

| OS        | Bitness  | Compiler   | CMake Script command                    | remark                                                                            |
|:---------:|:--------:|:----------:|:---------------------------------------:|:---------------------------------------------------------------------------------:|
| Windows   | 32       | MSVC 2017  | runCmake.ps1                            | builds Visual Studio Solution, use release configuration                          |
| Windows   | 64       | MSVC 2017  | runCmake.ps1                            | builds Visual Studio Solution, use release configuration                          |
| Windows   | 32       | MSYS2/GCC  | runCmake.sh x32                         | make files @ ./build/MinGw-x86_64/x32/, artifacts @ ./dist/MinGw-x86_64/x32/GCC/  |
| Windows   | 64       | MSYS2/GCC  | runCmake.sh x64                         | make files @ ./build/MinGw-x86_64/x64/, artifacts @ ./dist/MinGw-x86_64/x64/GCC/  |
| Linux     | 32       | GCC        | runCmake.sh x32                         | make files @ ./build/Linux-x86_64/x32/, artifacts @ ./dist/Linux-x86_64/x32/GCC/  |
| Linux     | 64       | GCC        | runCmake.sh x64                         | make files @ ./build/Linux-x86_64/x64/, artifacts @ ./dist/Linux-x86_64/x64/GCC/  |
| Mac OSX   | 32       | AppleClang | runCmake.sh x32                         | make files @ ./build/.../x32/, artifacts @ ./dist/.../x32/GCC/  |
| Mac OSX   | 64       | AppleClang | runCmake.sh x64                         | make files @ ./build/.../x64/, artifacts @ ./dist/.../x64/GCC/  |
| Android P | 32       | Clang/GCC  | runCmake.sh android-arm32 <path to NDK> | make files @ ./build/android-arm32/, artifacts @ ./dist/android-arm32/Clang/      |
| Android P | 64       | Clang/GCC  | runCmake.sh android-arm64 <path to NDK> | make files @ ./build/android-arm64/, artifacts @ ./dist/android-arm64/Clang/      |

Due to the specifics of an Android build, a modified (GCC support) NDK needs to be used, which is not part of this NFIQ2 distribution.

OpenCV version
--------------

The tested and approved OpenCV version is 2.4.13.6. The source were also experimentally compiled with the OpenCV version 3.4.8 and 4.1.2.
The OpenCV version can be changed within by setting the CMake variable `OPENCV_VERSION`:
```bash
cmake -DOPENCV_VERSION="4.1.2" ..
```

Known Limitations
-----------------

 * **macOS**:
   * Xcode 10 and later does not support 32 bit applications. In order to build
   NFIQ2 for 32 bit macOS, use Xcode 9.4.1.

Communication
-------------
If you found a bug and can provide steps to reliably reproduce it, or if you
have a feature request, please
[open an issue](https://github.com/usnistgov/NFIQ2/issues). Other
questions may be addressed to the
[project maintainers](mailto:nfiq2.development@nist.gov).

License
-------
NFIQ is released in the public domain. See the
[LICENSE](https://github.com/usnistgov/NFIQ2/blob/master/LICENSE.md)
for details.

