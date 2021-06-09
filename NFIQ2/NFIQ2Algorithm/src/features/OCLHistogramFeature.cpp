#include <features/FeatureFunctions.h>
#include <features/OCLHistogramFeature.h>
#include <nfiq2_exception.hpp>
#include <nfiq2_timer.hpp>

#include <sstream>

const char NFIQ2::Identifiers::QualityModules::OrientationCertainty[] {
	"OrientationCertainty"
};
static const char NFIQ2OCLFeaturePrefix[] { "OCL_Bin10_" };
const char NFIQ2::Identifiers::QualityFeatures::OrientationCertainty::
    Histogram::Bin0[] { "OCL_Bin10_0" };
const char NFIQ2::Identifiers::QualityFeatures::OrientationCertainty::
    Histogram::Bin1[] { "OCL_Bin10_1" };
const char NFIQ2::Identifiers::QualityFeatures::OrientationCertainty::
    Histogram::Bin2[] { "OCL_Bin10_2" };
const char NFIQ2::Identifiers::QualityFeatures::OrientationCertainty::
    Histogram::Bin3[] { "OCL_Bin10_3" };
const char NFIQ2::Identifiers::QualityFeatures::OrientationCertainty::
    Histogram::Bin4[] { "OCL_Bin10_4" };
const char NFIQ2::Identifiers::QualityFeatures::OrientationCertainty::
    Histogram::Bin5[] { "OCL_Bin10_5" };
const char NFIQ2::Identifiers::QualityFeatures::OrientationCertainty::
    Histogram::Bin6[] { "OCL_Bin10_6" };
const char NFIQ2::Identifiers::QualityFeatures::OrientationCertainty::
    Histogram::Bin7[] { "OCL_Bin10_7" };
const char NFIQ2::Identifiers::QualityFeatures::OrientationCertainty::
    Histogram::Bin8[] { "OCL_Bin10_8" };
const char NFIQ2::Identifiers::QualityFeatures::OrientationCertainty::
    Histogram::Bin9[] { "OCL_Bin10_9" };
const char NFIQ2::Identifiers::QualityFeatures::OrientationCertainty::Mean[] {
	"OCL_Bin10_Mean"
};
const char NFIQ2::Identifiers::QualityFeatures::OrientationCertainty::StdDev[] {
	"OCL_Bin10_StdDev"
};

NFIQ2::QualityFeatures::OCLHistogramFeature::OCLHistogramFeature(
    const NFIQ2::FingerprintImageData &fingerprintImage)
{
	this->setFeatures(computeFeatureData(fingerprintImage));
}

NFIQ2::QualityFeatures::OCLHistogramFeature::~OCLHistogramFeature() = default;

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::OCLHistogramFeature::computeFeatureData(
    const NFIQ2::FingerprintImageData &fingerprintImage)
{
	std::unordered_map<std::string, double> featureDataList;

	cv::Mat img;

	// check if input image has 500 dpi
	if (fingerprintImage.ppi !=
	    NFIQ2::FingerprintImageData::Resolution500PPI) {
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FeatureCalculationError,
		    "Only 500 dpi fingerprint images are supported!");
	}

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

	// compute OCL
	NFIQ2::Timer timerOCL;
	std::vector<double> oclres;
	try {
		timerOCL.start();

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
					cv::Mat bl_img = img(cv::Rect(
					    j, i, actualBS_X, actualBS_Y));

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
		addHistogramFeatures(featureDataList, NFIQ2OCLFeaturePrefix,
		    histogramBins10, oclres, 10);

		this->setSpeed(timerOCL.stop());
	} catch (const cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot compute feature OCL histogram: " << e.what();
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

bool
NFIQ2::QualityFeatures::OCLHistogramFeature::getOCLValueOfBlock(
    const cv::Mat &block, double &ocl)
{
	double eigv_max = 0.0, eigv_min = 0.0;
	// compute the numerical gradients of the block
	cv::Mat grad_x, grad_y;
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

std::string
NFIQ2::QualityFeatures::OCLHistogramFeature::getModuleName() const
{
	return NFIQ2::Identifiers::QualityModules::OrientationCertainty;
}

std::vector<std::string>
NFIQ2::QualityFeatures::OCLHistogramFeature::getQualityFeatureIDs()
{
	return { Identifiers::QualityFeatures::OrientationCertainty::Histogram::
		     Bin0,
		Identifiers::QualityFeatures::OrientationCertainty::Histogram::
		    Bin1,
		Identifiers::QualityFeatures::OrientationCertainty::Histogram::
		    Bin2,
		Identifiers::QualityFeatures::OrientationCertainty::Histogram::
		    Bin3,
		Identifiers::QualityFeatures::OrientationCertainty::Histogram::
		    Bin4,
		Identifiers::QualityFeatures::OrientationCertainty::Histogram::
		    Bin5,
		Identifiers::QualityFeatures::OrientationCertainty::Histogram::
		    Bin6,
		Identifiers::QualityFeatures::OrientationCertainty::Histogram::
		    Bin7,
		Identifiers::QualityFeatures::OrientationCertainty::Histogram::
		    Bin8,
		Identifiers::QualityFeatures::OrientationCertainty::Histogram::
		    Bin9,
		Identifiers::QualityFeatures::OrientationCertainty::Mean,
		Identifiers::QualityFeatures::OrientationCertainty::StdDev };
}
