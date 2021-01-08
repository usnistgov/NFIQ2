% NFIQ2(1) Version 1.0 | National Institute of Standards and Technology

NAME
====

**nfiq2** — Compute quality of fingerprint images

SYNOPSIS
========

| **nfiq2** **OPTIONS** _file/dir/rs_ [...]

| **nfiq2** -f _batch_ [-f ...] **OPTIONS** [_file/dir/rs_ ...]


DESCRIPTION
===========

| **nfiq2** is a tool for computing the NIST Fingerprint Image Quality (NFIQ) 2 of fingerprint images. It is used to produce quality scores for fingerprint images encoded in formats supported by Biometric Evaluation framework (BMP, JPEG, JPEGL, JPEG-2000, PBM, PNG, TIFF, and WSQ). Images can be encoded stand alone, within an ANSI/NIST ITL Type 14 record, or any combination of the two within a Biometric Evaluation Record Store. **nfiq2** serves as a reference implementation of ISO/IEC 29794-4:2017.

| **nfiq2** Prints scores in a Comma Separated Value (CSV) format. The default output includes information about the image including: the image's name, finger position (if applicable), quality score, error message (if applicable), whether the image was quantized, and whether the image was resampled.

| This tool features some additional options, including batch operation support, output redirection and multi-threading, listed below:

OPTIONS
=======
| **-i** _file/dir/rs_
> Allows for explicit _file/dir/rs_ arguments to be passed. Using this option with a _file/dir/rs_ is equivalent to providing file paths directly to the **nfiq2** executable.

| **-f** _batch_
> Batch files. A batch file is a plain text file, where each line is the path to a file to process.

| **-o** _file_
> Write all output to be printed to _file_. _file_ will be overwritten if it exists.

| **-j** _threads_
> Indicates the number of worker _threads_ that will be spawned when running batch or Record Store operations. This number may exceed the number of physical cores on a user's system; however, a warning will appear asking if the user would like to proceed or change the number of _threads_ to equal the number of physical cores. One additional thread will be spawed for coordinating output.

| **-a**
> Actionable quality output. Provides additonal actionable quality information pertainting to each processed fingerprint image.

| **-v**
> Verbose output. Provides additional feature quality information pertaining to each measurement of NFIQ2.

| **-q**
> Speed. Provides additional speed information pertaining to the amount of time each measurement of NFIQ2 took to compute.

| **-d**
> Debug. Provides additional information pertaining to program execution and details each step of computation.

| **-F**
> Force. Does not ask for user input when when an image does not meet NFIQ2 image requirements.
> See NOTES 1

| **-r**
> Recursion. Allows for directories to be recursively scanned for images/records. The recursion performed is depth-first and stops once all branches have been scanned.

| **-m** _model_
> Path to an model information file. Allows for alternative random forest parameters to be used in conjunction with NFIQ2's algorithm. A default file is provided.
> This file contains "Key = Value" pairs of information, with Keys:

> * **Name**: Name given to this set of random forest parameters
> * **Description**: Description of the training
> * **Version**: Version number of the training
> * **Path**: Path to the random forest parameters
> * **Hash**: Hash of random forest parameters, as parsed by OpenCV


NOTES
=====

1. NFIQ2 has restrictions on what kinds of fingerprint images it can process. The color depth and bit depth of an image must be 8 (i.e., maximum of 255 shades of gray). The PPI of an image must be 500. **nfiq2** has mechanisms to quantize and/or resample images that do not meet these qualifications so that a quality score can still be produced. These mechanisms will be automatically applied when utilizing the **-F** flag.

2. Output is generated in a CSV format. Headers are printed before any scores are printed. The exception to this format is when a single image is provided without the **-v** or **-q** flag. In this case, only the quality score is printed for the image.

3. Current supported formats include:
	1. Fingerprint image formats parseable through Biometric Evaluation Framework (e.g., PNG, JPEG, TIFF, WSQ, etc.)
	2. Batch files
	3. Record Stores
	4. ANSI/NIST-ITL 2007 (and later) binary files

EXAMPLES
========

| The following are examples of using **nfiq2** on different file types and option combinations.

| nfiq2 print1.wsq

> Prints the quality score of _print1.wsq_ to the screen.

| nfiq2 print1.wsq print2.png print3.an2

> Prints the standard CSV formatted scores of _print1.wsq_, _print2.png_, and _print3.an2_.

| nfiq2 fingerprintDir

> Searches the directory _fingerprintDir_ and processes all of the fingerprint images it can identify.

| nfiq2 -r fingerprintDir

> Recursively searches through _fingerprintDir_ and all directories within _fingerprintDir_ to find and process all identifiable fingerprints.

| nfiq2 -r -i print1.tif -i fingerprintDir -o output.csv print2.jpg print3.bmp

> Produces quality scores of _print1.tif_, _print2.jpg_, and _print3.bmp_. Recursively traverses _fingerprintDir_ and prints the quality scores of the fingerprint images it discovers in there. Saves all output to a file in the current directory named _output.csv_. This example showcases how **nfiq2** can support multiple types of arguments in a single execution.

| nfiq2 -v -q fingerprintDir

> Produces the quality scores of the fingerprint images stored inside of _fingerprintDir_. Additional NFIQ2 component algorithm results and their timings are also printed in CSV format -- appended to the standard CSV format.

| nfiq2 -F mixedFingerprintDir

> _mixedFingerprintDir_ contains a variety of fingerprint images. Some adhere to NFIQ2's 8 bit and color depth, and 500 PPI requirements, some do not. The _-F_ option automatically applies any quantizing and resampling applicable to each image scanned.

| nfiq2 -f batchFile1.txt

> The **-f** option denotes _batchFile1.txt_ as a batch file comprising of a list of paths to fingerprint images. **nfiq2** reads the content of  _batchFile1.txt_,  sequentially calculates the quality of each image, and prints it to the screen.

| nfiq2 -v -q -f batchFile1.txt -j 4

> This is a multi-threaded batch operation on _batchFile1.txt_, utilizing _4_ threads, denoted by the **-j** option. The **-v** and **-q** are also enabled, producing additional NFIQ2 sub component scores and their timings.

| nfiq2 recordStore1

> Iterates through the records of _recordStore1_, producing quality scores of the images stored within the Record Store sequentially.

| nfiq2 -j 8 recordStore1

> Multi-threaded operation processing the records of _recordStore1_, utilizing _8_ worker _threads_.

VERSION
=======

| This man page is current for version 1.0 of **nfiq2**

HISTORY
=======

| First released August 2020 by NIST.
