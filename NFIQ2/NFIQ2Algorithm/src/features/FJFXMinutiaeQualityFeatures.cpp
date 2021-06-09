#include <features/FJFXMinutiaeQualityFeatures.h>
#include <features/OCLHistogramFeature.h>
#include <nfiq2_exception.hpp>
#include <nfiq2_timer.hpp>

#include <sstream>

const char NFIQ2::Identifiers::QualityModules::MinutiaeQuality[] {
	"MinutiaeQuality"
};
const char NFIQ2::Identifiers::QualityFeatures::Minutiae::QualityMu2[] {
	"FJFXPos_Mu_MinutiaeQuality_2"
};
const char NFIQ2::Identifiers::QualityFeatures::Minutiae::QualityOCL80[] {
	"FJFXPos_OCL_MinutiaeQuality_80"
};

NFIQ2::QualityFeatures::FJFXMinutiaeQualityFeature::FJFXMinutiaeQualityFeature(
    const NFIQ2::FingerprintImageData &fingerprintImage,
    const std::vector<FingerJetFXFeature::Minutia> &minutiaData)
    : minutiaData_ { minutiaData }
{
	this->setFeatures(computeFeatureData(fingerprintImage));
};

NFIQ2::QualityFeatures::FJFXMinutiaeQualityFeature::
    ~FJFXMinutiaeQualityFeature() = default;

std::vector<NFIQ2::QualityFeatures::FingerJetFXFeature::Minutia>
NFIQ2::QualityFeatures::FJFXMinutiaeQualityFeature::getMinutiaData() const
{
	return (this->minutiaData_);
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::FJFXMinutiaeQualityFeature::computeFeatureData(
    const NFIQ2::FingerprintImageData &fingerprintImage)
{
	std::unordered_map<std::string, double> featureDataList;

	std::pair<std::string, double> fd_mu;
	fd_mu = std::make_pair(
	    Identifiers::QualityFeatures::Minutiae::QualityMu2, -1);

	std::pair<std::string, double> fd_ocl;
	fd_ocl = std::make_pair(
	    Identifiers::QualityFeatures::Minutiae::QualityOCL80, -1);

	try {
		NFIQ2::Timer timer;
		timer.start();

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
		// return relative value in relation to minutiae count
		fd_mu.second = (double)vecRanges.at(2) /
		    (double)this->minutiaData_.size();
		featureDataList[fd_mu.first] = fd_mu.second;

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

		// return relative value in relation to minutiae count
		fd_ocl.second = (double)vecRangesOCL.at(4) /
		    (double)this->minutiaData_.size();
		featureDataList[fd_ocl.first] = fd_ocl.second;

		this->setSpeed(timer.stop());
	} catch (const cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot compute FJFX based minutiae quality features: "
		      << e.what();
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FeatureCalculationError, ssErr.str());
	} catch (const NFIQ2::Exception &) {
		throw;
	} catch (...) {
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FeatureCalculationError,
		    "Unknown exception occurred!");
	}

	return featureDataList;
}

std::string
NFIQ2::QualityFeatures::FJFXMinutiaeQualityFeature::getModuleName() const
{
	return NFIQ2::Identifiers::QualityModules::MinutiaeQuality;
}

std::vector<std::string>
NFIQ2::QualityFeatures::FJFXMinutiaeQualityFeature::getQualityFeatureIDs()
{
	return { Identifiers::QualityFeatures::Minutiae::QualityMu2,
		Identifiers::QualityFeatures::Minutiae::QualityOCL80 };
}

std::vector<NFIQ2::QualityFeatures::FJFXMinutiaeQualityFeature::MinutiaData>
NFIQ2::QualityFeatures::FJFXMinutiaeQualityFeature::computeMuMinQuality(
    int bs, const NFIQ2::FingerprintImageData &fingerprintImage)
{
	std::vector<MinutiaData> vecMinData;

	// get matrix from fingerprint image
	cv::Mat img = cv::Mat(fingerprintImage.height, fingerprintImage.width,
	    CV_8UC1, (void *)fingerprintImage.data());

	// compute overall mean and stddev
	cv::Scalar me;
	cv::Scalar stddev;
	cv::meanStdDev(img, me, stddev);

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
		if ((leftX + bs) > (int)fingerprintImage.width) {
			takenBS_X = (fingerprintImage.width - leftX);
		}
		if ((topY + bs) > (int)fingerprintImage.height) {
			takenBS_Y = (fingerprintImage.height - topY);
		}

		cv::Mat block = img(
		    cv::Rect(leftX, topY, takenBS_X, takenBS_Y));
		cv::Scalar m = mean(block);
		// use normalization of mean and stddev of overall image
		minData.quality = ((me.val[0] - m.val[0]) / stddev.val[0]);

		vecMinData.push_back(minData);
	}

	return vecMinData;
}

std::vector<NFIQ2::QualityFeatures::FJFXMinutiaeQualityFeature::MinutiaData>
NFIQ2::QualityFeatures::FJFXMinutiaeQualityFeature::computeOCLMinQuality(
    int bs, const NFIQ2::FingerprintImageData &fingerprintImage)
{
	std::vector<MinutiaData> vecMinData;

	// get matrix from fingerprint image
	cv::Mat img = cv::Mat(fingerprintImage.height, fingerprintImage.width,
	    CV_8UC1, (void *)fingerprintImage.data());

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
		if ((leftX + bs) > (int)fingerprintImage.width) {
			leftX = (fingerprintImage.width - bs);
		}
		if ((topY + bs) > (int)fingerprintImage.height) {
			topY = (fingerprintImage.height - bs);
		}

		cv::Mat block = img(cv::Rect(leftX, topY, bs, bs));

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
