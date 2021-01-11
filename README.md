# NFIQ 2 <img src="cmake/nistident_flright_vec.svg" align="right" alt="NIST identifier" />

[![Linux Build Status (TravisCI)](https://travis-ci.org/usnistgov/NFIQ2.svg?branch=iso_wg3)](https://travis-ci.org/usnistgov/NFIQ2)
[![Windows Build Status (AppVeyor)](https://ci.appveyor.com/api/projects/status/0ilbxxilcyed409s/branch/iso_wg3?svg=true)](https://ci.appveyor.com/project/usnistgov/nfiq2/branch/iso_wg3)
[![Frequently Asked Questions](https://img.shields.io/badge/wiki-frequently%20asked%20questions-informational)](https://github.com/usnistgov/NFIQ2/wiki/Frequently-Asked-Questions)

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
------------
The following dependencies are included in this repository as git submodules:

 * [BiomDI](https://github.com/usnistgov/biomdi) ([public domain license](https://github.com/usnistgov/biomdi/blob/master/LICENSE.md))
 * [Biometric Evaluation Framework](https://github.com/usnistgov/libbiomeval) ([public domain license](https://github.com/usnistgov/libbiomeval/blob/master/LICENSE.md))
   * Only required for standalone executable.
   * Requires other non-bundled dependencies, please see the [README](https://github.com/usnistgov/libbiomeval/blob/master/README.md).
 * [digestpp](https://github.com/kerukuro/digestpp) ([public domain license](https://github.com/kerukuro/digestpp/blob/master/LICENSE))
 * [FingerJetFX OSE](https://github.com/FingerJetFXOSE/FingerJetFXOSE) ([LGPLv3 license](https://github.com/FingerJetFXOSE/FingerJetFXOSE/blob/master/COPYRIGHT.txt))
 * [OpenCV](https://github.com/opencv/opencv) ([Apache 2 License](https://github.com/opencv/opencv/blob/master/LICENSE))

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
Originally, all major versions of OpenCV were supported by NFIQ 2. Due to the
limited testing resources as well as slight differences in results between
versions, NIST has chosen to rely on the latest release of OpenCV 4 as of this
writing. **Using a different version of OpenCV may result in unstable NFIQ 2
scores and is not supported.** Future updates to OpenCV versions should run the
compliance test and larger sequestered tests without differences.

Known Limitations
-----------------

 * **macOS**:
   * Xcode 10 and later does not support 32-bit applications. In order to build
     NFIQ 2 for 32-bit macOS, use Xcode 9.4.x.
   * There has been no effort to test this code on Apple's M1 ARM architecture.

 * **All Platforms**:
   * The current NFIQ 2 executable binary is only supported on 64-bit machines.

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

