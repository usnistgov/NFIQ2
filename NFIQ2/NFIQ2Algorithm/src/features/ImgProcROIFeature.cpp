#include <features/ImgProcROIFeature.h>
#include <nfiq2_nfiqexception.hpp>
#include <nfiq2_timer.hpp>
#include <opencv2/imgproc.hpp>

#include <sstream>

using namespace NFIQ;

NFIQ::QualityFeatures::ImgProcROIFeature::ImgProcROIFeature(
    const NFIQ::FingerprintImageData &fingerprintImage)
{
	this->setFeatures(computeFeatureData(fingerprintImage));
}

NFIQ::QualityFeatures::ImgProcROIFeature::~ImgProcROIFeature() = default;

const std::string
    NFIQ::QualityFeatures::ImgProcROIFeature::speedFeatureIDGroup =
	"Region of interest";

NFIQ::QualityFeatures::ImgProcROIFeature::ImgProcROIResults
NFIQ::QualityFeatures::ImgProcROIFeature::getImgProcResults()
{
	if (!this->imgProcComputed_) {
		throw NFIQ::NFIQException { e_Error_NoDataAvailable,
			"Img Proc Results could not be computed." };
	}

	return (this->imgProcResults_);
}

std::vector<NFIQ::QualityFeatureResult>
NFIQ::QualityFeatures::ImgProcROIFeature::computeFeatureData(
    const NFIQ::FingerprintImageData &fingerprintImage)
{
	std::vector<NFIQ::QualityFeatureResult> featureDataList;

	// check if input image has 500 dpi
	if (fingerprintImage.m_ImageDPI != NFIQ::e_ImageResolution_500dpi) {
		throw NFIQ::NFIQException(NFIQ::e_Error_FeatureCalculationError,
		    "Only 500 dpi fingerprint images are supported!");
	}

	cv::Mat img;
	try {
		// get matrix from fingerprint image
		img = cv::Mat(fingerprintImage.m_ImageHeight,
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
	timer.start();

	// ---------------------------------------------
	// compute ROI (and other features based on ROI)
	// ---------------------------------------------
	try {
		this->imgProcResults_ = computeROI(
		    img, 16); // block size = 16x16 pixels

		NFIQ::QualityFeatureData fd_roi_pixel_area_mean;
		fd_roi_pixel_area_mean.featureID = "ImgProcROIArea_Mean";
		fd_roi_pixel_area_mean.featureDataType =
		    NFIQ::e_QualityFeatureDataTypeDouble;
		fd_roi_pixel_area_mean.featureDataDouble =
		    this->imgProcResults_.meanOfROIPixels;
		NFIQ::QualityFeatureResult res_roi_pixel_area_mean;
		res_roi_pixel_area_mean.featureData = fd_roi_pixel_area_mean;
		res_roi_pixel_area_mean.returnCode = 0;

		featureDataList.push_back(res_roi_pixel_area_mean);

		// Speed
		NFIQ::QualityFeatureSpeed speed;
		speed.featureIDGroup = ImgProcROIFeature::speedFeatureIDGroup;
		speed.featureIDs.push_back("ImgProcROIArea_Mean");
		speed.featureSpeed = timer.stop();
		this->setSpeed(speed);

	} catch (cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot compute feature (ImgProc)ROI area: "
		      << e.what();
		throw NFIQ::NFIQException(
		    NFIQ::e_Error_FeatureCalculationError, ssErr.str());
	} catch (NFIQ::NFIQException &e) {
		throw e;
	} catch (...) {
		throw NFIQ::NFIQException(NFIQ::e_Error_FeatureCalculationError,
		    "Unknown exception occurred!");
	}

	this->imgProcComputed_ = true;

	return featureDataList;
}

const std::string NFIQ::QualityFeatures::ImgProcROIFeature::moduleName {
	"NFIQ2_ImgProcROI"
};

std::string
NFIQ::QualityFeatures::ImgProcROIFeature::getModuleName() const
{
	return moduleName;
}

std::vector<std::string>
NFIQ::QualityFeatures::ImgProcROIFeature::getAllFeatureIDs()
{
	std::vector<std::string> featureIDs;
	featureIDs.push_back("ImgProcROIArea_Mean");
	return featureIDs;
}

NFIQ::QualityFeatures::ImgProcROIFeature::ImgProcROIResults
NFIQ::QualityFeatures::ImgProcROIFeature::computeROI(
    cv::Mat &img, unsigned int bs)
{
	ImgProcROIResults roiResults;

	// 1. erode image to get fingerprint details more clearly
	cv::Mat erodedImg;
	cv::Mat element(5, 5, CV_8U, cv::Scalar(1));
	erode(img, erodedImg, element);

	// 2. Gaussian blur to get important area
	cv::Mat blurImg;
	GaussianBlur(erodedImg, blurImg, cv::Size(41, 41), 0.0);

	// 3. Binarize image with Otsu method
	cv::Mat threshImg;
	threshold(blurImg, threshImg, 0, 255, cv::THRESH_OTSU);

	// 4. Blur image again
	cv::Mat blurImg2;
	GaussianBlur(threshImg, blurImg2, cv::Size(91, 91), 0.0);

	// 5. Binarize image again with Otsu method
	cv::Mat threshImg2;
	threshold(blurImg2, threshImg2, 0, 255, cv::THRESH_OTSU);

	// 6. try find white holes in black image
	cv::Mat contImg = threshImg2.clone();
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	// find contours in image
#if CV_MAJOR_VERSION <= 2
	findContours(contImg, contours, hierarchy, CV_RETR_CCOMP,
	    CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
#else
	findContours(contImg, contours, hierarchy, cv::RETR_CCOMP,
	    cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
#endif /* CV_MAJOR_VERSION */

	// if holes are found -> close holes
	if (hierarchy.size() > 2) {
		cv::Mat filledImg;
#if CV_MAJOR_VERSION <= 2
		cvtColor(threshImg2, filledImg, CV_GRAY2BGR);
#else
		cvtColor(threshImg2, filledImg, cv::COLOR_GRAY2BGR);
#endif /* CV_MAJOR_VERSION */

		for (unsigned int idx = 0; idx < (hierarchy.size() - 2);
		     idx++) {
#if CV_MAJOR_VERSION <= 2
			drawContours(filledImg, contours, idx,
			    cv::Scalar(0, 0, 0, 0), CV_FILLED, 8, hierarchy);
#else
			drawContours(filledImg, contours, idx,
			    cv::Scalar(0, 0, 0, 0), cv::FILLED, 8, hierarchy);
#endif /* CV_MAJOR_VERSION */
		}
#if CV_MAJOR_VERSION <= 2
		cvtColor(filledImg, threshImg2, CV_BGR2GRAY);
#else
		cvtColor(filledImg, threshImg2, cv::COLOR_BGR2GRAY);
#endif /* CV_MAJOR_VERSION */
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
		floodFill(ffImg, point, cv::Scalar(255, 255, 255, 0), &rect);
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
			floodFill(threshImg2,
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
#if CV_MAJOR_VERSION <= 2
				rectangle(bsImg, cv::Point(j, i),
				    cv::Point(j + takenBS_X, i + takenBS_Y),
				    cv::Scalar(0, 0, 0, 0), CV_FILLED);
#else
				rectangle(bsImg, cv::Point(j, i),
				    cv::Point(j + takenBS_X, i + takenBS_Y),
				    cv::Scalar(0, 0, 0, 0), cv::FILLED);
#endif /* CV_MAJOR_VERSION */
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
NFIQ::QualityFeatures::ImgProcROIFeature::isBlackPixelAvailable(
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
