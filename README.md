NFIQ 2.0
========
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

How to Build
------------
The build process is based on CMAKE (https://cmake.org/) which is available for all major platforms. For convenients, a shell script and a powershell 
script (for Windows MSVC) are available at the root folder.

The build process is a two step process.

1.) Using CMAKE (https://cmake.org/) to generate the compiler makefiles (makefiles, project files etc)

    This step can be started by executing the runCMake.ps1 or runCMake.sh script
    For Visual Studio use runCMake.ps1
      When using CMAKE for Visual Studio, the solutions is configured for a release build only, even the project contains the Debug option too. 
    For all other compilers (GCC, Clang) use runCmake.sh 
      Requires as argument eihter x32 or x64 as argument for the script (see table below). Only the release configuration will be configured.

    During the CMAKE step all the required dependencies will be downbloaded
      - biomdi              downloaded from http://nigos.nist.gov:8080/nist/biomdi/biomdi_current.zip
      - fingerjetfx (OSE)   downloaded from https://github.com/FingerJetFXOSE/FingerJetFXOSE/archive/master.zip
      - opencv              downloaded from https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.zip
                            the open CV version is controlled by the variable ${OPENCV_VERSION}, which is defined in 
                            the top level cmake file (default is "2.4.13.6")
    After the download the downloaded files can be found in the according subfolder download. 
    Remark: CMAKE will skip the download if the according subfolder already exists!

2.) CMAKE will generate the make files or project files according your compiler settings. The table below note the according directories.
    After CMake generated the according make solution or files, NFIQ2 can be build from the dedicated "build" folder. All generated artefacts will be 
    placed in a dedicated "dist" folder.

The build process has been tested for the following platforms

| OS        | Bitness  | Compiler   | CMake Script command | remark                                                                                               |
|:---------:|:--------:|:----------------------------------------------------:|:---------------------------------------------------------------------------------:|
| Windows   | 32       | MSVC 2017  | runCmake.ps1                            | builds Visual Studio Solution, use release configuration                          |
| Windows   | 64       | MSVC 2017  | runCmake.ps1                            | builds Visual Studio Solution, use release configuration                          |
| Windows   | 32       | MSYS2/GCC  | runCmake.sh x32                         | make files @ ./build/MinGw-x86_64/x32/, artefacts @ ./dist/MinGw-x86_64/x32/GCC/  |
| Windows   | 64       | MSYS2/GCC  | runCmake.sh x64                         | make files @ ./build/MinGw-x86_64/x64/, artefacts @ ./dist/MinGw-x86_64/x64/GCC/  |
| Linux     | 32       | GCC        | runCmake.sh x32                         | make files @ ./build/Linux-x86_64/x32/, artefacts @ ./dist/Linux-x86_64/x32/GCC/  |
| Linux     | 64       | GCC        | runCmake.sh x64                         | make files @ ./build/Linux-x86_64/x64/, artefacts @ ./dist/Linux-x86_64/x64/GCC/  |
| Mac OSX   | 32       | AppleClang | runCmake.sh x32                         | make files @ ./build/.../x32/, artefacts @ ./dist/.../x32/GCC/  |
| Mac OSX   | 64       | AppleClang | runCmake.sh x64                         | make files @ ./build/.../x64/, artefacts @ ./dist/.../x64/GCC/  |
| Android P | 32       | Clang/GCC  | runCmake.sh android-arm32 <path to NDK> | make files @ ./build/android-arm32/, artefacts @ ./dist/android-arm32/Clang/      |
| Android P | 64       | Clang/GCC  | runCmake.sh android-arm64 <path to NDK> | make files @ ./build/android-arm64/, artefacts @ ./dist/android-arm64/Clang/      |

Due to the specifics of an Android build, a modified (GCC support) NDK needs to be used, which is not part of this NFIQ2 distribution.

OpenCV version
--------------

The tested and approved OpenCV version is 2.4.13.6. The source were also experimentally compiled with the OpenCV version 3.4.5.
the open CV version can be changed within the top level Cmake file (see: set( OPENCV_VERSION "2.4.13.6")).

Communication
-------------
If you found a bug and can provide steps to reliably reproduce it, or if you
have a feature request, please
[open an issue](https://github.com/usnistgov/NFIQ/issues). Other
questions may be addressed to the
[project maintainers](mailto:nfiq2.development@nist.gov).

License
-------
NFIQ is released in the public domain. See the
[LICENSE](https://github.com/usnistgov/NFIQ/blob/master/LICENSE.md)
for details.

