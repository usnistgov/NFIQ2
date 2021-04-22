#ifndef IMGPROCROIFEATURE_H
#define IMGPROCROIFEATURE_H

#include <features/BaseFeature.h>
#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_interfacedefinitions.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <string>
#include <vector>

namespace NFIQ { namespace QualityFeatures {

class ImgProcROIFeature : public BaseFeature {
    public:
	struct ImgProcROIResults {
		/** input block size in pixels */
		unsigned int chosenBlockSize {};
		/**
		 * overall number of complete blocks (with full block size)
		 * in the image
		 */
		unsigned int noOfCompleteBlocks {};
		/** overall number of blocks in the image */
		unsigned int noOfAllBlocks {};
		/** detected ROI blocks with position and size */
		std::vector<cv::Rect> vecROIBlocks {};
		/** number of ROI pixels detected in the image (not blocks) */
		unsigned int noOfROIPixels {};
		/** number of pixels of the image */
		unsigned int noOfImagePixels {};
		/** mean of all grayvalues of all ROI pixels */
		double meanOfROIPixels {};
		/** standard deviation of all grayvalues of all ROI pixels */
		double stdDevOfROIPixels {};
	};

	ImgProcROIFeature(const NFIQ::FingerprintImageData &fingerprintImage);
	virtual ~ImgProcROIFeature();

	std::string getModuleName() const override;

	static std::vector<std::string> getAllFeatureIDs();
	static const std::string speedFeatureIDGroup;
	static const std::string moduleName;

	static ImgProcROIResults computeROI(cv::Mat &img, unsigned int bs);

	/** @throw NFIQ::NFIQException
	 * Img Proc Results could not be computed.
	 */
	ImgProcROIResults getImgProcResults();

    private:
	std::vector<NFIQ::QualityFeatureResult> computeFeatureData(
	    const NFIQ::FingerprintImageData &fingerprintImage);

	ImgProcROIResults imgProcResults_ {};
	bool imgProcComputed_ { false };
	static bool isBlackPixelAvailable(cv::Mat &img, cv::Point &point);
};

}}

#endif

/******************************************************************************/
