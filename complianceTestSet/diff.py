#!/usr/bin/env python

import sys, pandas as pd, argparse, os.path
from distutils.version import StrictVersion

# Need Pandas v1.1.0 
if StrictVersion(pd.__version__) < StrictVersion("1.1.0"):
  print("This script requires the use of Pandas v1.1.0.")
  sys.exit(1)
    
# Argument parser
parser = argparse.ArgumentParser()
parser.add_argument("csv1", type = str, help="First CSV")
parser.add_argument("csv2", type = str, help="Second CSV")
parser.add_argument("-o", type = str, help="Diff output")
parser.add_argument("-s", action='store_true', help="Indicates whether the files are the same")
args = parser.parse_args()

# Check if both CSV files passed in are valid files.
if not(os.path.isfile(args.csv1) and os.path.isfile(args.csv1)):
    print("Please provide 2 valid CSV files as arguments to this script\n")
    sys.exit(1)

# Set precision to 5. Same precision used in NFIQ2 output.
pd.set_option('precision', 5)
pd.set_option('display.precision', 5)

# Try to read data from each CSV. Exit upon failure.
try:
  df = pd.read_csv(args.csv1, low_memory = False, float_precision='high')
except:
  print("Failed to read in data from {}. Exiting.".format(args.csv1))
  sys.exit(1)

try:
  df2 = pd.read_csv(args.csv2, low_memory = False, float_precision='high')
except:
  print("Failed to read in data from {}. Exiting.".format(args.csv2))
  sys.exit(1)

# Check and make sure that the two CSVs are the same size.
# If the sizes differ then you are comparing two different dataset outputs.
if df.shape[0] != df2.shape[0]:
  print("Please only compare CSV results from the same dataset\n")
  sys.exit(1)

# All headers that are being compared. Contains standard headers and Quality headers. No speeds are being compared.
HEADERS = ["Filename", "FingerCode", "QualityScore", "OptionalError", "Quantized", "Resampled",
           "EmptyImageOrContrastTooLow", "UniformImage", "FingerprintImageWithMinutiae", "SufficientFingerprintForeground",
           "FDA_Bin10_0", "FDA_Bin10_1", "FDA_Bin10_2", "FDA_Bin10_3", "FDA_Bin10_4", "FDA_Bin10_5",
           "FDA_Bin10_6", "FDA_Bin10_7", "FDA_Bin10_8", "FDA_Bin10_9", "FDA_Bin10_Mean", "FDA_Bin10_StdDev",
           "FingerJetFX_MinCount_COMMinRect200x200", "FingerJetFX_MinutiaeCount", "FJFXPos_Mu_MinutiaeQuality_2",
           "FJFXPos_OCL_MinutiaeQuality_80", "ImgProcROIArea_Mean",
           "LCS_Bin10_0", "LCS_Bin10_1", "LCS_Bin10_2", "LCS_Bin10_3", "LCS_Bin10_4", "LCS_Bin10_5",
           "LCS_Bin10_6", "LCS_Bin10_7", "LCS_Bin10_8", "LCS_Bin10_9", "LCS_Bin10_Mean", "LCS_Bin10_StdDev",
           "MMB", "Mu",
           "OCL_Bin10_0", "OCL_Bin10_1", "OCL_Bin10_2", "OCL_Bin10_3", "OCL_Bin10_4", "OCL_Bin10_5", 
           "OCL_Bin10_6", "OCL_Bin10_7", "OCL_Bin10_8", "OCL_Bin10_9", "OCL_Bin10_Mean", "OCL_Bin10_StdDev",
           "OF_Bin10_0", "OF_Bin10_1", "OF_Bin10_2", "OF_Bin10_3", "OF_Bin10_4", "OF_Bin10_5",
           "OF_Bin10_6", "OF_Bin10_7", "OF_Bin10_8", "OF_Bin10_9", "OF_Bin10_Mean", "OF_Bin10_StdDev",
           "OrientationMap_ROIFilter_CoherenceRel", "OrientationMap_ROIFilter_CoherenceSum",
           "RVUP_Bin10_0", "RVUP_Bin10_1", "RVUP_Bin10_2", "RVUP_Bin10_3", "RVUP_Bin10_4", "RVUP_Bin10_5",
           "RVUP_Bin10_6", "RVUP_Bin10_7", "RVUP_Bin10_8", "RVUP_Bin10_9", "RVUP_Bin10_Mean", "RVUP_Bin10_StdDev"]

# Speed headers are filtered out, if present
df = df.filter(HEADERS)
df2 = df2.filter(HEADERS)

# Replace the full filename path with just the file base name for both dataframes
bnTemp = []
for col in df['Filename']:
    bnTemp.append(os.path.basename(col.replace('\\',os.sep)))
df['Filename'] = bnTemp

bn2Temp = []
for col in df2['Filename']:
    bn2Temp.append(os.path.basename(col.replace('\\',os.sep)))
df2['Filename'] = bn2Temp

# Sort each dataframe on key 'Filename'
df = df.sort_values(by=['Filename'])
df2 = df2.sort_values(by=['Filename'])

# Remove the existing indicies for comparison to work correctly
df = df.reset_index(drop=True)
df2 = df2.reset_index(drop=True)

# Compute the diff dataframe using Pandas compare - NEED Pandas V1.1.0 MINIMUM    
diff_df = df.compare(df2, keep_equal=False, keep_shape=False, align_axis=1)

# Used to re-add filenames post comparison at the front of the diff dataframe
fn_cols = []
for i, row in diff_df.iterrows():
  fn_cols.append(df.at[i, 'Filename'])

diff_df.insert(0, 'Filename', fn_cols)

# Melt the diff so that it looks more like a conventional diff
diff_df = pd.melt(diff_df, id_vars='Filename')

# Filter out duplicates resulting from the melt
a = diff_df.query('variable_1 == "self"').drop('variable_1', axis = 1)
b = diff_df.query('variable_1 != "self"').drop('variable_1', axis = 1)

# Join the results on 'Filename' and 'variable_0' (The attribute name that has a difference)
diff_df = pd.merge(a, b, on=['Filename', 'variable_0']).dropna()

# Clarify column names
diff_df = diff_df.rename(columns={'variable_0':'Variable', 'value_x':'X', 'value_y':'Y'})

# Printout Logic:
# If output is set and result is not empty, save as a CSV with given '-o' name
if args.o and len(diff_df.index) != 0:
    diff_df.to_csv(args.o, index = False)

# If no '-o' given, print diff to stdout
elif len(diff_df.index) != 0:
    print(diff_df.to_csv(index = False))

# If '-s' is enabled. Print True or False depending on whether the files are identical or not
if args.s:
    print(len(diff_df.index) == 0)

# Exit with 0 with empty diff, 1 otherwises
sys.exit(0 if len(diff_df.index) == 0 else 1)
