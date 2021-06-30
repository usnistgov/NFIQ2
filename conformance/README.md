# Conformance

This directory contains the tools required to test conformance of the NFIQ 2
code to ISO/IEC 29794-4.

**The images forming the conformance dataset are subject to a license agreement
and _are not included_ on GitHub. They can be [downloaded from the NIST
website](https://nigos.nist.gov/datasets/nfiq2_conformance/).**

## Included Files

 * **conformance_expected_output.csv**
   - Expected output from NFIQ 2 when run against the
     [conformance dataset](https://nigos.nist.gov/datasets/nfiq2_conformance/).
   - Contains unified quality score, quality metric values (`-v`), and
     actionable feedback (`-a`).
   - These values differ from ISO/IEC 29794-4:2017 Annex A due to the change in
     imagery and bug fixes. The standard will be revised to correct this.
 * **diff.py**
   - Python script used to compare the contents of two CSV outputs of NFIQ ≥2.1.
   - **NOTE:** Python 3 and Pandas v1.1.0 (or greater) are required.

## Usage

    $ python3 diff.py conformance_expected_output.csv YOUR_NFIQ2_OUTPUT.csv [-o {differences.csv}] [-s]

 - **conformance_expected_output.csv**:
   - The file included in this directory.
 - **YOUR_NFIQ2_OUTPUT.csv** contains the user generated NFIQ 2.0 csv output to be checked.
 - **`-o differences.csv`**
   - Optional system path where any differences are written.
 - **`-s`**: Print either _True_ or _False_ to indicate whether the two CSVs
   files are the same.

### Output

If conformant, there will no output. Otherwise, values that differ will be
printed.
