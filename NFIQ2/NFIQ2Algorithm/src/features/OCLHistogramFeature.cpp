#include <features/FeatureFunctions.h>
#include <features/OCLHistogramFeature.h>
#include <nfiq2_nfiqexception.hpp>
#include <nfiq2_timer.hpp>

#include <sstream>

#if defined WINDOWS || defined WIN32
#include <float.h>
#include <windows.h>
#endif

using namespace NFIQ;
using namespace cv;

NFIQ::QualityFeatures::OCLHistogramFeature::~OCLHistogramFeature()
{
}

const std::string
    NFIQ::QualityFeatures::OCLHistogramFeature::speedFeatureIDGroup =
	"Orientation certainty";

std::vector<NFIQ::QualityFeatureResult>
NFIQ::QualityFeatures::OCLHistogramFeature::computeFeatureData(
    const NFIQ::FingerprintImageData &fingerprintImage)
{
	std::vector<NFIQ::QualityFeatureResult> featureDataList;

	Mat img;

	// check if input image has 500 dpi
	if (fingerprintImage.m_ImageDPI != NFIQ::e_ImageResolution_500dpi) {
		throw NFIQ::NFIQException(NFIQ::e_Error_FeatureCalculationError,
		    "Only 500 dpi fingerprint images are supported!");
	}

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

	// compute OCL
	NFIQ::Timer timerOCL;
	double timeOCL = 0.0;
	std::vector<double> oclres;
	try {
		timerOCL.startTimer();

		// divide into blocks
		for (int i = 0; i < img.rows; i += BS_OCL) {
			for (int j = 0; j < img.cols; j += BS_OCL) {
				unsigned int actualBS_X = ((img.cols - j) <
							      BS_OCL) ?
					  (img.cols - j) :
					  BS_OCL;
				unsigned int actualBS_Y = ((img.rows - i) <
							      BS_OCL) ?
					  (img.rows - i) :
					  BS_OCL;

				if (actualBS_X == BS_OCL &&
				    actualBS_Y == BS_OCL) {
					// only take blocks of full size
					// ignore other blocks

					// get current block
					Mat bl_img = img(
					    Rect(j, i, actualBS_X, actualBS_Y));

					// get OCL value of current block
					double bl_ocl = 0.0;
					if (!getOCLValueOfBlock(
						bl_img, bl_ocl)) {
						continue; // block is not used
					}

					oclres.push_back(bl_ocl);
				}
			}
		}

		std::vector<double> histogramBins10;
		histogramBins10.push_back(OCLPHISTLIMITS[0]);
		histogramBins10.push_back(OCLPHISTLIMITS[1]);
		histogramBins10.push_back(OCLPHISTLIMITS[2]);
		histogramBins10.push_back(OCLPHISTLIMITS[3]);
		histogramBins10.push_back(OCLPHISTLIMITS[4]);
		histogramBins10.push_back(OCLPHISTLIMITS[5]);
		histogramBins10.push_back(OCLPHISTLIMITS[6]);
		histogramBins10.push_back(OCLPHISTLIMITS[7]);
		histogramBins10.push_back(OCLPHISTLIMITS[8]);
		addHistogramFeatures(
		    featureDataList, "OCL_Bin10_", histogramBins10, oclres, 10);

		timeOCL = timerOCL.endTimerAndGetElapsedTime();
		if (m_bOutputSpeed) {
			NFIQ::QualityFeatureSpeed speed;
			speed.featureIDGroup =
			    OCLHistogramFeature::speedFeatureIDGroup;

			addHistogramFeatureNames(
			    speed.featureIDs, "OCL_Bin10_", 10);

			speed.featureSpeed = timeOCL;
			m_lSpeedValues.push_back(speed);
		}
	} catch (cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot compute feature OCL histogram: " << e.what();
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

bool
NFIQ::QualityFeatures::OCLHistogramFeature::getOCLValueOfBlock(
    const cv::Mat &block, double &ocl)
{
	double eigv_max = 0.0, eigv_min = 0.0;
	// compute the numerical gradients of the block
	Mat grad_x, grad_y;
	computeNumericalGradients(block, grad_x, grad_y);

	// compute covariance matrix
	double a = 0.0;
	double b = 0.0;
	double c = 0.0;
	for (unsigned int k = 0; k < BS_OCL; k++) {
		for (unsigned int l = 0; l < BS_OCL; l++) {
			a += (grad_x.at<double>(l, k) *
			    grad_x.at<double>(l, k));
			b += (grad_y.at<double>(l, k) *
			    grad_y.at<double>(l, k));
			c += (grad_x.at<double>(l, k) *
			    grad_y.at<double>(l, k));
		}
	}
	// take mean value covariance matrix values
	a /= (BS_OCL * BS_OCL);
	b /= (BS_OCL * BS_OCL);
	c /= (BS_OCL * BS_OCL);

	// compute the eigenvalues
	eigv_max = ((a + b) + sqrt(pow(a - b, 2) + 4 * pow(c, 2))) / 2.0;
	eigv_min = ((a + b) - sqrt(pow(a - b, 2) + 4 * pow(c, 2))) / 2.0;

	if (eigv_max == 0) {
		// block is excluded from usage
		ocl = 0.0;
		return false;
	}

	// compute the OCL value of the block
	ocl = (1.0 - (eigv_min / eigv_max)); // 0 (worst), 1 (best)
	return true;
}

const std::string NFIQ::QualityFeatures::OCLHistogramFeature::moduleName {
	"NFIQ2_OCLHistogram"
};

std::string
NFIQ::QualityFeatures::OCLHistogramFeature::getModuleName() const
{
	return "NFIQ2_OCLHistogram";
}

std::vector<std::string>
NFIQ::QualityFeatures::OCLHistogramFeature::getAllFeatureIDs()
{
	std::vector<std::string> featureIDs;
	addHistogramFeatureNames(featureIDs, "OCL_Bin10_", 10);

	return featureIDs;
}
