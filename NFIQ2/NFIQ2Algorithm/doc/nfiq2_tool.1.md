% NFIQ2(1) Version 2.1 | National Institute of Standards and Technology

NAME
====

**nfiq2** — Compute quality of 500 PPI fingerprint images

SYNOPSIS
========

One (or more) images, records, directories, or RecordStores:
: **nfiq2** [OPTION...] _path_ [_path_ ...]

Batch files:
: **nfiq2** [OPTION...] -f _batchfile_ [-f _batchfile_ ...]


DESCRIPTION
===========

**nfiq2** is a tool for computing the NIST Fingerprint Image Quality (NFIQ) 2 quality score of **500 PPI** fingerprint images. It supports most common image formats, including images within ANSI/NIST ITL Type 14 records, ISO/IEC 19794-4 records, and Biometric Evaluation Framework RecordStores. **nfiq2** serves as a reference implementation of ISO/IEC 29794-4:2017.

**nfiq2** outputs scores in Comma Separated Values (CSV) format. The default output includes information about the image including: the image's name, finger position (if applicable), quality score, error message (if applicable), whether the image was quantized, and whether the image was resampled.

This tool features some additional options, including batch operation support, output redirection, and multi-threading, listed below.

OPTIONS
=======
**-i** _image/record/directory/rs_
: Allows for explicit _image/record/directory/rs_ arguments to be passed. Using this option with a _image/record/directory/rs_ is equivalent to providing file paths directly to **nfiq2**.

**-f** _batch_
: Batch files. A batch file is a plain text file, where each line is the path to a file to process.

**-o** _file_
: Write all output to be printed to _file_. _file_ will be overwritten if it exists.

**-j** _threads_
: Indicates the number of worker _threads_ that will be spawned when running batch or RecordStore operations. This number may exceed the number of physical cores on a user's system; however, a warning will appear asking if the user would like to proceed or change the number of _threads_ to equal the number of physical cores. Note that one additional thread will be spawned for coordinating output.

**-a**
: Actionable quality output. Provides additonal actionable quality feedback pertainting to each processed fingerprint image.

**-v**
: Verbose output. Provides individual quality feature values computed during the calculation of the overall quality score.

**-q**
: Speed. Provides elapsed time for each quality feature computation.

**-d**
: Debug. Provides additional information pertaining to program execution and details each step of computation.

**-F**
: Force. Does not ask for user input when when an image does not meet image requirements. See **NOTES 2**.

**-r**
: Recursion. Allows for directories to be recursively scanned for images/records. The recursion performed is depth-first and stops once all branches have been scanned.

**-m** _model_
: Path to an model information file. Allows for alternative random forest parameters to be used in conjunction with **nfiq2**. A default file is provided. This file contains "_Key_ = _Value_" pairs of information, with _Key_:

	* **Name**: Name given to this set of random forest parameters.
	* **Description**: Description of the training.
	* **Trainer**: Entity that created this set of parameters.
	* **Version**: Version number of the parameters.
	* **Path**: Path to the random forest parameters. If the path provided is relative, it must be relative to the directory containing the file passed with *-m*, not the current working directory or the nfiq2 executable.
	* **Hash**: Hash of random forest parameters, as parsed by OpenCV.

NOTES
=====

1. NFIQ 2 has restrictions on image dimensions via a restriction in one of its dependencies. Images width must be greater than 196 pixels and less than 800 pixels. Image height must be greater than 196 pixels and less than 1000 pixels. These dimensions are calculated *after* NFIQ 2 crops whitespace from the edges of the image.

2. NFIQ 2 has restrictions on what kinds of fingerprint images it can process. The color depth and bit depth of an image must be 8 (i.e., maximum of 255 shades of gray). The PPI of an image must be 500. **nfiq2** has mechanisms to quantize and/or resample images that do not meet these qualifications so that a quality score can still be produced. These mechanisms will be automatically applied when utilizing the **-F** flag when the source resolution is known. If the resolution cannot be determined, the **-F** flag will assume the resolution is 500 PPI.

3. Output is generated in a CSV format. Headers are printed before any scores are printed. The exception to this format is when a single image is provided without the **-v** or **-q** flag. In this case, only the quality score is printed for the image.

4. Current supported formats include:

	1. BMP, JPEG, JPEGL, JPEG 2000, PBM/PGM/PPM, PNG, TIFF, WSQ
	3. Biometric Evaluation Framework RecordStores
	4. ANSI/NIST-ITL 1-2007 and later Type 14 (binary only, not XML)
	5. ISO/IEC 19794-4

EXAMPLES
========

The following are examples of using **nfiq2** on different file types and option combinations.

nfiq2 print1.wsq
: Prints the quality score of _print1.wsq_ to the screen.

nfiq2 print1.wsq print2.png print3.an2
: Prints the standard CSV formatted scores of _print1.wsq_, _print2.png_, and _print3.an2_.

nfiq2 fingerprintDir
: Searches the directory _fingerprintDir_ and processes all of the fingerprint images it can identify.

nfiq2 -r fingerprintDir
: Recursively searches through _fingerprintDir_ and all directories within _fingerprintDir_ to find and process all identifiable fingerprints.

nfiq2 -r -i print1.tif -i fingerprintDir -o output.csv print2.jpg print3.bmp
:  Produces quality scores of _print1.tif_, _print2.jpg_, and _print3.bmp_. Recursively traverses _fingerprintDir_ and prints the quality scores of the fingerprint images it discovers in there. Saves all output to a file in the current directory named _output.csv_. This example showcases how **nfiq2** can support multiple types of arguments in a single execution.

nfiq2 -v -q fingerprintDir
: Produces the quality scores of the fingerprint images stored inside of _fingerprintDir_. NFIQ 2 quality feature values and their timings are also printed in additional CSV columns.

nfiq2 -F mixedFingerprintDir
: _mixedFingerprintDir_ contains a variety of fingerprint images. Some adhere to NFIQ 2's 8 bit and color depth, and 500 PPI requirements, some do not. The _-F_ option automatically applies any quantizing and resampling applicable to each image scanned.

nfiq2 -f batchFile1.txt
: The **-f** option denotes _batchFile1.txt_ as a batch file comprising of a list of paths to fingerprint images. **nfiq2** reads the content of  _batchFile1.txt_,  sequentially calculates the quality of each image, and prints it to the console.

nfiq2 -v -q -f batchFile1.txt -j 4
: This is a multi-threaded batch operation on _batchFile1.txt_, utilizing _4_ threads, denoted by the **-j** option. The **-v** and **-q** options are also enabled, outputting NFIQ 2 quality feature values and their speeds.

nfiq2 recordStore1
: Iterates through the records of _recordStore1_, producing quality scores of the images stored within the RecordStore sequentially.

nfiq2 -j 8 recordStore1
: Multi-threaded operation processing the records of _recordStore1_, utilizing _8_ worker _threads_.

VERSION
=======

This man page is current for version 2.1 of **nfiq2**.

HISTORY
=======

The NFIQ 2 algorithm was first released in April 2016 by NIST, in collaboration with Germany's Federal Office for Information Security and Federal Criminal Police Office, as well as research and development entities MITRE, Fraunhofer IGD, Hochschule Darmstadt, and Secunet.

This revision of the NFIQ 2 command line interface was first released by NIST in August 2020.
