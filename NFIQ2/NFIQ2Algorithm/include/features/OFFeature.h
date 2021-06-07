#ifndef OF_FEATURE_H
#define OF_FEATURE_H

#include <features/Module.h>
#include <nfiq2_constants.hpp>
#include <nfiq2_fingerprintimagedata.hpp>

#include <string>
#include <vector>

/**
******************************************************************************
* @class OFFeature
* @brief NFIQ2 Orientation Flow Quality Feature
******************************************************************************/

namespace NFIQ2 { namespace QualityFeatures {

static double OFHISTLIMITS[9] = { 1.715e-2, 3.5e-2, 5.57e-2, 8.1e-2, 1.15e-1,
	1.718e-1, 2.569e-1, 4.758e-1, 7.48e-1 };

class OFFeature : public Module {
    public:
	OFFeature(const NFIQ2::FingerprintImageData &fingerprintImage);
	virtual ~OFFeature();

	std::string getModuleName() const override;

	static std::vector<std::string> getQualityFeatureIDs();

    private:
	std::unordered_map<std::string, double> computeFeatureData(
	    const NFIQ2::FingerprintImageData &fingerprintImage);

	/** Processing is done in subblocks of this size. */
	const int blocksize { 16 };
	/** Size of the rotated block in the x dimension */
	const int slantedBlockSizeX { 32 };
	/** Size of the rotated block in the y dimension */
	const int slantedBlockSizeY { 16 };
	/**Threshold for differentiating foreground/background blocks */
	const double threshold { .1 };
	/**Minimum angle change inclusion in the quality measure */
	const double angleMin { 4.0 };
};
}}

#endif

/******************************************************************************/
