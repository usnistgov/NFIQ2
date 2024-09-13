#ifndef NFIQ2_QUALITYMODULES_FDA_H_
#define NFIQ2_QUALITYMODULES_FDA_H_
#include <nfiq2_constants.hpp>
#include <nfiq2_fingerprintimagedata.hpp>
#include <quality_modules/Module.h>

#include <string>
#include <vector>

/**
******************************************************************************
* @class FDA
* @brief NFIQ2 Frequency Domain Analysis Quality Feature
******************************************************************************/

namespace NFIQ2 { namespace QualityMeasures {

static double FDAHISTLIMITS[9] = { 0.268, 0.304, 0.33, 0.355, 0.38, 0.407, 0.44,
	0.50, 1 };

class FDA : public Algorithm {
    public:
	FDA(const NFIQ2::FingerprintImageData &fingerprintImage);
	virtual ~FDA();

	std::string getName() const override;

	static std::vector<std::string> getNativeQualityMeasureIDs();
	static const char moduleName[];

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
	const bool padFlag { true }; // used by getRotatedBlock
};
}}

#endif /* NFIQ2_QUALITYMODULES_FDA_H_ */
