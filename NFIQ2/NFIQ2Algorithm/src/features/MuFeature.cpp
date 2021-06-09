#include <features/MuFeature.h>
#include <nfiq2_exception.hpp>
#include <nfiq2_timer.hpp>
#include <opencv2/core.hpp>

#include <sstream>

const char NFIQ2::Identifiers::QualityModules::Contrast[] { "Contrast" };
const char NFIQ2::Identifiers::QualityFeatures::Contrast::Mean[] { "Mu" };
const char NFIQ2::Identifiers::QualityFeatures::Contrast::MeanBlock[] { "MMB" };

NFIQ2::QualityFeatures::MuFeature::MuFeature(
    const NFIQ2::FingerprintImageData &fingerprintImage)
{
	this->setFeatures(computeFeatureData(fingerprintImage));
}

NFIQ2::QualityFeatures::MuFeature::~MuFeature() = default;

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::MuFeature::computeFeatureData(
    const NFIQ2::FingerprintImageData &fingerprintImage)
{
	std::unordered_map<std::string, double> featureDataList;

	// check if input image has 500 dpi
	if (fingerprintImage.ppi !=
	    NFIQ2::FingerprintImageData::Resolution500PPI) {
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FeatureCalculationError,
		    "Only 500 dpi fingerprint images are supported!");
	}

	cv::Mat img;
	try {
		// get matrix from fingerprint image
		img = cv::Mat(fingerprintImage.height, fingerprintImage.width,
		    CV_8UC1, (void *)fingerprintImage.data());
	} catch (const cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot get matrix from fingerprint image: "
		      << e.what();
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FeatureCalculationError, ssErr.str());
	}

	NFIQ2::Timer timer;
	timer.start();

	// -------------------------
	// compute Mu Mu Block (MMB)
	// -------------------------

	try {
		unsigned int blockSize = 32;
		unsigned int width = fingerprintImage.width;
		unsigned int height = fingerprintImage.height;
		std::vector<double> vecMeans;

		// calculate blockwise mean values
		for (unsigned int i = 0; i < height; i += blockSize) {
			for (unsigned int j = 0; j < width; j += blockSize) {
				unsigned int takenBS_X = blockSize;
				unsigned int takenBS_Y = blockSize;
				if ((width - j) < blockSize) {
					takenBS_X = (width - j);
				}
				if ((height - i) < blockSize) {
					takenBS_Y = (height - i);
				}

				// create block and calculate mean of greyscale
				// values
				cv::Mat block = img(
				    cv::Rect(j, i, takenBS_X, takenBS_Y));
				cv::Scalar m = mean(block);
				vecMeans.push_back(m.val[0]);
			}
		}

		// calculate arithmetic mean of all block mean values
		double avg = 0.0;
		double count = (double)vecMeans.size();
		for (unsigned int i = 0; i < vecMeans.size(); i++) {
			avg += (vecMeans.at(i) / count);
		}

		// return MMB value
		std::pair<std::string, double> fd_mmb;
		fd_mmb = std::make_pair(
		    Identifiers::QualityFeatures::Contrast::MeanBlock, avg);

		featureDataList[fd_mmb.first] = fd_mmb.second;
	} catch (const cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot compute feature Mu Mu Block (MMB): "
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

	// -----------------------------------------
	// compute Mu and Standard Deviation = Sigma
	// -----------------------------------------

	cv::Scalar stddev;
	cv::Scalar mu;
	try {
		// calculate stddev of input image = sigma and mu = mean
		cv::meanStdDev(img, mu, stddev);
		// assign sigma value
		this->sigma = stddev.val[0];
		this->sigmaComputed = true;

		// return mu value
		std::pair<std::string, double> fd_mu;
		fd_mu = std::make_pair(
		    Identifiers::QualityFeatures::Contrast::Mean, mu.val[0]);

		featureDataList[fd_mu.first] = fd_mu.second;
	} catch (const cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot compute feature Sigma (stddev) and Mu (mean): "
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

	this->setSpeed(timer.stop());

	return featureDataList;
}

double
NFIQ2::QualityFeatures::MuFeature::getSigma() const
{
	if (!this->sigmaComputed)
		throw NFIQ2::Exception { NFIQ2::ErrorCode::NoDataAvailable,
			"Sigma has not been computed." };

	return (this->sigma);
}

std::string
NFIQ2::QualityFeatures::MuFeature::getModuleName() const
{
	return NFIQ2::Identifiers::QualityModules::Contrast;
}

std::vector<std::string>
NFIQ2::QualityFeatures::MuFeature::getQualityFeatureIDs()
{
	std::vector<std::string> featureIDs;
	featureIDs.push_back(Identifiers::QualityFeatures::Contrast::MeanBlock);
	featureIDs.push_back(Identifiers::QualityFeatures::Contrast::Mean);
	return featureIDs;
}
