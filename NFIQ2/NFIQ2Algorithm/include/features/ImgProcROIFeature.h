#ifndef IMGPROCROIFEATURE_H
#define IMGPROCROIFEATURE_H

#include <features/BaseFeature.h>
#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_interfacedefinitions.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <string>
#include <vector>

namespace NFIQ { namespace QualityFeatures {

class ImgProcROIFeature : BaseFeature {
    public:
	struct ImgProcROIResults {
		unsigned int
		    chosenBlockSize; ///< the input block size in pixels
		unsigned int noOfCompleteBlocks; ///< the overall number of
						 ///< complete blocks (with full
						 ///< block size) in the image
		unsigned int noOfAllBlocks; ///< the overall number of blocks
					    ///< in the image
		std::vector<cv::Rect> vecROIBlocks; ///< the detected ROI blocks
						    ///< with position and size
		unsigned int
		    noOfROIPixels; ///< the number of ROI pixels detected
				   ///< in the image (not blocks)
		unsigned int
		    noOfImagePixels;	  ///< the number of pixels of the image
		double meanOfROIPixels;	  ///< the mean of all grayvalues of all
					  ///< ROI pixels
		double stdDevOfROIPixels; ///< the standard deviation of all
					  ///< grayvalues of all ROI pixels
	};

	ImgProcROIFeature()
	    : BaseFeature() {};
	virtual ~ImgProcROIFeature();

	std::vector<NFIQ::QualityFeatureResult> computeFeatureData(
	    const NFIQ::FingerprintImageData &fingerprintImage) override;

	std::vector<NFIQ::QualityFeatureSpeed> getSpeedValues() const override;

	std::string getModuleName() const override;

	void initModule() { /* not needed here */ };

	static std::vector<std::string> getAllFeatureIDs();
	static const std::string speedFeatureIDGroup;
	static const std::string moduleName;

	static ImgProcROIResults computeROI(cv::Mat &img, unsigned int bs);

	/** @throw NFIQ::NFIQException
	 * Img Proc Results could not be computed.
	 */
	ImgProcROIResults getImgProcResults();

    private:
	ImgProcROIResults imgProcResults_ {};
	bool imgProcComputed_ { false };
	static bool isBlackPixelAvailable(cv::Mat &img, cv::Point &point);
};

}}

#endif

/******************************************************************************/
