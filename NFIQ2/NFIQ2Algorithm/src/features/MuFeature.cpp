#include <features/MuFeature.h>
#include <nfiq2_nfiqexception.hpp>
#include <nfiq2_timer.hpp>

#include <sstream>

using namespace NFIQ;
using namespace cv;

NFIQ::QualityFeatures::MuFeature::MuFeature(
    const NFIQ::FingerprintImageData &fingerprintImage)
{
	this->setFeatures(computeFeatureData(fingerprintImage));
}

NFIQ::QualityFeatures::MuFeature::~MuFeature() = default;

const std::string NFIQ::QualityFeatures::MuFeature::speedFeatureIDGroup =
    "Contrast";

std::vector<NFIQ::QualityFeatureResult>
NFIQ::QualityFeatures::MuFeature::computeFeatureData(
    const NFIQ::FingerprintImageData &fingerprintImage)
{
	std::vector<NFIQ::QualityFeatureResult> featureDataList;

	// check if input image has 500 dpi
	if (fingerprintImage.m_ImageDPI != NFIQ::e_ImageResolution_500dpi) {
		throw NFIQ::NFIQException(NFIQ::e_Error_FeatureCalculationError,
		    "Only 500 dpi fingerprint images are supported!");
	}

	Mat img;
	try {
		// get matrix from fingerprint image
		img = Mat(fingerprintImage.m_ImageHeight,
		    fingerprintImage.m_ImageWidth, CV_8UC1,
		    (void *)fingerprintImage.data());
	} catch (cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot get matrix from fingerprint image: "
		      << e.what();
		throw NFIQ::NFIQException(
		    NFIQ::e_Error_FeatureCalculationError, ssErr.str());
	}

	NFIQ::Timer timer;
	timer.startTimer();

	// -------------------------
	// compute Mu Mu Block (MMB)
	// -------------------------

	try {
		unsigned int blockSize = 32;
		unsigned int width = fingerprintImage.m_ImageWidth;
		unsigned int height = fingerprintImage.m_ImageHeight;
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
				Mat block = img(
				    Rect(j, i, takenBS_X, takenBS_Y));
				Scalar m = mean(block);
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
		NFIQ::QualityFeatureData fd_mmb;
		fd_mmb.featureID = "MMB";
		fd_mmb.featureDataType = NFIQ::e_QualityFeatureDataTypeDouble;
		fd_mmb.featureDataDouble = avg;
		NFIQ::QualityFeatureResult res_mmb;
		res_mmb.featureData = fd_mmb;
		res_mmb.returnCode = 0;

		featureDataList.push_back(res_mmb);
	} catch (cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot compute feature Mu Mu Block (MMB): "
		      << e.what();
		throw NFIQ::NFIQException(
		    NFIQ::e_Error_FeatureCalculationError, ssErr.str());
	} catch (NFIQ::NFIQException &e) {
		throw e;
	} catch (...) {
		throw NFIQ::NFIQException(NFIQ::e_Error_FeatureCalculationError,
		    "Unknown exception occurred!");
	}

	// -----------------------------------------
	// compute Mu and Standard Deviation = Sigma
	// -----------------------------------------

	Scalar stddev;
	Scalar mu;
	try {
		// calculate stddev of input image = sigma and mu = mean
		meanStdDev(img, mu, stddev);
		// assign sigma value
		this->sigma = stddev.val[0];
		this->sigmaComputed = true;

		// return mu value
		NFIQ::QualityFeatureData fd_mu;
		fd_mu.featureID = "Mu";
		fd_mu.featureDataType = NFIQ::e_QualityFeatureDataTypeDouble;
		fd_mu.featureDataDouble = mu.val[0];
		NFIQ::QualityFeatureResult res_mu;
		res_mu.featureData = fd_mu;
		res_mu.returnCode = 0;

		featureDataList.push_back(res_mu);
	} catch (cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot compute feature Sigma (stddev) and Mu (mean): "
		      << e.what();
		throw NFIQ::NFIQException(
		    NFIQ::e_Error_FeatureCalculationError, ssErr.str());
	} catch (NFIQ::NFIQException &e) {
		throw e;
	} catch (...) {
		throw NFIQ::NFIQException(NFIQ::e_Error_FeatureCalculationError,
		    "Unknown exception occurred!");
	}

	// Speed
	NFIQ::QualityFeatureSpeed speed;
	speed.featureIDGroup = MuFeature::speedFeatureIDGroup;
	speed.featureIDs.push_back("MMB");
	speed.featureIDs.push_back("Mu");
	speed.featureSpeed = timer.endTimerAndGetElapsedTime();
	this->setSpeed(speed);

	return featureDataList;
}

double
NFIQ::QualityFeatures::MuFeature::getSigma() const
{
	if (!this->sigmaComputed)
		throw NFIQ::NFIQException { e_Error_NoDataAvailable,
			"Sigma has not been computed." };

	return (this->sigma);
}

const std::string NFIQ::QualityFeatures::MuFeature::moduleName { "NFIQ2_Mu" };

std::string
NFIQ::QualityFeatures::MuFeature::getModuleName() const
{
	return moduleName;
}

std::vector<std::string>
NFIQ::QualityFeatures::MuFeature::getAllFeatureIDs()
{
	std::vector<std::string> featureIDs;
	featureIDs.push_back("MMB");
	featureIDs.push_back("Mu");
	return featureIDs;
}
