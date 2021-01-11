#ifndef IMGPROCROIFEATURE_H
#define IMGPROCROIFEATURE_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <stdlib.h>

#include "include/nfiq2/FingerprintImageData.h"
#include "include/nfiq2/InterfaceDefinitions.h"
#include "include/features/BaseFeature.h"

#include <list>
#include <string>
#include <vector>

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

	ImgProcROIFeature(bool bOutputSpeed,
	    std::list<NFIQ::QualityFeatureSpeed> &speedValues)
	    : BaseFeature(bOutputSpeed, speedValues) {};
	virtual ~ImgProcROIFeature();

	std::list<NFIQ::QualityFeatureResult> computeFeatureData(
	    const NFIQ::FingerprintImageData &fingerprintImage,
	    ImgProcROIFeature::ImgProcROIResults &imgProcResults);

	std::string getModuleID();

	void initModule() { /* not needed here */ };

	static std::list<std::string> getAllFeatureIDs();
	static const std::string speedFeatureIDGroup;

	static ImgProcROIResults computeROI(cv::Mat &img, unsigned int bs);

    private:
	static bool isBlackPixelAvailable(cv::Mat &img, cv::Point &point);
};

#endif

/******************************************************************************/
