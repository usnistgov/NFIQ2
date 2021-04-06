#ifndef OCLHISTOGRAMFEATURE_H
#define OCLHISTOGRAMFEATURE_H

#define BS_OCL 32 // block size for OCL

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

static double OCLPHISTLIMITS[9] = { 0.337, 0.479, 0.579, 0.655, 0.716, 0.766,
	0.81, 0.852, 0.898 };

class OCLHistogramFeature : public BaseFeature {
    public:
	OCLHistogramFeature();
	virtual ~OCLHistogramFeature();

	std::vector<NFIQ::QualityFeatureResult> computeFeatureData(
	    const NFIQ::FingerprintImageData &fingerprintImage) override;

	std::string getModuleName() const override;

	static std::vector<std::string> getAllFeatureIDs();
	static const std::string speedFeatureIDGroup;
	static const std::string moduleName;

	// compute OCL value of a given block with block size BSxBS
	static bool getOCLValueOfBlock(const cv::Mat &block, double &ocl);
};

}}

#endif

/******************************************************************************/
