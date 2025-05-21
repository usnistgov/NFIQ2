#!/usr/bin/env python

import pandas as pd
import sys
import argparse

REQUIRED_COLUMNS = [
	"Filename",
	"FingerCode",
	"QualityScore",
	"OptionalError",
	"Quantized",
	"Resampled",
	"EmptyImageOrContrastTooLow",
	"UniformImage",
	"FingerprintImageWithMinutiae",
	"SufficientFingerprintForeground",
	"FDA_Bin10_0",
	"FDA_Bin10_1",
	"FDA_Bin10_2",
	"FDA_Bin10_3",
	"FDA_Bin10_4",
	"FDA_Bin10_5",
	"FDA_Bin10_6",
	"FDA_Bin10_7",
	"FDA_Bin10_8",
	"FDA_Bin10_9",
	"FDA_Bin10_Mean",
	"FDA_Bin10_StdDev",
	"FingerJetFX_MinCount_COMMinRect200x200",
	"FingerJetFX_MinutiaeCount",
	"FJFXPos_Mu_MinutiaeQuality_2",
	"FJFXPos_OCL_MinutiaeQuality_80",
	"ImgProcROIArea_Mean",
	"LCS_Bin10_0",
	"LCS_Bin10_1",
	"LCS_Bin10_2",
	"LCS_Bin10_3",
	"LCS_Bin10_4",
	"LCS_Bin10_5",
	"LCS_Bin10_6",
	"LCS_Bin10_7",
	"LCS_Bin10_8",
	"LCS_Bin10_9",
	"LCS_Bin10_Mean",
	"LCS_Bin10_StdDev",
	"MMB",
	"Mu",
	"OCL_Bin10_0",
	"OCL_Bin10_1",
	"OCL_Bin10_2",
	"OCL_Bin10_3",
	"OCL_Bin10_4",
	"OCL_Bin10_5",
	"OCL_Bin10_6",
	"OCL_Bin10_7",
	"OCL_Bin10_8",
	"OCL_Bin10_9",
	"OCL_Bin10_Mean",
	"OCL_Bin10_StdDev",
	"OF_Bin10_0",
	"OF_Bin10_1",
	"OF_Bin10_2",
	"OF_Bin10_3",
	"OF_Bin10_4",
	"OF_Bin10_5",
	"OF_Bin10_6",
	"OF_Bin10_7",
	"OF_Bin10_8",
	"OF_Bin10_9",
	"OF_Bin10_Mean",
	"OF_Bin10_StdDev",
	"OrientationMap_ROIFilter_CoherenceRel",
	"OrientationMap_ROIFilter_CoherenceSum",
	"RVUP_Bin10_0",
	"RVUP_Bin10_1",
	"RVUP_Bin10_2",
	"RVUP_Bin10_3",
	"RVUP_Bin10_4",
	"RVUP_Bin10_5",
	"RVUP_Bin10_6",
	"RVUP_Bin10_7",
	"RVUP_Bin10_8",
	"RVUP_Bin10_9",
	"RVUP_Bin10_Mean",
	"RVUP_Bin10_StdDev",
]

def load_csv(path):
	try:
		df = pd.read_csv(
			path,
			low_memory=False,
			keep_default_na=False,
			dtype=str,
			na_values=[],
		)
	except FileNotFoundError:
		raise FileNotFoundError(f"Error: File not found — {path}")
	except pd.errors.EmptyDataError:
		raise pd.errors.EmptyDataError(f"Error: File is empty — {path}")
	except pd.errors.ParserError as e:
		raise pd.errors.ParserError(f"Error: Failed to parse CSV — {path}\n{e}")
	except UnicodeDecodeError:
		raise UnicodeDecodeError(f"Error: Encoding issue — could not decode file {path}")
	except MemoryError:
		raise MemoryError(f"Error: Not enough memory to read file {path}. Consider processing it in chunks.")
	except OSError as e:
		raise OSError(f"OS Error reading file {path}: {e}")
	except ValueError as e:
		raise ValueError(f"ValueError reading {path}: {e}")
	except Exception as e:
		raise Exception(f"Unexpected error reading {path}: {e}")

	missing = [col for col in REQUIRED_COLUMNS if col not in df.columns]
	if missing:
		raise ValueError(f"Error: The following required columns are missing in {path}:\n{missing}")


def perform_diff(csv1_path, csv2_path, output_diffs):
	df1 = load_csv(csv1_path)
	df2 = load_csv(csv2_path)

	if df1.shape[0] != df2.shape[0]:
		raise ValueError("Error: The number of rows in the CSV files differ.")

	df1 = df1[REQUIRED_COLUMNS]
	df2 = df2[REQUIRED_COLUMNS]

	merged = pd.merge(
		df1, df2, on="Filename", suffixes=("_1", "_2"), how="outer", indicator=True
	)

	if (merged["_merge"] != "both").any():
		unmatched = merged[merged["_merge"] != "both"]
		raise ValueError(f'Error: Some "Filename" keys are not present in both files.\n{unmatched[["Filename", "_merge"]]}')

	compare_cols = [col for col in REQUIRED_COLUMNS if col != "Filename"]

	diffs_found = False
	header_printed = False
	for col in compare_cols:
		col1 = f"{col}_1"
		col2 = f"{col}_2"

		diff_rows = merged[merged[col1] != merged[col2]]
		col_has_diffs = not diff_rows.empty
		diffs_found = diffs_found or col_has_diffs

		if output_diffs and col_has_diffs:
			if not header_printed:
				print('"filename","column",csv1_value,csv2_value,difference')
				header_printed = True

			for row in diff_rows.itertuples(index=False):
				val1 = getattr(row, col1)
				val2 = getattr(row, col2)
				try:
					diff = float(val1) - float(val2)
					diff_str = f"{diff}"
				except (ValueError, TypeError):
					diff_str = "NA"
				print(f'"{getattr(row, "Filename")}","{col}",{val1},{val2},{diff_str}')

	return diffs_found


if __name__ == "__main__":
	parser = argparse.ArgumentParser(
		description="Compare two CSVs created by NFIQ 2."
	)
	parser.add_argument("csv1", help="Path to first CSV file")
	parser.add_argument("csv2", help="Path to second CSV file")
	parser.add_argument("-o", action="store_true", help="Output differences")

	args = parser.parse_args()

	try:
		diffs_found = perform_diff(args.csv1, args.csv2, args.o)
	except Exception as e:
		diffs_found = True
		print(f"{e}", file=sys.stderr)


	sys.exit(1 if diffs_found else 0)
