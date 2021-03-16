#ifndef OCLHISTOGRAMFEATURE_H
#define OCLHISTOGRAMFEATURE_H

#define BS_OCL 32 // block size for OCL

#include <nfiq2/features/BaseFeature.h>
#include <nfiq2/fingerprintimagedata.hpp>
#include <nfiq2/interfacedefinitions.hpp>
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

class OCLHistogramFeature : BaseFeature {
    public:
	OCLHistogramFeature(bool bOutputSpeed,
	    std::list<NFIQ::QualityFeatureSpeed> &speedValues)
	    : BaseFeature(bOutputSpeed, speedValues) {};
	virtual ~OCLHistogramFeature();

	virtual std::list<NFIQ::QualityFeatureResult> computeFeatureData(
	    const NFIQ::FingerprintImageData &fingerprintImage);

	virtual std::string getModuleID();

	virtual void initModule() { /* not needed here */ };

	static std::list<std::string> getAllFeatureIDs();
	static const std::string speedFeatureIDGroup;

	// compute OCL value of a given block with block size BSxBS
	static bool getOCLValueOfBlock(const cv::Mat &block, double &ocl);
};

}}

#endif

/******************************************************************************/
