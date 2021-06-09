#include <features/ImgProcROIFeature.h>
#include <nfiq2_exception.hpp>
#include <nfiq2_timer.hpp>
#include <opencv2/imgproc.hpp>

#include <sstream>

const char NFIQ2::Identifiers::QualityModules::RegionOfInterestMean[] {
	"RegionOfInterestMean"
};
const char NFIQ2::Identifiers::QualityFeatures::RegionOfInterest::Mean[] {
	"ImgProcROIArea_Mean"
};

NFIQ2::QualityFeatures::ImgProcROIFeature::ImgProcROIFeature(
    const NFIQ2::FingerprintImageData &fingerprintImage)
{
	this->setFeatures(computeFeatureData(fingerprintImage));
}

NFIQ2::QualityFeatures::ImgProcROIFeature::~ImgProcROIFeature() = default;

NFIQ2::QualityFeatures::ImgProcROIFeature::ImgProcROIResults
NFIQ2::QualityFeatures::ImgProcROIFeature::getImgProcResults()
{
	if (!this->imgProcComputed_) {
		throw NFIQ2::Exception { NFIQ2::ErrorCode::NoDataAvailable,
			"Img Proc Results could not be computed." };
	}

	return (this->imgProcResults_);
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::ImgProcROIFeature::computeFeatureData(
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

	// ---------------------------------------------
	// compute ROI (and other features based on ROI)
	// ---------------------------------------------
	try {
		this->imgProcResults_ = computeROI(
		    img, 16); // block size = 16x16 pixels

		std::pair<std::string, double> fd_roi_pixel_area_mean;
		fd_roi_pixel_area_mean = std::make_pair(
		    Identifiers::QualityFeatures::RegionOfInterest::Mean,
		    this->imgProcResults_.meanOfROIPixels);
		featureDataList[fd_roi_pixel_area_mean.first] =
		    fd_roi_pixel_area_mean.second;

		this->setSpeed(timer.stop());
	} catch (const cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot compute feature (ImgProc)ROI area: "
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

	this->imgProcComputed_ = true;

	return featureDataList;
}

std::string
NFIQ2::QualityFeatures::ImgProcROIFeature::getModuleName() const
{
	return NFIQ2::Identifiers::QualityModules::RegionOfInterestMean;
}

std::vector<std::string>
NFIQ2::QualityFeatures::ImgProcROIFeature::getQualityFeatureIDs()
{
	return { Identifiers::QualityFeatures::RegionOfInterest::Mean };
}

NFIQ2::QualityFeatures::ImgProcROIFeature::ImgProcROIResults
NFIQ2::QualityFeatures::ImgProcROIFeature::computeROI(
    cv::Mat &img, unsigned int bs)
{
	ImgProcROIResults roiResults;

	// 1. erode image to get fingerprint details more clearly
	cv::Mat erodedImg;
	cv::Mat element(5, 5, CV_8U, cv::Scalar(1));
	cv::erode(img, erodedImg, element);

	// 2. Gaussian blur to get important area
	cv::Mat blurImg;
	cv::GaussianBlur(erodedImg, blurImg, cv::Size(41, 41), 0.0);

	// 3. Binarize image with Otsu method
	cv::Mat threshImg;
	cv::threshold(blurImg, threshImg, 0, 255, cv::THRESH_OTSU);

	// 4. Blur image again
	cv::Mat blurImg2;
	cv::GaussianBlur(threshImg, blurImg2, cv::Size(91, 91), 0.0);

	// 5. Binarize image again with Otsu method
	cv::Mat threshImg2;
	cv::threshold(blurImg2, threshImg2, 0, 255, cv::THRESH_OTSU);

	// 6. try find white holes in black image
	cv::Mat contImg = threshImg2.clone();
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	// find contours in image
	cv::findContours(contImg, contours, hierarchy, cv::RETR_CCOMP,
	    cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	// if holes are found -> close holes
	if (hierarchy.size() > 2) {
		cv::Mat filledImg;
		cv::cvtColor(threshImg2, filledImg, cv::COLOR_GRAY2BGR);

		for (unsigned int idx = 0; idx < (hierarchy.size() - 2);
		     idx++) {
			cv::drawContours(filledImg, contours, idx,
			    cv::Scalar(0, 0, 0, 0), cv::FILLED, 8, hierarchy);
		}

		cv::cvtColor(filledImg, threshImg2, cv::COLOR_BGR2GRAY);
	}

	// 7. remove smaller blobs at the edges that are not part of the
	// fingerprint
	cv::Mat ffImg = threshImg2.clone();
	cv::Point point;
	std::vector<cv::Rect> vecRects;
	std::vector<cv::Point> vecPoints;
	while (isBlackPixelAvailable(ffImg, point)) {
		// execute flood fill algorithm starting with discovered seed
		// and save flooded area on copied image
		cv::Rect rect;
		cv::floodFill(
		    ffImg, point, cv::Scalar(255, 255, 255, 0), &rect);
		vecRects.push_back(rect);
		vecPoints.push_back(point);
	}

	// find largest region based on returned area
	unsigned int maxIdx = 0;
	int maxSize = 0;
	for (unsigned int i = 0; i < vecRects.size(); i++) {
		if ((vecRects.at(i).width * vecRects.at(i).height) > maxSize) {
			maxIdx = i;
			maxSize = (vecRects.at(i).width *
			    vecRects.at(i).height);
		}
	}

	// now apply floodfill algorithm on all areas that are not the biggest
	// one
	for (unsigned int i = 0; i < vecRects.size(); i++) {
		if (i != maxIdx) {
			// apply floodfill on original image
			// start seed first detected point
			cv::floodFill(threshImg2,
			    cv::Point(vecPoints.at(i).x, vecPoints.at(i).y),
			    cv::Scalar(255, 255, 255, 0));
		}
	}

	// count ROI pixels ( = black pixels)
	// and get mean value of ROI pixels
	unsigned int noOfROIPixels = 0;
	double meanOfROIPixels = 0.0;
	for (int i = 0; i < threshImg2.rows; i++) {
		for (int j = 0; j < threshImg2.cols; j++) {
			if (((int)threshImg2.at<uchar>(i, j)) == 0) {
				noOfROIPixels++;
				// get gray value of original image (0 = black,
				// 255 = white)
				meanOfROIPixels += (int)img.at<uchar>(i, j);
			}
		}
	}
	// divide value by absolute number of ROI pixels to get mean
	if (noOfROIPixels <= 0) {
		meanOfROIPixels = 255.0; // "white" image
	} else {
		meanOfROIPixels = (meanOfROIPixels / (double)noOfROIPixels);
	}

	// get standard deviation of ORI pixels
	double sumSquare = 0.0;
	for (int i = 0; i < threshImg2.rows; i++) {
		for (int j = 0; j < threshImg2.cols; j++) {
			if (((int)threshImg2.at<uchar>(i, j)) == 0) {
				// get gray value of original image (0 = black,
				// 255 = white)
				unsigned int x = (unsigned int)img.at<uchar>(
				    i, j);
				sumSquare += (((double)x - meanOfROIPixels) *
				    ((double)x - meanOfROIPixels));
			}
		}
	}
	sumSquare = (1.0 / ((double)noOfROIPixels - 1.0) * sumSquare);
	double stdDevOfROIPixels = 0.0;
	if (sumSquare >= 0) {
		stdDevOfROIPixels = sqrt(sumSquare);
	}

	// 8. compute and draw blocks
	unsigned int width = img.cols;
	unsigned int height = img.rows;
	cv::Mat bsImg(height, width, CV_8UC1, cv::Scalar(255, 0, 0, 0));

	unsigned int noOfAllBlocks = 0;
	unsigned int noOfCompleteBlocks = 0;
	for (unsigned int i = 0; i < height; i += bs) {
		for (unsigned int j = 0; j < width; j += bs) {
			unsigned int takenBS_X = bs;
			unsigned int takenBS_Y = bs;
			if ((width - j) < bs) {
				takenBS_X = (width - j);
			}
			if ((height - i) < bs) {
				takenBS_Y = (height - i);
			}

			cv::Mat block = threshImg2(
			    cv::Rect(j, i, takenBS_X, takenBS_Y));
			noOfAllBlocks++;
			if (takenBS_X == bs && takenBS_Y == bs) {
				noOfCompleteBlocks++;
			}
			// count number of black pixels in block
			cv::Scalar m = mean(block);
			if (m.val[0] < 255) {
				// take block
				cv::rectangle(bsImg, cv::Point(j, i),
				    cv::Point(j + takenBS_X, i + takenBS_Y),
				    cv::Scalar(0, 0, 0, 0), cv::FILLED);
				roiResults.vecROIBlocks.push_back(
				    cv::Rect(j, i, takenBS_X, takenBS_Y));
			}
		}
	}

	roiResults.chosenBlockSize = bs;
	roiResults.noOfAllBlocks = noOfAllBlocks;
	roiResults.noOfCompleteBlocks = noOfCompleteBlocks;
	roiResults.noOfImagePixels = (img.cols * img.rows);
	roiResults.noOfROIPixels = noOfROIPixels;
	roiResults.meanOfROIPixels = meanOfROIPixels;
	roiResults.stdDevOfROIPixels = stdDevOfROIPixels;

	return roiResults;
}

bool
NFIQ2::QualityFeatures::ImgProcROIFeature::isBlackPixelAvailable(
    cv::Mat &img, cv::Point &point)
{
	bool found = false;
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			if (((int)img.at<uchar>(i, j)) == 0) {
				point.x = j;
				point.y = i;
				found = true;
				break;
			}
		}
		if (found) {
			break;
		}
	}
	return found;
}
