#ifndef NFIQ2_QUALITYMODULES_QUALITYMAP_H_
#define NFIQ2_QUALITYMODULES_QUALITYMAP_H_

#include <nfiq2_constants.hpp>
#include <nfiq2_fingerprintimagedata.hpp>
#include <opencv2/core.hpp>
#include <quality_modules/Module.h>

#include "ImgProcROI.h"

#include <string>
#include <vector>

namespace NFIQ2 { namespace QualityMeasures {

#define LOW_FLOW_MAP_NO_DIRECTION 0
#define LOW_FLOW_MAP_LOW_FLOW 127
#define LOW_FLOW_MAP_HIGH_FLOW 255

class QualityMap : public Algorithm {
    public:
	QualityMap(const NFIQ2::FingerprintImageData &fingerprintImage,
	    const ImgProcROI::ImgProcROIResults &imgProcResults);
	virtual ~QualityMap();

	std::string getName() const override;

	static std::vector<std::string> getNativeQualityMeasureIDs();

	// compute orientation angle of a block
	static bool getAngleOfBlock(const cv::Mat &block, double &angle,
	    double &coherence);

	// computes low flow value of block
	static double computeLowFlowBlockValue(const cv::Mat &block);

    private:
	// compute low flow map with ROI filter
	static cv::Mat computeLowFlowMapWithROIFilter(cv::Mat &img,
	    bool bUseSurroundingWindow, unsigned int bs,
	    ImgProcROI::ImgProcROIResults &roiResults,
	    unsigned int &noOfHighFlowBlocks, unsigned int &noOfLowFlowBlocks);

	// compute orientation map
	static cv::Mat computeOrientationMap(cv::Mat &img, bool bFilterByROI,
	    double &coherenceSum, double &coherenceRel, unsigned int bs,
	    ImgProcROI::ImgProcROIResults roiResults);

	// static helper functions for numberical gradient computation
	static cv::Mat computeNumericalGradientX(const cv::Mat &mat);
	static void computeNumericalGradients(const cv::Mat &mat,
	    cv::Mat &grad_x, cv::Mat &grad_y);

    private:
	std::unordered_map<std::string, double> computeFeatureData(
	    const NFIQ2::FingerprintImageData &fingerprintImage);

	ImgProcROI::ImgProcROIResults imgProcResults_ {};
};

}}

#endif

/******************************************************************************/
