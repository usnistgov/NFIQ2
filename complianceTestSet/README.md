Compliance Test
===============

Contained in this directory are the tools required to perform compliance testing on NFIQ 2.0. 

**NOTE** It is important to have Python Pandas v1.1.0 installed for the diff script to work correctly. 


1. CTS_MASTER_OUTPUT.csv
	The correct NFIQ2 score output for the Compliance Test dataset found here: [NFIQ2 Compliance Test Set](INSERT_CTS_LINK_HERE)
	This CSV output contains NFIQ2 scores and their individual quality scores. 
	It also contains actionable quality scores. Speed output has been omitted.


2. diff.py
	The python script that will be used to compare the contents of two CSV outputs. 
	Use this script to detect any diffs present between a modified version of NFIQ 2.0 and the master.

	
	The script takes two CSV files to be compared as input and two optional flags (-o and -s)
		| -o allows you to save the resulting diff output to a designated _OUTPUT_PATH_
		| -s prints either **True** or **False** depending on whether the CSV files were identical

	
	diff.py Usage:
		| **python3** **diff.py** _CTS_MASTER_OUTPUT.csv_ _Your_NFIQ2_Output.csv_ [-o {_OUTPUT_PATH_}] [-s]
	

A compliant NFIQ 2.0 implementation will produce a clean diff upon performing compliance testing. 
