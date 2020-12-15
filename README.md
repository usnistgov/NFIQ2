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

Dependencies
-----------

This version of NFIQ2 uses The Biometric Evaluation Framework (libbiomeval). 

A link to The Framework can be found [here](https://github.com/usnistgov/libbiomeval). See the Libbiomeval README for more information.

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
cmake .. -DCMAKE_TOOLCHAIN_FILE=%vcpkg_root%\\scripts\\buildsystems\\vcpkg.cmake -DCMAKE_CONFIGURATION_TYPES=Release -DCMAKE_BUILD_TYPE=Release -A %platform%
cmake --build .
```

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

