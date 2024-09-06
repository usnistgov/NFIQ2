#ifndef NFIQ2_QUALITYMODULES_RVUPHISTOGRAM_H_
#define NFIQ2_QUALITYMODULES_RVUPHISTOGRAM_H_

#include <nfiq2_constants.hpp>
#include <nfiq2_fingerprintimagedata.hpp>
#include <quality_modules/Module.h>

#include <string>
#include <vector>

namespace NFIQ2 { namespace QualityMeasures {

static double RVUPHISTLIMITS[9] = { 0.5, 0.667, 0.8, 1, 1.25, 1.5, 2, 24, 30 };

class RVUPHistogram : public Algorithm {
    public:
	RVUPHistogram(const NFIQ2::FingerprintImageData &fingerprintImage);
	virtual ~RVUPHistogram();

	std::string getName() const override;

	static std::vector<std::string> getNativeQualityMeasureIDs();

    private:
	std::unordered_map<std::string, double> computeFeatureData(
	    const NFIQ2::FingerprintImageData &fingerprintImage);

	const int blocksize { Sizes::LocalRegionSquare };
	const double threshold { .1 };
	const int slantedBlockSizeX {
		Sizes::VerticallyAlignedLocalRegionWidth
	};
	const int slantedBlockSizeY {
		Sizes::VerticallyAlignedLocalRegionHeight
	};
	const bool padFlag { true };
};

}}

#endif /* NFIQ2_QUALITYMODULES_RVUPHISTOGRAM_H_ */

/******************************************************************************/
