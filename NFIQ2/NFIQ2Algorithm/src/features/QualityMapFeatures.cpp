#include <features/ImgProcROIFeature.h>
#include <features/QualityMapFeatures.h>
#include <nfiq2_exception.hpp>
#include <nfiq2_timer.hpp>

#include <cmath>
#include <sstream>

const char NFIQ2::Identifiers::QualityModules::RegionOfInterestCoherence[] {
	"RegionOfInterestCoherence"
};
const char
    NFIQ2::Identifiers::QualityFeatures::RegionOfInterest::CoherenceSum[] {
	    "OrientationMap_ROIFilter_CoherenceSum"
    };
const char
    NFIQ2::Identifiers::QualityFeatures::RegionOfInterest::CoherenceMean[] {
	    "OrientationMap_ROIFilter_CoherenceRel"
    };

NFIQ2::QualityFeatures::QualityMapFeatures::QualityMapFeatures(
    const NFIQ2::FingerprintImageData &fingerprintImage,
    const ImgProcROIFeature::ImgProcROIResults &imgProcResults)
    : imgProcResults_ { imgProcResults }
{
	this->setFeatures(computeFeatureData(fingerprintImage));
}

NFIQ2::QualityFeatures::QualityMapFeatures::~QualityMapFeatures() = default;

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::QualityMapFeatures::computeFeatureData(
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

	NFIQ2::Timer timer;
	timer.start();

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

	try {
		// ------------------------
		// orientation map features
		// ------------------------

		// get orientation map with ROI filter
		// uses block size 16
		double coherenceSumFilter = 0.0;
		double coherenceRelFilter = 0.0;
		cv::Mat orientationMapImgFilter = computeOrientationMap(img,
		    true, coherenceSumFilter, coherenceRelFilter, 16,
		    this->imgProcResults_);

		// return features based on coherence values of orientation map
		std::pair<std::string, double> fd_om_2;
		fd_om_2 = std::make_pair(Identifiers::QualityFeatures::
					     RegionOfInterest::CoherenceMean,
		    coherenceRelFilter);

		featureDataList[fd_om_2.first] = fd_om_2.second;

		std::pair<std::string, double> fd_om_1;
		fd_om_1 = std::make_pair(Identifiers::QualityFeatures::
					     RegionOfInterest::CoherenceSum,
		    coherenceSumFilter);

		featureDataList[fd_om_1.first] = fd_om_1.second;

		this->setSpeed(timer.stop());
	} catch (const cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot compute orientation map: " << e.what();
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FeatureCalculationError, ssErr.str());
	} catch (const NFIQ2::Exception &) {
		throw;
	}

	return featureDataList;
}

cv::Mat
NFIQ2::QualityFeatures::QualityMapFeatures::computeOrientationMap(cv::Mat &img,
    bool bFilterByROI, double &coherenceSum, double &coherenceRel,
    unsigned int bs, ImgProcROIFeature::ImgProcROIResults roiResults)
{
	coherenceSum = 0.0;
	coherenceRel = 0.0;

	cv::Mat visImage;

	// result image (block pixel values = orientation in degrees)
	cv::Mat omImg = cv::Mat(img.rows, img.cols, CV_8UC1,
	    cv::Scalar(0, 0, 0, 0)); // empty black image

	// divide into blocks
	for (int i = 0; i < img.rows; i += bs) {
		for (int j = 0; j < img.cols; j += bs) {
			int actualBS_X = ((img.cols - j) < (int)bs) ?
				  (img.cols - j) :
				  bs;
			int actualBS_Y = ((img.rows - i) < (int)bs) ?
				  (img.rows - i) :
				  bs;

			// check if block is vector of ROI blocks
			if (bFilterByROI) {
				// search for ROI block
				bool bBlockFound = false;
				for (unsigned int k = 0;
				     k < roiResults.vecROIBlocks.size(); k++) {
					if (roiResults.vecROIBlocks.at(k).x ==
						j &&
					    roiResults.vecROIBlocks.at(k).y ==
						i &&
					    roiResults.vecROIBlocks.at(k)
						    .width == actualBS_X &&
					    roiResults.vecROIBlocks.at(k)
						    .height == actualBS_Y) {
						// block found
						bBlockFound = true;
						break;
					}
				}

				if (!bBlockFound) {
					for (int k = i; k < (i + actualBS_Y);
					     k++) {
						for (int l = j;
						     l < (j + actualBS_X);
						     l++) {
							omImg.at<uchar>(k,
							    l) = 255; // set
								      // value
								      // of
								      // block
								      // to
								      // white
						}
					}
					continue; // do not compute angle for a
						  // non-ROI block (as no ridge
						  // lines will be there)
				}
			}

			// get current block
			cv::Mat bl_img = img(
			    cv::Rect(j, i, actualBS_X, actualBS_Y));

			// get orientation angle of current block
			double angle = 0.0;
			double coherence = 0.0;
			if (!getAngleOfBlock(bl_img, angle, coherence)) {
				continue; // block does not have angle = no
					  // ridge line
			}
			if (std::isnan(coherence)) {
				coherence = 0.0;
			}
			coherenceSum += coherence;

			// draw angle to final orientation map
			// angle in degrees = greyvalue of block
			// is in range [0..180] degrees
			int angleDegree = (int)((angle * 180 / M_PI) + 0.5);
			for (int k = i; k < (i + actualBS_Y); k++) {
				for (int l = j; l < (j + actualBS_X); l++) {
					omImg.at<uchar>(
					    k, l) = angleDegree; // set to angle
								 // value
				}
			}
		}
	}

	if (bFilterByROI) {
		if (roiResults.vecROIBlocks.size() <= 0) {
			coherenceRel = 0.0;
		} else {
			coherenceRel = (coherenceSum /
			    roiResults.vecROIBlocks.size());
		}
	} else {
		if (roiResults.noOfAllBlocks <= 0) {
			coherenceRel = 0.0;
		} else {
			coherenceRel = (coherenceSum /
			    roiResults.noOfAllBlocks);
		}
	}

	return omImg;
}

bool
NFIQ2::QualityFeatures::QualityMapFeatures::getAngleOfBlock(
    const cv::Mat &block, double &angle, double &coherence)
{
	// compute the numerical gradients of the block
	// in x and y direction
	cv::Mat grad_x, grad_y;
	computeNumericalGradients(block, grad_x, grad_y);

	// compute gsx and gsy which are average squared gradients
	double sum_y = 0.0;
	double sum_x = 0.0;
	double coh_sum2 = 0.0;
	for (int k = 0; k < block.rows; k++) {
		for (int l = 0; l < block.cols; l++) {
			// 2 * gx * gy
			double gy = (2 * grad_x.at<double>(k, l) *
			    grad_y.at<double>(k, l));
			if (std::isnan(gy)) {
				sum_y += 0;
			} else {
				sum_y += gy;
			}
			// gx^2 - gy^2
			double gx = ((grad_x.at<double>(k, l) *
					 grad_x.at<double>(k, l)) -
			    (grad_y.at<double>(k, l) *
				grad_y.at<double>(k, l)));
			if (std::isnan(gx)) {
				sum_x += 0;
			} else {
				sum_x += gx;
			}

			// values for coherence
			coh_sum2 += sqrt(gy * gy + gx * gx);
		}
	}

	// get radiant and convert to correct orientation angle
	// angle is in range [0..pi]
	angle = 0.5 * atan2(sum_y, sum_x) + (M_PI / 2.0);

	// compute coherence value of gradient vector
	double coh_sum1 = sqrt(sum_x * sum_x + sum_y * sum_y);
	if (std::isnan(coh_sum1)) {
		coh_sum1 = 0.0;
	}
	if (std::isnan(coh_sum2)) {
		coh_sum2 = 0.0;
	}

	if (coh_sum2 != 0) {
		coherence = (coh_sum1 / coh_sum2);
	} else {
		coherence = 0;
	}

	return true;
}

cv::Mat
NFIQ2::QualityFeatures::QualityMapFeatures::computeNumericalGradientX(
    const cv::Mat &mat)
{
	cv::Mat out(mat.rows, mat.cols, CV_64F, cv::Scalar(0));

	for (int y = 0; y < mat.rows; ++y) {
		const uchar *in_r = mat.ptr<uchar>(y);
		double *out_r = out.ptr<double>(y);

		if (mat.cols > 1) {
			out_r[0] = in_r[1] - in_r[0];
			for (int x = 1; x < mat.cols - 1; ++x) {
				out_r[x] = (in_r[x + 1] - in_r[x - 1]) / 2.0;
			}
			out_r[mat.cols - 1] = in_r[mat.cols - 1] -
			    in_r[mat.cols - 2];
		}
	}
	return out;
}

void
NFIQ2::QualityFeatures::QualityMapFeatures::computeNumericalGradients(
    const cv::Mat &mat, cv::Mat &grad_x, cv::Mat &grad_y)
{
	// get x-gradient
	grad_x = computeNumericalGradientX(mat);
	// to get the y-gradient: take the x-gradient of the transpose matrix
	// and transpose it again
	grad_y = computeNumericalGradientX(mat.t()).t();
}

std::string
NFIQ2::QualityFeatures::QualityMapFeatures::getModuleName() const
{
	return NFIQ2::Identifiers::QualityModules::RegionOfInterestCoherence;
}

std::vector<std::string>
NFIQ2::QualityFeatures::QualityMapFeatures::getQualityFeatureIDs()
{
	return { Identifiers::QualityFeatures::RegionOfInterest::CoherenceMean,
		Identifiers::QualityFeatures::RegionOfInterest::CoherenceSum };
}
