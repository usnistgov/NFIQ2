#ifndef OF_FEATURE_H
#define OF_FEATURE_H

#include <nfiq2_constants.hpp>
#include <nfiq2_fingerprintimagedata.hpp>
#include <quality_modules/Module.h>

#include <string>
#include <vector>

/**
******************************************************************************
* @class OF
* @brief NFIQ2 Orientation Flow Quality Feature
******************************************************************************/

namespace NFIQ2 { namespace QualityMeasures {

static double OFHISTLIMITS[9] = { 1.715e-2, 3.5e-2, 5.57e-2, 8.1e-2, 1.15e-1,
	1.718e-1, 2.569e-1, 4.758e-1, 7.48e-1 };

class OF : public Algorithm {
    public:
	OF(const NFIQ2::FingerprintImageData &fingerprintImage);
	virtual ~OF();

	std::string getName() const override;

	static std::vector<std::string> getNativeQualityMeasureIDs();

	/**Minimum angle change inclusion in the quality measure */
	static constexpr double angleMin { 4.0 };

    private:
	std::unordered_map<std::string, double> computeFeatureData(
	    const NFIQ2::FingerprintImageData &fingerprintImage);

	/** Processing is done in subblocks of this size. */
	const int blocksize { Sizes::LocalRegionSquare };
	/** Size of the rotated block in the x dimension */
	const int slantedBlockSizeX {
		Sizes::VerticallyAlignedLocalRegionWidth
	};
	/** Size of the rotated block in the y dimension */
	const int slantedBlockSizeY {
		Sizes::VerticallyAlignedLocalRegionHeight
	};
	/**Threshold for differentiating foreground/background blocks */
	const double threshold { .1 };
};
}}

#endif

/******************************************************************************/
