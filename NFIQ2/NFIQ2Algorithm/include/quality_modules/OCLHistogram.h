#ifndef NFIQ2_QUALITYMODULES_OCLHISTOGRAM_H_
#define NFIQ2_QUALITYMODULES_OCLHISTOGRAM_H_

#define BS_OCL NFIQ2::Sizes::LocalRegionSquare // block size for OCL

#include <nfiq2_constants.hpp>
#include <nfiq2_fingerprintimagedata.hpp>
#include <opencv2/core.hpp>
#include <quality_modules/Module.h>

#include <string>
#include <vector>

namespace NFIQ2 { namespace QualityMeasures {

static double OCLPHISTLIMITS[9] = { 0.337, 0.479, 0.579, 0.655, 0.716, 0.766,
	0.81, 0.852, 0.898 };

class OCLHistogram : public Algorithm {
    public:
	OCLHistogram(const NFIQ2::FingerprintImageData &fingerprintImage);
	virtual ~OCLHistogram();

	std::string getName() const override;

	static std::vector<std::string> getNativeQualityMeasureIDs();

	// compute OCL value of a given block with block size BSxBS
	static bool getOCLValueOfBlock(const cv::Mat &block, double &ocl);

    private:
	std::unordered_map<std::string, double> computeFeatureData(
	    const NFIQ2::FingerprintImageData &fingerprintImage);
};

}}

#endif /* NFIQ2_QUALITYMODULES_OCLHISTOGRAM_H_ */

/******************************************************************************/
