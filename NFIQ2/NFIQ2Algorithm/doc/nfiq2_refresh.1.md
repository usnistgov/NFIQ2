% REFRESH_NFIQ2(1) Version 1.0 | BSD General Commands Manual

NAME
====

**refresh_nfiq2** — Produces NFIQ2 quality scores for finger print images

SYNOPSIS
========

| **refresh_nfiq2** **OPTIONS** [_file/dir_ ...] 

| **refresh_nfiq2** -f [_batch_ -f ...] **OPTIONS** [_file/dir_ ...]


DESCRIPTION
===========

| **refresh_nfiq2** is a tool for the biometric fingerprint quality algorithm NFIQ2. It is used to produce quality scores for fingerprint image formats supported by the Biometric Evaluation Framework.

| **refresh_nfiq2** has the ability to take multiple file/directory paths as arguments. The exception to this is batch files, which must be following a "-f" option. If multiple images are provided, **refresh_nfiq2** will attempt to process all images. If an image fails to be scanned for whatever reason, an error is printed in absence of a quality score. 

| **refresh_nfiq2** Prints scores in a Comma Separated Value format (CSV). The standard output includes information about the image including: the image's name, finger position (if applicable), quality score, error message (if applicable), whether the image was quantized, and whether the image was resampled.

| This tool features some additional options, including output redirection and multi-threading, listed below:

OPTIONS
=======
| **-i** _file/dir_
> Allows for explicit _file/dir_ arguments to be given to the command line. Using this option with a _file/dir_ is equivalent to providing file paths directly to the **refresh_nfiq2** executable. 

| **-f** _batch_
> Allows for batch files to be given to **refresh_nfiq2**. This option must be used for a batch file to be correctly processed. Each filepath in a batch file is read in and an image tries to be parsed from that location. If unsuccessful, an error message is printed out and computation continues until all paths have been scanned. This operation can be sequential or multi-threaded. Output is not guaranteed to be in order if the multi-threaded option is selected.

| **-o** _file_ 						
> Allows for all output to be printed to a _file_. A _file_ will be created at the designated path if one does not exist already. Output includes all scores, error messages, verbose, speed, and debug dialogue.  

| **-j** _threads_ 						
> Indicates the number of worker _threads_ that will be generated when running multi-threaded batch or record store operations. This number may exceed the number of physical cores on a user's system; however, a warning will appear asking if the user would like to proceed or change the number of _threads_ to equal the number of physical cores. 

| **-v** 								
> Verbose option provides additional feature information pertaining to each subcomponent of the NFIQ2 algorithm.

| **-q** 								
> Speed option provides additional speed information pertaining to the amount of time each subcomponent of the NFIQ2 algorithm took to compute.

| **-d** 								
> Debug option provides additional information pertaining to program execution and details each step of computation. 

| **-F** 								
> Force option ignores user-input from the program when an image does not meet NFIQ2 image requirements and proceeds with execution as normal.
> See NOTES 1

| **-r** 								
> Recursion option allows for directories to be recursively scanned for images. I.E. if a directory exists within another directory. The Recursion option also traverses that inner directory. The recursion performed is depth-first and stops once all branches have been scanned. 

| **-m** _model_						
> Allows for an alternative Machine Learning model to be used in conjunction with NFIQ2's algorithm.  					
> Not implemented yet

NOTES
=====

1. NFIQ2 has restrictions on what kinds of finger print images it can process. The color depth and bit depth of an image must be 8. The PPI of an image must be 500. **refresh_nfiq2** has mechanisms built in to try and quantize/resample images that do not meet these qualifications so that a score can still be produced. These mechanisms will be automatically applied when utilizing "-F" 'force' option.

2. Output is generated in a standard Comma Separated Value (CSV) format. Headers are printed before any scores are printed. The exception to this format is when a single image is provided with no "-v" or "-q" options. In this case, only the quality score is printed for the image.

3. Current supported image types/file formats include:
	1. Finger print image formats parse-able through the Biometric Evaluation Framework (i.e. png, jpeg, tiff, wsq, etc.)
	2. Batch files
	3. RecordStores
	4. ANSI/NIST AN2K files
	5. ANSI/ISO ANSI2004 files

EXAMPLES
========

| The following are examples of using **refresh_NFIQ2** on different file types and option combinations.

| refresh_NFIQ2 print1

> Prints the quality score of _print1_ to the screen. 

| refresh_NFIQ2 print1 print2 print3

> Prints the standard CSV formatted scores of _print1_, _print2_, and _print3_.

| refresh_NFIQ2 fingerprintDir

> Searches the directory _fingerprintDir_ and processes all of the finger print images it can identify. 

| refresh_NFIQ2 -r fingerprintDir

> Recursively searches through _fingerprintDir_ and all directories within _fingerprintDir_ to find and process all identifiable finger prints. 

| refresh_NFIQ2 -r -i print1 -i fingerprintDir -o output.txt print2 print3

> Produces the quality scores of _print1_, _print2_, and _print3_. Recursively traverses _fingerprintDir_ and prints the quality scores of the finger print images it discovers in there. Saves all score output to a file in the current directory named _output.txt_. This example showcases how **refresh_NFIQ2** can support multiple types of arguments in a single execution. 

| refresh_NFIQ2 -v -q fingerprintDir

> Produces the quality scores of the finger print images stored inside of _fingerprintDir_. Additional NFIQ2 component algorithm results and their timings are also printed in CSV format -- appended to the standard CSV format. 

| refresh_NFIQ2 -F mixedfingerprintDir

> _mixedfingerprintDir_ contains a variety of finger print images. Some do adhere to NFIQ2's 8 bit color and depth, and 500 PPI requirements, some do not. The _-F_ option automatically applies any quantizing or resampling applicable to each image scanned. 

| refresh_NFIQ2 -f batchFile1

> The _-f_ option denotes _batchFile1_ as a batch file comprising up of a list of paths to finger print images. **refresh_NFIQ2** grabs the content of this _batchFile1_ and sequentially calculates the quality of each image and prints it to the screen.

| refresh_NFIQ2 -v -q -f batchFile1 -j 4

> This is a multi-threaded batch operation on _batchFile1_, utilizing _4_ threads, denoted by the _-j_ option. The _-v_ and _-q_ are also enabled, producing additional NFIQ2 sub component scores and their timings. 

| refresh_NFIQ2 recordStore1

> Iterates through the records of _recordStore1_ and produces the scores of the images stored within those records sequentially. 

| refresh_NFIQ2 -j 8 recordStore1

> Multi-threaded operation processing the records of _recordStore1_, utilizing _8_ worker _threads_.

VERSION
=======

| This man page is current for version 1.0 of **refresh_nfiq2**

HISTORY
=======

| First Released Summer 2020 by NIST.
