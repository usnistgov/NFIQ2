#ifndef FDAFEATURE_H
#define FDAFEATURE_H
#include <features/Module.h>
#include <nfiq2_constants.hpp>
#include <nfiq2_fingerprintimagedata.hpp>

#include <string>
#include <vector>

/**
******************************************************************************
* @class FDAFeature
* @brief NFIQ2 Frequency Domain Analysis Quality Feature
******************************************************************************/

namespace NFIQ2 { namespace QualityFeatures {

static double FDAHISTLIMITS[9] = { 0.268, 0.304, 0.33, 0.355, 0.38, 0.407, 0.44,
	0.50, 1 };

class FDAFeature : public Module {
    public:
	FDAFeature(const NFIQ2::FingerprintImageData &fingerprintImage);
	virtual ~FDAFeature();

	std::string getModuleName() const override;

	static std::vector<std::string> getQualityFeatureIDs();
	static const char moduleName[];

    private:
	std::unordered_map<std::string, double> computeFeatureData(
	    const NFIQ2::FingerprintImageData &fingerprintImage);

	const int blocksize { 32 };
	const double threshold { .1 };
	const int slantedBlockSizeX { 32 };
	const int slantedBlockSizeY { 16 };
	const bool padFlag { true }; // used by getRotatedBlock
};
}}

#endif /* FDAFEATURE_H */
