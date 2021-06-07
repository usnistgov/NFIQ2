#ifndef QUALITYMAPFEATURES_H
#define QUALITYMAPFEATURES_H

#include <features/Module.h>
#include <nfiq2_constants.hpp>
#include <nfiq2_fingerprintimagedata.hpp>
#include <opencv2/core.hpp>

#include "ImgProcROIFeature.h"

#include <string>
#include <vector>

namespace NFIQ2 { namespace QualityFeatures {

#define LOW_FLOW_MAP_NO_DIRECTION 0
#define LOW_FLOW_MAP_LOW_FLOW 127
#define LOW_FLOW_MAP_HIGH_FLOW 255

class QualityMapFeatures : public Module {
    public:
	QualityMapFeatures(const NFIQ2::FingerprintImageData &fingerprintImage,
	    const ImgProcROIFeature::ImgProcROIResults &imgProcResults);
	virtual ~QualityMapFeatures();

	std::string getModuleName() const override;

	static std::vector<std::string> getQualityFeatureIDs();

	// compute orientation angle of a block
	static bool getAngleOfBlock(
	    const cv::Mat &block, double &angle, double &coherence);

	// computes low flow value of block
	static double computeLowFlowBlockValue(const cv::Mat &block);

    private:
	// compute low flow map with ROI filter
	static cv::Mat computeLowFlowMapWithROIFilter(cv::Mat &img,
	    bool bUseSurroundingWindow, unsigned int bs,
	    ImgProcROIFeature::ImgProcROIResults &roiResults,
	    unsigned int &noOfHighFlowBlocks, unsigned int &noOfLowFlowBlocks);

	// compute orientation map
	static cv::Mat computeOrientationMap(cv::Mat &img, bool bFilterByROI,
	    double &coherenceSum, double &coherenceRel, unsigned int bs,
	    ImgProcROIFeature::ImgProcROIResults roiResults);

	// static helper functions for numberical gradient computation
	static cv::Mat computeNumericalGradientX(const cv::Mat &mat);
	static void computeNumericalGradients(
	    const cv::Mat &mat, cv::Mat &grad_x, cv::Mat &grad_y);

    private:
	std::unordered_map<std::string, double> computeFeatureData(
	    const NFIQ2::FingerprintImageData &fingerprintImage);

	ImgProcROIFeature::ImgProcROIResults imgProcResults_ {};
};

}}

#endif

/******************************************************************************/
