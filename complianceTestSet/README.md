# Compliance Test


Contained in this directory are the tools required to perform compliance testing on NFIQ 2.0. A compliant NFIQ 2.0 implementation will produce a clean diff upon performing compliance testing. 

**NOTE:** It is important to have Python Pandas v1.1.0 (or greater) installed for the diff script to work correctly. 

## Included Files

#### CTS_MASTER_OUTPUT.csv
- The correct NFIQ2 score output for the Compliance Test dataset found here: [NFIQ2 Compliance Test Set](https://nigos.nist.gov/datasets/nfiq2_compliance/)
- This CSV output contains NFIQ2 scores and their individual quality scores. 
It also contains actionable quality scores. Speed output has been omitted.


#### diff.py
- The python script that will be used to compare the contents of two CSV outputs. 
- Use this script to detect any diffs present between a modified version of NFIQ 2.0 and the master.
	

## diff.py Usage


    $ diff.py CTS_MASTER_OUTPUT.csv YOUR_NFIQ2_OUTPUT.csv [-o {DIFF_OUTPUT_PATH}] [-s]

 - ***YOUR_NFIQ2_OUTPUT.csv*** contains the user generated NFIQ 2.0 csv output to be checked.
 - ***DIFF_OUTPUT_PATH*** is a an optional system path where the user can save diff output to a file using the ***-o*** flag.
 - To print either a "**True**" or "**False**" to indicate whether the two csv files are the same, the user can add the option ***-s*** flag.

