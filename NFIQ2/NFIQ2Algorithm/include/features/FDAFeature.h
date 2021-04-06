#ifndef FDAFEATURE_H
#define FDAFEATURE_H
#include <features/BaseFeature.h>
#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_interfacedefinitions.hpp>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <string>
#include <vector>

/**
******************************************************************************
* @class FDAFeature
* @brief NFIQ2 Frequency Domain Analysis Quality Feature
******************************************************************************/

namespace NFIQ { namespace QualityFeatures {

static double FDAHISTLIMITS[9] = { 0.268, 0.304, 0.33, 0.355, 0.38, 0.407, 0.44,
	0.50, 1 };

class FDAFeature : public BaseFeature {
    public:
	FDAFeature();
	virtual ~FDAFeature();

	std::vector<NFIQ::QualityFeatureResult> computeFeatureData(
	    const NFIQ::FingerprintImageData &fingerprintImage) override;

	std::string getModuleName() const override;

	static std::vector<std::string> getAllFeatureIDs();
	static const std::string speedFeatureIDGroup;
	static const std::string moduleName;

    private:
	const int blocksize { 32 };
	const double threshold { .1 };
	const int slantedBlockSizeX { 32 };
	const int slantedBlockSizeY { 16 };
	const bool padFlag { true }; // used by getRotatedBlock
};
}}

#endif /* FDAFEATURE_H */
