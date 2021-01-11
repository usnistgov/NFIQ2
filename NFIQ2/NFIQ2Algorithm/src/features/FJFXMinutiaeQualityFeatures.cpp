#include "FJFXMinutiaeQualityFeatures.h"
#include "OCLHistogramFeature.h"
#include "include/nfiq2/NFIQException.h"

#include "include/Timer.hpp"
#include <sstream>

#define CBEFF (0x00330502)

using namespace NFIQ;
using namespace cv;

FJFXMinutiaeQualityFeature::~FJFXMinutiaeQualityFeature()
{
}

const std::string FJFXMinutiaeQualityFeature::speedFeatureIDGroup =
    "Minutiae quality";

std::list<NFIQ::QualityFeatureResult>
FJFXMinutiaeQualityFeature::computeFeatureData(
    const NFIQ::FingerprintImageData &fingerprintImage,
    unsigned char templateData[], size_t &templateSize,
    bool &templateCouldBeExtracted)
{
#ifndef WITHOUT_BIOMDI_SUPPORT

	std::list<NFIQ::QualityFeatureResult> featureDataList;

	std::vector<NFIQ::QualityFeatureResult> vecResultMuMinQuality;
	NFIQ::QualityFeatureData fd_mu;
	fd_mu.featureID = "FJFXPos_Mu_MinutiaeQuality_2";
	fd_mu.featureDataType = NFIQ::e_QualityFeatureDataTypeDouble;
	fd_mu.featureDataDouble = -1;
	NFIQ::QualityFeatureResult res_mu;
	res_mu.featureData = fd_mu;
	res_mu.returnCode = 0;

	NFIQ::QualityFeatureData fd_ocl;
	fd_ocl.featureID = "FJFXPos_OCL_MinutiaeQuality_80";
	fd_ocl.featureDataType = NFIQ::e_QualityFeatureDataTypeDouble;
	fd_ocl.featureDataDouble = -1;
	NFIQ::QualityFeatureResult res_ocl;
	res_ocl.featureData = fd_ocl;
	res_ocl.returnCode = 0;

	if (!templateCouldBeExtracted) {
		res_mu.featureData.featureDataDouble = -1;
		res_mu.returnCode = 0;
		featureDataList.push_back(res_mu);

		res_ocl.featureData.featureDataDouble = -1;
		res_ocl.returnCode = 0;
		featureDataList.push_back(res_ocl);

		if (m_bOutputSpeed) {
			NFIQ::QualityFeatureSpeed speed;
			speed.featureIDGroup =
			    FJFXMinutiaeQualityFeature::speedFeatureIDGroup;
			speed.featureIDs.push_back(
			    "FJFXPos_Mu_MinutiaeQuality_2");
			speed.featureIDs.push_back(
			    "FJFXPos_OCL_MinutiaeQuality_80");
			speed.featureSpeed = 0;
			m_lSpeedValues.push_back(speed);
		}
		return featureDataList;
	}

	try {
		NFIQ::Timer timer;
		timer.startTimer();

		// parse ISO template
		// validity check
		if (templateSize < 28) {
			throw NFIQ::NFIQException(
			    NFIQ::e_Error_FeatureCalculationError,
			    "Created ISO template is too small.");
		}

		struct finger_minutiae_record *fmr;
		if (new_fmr(FMR_STD_ISO, &fmr) < 0) {
			throw NFIQ::NFIQException(
			    NFIQ::e_Error_FeatureCalculationError,
			    "Could not allocate FMR.");
		}

		BDB bdb;
		INIT_BDB(&bdb, templateData, templateSize);
		if (scan_fmr(&bdb, fmr) != READ_OK) {
			if (fmr != NULL) {
				free_fmr(fmr);
			}
			throw NFIQ::NFIQException(
			    NFIQ::e_Error_FeatureCalculationError,
			    "Could not init BDB.");
		}

		// Get all of the minutiae records
		int rcount = get_fvmr_count(fmr);
		if (rcount == 0) {
			if (fmr != NULL) {
				free_fmr(fmr);
			}
			throw NFIQ::NFIQException(
			    NFIQ::e_Error_FeatureCalculationError,
			    "No minutiae records found.");
		}
		struct finger_view_minutiae_record **fvmrs = NULL;
		fvmrs = (struct finger_view_minutiae_record **)malloc(
		    rcount * sizeof(struct finger_view_minutiae_record **));
		if (get_fvmrs(fmr, fvmrs) != rcount) {
			if (fvmrs) {
				free(fvmrs);
			}
			if (fmr != NULL) {
				free_fmr(fmr);
			}
			throw NFIQ::NFIQException(
			    NFIQ::e_Error_FeatureCalculationError,
			    "Cannot get minutiae records.");
		}

		// get number of minutiae
		int minCnt = get_fmd_count(fvmrs[0]);
		if (minCnt == 0) {
			if (fvmrs) {
				free(fvmrs);
			}
			if (fmr != NULL) {
				free_fmr(fmr);
			}

			res_mu.featureData.featureDataDouble = -1;
			res_mu.returnCode = 0;
			featureDataList.push_back(res_mu);

			res_ocl.featureData.featureDataDouble = -1;
			res_ocl.returnCode = 0;
			featureDataList.push_back(res_ocl);

			if (m_bOutputSpeed) {
				NFIQ::QualityFeatureSpeed speed;
				speed.featureIDGroup =
				    FJFXMinutiaeQualityFeature::
					speedFeatureIDGroup;
				speed.featureIDs.push_back(
				    "FJFXPos_Mu_MinutiaeQuality_2");
				speed.featureIDs.push_back(
				    "FJFXPos_OCL_MinutiaeQuality_80");
				speed.featureSpeed =
				    timer.endTimerAndGetElapsedTime();
				m_lSpeedValues.push_back(speed);
			}
			return featureDataList;
		}

		struct finger_minutiae_data **fmds;
		fmds = (struct finger_minutiae_data **)malloc(
		    minCnt * sizeof(struct finger_minutiae_data **));

		if (get_fmds(fvmrs[0], fmds) != minCnt) {
			if (fmds) {
				free(fmds);
			}
			if (fvmrs) {
				free(fvmrs);
			}
			if (fmr != NULL) {
				free_fmr(fmr);
			}
			throw NFIQ::NFIQException(
			    NFIQ::e_Error_FeatureCalculationError,
			    "Cannot get minutiae data.");
		}

		// compute minutiae quality based on Mu feature computated at
		// minutiae positions
		std::vector<MinutiaData> vecMuMinQualityData =
		    computeMuMinQuality(fmds, minCnt, 32, fingerprintImage);

		std::vector<unsigned int> vecRanges(
		    4); // index 0 = -1 .. -0.5, ....
		for (unsigned int i = 0; i < 4; i++) {
			vecRanges.at(i) = 0;
		}

		for (unsigned int i = 0; i < vecMuMinQualityData.size(); i++) {
			if (vecMuMinQualityData.at(i).quality <= -0.5) {
				vecRanges.at(0)++;
			} else if (vecMuMinQualityData.at(i).quality > -0.5 &&
			    vecMuMinQualityData.at(i).quality <= 0.0) {
				vecRanges.at(1)++;
			} else if (vecMuMinQualityData.at(i).quality > 0.0 &&
			    vecMuMinQualityData.at(i).quality <= 0.5) {
				vecRanges.at(2)++;
			} else if (vecMuMinQualityData.at(i).quality > 0.5) {
				vecRanges.at(3)++;
			}
		}

		// return mu_2 quality value
		res_mu.returnCode = 0;
		// return relative value in relation to minutiae count
		res_mu.featureData.featureDataDouble = (double)vecRanges.at(2) /
		    (double)minCnt;
		featureDataList.push_back(res_mu);

		// compute minutiae quality based on OCL feature computed at
		// minutiae positions
		std::vector<MinutiaData> vecOCLMinQualityData =
		    computeOCLMinQuality(
			fmds, minCnt, BS_OCL, fingerprintImage);

		std::vector<unsigned int> vecRangesOCL(
		    5); // index 0 = 0-20, 1 = 20-40, ..., 5 = 80-100
		for (unsigned int i = 0; i < 5; i++) {
			vecRangesOCL.at(i) = 0;
		}

		for (unsigned int i = 0; i < vecOCLMinQualityData.size(); i++) {
			if (vecOCLMinQualityData.at(i).quality <= 20) {
				vecRangesOCL.at(0)++;
			} else if (vecOCLMinQualityData.at(i).quality > 20 &&
			    vecOCLMinQualityData.at(i).quality <= 40) {
				vecRangesOCL.at(1)++;
			} else if (vecOCLMinQualityData.at(i).quality > 40 &&
			    vecOCLMinQualityData.at(i).quality <= 60) {
				vecRangesOCL.at(2)++;
			} else if (vecOCLMinQualityData.at(i).quality > 60 &&
			    vecOCLMinQualityData.at(i).quality <= 80) {
				vecRangesOCL.at(3)++;
			} else if (vecOCLMinQualityData.at(i).quality > 80) {
				vecRangesOCL.at(4)++;
			}
		}

		res_ocl.returnCode = 0;
		// return relative value in relation to minutiae count
		res_ocl.featureData.featureDataDouble = (double)vecRangesOCL.at(
							    4) /
		    (double)minCnt;
		featureDataList.push_back(res_ocl);

		if (fmds) {
			free(fmds);
		}

		if (fvmrs) {
			free(fvmrs);
		}

		if (fmr != NULL) {
			free_fmr(fmr);
		}

		if (m_bOutputSpeed) {
			NFIQ::QualityFeatureSpeed speed;
			speed.featureIDGroup =
			    FJFXMinutiaeQualityFeature::speedFeatureIDGroup;
			speed.featureIDs.push_back(
			    "FJFXPos_Mu_MinutiaeQuality_2");
			speed.featureIDs.push_back(
			    "FJFXPos_OCL_MinutiaeQuality_80");
			speed.featureSpeed = timer.endTimerAndGetElapsedTime();
			m_lSpeedValues.push_back(speed);
		}
	} catch (cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot compute FJFX based minutiae quality features: "
		      << e.what();
		throw NFIQ::NFIQException(
		    NFIQ::e_Error_FeatureCalculationError, ssErr.str());
	} catch (NFIQ::NFIQException &e) {
		throw e;
	} catch (...) {
		throw NFIQ::NFIQException(NFIQ::e_Error_FeatureCalculationError,
		    "Unknown exception occurred!");
	}

	return featureDataList;
#else
	throw NFIQ::NFIQException(NFIQ::e_Error_FunctionNotImplemented,
	    "libbiomdi support is not enabled. FJFX minutiae "
	    "quality feature computation is not possible.");
#endif
}

std::string
FJFXMinutiaeQualityFeature::getModuleID()
{
	return "NFIQ2_FJFXPos_MinutiaeQuality";
}

std::list<std::string>
FJFXMinutiaeQualityFeature::getAllFeatureIDs()
{
#ifndef WITHOUT_BIOMDI_SUPPORT
	std::list<std::string> featureIDs;
	featureIDs.push_back("FJFXPos_Mu_MinutiaeQuality_2");
	featureIDs.push_back("FJFXPos_OCL_MinutiaeQuality_80");
	return featureIDs;
#else
	// return empty feature list
	std::list<std::string> featureIDs;
	return featureIDs;
#endif
}

#ifndef WITHOUT_BIOMDI_SUPPORT

std::vector<FJFXMinutiaeQualityFeature::MinutiaData>
FJFXMinutiaeQualityFeature::computeMuMinQuality(
    struct finger_minutiae_data **fmds, unsigned int minCount, int bs,
    const NFIQ::FingerprintImageData &fingerprintImage)
{
	std::vector<MinutiaData> vecMinData;

	// get matrix from fingerprint image
	Mat img = Mat(fingerprintImage.m_ImageHeight,
	    fingerprintImage.m_ImageWidth, CV_8UC1,
	    (void *)fingerprintImage.data());

	// compute overall mean and stddev
	Scalar me;
	Scalar stddev;
	meanStdDev(img, me, stddev);

	// iterate through all minutiae positions and
	// compute own minutiae quality values
	// based on block-wise Mu computation around FJFX minutiae location
	for (unsigned int i = 0; i < minCount; i++) {
		MinutiaData minData;
		int x = (int)fmds[i]->x_coord;
		int y = (int)fmds[i]->y_coord;
		minData.x = x;
		minData.y = y;

		int leftX = (x - (bs / 2));
		if (leftX < 0) {
			leftX = 0;
		}

		int topY = (y - (bs / 2));
		if (topY < 0) {
			topY = 0;
		}

		unsigned int takenBS_X = bs;
		unsigned int takenBS_Y = bs;
		if ((leftX + bs) > (int)fingerprintImage.m_ImageWidth) {
			takenBS_X = (fingerprintImage.m_ImageWidth - leftX);
		}
		if ((topY + bs) > (int)fingerprintImage.m_ImageHeight) {
			takenBS_Y = (fingerprintImage.m_ImageHeight - topY);
		}

		Mat block = img(Rect(leftX, topY, takenBS_X, takenBS_Y));
		Scalar m = mean(block);
		// use normalization of mean and stddev of overall image
		minData.quality = ((me.val[0] - m.val[0]) / stddev.val[0]);

		vecMinData.push_back(minData);
	}

	return vecMinData;
}

std::vector<FJFXMinutiaeQualityFeature::MinutiaData>
FJFXMinutiaeQualityFeature::computeOCLMinQuality(
    struct finger_minutiae_data **fmds, unsigned int minCount, int bs,
    const NFIQ::FingerprintImageData &fingerprintImage)
{
	std::vector<MinutiaData> vecMinData;

	// get matrix from fingerprint image
	Mat img = Mat(fingerprintImage.m_ImageHeight,
	    fingerprintImage.m_ImageWidth, CV_8UC1,
	    (void *)fingerprintImage.data());

	// iterate through all minutiae positions and
	// compute own minutiae quality values
	// based on OCL value computation around FJFX minutiae location
	for (unsigned int i = 0; i < minCount; i++) {
		MinutiaData minData;
		int x = (int)fmds[i]->x_coord;
		int y = (int)fmds[i]->y_coord;
		minData.x = x;
		minData.y = y;

		int leftX = (x - (bs / 2));
		if (leftX < 0) {
			leftX = 0;
		}

		int topY = (y - (bs / 2));
		if (topY < 0) {
			topY = 0;
		}

		// always take full blocks centered around minutiae location
		// if in edge reason -> don't center around minutiae but take
		// full block that is closest
		if ((leftX + bs) > (int)fingerprintImage.m_ImageWidth) {
			leftX = (fingerprintImage.m_ImageWidth - bs);
		}
		if ((topY + bs) > (int)fingerprintImage.m_ImageHeight) {
			topY = (fingerprintImage.m_ImageHeight - bs);
		}

		Mat block = img(Rect(leftX, topY, bs, bs));

		// get OCL value of block
		// ignore return value as if false is returned OCL value is 0
		// anyway
		double ocl = 0.0;
		OCLHistogramFeature::getOCLValueOfBlock(block, ocl);

		// assign minutiae quality value
		// in range 0 (worst) - 100 (best)
		minData.quality = (int)((ocl * 100) + 0.5);

		vecMinData.push_back(minData);
	}

	return vecMinData;
}

#endif
