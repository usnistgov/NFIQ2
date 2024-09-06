# NFIQ 2 <img src="cmake/nist_itl_two_color.svg" align="right" alt="NIST Information Technology Laboratory" style="width:250px;" />

[![Download Latest Version](https://img.shields.io/badge/download-v2.3-informational)](https://github.com/usnistgov/NFIQ2/releases)
[![Build Libraries and CLI + Run CTS](https://github.com/usnistgov/NFIQ2/actions/workflows/build-member.yml/badge.svg)](https://github.com/usnistgov/NFIQ2/actions/workflows/build-member.yml)
[![Frequently Asked Questions](https://img.shields.io/badge/wiki-frequently%20asked%20questions-informational)](https://github.com/usnistgov/NFIQ2/wiki/Frequently-Asked-Questions)

About
-----
[National Institute of Standards and Technology (NIST)](https://www.nist.gov)
Fingerprint Image Quality (NFIQ) is software that links image
quality of optical and ink plain impression 500 pixel per inch fingerprints to operational
recognition performance. This allows quality values to be tightly defined and
then numerically calibrated, which in turn allows for the standardization needed
to support a worldwide deployment of fingerprint sensors with universally
interpretable image qualities. NFIQ 2 quality features are formally standardized
as part of [ISO/IEC 29794-4](http://www.iso.org/iso/catalogue_detail.htm?csnumber=62791).
This repository serves as a formally recognized reference implementation of the
2024 international standard.

Download
--------
Pre-built versions of the NFIQ 2 library and standalone executable for many
platforms are available to download on the
[GitHub Releases](https://github.com/usnistgov/NFIQ2/releases) page.

History
-------
In 2004, [NIST](https://www.nist.gov) developed the first open source and publicly available fingerprint quality assessment tool, [NFIQ](https://www.nist.gov/services-resources/software/nist-biometric-image-software-nbis#NFIQ).
NFIQ 2 is a revision of that tool.
Advances in fingerprint technology since 2004 necessitated an update to NFIQ.
As such, development of NFIQ 2 was initiated in 2011 as collaboration between
NIST and Germany's [Federal Office for Information Security (BSI)](https://www.bsi.bund.de/)
and [Federal Criminal Police Office (BKA)](https://www.bka.de), as well as research and development entities [MITRE](https://www.mitre.org),
[Fraunhofer IGD](https://www.igd.fraunhofer.de/), [Hochschule Darmstadt (h_da)](https://h-da.de), and [Secunet](https://www.secunet.com).
Subsequent development efforts and improvements have been supported by experts
from [ISO/IEC JTC 1/Subcommittee 37](https://www.iso.org/committee/313770.html)
and the [community](https://github.com/usnistgov/NFIQ2/graphs/contributors).

NFIQ 2 provides a higher resolution quality score in the range of [0-100], adhering
to the international biometric sample quality standard [ISO/IEC 29794-1:2024](https://www.iso.org/standard/79519.html) (as opposed to the original NFIQ's 5-1),
lower computation complexity, and support for quality assessment on mobile platforms.

NFIQ 2 is formally recognized as a reference implementation of the normative
metrics presented in
[ISO/IEC 29794-4:2017](https://www.iso.org/standard/62791.html) and was updated
in sync with the second edition revision effort,
[ISO/IEC 29794-4:2024](https://www.iso.org/standard/83827.html).

Operationally, NFIQ has increased the reliability, accuracy, and interoperability of fingerprint recognition
systems by identifying the samples that are likely to cause recognition failure.

If you would like more information, please read the [NFIQ 2 Report](https://doi.org/10.6028/NIST.IR.8382)
and [ISO/IEC 29794-4:2024](https://www.iso.org/standard/83827.html).

--------------------------------------------------------------------------------

Dependencies
------------

Building the NFIQ 2 library requires the following dependencies, included in
this repository as git submodules:

 * [digestpp](https://github.com/kerukuro/digestpp) ([public domain license](https://github.com/kerukuro/digestpp/blob/master/LICENSE))
 * [FingerJetFX OSE](https://github.com/FingerJetFXOSE/FingerJetFXOSE) ([LGPLv3 license](https://github.com/FingerJetFXOSE/FingerJetFXOSE/blob/master/COPYRIGHT.txt))
 * [OpenCV](https://github.com/opencv/opencv) ([Apache 2 License](https://github.com/opencv/opencv/blob/master/LICENSE))

If building the standalone command-line executable, additional dependencies are
required, included in this repository as git submodules:

 * [Biometric Evaluation Framework](https://github.com/usnistgov/libbiomeval) ([public domain license](https://github.com/usnistgov/libbiomeval/blob/master/LICENSE.md))
   * Requires other non-bundled dependencies, please see the [README](https://github.com/usnistgov/libbiomeval/blob/master/README.md).
 * [NIST Fingerprint Image Resampler](https://github.com/usnistgov/nfir) (public domain license)
   * Requires [OpenCV](https://github.com/opencv/opencv), which is required by NFIQ 2 library.

Quick Build: Library
--------------------

> [!IMPORTANT]
> Unless you are *actively developing* code for NFIQ 2, we **highly** suggest
> you download from [Releases](https://github.com/usnistgov/NFIQ2/releases)
> instead of attempting to compile.

> [!NOTE]
> You must *recursively* clone the repository to retrieve git submodules
> (i.e., do **not** use the GitHub ZIP file download).

```bash
git clone --recursive https://github.com/usnistgov/NFIQ2.git
cd NFIQ2
mkdir build
cd build
cmake .. -DBUILD_NFIQ2_CLI=OFF
cmake --build .
```

Quick Build: Library + Command-line Interface
---------------------------------------------

> [!IMPORTANT]
> Unless you are *actively developing* code for NFIQ 2, we **highly** suggest
> you download from [Releases](https://github.com/usnistgov/NFIQ2/releases)
> instead of attempting to compile.

> [!NOTE]
> You must *recursively* clone the repository to retrieve git submodules
> (i.e., do **not** use the GitHub ZIP file download).

```bash
git clone --recursive https://github.com/usnistgov/NFIQ2.git
cd NFIQ2
mkdir build
cd build
cmake ..
cmake --build .
```

### Build Notes:
 * Standard CMake arguments are interpreted.
   * On Windows, change architectures with `-A x64` or `-A Win32`
   * Change generators with `-G`
   * Change build types with `-DCMAKE_CONFIGURATION_TYPES` or
     `-DCMAKE_BUILD_TYPE`

 * Dependencies for `libbiomeval` must be satisfied.
    * On Windows with Visual Studio, this is done with
      [vcpkg](https://github.com/microsoft/vcpkg), which will require passing
      the vcpkg `CMAKE_TOOLCHAIN_FILE` and `VCPKG_TARGET_TRIPLET` options
      to CMake.

For example, a 64-bit Release-only build with the default Visual Studio
generator might look like:

```
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -DCMAKE_CONFIGURATION_TYPES=Release -A x64
cmake --build . --config Release
```

Builds for other OS can typically find dependencies on the system without
intervention.

OpenCV Version
--------------
Originally, all major versions of OpenCV were supported by NFIQ 2. Due to the
limited testing resources as well as slight differences in results between
versions, NIST has chosen to rely on the latest release of OpenCV 4 as of this
writing. **Using a different version of OpenCV may result in unstable NFIQ 2
scores and is not supported.** Future updates to OpenCV versions should run the
conformance test and larger sequestered tests without differences.

Known Limitations
-----------------

 * **macOS**:
   * Xcode 10 and later does not support 32-bit applications. In order to build
     NFIQ 2 for 32-bit macOS, use Xcode 9.4.x.

Build Options
-------------
The CMake builds supports the following options:

 * `BUILD_NFIQ2_CLI` (default: `ON`)
   * Whether or not to build the standalone command-line executable.
 * `EMBED_RANDOM_FOREST_PARAMETERS` (default: `OFF`)
   * Whether or not to embed random forest parameters into the library.
 * `EMBEDDED_RANDOM_FOREST_PARAMETER_FCT` (default: `0`)
   * Friction ridge capture technology code for embedded random forest
     parameters. Only valid if `EMBED_RANDOM_FOREST_PARAMETERS` is `ON`.

Communication
-------------
If you found a bug and can provide steps to reliably reproduce it, or if you
have a feature request, please
[open an issue](https://github.com/usnistgov/NFIQ2/issues). Other
questions may be addressed to the
[NIST project maintainers](mailto:nfiq2@nist.gov).

License
-------
NFIQ is released in the public domain. See the
[LICENSE](https://github.com/usnistgov/NFIQ2/blob/master/LICENSE.md)
for details.

