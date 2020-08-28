import subprocess, platform, argparse, sys, os.path, pandas as pd

pd.options.mode.chained_assignment = None

def diff_func(csv1, csv2, outputDir, tf):

	if not(os.path.isfile(csv1) and os.path.isfile(csv1)):
	    print("Please provide 2 valid CSV files as arguments to this script")
	    return(1)

	df = pd.read_csv(csv1, low_memory = False)
	df2 = pd.read_csv(csv2, low_memory = False)

	HEADERS = ["Filename", "FingerCode", "QualityScore", "OptionalError", "Quantized", "Re-sampled",
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

	df = df.filter(HEADERS)
	df2 = df2.filter(HEADERS)

	bnTemp = []
	for col in df['Filename']:
	    bnTemp.append(os.path.basename(col.replace('\\',os.sep)))
	df['Filename'] = bnTemp

	bn2Temp = []
	for col in df2['Filename']:
	    bn2Temp.append(os.path.basename(col.replace('\\',os.sep)))
	df2['Filename'] = bn2Temp
	      
	comp_df = df.merge(df2, indicator=True, how='outer')
	diff_df = comp_df[comp_df['_merge'] != 'both']

	diff_df._merge.replace('left_only', csv1, inplace = True)
	diff_df._merge.replace('right_only', csv2, inplace = True)
	diff_df.rename(columns={'_merge':'csv_name'}, inplace=True)

	if outputDir and len(diff_df.index) != 0:
	    diff_df.to_csv(outputDir)
	elif len(diff_df.index) != 0:
	    print(diff_df.to_csv())

	if tf:
	    print(len(diff_df.index) == 0)

	return(0 if len(diff_df.index) == 0 else 1)

if __name__ == '__main__':

	parser = argparse.ArgumentParser()
	parser.add_argument("-o", type = str, help="Diff output")
	parser.add_argument("-s", action='store_true', help="Indicates whether the files are the same")
	parser.add_argument("bitness", type = str, help="Indicate the platform of the build")
	args = parser.parse_args()

	if args.bitness != "Win32" and args.bitness != "x64":
		print("Please provide a valid platform: 'Win32' or 'x64'")
		sys.exit(1)

	os_type = platform.system()

  #Change dir to location of testing images
	image_dir = "images/"

	if os_type == "Darwin":
		subprocess.run(["./../dist/NFIQ2/build/bin/nfiq2", "-v", "-o", "output.AppleClang.apple64.csv", image_dir])
		sys.exit(diff_func("result.AppleClang.apple64.csv", "result.AppleClang.apple64.csv", "diffoutput.csv", args.s))

	elif os_type == "Linux":
		subprocess.run(["./../dist/NFIQ2/build/bin/nfiq2", "-v", "-o", "output.gnu.linux64.csv", image_dir])
		sys.exit(diff_func("output.gnu.linux64.csv", "result.gnu.linux64.csv", "diffoutput.csv", args.s))

	elif os_type == "Windows" and args.bitness == "x64":
		subprocess.run([".\\..\\dist\\NFIQ2\\build\\bin\\nfiq2", "-v", "-o", "output.msvc.win64.csv", image_dir])
		sys.exit(diff_func("output.msvc.win64.csv", "result.msvc.win64.csv", "diffoutput.csv", args.s))

	elif os_type == "Windows" and args.bitness == "Win32":
		subprocess.run([".\\..\\dist\\NFIQ2\\build\\bin\\nfiq2", "-v", "-o", "output.msvc.win32.csv", image_dir])
		sys.exit(diff_func("output.msvc.win32.csv", "result.msvc.win32.csv", "diffoutput.csv", args.s))

	else:
		print("Error Detecting OS and/or Platform")
		sys.exit(1)
