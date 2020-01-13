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
Technical Report [ISO/IEC 29794-4 Biometric sample quality -- Part 4:Finger image data:2010](http://www.iso.org/iso/catalogue_detail.htm?csnumber=50911) into an international standard.  Specifically, NFIQ quality features are being formally standardized as part of [ISO/IEC 29794-4 Biometric sample quality -- Part 4: Finger image data](http://www.iso.org/iso/catalogue_detail.htm?csnumber=62791) and NFIQ source code serves as the reference implementation of the standard.

Operationally, NFIQ has increased the reliability, accuracy, and interoperability  of fingerprint recognition 
systems by identifying the samples that are likely to cause recognition failure.

If you would like more information please read the [NFIQ 2.0 Report](https://www.nist.gov/document/nfiq2reportpdf).

How to Build
------------
If all requirements have been met, building is as simple as:
```bash
make
sudo make install
```

Library Path for `libbiomdi` will need to be set to run the binary.

Requirements
------------
 * A supported operating system:
    * RHEL/CentOS >= 6.x
    * Ubuntu
    * macOS >= 10.11

 * System packages

System Packages
---------------
Some modules require system packages that may not be installed by default on
all operating systems. Package names are listed below for RHEL/CentOS, Ubuntu and macOS
(via [MacPorts](https://www.macports.org)). Other operating systems may use
similarly-named packages.

| Name      | RHEL/CentOS         | MacPorts                | Ubuntu             |
|:---------:|:-------------------:|:-----------------------:|:------------------:|
| gcc       | `gcc`               | n/a (requires  [Command Line Tools](https://developer.apple.com/download/more/))| `gcc-6`        |
| g++       | `gcc-c++`           | n/a (requires  [Command Line Tools](https://developer.apple.com/download/more/))| `g++-6`        |
| CMAKE     | `cmake`             | `cmake`                 | `cmake`            |
| OpenCV    | `opencv-devel`      | Build from source included | `libopencv-dev` |

*** A minimum version of OpenCV 2.4.2 is required and OpenCV 3.0 => is not supported at this time. ***

*** MacOS users running 10.12 must use OpenCV 2.4.13.2 ***

####OpenCV build steps if building locally

```bash
 mkdir libOpenCV && cd libOpenCV && cmake -D CMAKE_MAKE_PROGRAM=make ../OpenCV && make opencv_core opencv_ts opencv_imgproc opencv_highgui opencv_flann opencv_features2d opencv_calib3d opencv_ml opencv_video opencv_objdetect opencv_contrib opencv_nonfree opencv_gpu opencv_photo opencv_stitching opencv_videostab
 sudo make install
```

If you do not have root access or do not wish to install OpenCV you will need to set the Library Path for OpenCV


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

