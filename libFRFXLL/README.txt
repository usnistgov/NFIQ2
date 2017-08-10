FingerJetFX OSE -- Fingerprint Feature Extractor, Open Source Edition

Copyright (c) 2011 by DigitalPersona, Inc. All rights reserved.

DigitalPersona, FingerJet, and FingerJetFX are registered trademarks 
or trademarks of DigitalPersona, Inc. in the United States and other
countries.

FingerJetFX OSE is open source software that you may modify and/or
redistribute under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation, either version 3 of the 
License, or (at your option) any later version, provided that the 
conditions specified in the COPYRIGHT.txt file provided with this 
software are met.

For more information, please visit digitalpersona.com/fingerjetfx.

--------------------------------------------------------------------------------------------


FingerJetFX OSE
--------------------------------------------------------------------------------------------
Fingerprint feature extractor library, initial contribution by DigitalPersona, Inc. 
digitalpersona.com/fingerjetfx 
--------------------------------------------------------------------------------------------

Overview
--------------------------------------------------------------------------------------------
To use this software, application takes the fingerprint image, and makes one call into 
the FingerJetFX library to obtain fingerprint minutiae data, which can be used with 
industry-standard fingerprint recognition engines.


Platforms
--------------------------------------------------------------------------------------------
The FingerJetFX OSE fingerprint feature extractor is platform-independent and can be built 
for, with appropriate changes to the make files, and run in environments with or without 
operating systems, including 
 - Linux
 - Android
 - Windows
 - Windows CE 
 - various RTOSs

FingerJetFX OSE can run on 
 - general purpose 32-bit and 64-bit CPUs 
 - 32-but microcontrollers (ARM, MIPS amd others)

In addition, FingerJetFX OSE 
 - does not reqire DSPs or co-processors
 - does not use floating point

The library has a very small memory foorprint: 
 - ~128K of codespace (can run from ROM)
 - ~128K of RAM  (actual RAM requirement is the size of the image buffer + 32K)


Performance and Recognition Accuracy
--------------------------------------------------------------------------------------------
The initial contribution by DigitalPersona, Inc. (www.digitalpersona.com) 
has met the required PIV performance thresholds for fingerprint minutiae generation 
in MINEX test (SDK 3F).

See http://www.nist.gov/itl/iad/ig/ominex_test-results.cfm for details.

On an ARM Cortex-M3 150MHz processor, feature extraction takes 0.5 to 1.25 sec. 
On an Intel i7 PC, feature extraction takes 10 to 25 mS on a single CPU core.

The code is provided with extensive set of unit tests to ensure accurate 
and reliable operation in the target environments.


The Sample Application
--------------------------------------------------------------------------------------------
The sample application bin/fjfxSample is a command-line tool that does feature extraction
on 500DPI fingerprint image files in PGM (portable graymap) format and saves the 
fingerprint minutiae data in ISO/IEC 19794-2:2005 format.


Required Libraries 
--------------------------------------------------------------------------------------------
FingerJetFX OSE requires
 - CxxTest - open source test framework
 - STLport - portable implementation of Standard Template Library (STL)
These third party components are included in the folder src/ExternalDependencies for your 
convenience to ensure that the library builds correctly.


Standards and Compatibility
--------------------------------------------------------------------------------------------
The output from FingerJetFX OSE is formatted according to the ANSI INSITS 378-2004 
or ISO/IEC 19794-2:2005 specifications for fingerprint minutiae data.


Building FingerJetFX OSE
--------------------------------------------------------------------------------------------

Run 'make' to build the library and the sample application. 
     - lib/libfjfx.so 
     - bin/fjfxSample

Run 'make check' to build the library, sample application and the unit tests and to test 
the libraries using the unit tests and the sample application.

Run 'make debug' to build debug version of the library and sample application:
     - lib/debug/libfjfx.so
     - bin/debug/fjfxSample 
Note that the debug version is significantly slower than the release version.

Run 'make checkdebug' to build the debug version of library, sample application and the 
unit tests and to test the libraries using the unit tests and the sample application.


Contents
--------------------------------------------------------------------------------------------
README.txt           This file
COPYRIGHT.txt        COPYRIGHT NOTICE
COPYING.txt          GNU GENERAL PUBLIC LICENSE
COPYING.LESSER.txt   GNU LESSER GENERAL PUBLIC LICENSE
Makefile             GNU make file (see Building FingerJetFX OSE)
.gitignore           Rules for the files that you do 'not' want to track with git
include              C include folder
src                  Source folder
samples              Sample code folder


Questions
--------------------------------------------------------------------------------------------
This software is provided AS-IS, however we'll do our best to answer questions. 
Please see digitalpersona.com/fingerjetfx for assistance.


