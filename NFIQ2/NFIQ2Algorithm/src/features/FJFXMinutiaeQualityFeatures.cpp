#include <features/FJFXMinutiaeQualityFeatures.h>
#include <features/OCLHistogramFeature.h>
#include <nfiq2_nfiqexception.hpp>
#include <nfiq2_timer.hpp>

#include <sstream>

using namespace NFIQ;
using namespace cv;

NFIQ::QualityFeatures::FJFXMinutiaeQualityFeature::FJFXMinutiaeQualityFeature(
    const std::vector<FingerJetFXFeature::Minutia> &minutiaData,
    const bool templateCouldBeExtracted)
    : minutiaData_ { minutiaData }
    , templateCouldBeExtracted_ { templateCouldBeExtracted } {};

NFIQ::QualityFeatures::FJFXMinutiaeQualityFeature::
    ~FJFXMinutiaeQualityFeature() = default;

const std::string
    NFIQ::QualityFeatures::FJFXMinutiaeQualityFeature::speedFeatureIDGroup =
	"Minutiae quality";

std::vector<NFIQ::QualityFeatures::FingerJetFXFeature::Minutia>
NFIQ::QualityFeatures::FJFXMinutiaeQualityFeature::getMinutiaData() const
{
	if (!this->templateCouldBeExtracted_) {
		// This should never be reached but is here for consistency
		throw NFIQ::NFIQException { e_Error_NoDataAvailable,
			"Template could not be extracted." };
	}

	return (this->minutiaData_);
}

bool
NFIQ::QualityFeatures::FJFXMinutiaeQualityFeature::getTemplateStatus() const
{
	return (this->templateCouldBeExtracted_);
}

std::vector<NFIQ::QualityFeatureResult>
NFIQ::QualityFeatures::FJFXMinutiaeQualityFeature::computeFeatureData(
    const NFIQ::FingerprintImageData &fingerprintImage)
{
	std::vector<NFIQ::QualityFeatureResult> featureDataList;

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

	if (!this->templateCouldBeExtracted_) {
		res_mu.featureData.featureDataDouble = -1;
		res_mu.returnCode = 0;
		featureDataList.push_back(res_mu);

		res_ocl.featureData.featureDataDouble = -1;
		res_ocl.returnCode = 0;
		featureDataList.push_back(res_ocl);

		// Speed
		NFIQ::QualityFeatureSpeed speed;
		speed.featureIDGroup =
		    FJFXMinutiaeQualityFeature::speedFeatureIDGroup;
		speed.featureIDs.push_back("FJFXPos_Mu_MinutiaeQuality_2");
		speed.featureIDs.push_back("FJFXPos_OCL_MinutiaeQuality_80");
		speed.featureSpeed = 0;
		this->setSpeed(speed);

		return featureDataList;
	}

	try {
		NFIQ::Timer timer;
		timer.startTimer();

		// compute minutiae quality based on Mu feature computated at
		// minutiae positions
		std::vector<MinutiaData> vecMuMinQualityData =
		    computeMuMinQuality(32, fingerprintImage);

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
		    (double)this->minutiaData_.size();
		featureDataList.push_back(res_mu);

		// compute minutiae quality based on OCL feature computed at
		// minutiae positions
		std::vector<MinutiaData> vecOCLMinQualityData =
		    computeOCLMinQuality(BS_OCL, fingerprintImage);

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
		    (double)this->minutiaData_.size();
		featureDataList.push_back(res_ocl);

		// Speed
		NFIQ::QualityFeatureSpeed speed;
		speed.featureIDGroup =
		    FJFXMinutiaeQualityFeature::speedFeatureIDGroup;
		speed.featureIDs.push_back("FJFXPos_Mu_MinutiaeQuality_2");
		speed.featureIDs.push_back("FJFXPos_OCL_MinutiaeQuality_80");
		speed.featureSpeed = timer.endTimerAndGetElapsedTime();
		this->setSpeed(speed);

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
}

const std::string
    NFIQ::QualityFeatures::FJFXMinutiaeQualityFeature::moduleName {
	    "NFIQ2_FJFXPos_MinutiaeQuality"
    };
std::string
NFIQ::QualityFeatures::FJFXMinutiaeQualityFeature::getModuleName() const
{
	return moduleName;
}

std::vector<std::string>
NFIQ::QualityFeatures::FJFXMinutiaeQualityFeature::getAllFeatureIDs()
{
	std::vector<std::string> featureIDs;
	featureIDs.push_back("FJFXPos_Mu_MinutiaeQuality_2");
	featureIDs.push_back("FJFXPos_OCL_MinutiaeQuality_80");
	return featureIDs;
}

std::vector<NFIQ::QualityFeatures::FJFXMinutiaeQualityFeature::MinutiaData>
NFIQ::QualityFeatures::FJFXMinutiaeQualityFeature::computeMuMinQuality(
    int bs, const NFIQ::FingerprintImageData &fingerprintImage)
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
	for (const auto &minutia : this->minutiaData_) {
		MinutiaData minData;
		minData.x = static_cast<int>(minutia.x);
		minData.y = static_cast<int>(minutia.y);

		int leftX = (minData.x - (bs / 2));
		if (leftX < 0) {
			leftX = 0;
		}

		int topY = (minData.y - (bs / 2));
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

std::vector<NFIQ::QualityFeatures::FJFXMinutiaeQualityFeature::MinutiaData>
NFIQ::QualityFeatures::FJFXMinutiaeQualityFeature::computeOCLMinQuality(
    int bs, const NFIQ::FingerprintImageData &fingerprintImage)
{
	std::vector<MinutiaData> vecMinData;

	// get matrix from fingerprint image
	Mat img = Mat(fingerprintImage.m_ImageHeight,
	    fingerprintImage.m_ImageWidth, CV_8UC1,
	    (void *)fingerprintImage.data());

	// iterate through all minutiae positions and
	// compute own minutiae quality values
	// based on OCL value computation around FJFX minutiae location
	for (const auto &minutia : this->minutiaData_) {
		MinutiaData minData;
		minData.x = static_cast<int>(minutia.x);
		minData.y = static_cast<int>(minutia.y);

		int leftX = (minData.x - (bs / 2));
		if (leftX < 0) {
			leftX = 0;
		}

		int topY = (minData.y - (bs / 2));
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
