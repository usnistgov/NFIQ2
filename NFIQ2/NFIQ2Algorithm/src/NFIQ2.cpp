#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <list>
#include <fstream>

#include "include/NFIQException.h"
#include "include/Timer.hpp"
#include "include/FingerprintImageData.h"
#include "include/NFIQ2Algorithm.h"

using namespace NFIQ;

void printUsage()
{
	std::cout << "USAGE:" << std::endl;
	std::cout << "  NFIQ2 <runMode> [specific run mode arguments]" << std::endl;
	
	std::cout << "  <runMode>: run mode of NFIQ2 tool, possible values" << std::endl;
	std::cout << "              SINGLE, BATCH" << std::endl << std::endl;

	std::cout << "  run mode SINGLE:" << std::endl;
	std::cout << "  ----------------" << std::endl;
	std::cout << "  NFIQ2 SINGLE <fingerprintImage> <imageFormat> <outputFeatureData> <outputSpeed>" << std::endl << std::endl;
	
	std::cout << "    <fingerprintImage>: path and filename to a fingerprint image" << std::endl;
	std::cout << "    <imageFormat>: one of following values describing the fingerprint image format" << std::endl;
	std::cout << "              BMP, WSQ" << std::endl;
	std::cout << "    <outputFeatureData>: if to print computed quality feature values" << std::endl;
	std::cout << "             true, false" << std::endl;
	std::cout << "    <outputSpeed>: if to print speed of quality feature computation" << std::endl;
	std::cout << "             true, false" << std::endl << std::endl;

	std::cout << "  run mode BATCH:" << std::endl;
	std::cout << "  ---------------" << std::endl;
	std::cout << "  NFIQ2 BATCH <fingerprintImageList> <imageFormat> <resultList> <outputFeatureData> <outputSpeed> [<speedResultList>]" << std::endl << std::endl;

	std::cout << "    <fingerprintImageList>: path and filename to a list of fingerprint images" << std::endl;
	std::cout << "    <imageFormat>: one of following values describing the fingerprint image format of all images in the input list" << std::endl;
	std::cout << "              BMP, WSQ" << std::endl;
	std::cout << "    <resultList>: path and filename of the CSV output file that will contain NFIQ2 values and (optional) feature values" << std::endl;
	std::cout << "    <outputFeatureData>: if to add computed quality feature values to the resulting CSV output file" << std::endl;
	std::cout << "             true, false" << std::endl;
	std::cout << "    <outputSpeed>: if to compute the speed of NFIQ2 computation" << std::endl;
	std::cout << "             true, false" << std::endl;
	std::cout << "    <speedResultList>: path and filename of another CSV output file that will contain the NFIQ2 speed values (optional argument, only applied if outputSpeed = true)" << std::endl;

}


std::vector<std::string> getFileContent(const std::string & fileName)
{
	std::vector<std::string> vecLines;

	bool success = false;
	std::string line = "";
	std::ifstream ifs(fileName.c_str(), std::ios::in);
	if (ifs.is_open())
	{
		while (!ifs.eof())
		{
			getline(ifs, line);
			vecLines.push_back(line);
		}
		success = !ifs.bad(); // badbit is set if read was incomplete or failed
		ifs.close();
	}
	else
		throw NFIQ::NFIQException(NFIQ::e_Error_CannotReadFromFile);

	if (!success)
		throw NFIQ::NFIQException(NFIQ::e_Error_CannotReadFromFile);

	return vecLines;
}

int executeRunModeSingle(std::string fpImagePath, std::string imageFormat, bool bOutputFeatureData, bool bOutputSpeed)
{
	try
	{
		std::cout << "NFIQ2: Compute quality score for fingerprint image " << fpImagePath << std::endl;
		
		// read fingerprint image
		NFIQ::FingerprintImageData fpImage;
		fpImage.readFromFile(fpImagePath);
		NFIQ::FingerprintImageData rawImage;
		if (imageFormat.compare("BMP") == 0)
			rawImage.fromBMP(fpImage);
		else if (imageFormat.compare("WSQ") == 0)
			rawImage.fromWSQ(fpImage);
		else
		{
			std::cerr << "ERROR => Unknown image format specified" << std::endl;
			return -1;
		}

		// start timer for initialization routine
		NFIQ::Timer timerInit;
		double timeInit = 0.0;
		timerInit.startTimer();

		// do initialization
		NFIQ::NFIQ2Algorithm nfiq2;
		std::list<NFIQ::ActionableQualityFeedback> actionableQuality;
		timeInit = timerInit.endTimerAndGetElapsedTime();

		std::cout << "       Time needed for initialization of module: " << std::setprecision(3) << std::fixed << timeInit << " ms" << std::endl;

		// start timer for quality computation
		NFIQ::Timer timer;
		double time = 0.0;
		timer.startTimer();

		// compute quality now
		// call wrapper class with fingerprint image to get score
		// input is always raw image with set image parameters
		std::list<NFIQ::QualityFeatureData> featureVector;
		std::list<NFIQ::QualityFeatureSpeed> featureTimings;
		unsigned int qualityScore = nfiq2.computeQualityScore(
			rawImage, 
			true, actionableQuality, // always return actionable quality
			bOutputFeatureData, featureVector,
			bOutputSpeed, featureTimings);

		// get elapsed time
		time = timer.endTimerAndGetElapsedTime();

		if (bOutputFeatureData)
		{
			std::cout << std::endl << "NFIQ2: Computed quality feature values:" << std::endl;
			std::list<NFIQ::QualityFeatureData>::iterator it;
			for (it = featureVector.begin(); it != featureVector.end(); ++it)
				std::cout << "  " << it->featureID << ": " << std::setprecision(3) << it->featureDataDouble << std::endl;
		}

		if (bOutputSpeed)
		{
			std::cout << std::endl << "NFIQ2: Computed quality feature speed values:" << std::endl;
			std::list<NFIQ::QualityFeatureSpeed>::iterator it;
			for (it = featureTimings.begin(); it != featureTimings.end(); ++it)
			{
				std::cout << "  ";
				if (it->featureIDGroup != "")
					std::cout << it->featureIDGroup << " (";
				std::list<std::string>::iterator it_ids;
				unsigned int k = 0;
				for (it_ids = it->featureIDs.begin(); it_ids != it->featureIDs.end(); ++it_ids)
				{
					std::cout << *it_ids;
					if (k != (it->featureIDs.size() - 1))
						std::cout << ", ";
					k++;
				}
				if (it->featureIDGroup != "")
					std::cout << ")";
				std::cout << ": " << std::setprecision(3) << it->featureSpeed << " ms" << std::endl;
			}
		}

		std::cout << std::endl << "NFIQ2: Achieved quality score: " << qualityScore << std::endl;
		std::cout << "       Time needed for quality score computation: " << std::setprecision(3) << std::fixed << time << " ms" << std::endl;

		if (actionableQuality.size() > 0 && bOutputFeatureData)
		{
			std::list<NFIQ::ActionableQualityFeedback>::iterator it;
			for (it = actionableQuality.begin(); it != actionableQuality.end(); ++it)
			{
				std::cout << "       Actionable quality (" << it->identifier << "): " << it->actionableQualityValue << std::endl;
			}
		}
	}
	catch (NFIQException& ex)
	{
		// exceptions may occur e.g. if fingerprint image cannot be read or parsed
		std::cerr << "ERROR => Return code [" << ex.getReturnCode() << "]: " << ex.getErrorMessage() << std::endl;
		return -1;
	}
	return 0;
}

int executeRunModeBatch(std::string fpImageListPath, std::string imageFormat, std::string resultListPath,
						bool bOutputFeatureData, bool bOutputSpeed, std::string speedOutputPath)
{
	try
	{
		if (imageFormat.compare("BMP") != 0 && imageFormat.compare("WSQ") != 0)
		{
			std::cerr << "ERROR => Unknown image format specified" << std::endl;
			return -1;
		}

		std::cout << "NFIQ2: Compute quality score for fingerprint images in list " << fpImageListPath << std::endl;

		// read all filenames from input file list
		std::vector<std::string> vecLines = getFileContent(fpImageListPath);

		// create result file and header
		std::ofstream ofs(resultListPath.c_str());
		if (!ofs.is_open())
		{
			std::cerr << "ERROR => Cannot create output file " << resultListPath << std::endl;
			return -1;
		}

		// create speed file and header
		std::ofstream sfs;
		if (bOutputSpeed)
		{
			sfs.open(speedOutputPath.c_str());
			if (!sfs.is_open())
			{
				std::cerr << "ERROR => Cannot create speed output file " << speedOutputPath << std::endl;
				return -1;
			}
		}

		// start timer for initialization routine
		NFIQ::Timer timerInit;
		double timeInit = 0.0;
		timerInit.startTimer();

		// do initialization
		NFIQ::NFIQ2Algorithm nfiq2;
		timeInit = timerInit.endTimerAndGetElapsedTime();

		std::cout << "       Time needed for initialization of module: " << std::setprecision(3) << std::fixed << timeInit << " ms" << std::endl;


		// header with data that is always present
		ofs << "File name" << ";" << "NFIQ2 score";

		if (bOutputSpeed)
		{		
			// header for speed output file
			sfs << "File name" << ";" << "NFIQ2 computation speed";
		}

		std::cout << "       Running batch computation ..." << std::endl;

		for (unsigned int i = 0; i < vecLines.size(); i++)
		{
			// read fingerprint image
			NFIQ::FingerprintImageData rawImage;
			try
			{
				NFIQ::FingerprintImageData fpImage;
				fpImage.readFromFile(vecLines.at(i));
				if (imageFormat.compare("BMP") == 0)
					rawImage.fromBMP(fpImage);
				else if (imageFormat.compare("WSQ") == 0)
					rawImage.fromWSQ(fpImage);
			}
			catch (NFIQException)
			{
				// do not quit if exception occurs, e.g. file not found or not readable
				std::cout << "       " << vecLines.at(i) << ": NFIQ2 score = N/A" << std::endl;

				// log score N/A to file (all other values are ignored)
				if (i == 0)
					ofs << std::endl; // finish header
				ofs << vecLines.at(i) << ";";
				ofs << "N/A";
				ofs << std::endl;

				// speed loggings, only N/A for overall time
				if (bOutputSpeed)
				{
					if (i == 0)
						sfs << std::endl; // finish header
					sfs << vecLines.at(i) << ";";
					sfs << "N/A";
					sfs << std::endl;
				}

				continue;
 			}

			// start timer for quality computation
			NFIQ::Timer timer;
			double time = 0.0;
			timer.startTimer();
			
			// compute quality now
			// call wrapper class with fingerprint image to get score
			// input is always raw image with set image parameters
			std::list<NFIQ::ActionableQualityFeedback> actionableQuality;
			std::list<NFIQ::QualityFeatureData> featureVector;
			std::list<NFIQ::QualityFeatureSpeed> featureTimings;
			unsigned int qualityScore = nfiq2.computeQualityScore(
				rawImage, 
				true, actionableQuality, 
				bOutputFeatureData, featureVector,
				bOutputSpeed, featureTimings);

			// get elapsed time
			time = timer.endTimerAndGetElapsedTime();

			std::cout << "       " << vecLines.at(i) << ": NFIQ2 score = " << qualityScore << std::endl;

			// finish header
			if (i == 0)
			{
				// actionable quality feedback values, add to header
				std::list<NFIQ::ActionableQualityFeedback>::iterator it_aq;
				for (it_aq = actionableQuality.begin(); it_aq != actionableQuality.end(); ++it_aq)
				{
					ofs << ";" << it_aq->identifier;
				}

				if (bOutputFeatureData)
				{
					// feature IDs need to be written to output now
					std::list<NFIQ::QualityFeatureData>::iterator it;
					for (it = featureVector.begin(); it != featureVector.end(); ++it)
					{
						ofs << ";" << it->featureID;
					}
				}

				ofs << std::endl; // header is finished

				// finish header for speed output file
				if (bOutputSpeed)
				{
					// feature IDs need to be written to output now
					std::list<NFIQ::QualityFeatureSpeed>::iterator it;
					for (it = featureTimings.begin(); it != featureTimings.end(); ++it)
					{
						sfs << ";" << it->featureIDGroup;
					}

					sfs << std::endl;
				}
			}

			// log filename and NFIQ2 score
			ofs << vecLines.at(i) << ";";
			ofs << std::setprecision(5) << qualityScore;

			// log filename and NFIQ2 computation time
			if (bOutputSpeed)
			{
				sfs << vecLines.at(i) << ";";
				sfs << std::setprecision(3) << time;
			}
			
			// log actionable quality feedback
			std::list<NFIQ::ActionableQualityFeedback>::iterator it_aq;
			for (it_aq = actionableQuality.begin(); it_aq != actionableQuality.end(); ++it_aq)
			{
				ofs << ";" << std::setprecision(5) << it_aq->actionableQualityValue;
			}

			if (bOutputFeatureData)
			{
				// log quality feature data
				std::list<NFIQ::QualityFeatureData>::iterator it;
				for (it = featureVector.begin(); it != featureVector.end(); ++it)
				{
					ofs << ";" << std::setprecision(5) << it->featureDataDouble;
				}
			}

			if (bOutputSpeed)
			{
				std::list<NFIQ::QualityFeatureSpeed>::iterator it;
				for (it = featureTimings.begin(); it != featureTimings.end(); ++it)
				{
					sfs << ";" << std::setprecision(3) << it->featureSpeed;
				}
			}

			ofs << std::endl;
			if (bOutputSpeed)
				sfs << std::endl;
		}

		std::cout << "       Batch computation done" << std::endl;

		ofs.close();
		if (bOutputSpeed)
			sfs.close();
	}
	catch (NFIQException& ex)
	{
		// exceptions may occur e.g. if fingerprint image cannot be read or parsed
		std::cerr << "ERROR => Return code [" << ex.getReturnCode() << "]: " << ex.getErrorMessage() << std::endl;
		return -1;
	}

	return 0;
}

int main(int argc, const char* argv[])
{
	try
	{
		// parse input arguments
		if (argc < 6)
		{
			printUsage();
			return -1;
		}

		// get parameters
		std::string runMode = std::string(argv[1]);
		if (runMode == "SINGLE")
		{
			if (argc != 6)
			{
				printUsage();
				return -1;
			}
			std::string fpImagePath = std::string(argv[2]);
			std::string imageFormat = std::string(argv[3]);
			bool bOutputFeatureData = (std::string(argv[4]).compare("true") == 0 ? true : false);
			bool bOutputSpeed = (std::string(argv[5]).compare("true") == 0 ? true : false);

			return executeRunModeSingle(fpImagePath, imageFormat, bOutputFeatureData, bOutputSpeed);
		}
		else if (runMode == "BATCH")
		{
			if (argc != 7 && argc != 8)
			{
				printUsage();
				return -1;
			}

			std::string fpImageListPath = std::string(argv[2]);
			std::string imageFormat = std::string(argv[3]);
			std::string resultListPath = std::string(argv[4]);
			bool bOutputFeatureData = (std::string(argv[5]).compare("true") == 0 ? true : false);
			bool bOutputSpeed = (std::string(argv[6]).compare("true") == 0 ? true : false);
			std::string speedOutputPath = "";
			if (bOutputSpeed && argc != 8)
			{
				std::cerr << "ERROR => Missing output file for speed values" << std::endl;
				printUsage();
				return -1;
			}
			else if (bOutputSpeed)
			{
				speedOutputPath = std::string(argv[7]);
			}

			return executeRunModeBatch(fpImageListPath, imageFormat, resultListPath,
				bOutputFeatureData, bOutputSpeed, speedOutputPath);			
		}
		else
		{
			std::cerr << "ERROR => Wrong run mode entered" << std::endl;
			printUsage();
			return -1;
		}
	}
	catch (...)
	{
		std::cerr << "ERROR => Unknown error occurred" << std::endl;
		return -1;
	}
		
	return 0;
}
