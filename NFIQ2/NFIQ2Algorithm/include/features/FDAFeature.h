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

class FDAFeature : BaseFeature {
    public:
	FDAFeature(bool bOutputSpeed,
	    std::vector<NFIQ::QualityFeatureSpeed> &speedValues)
	    : BaseFeature(bOutputSpeed, speedValues)
	    , blocksize(32)
	    , threshold(0.1)
	    , slantedBlockSizeX(32)
	    , slantedBlockSizeY(16)
	    , padFlag(true) {};

	virtual ~FDAFeature();
	virtual std::vector<NFIQ::QualityFeatureResult> computeFeatureData(
	    const NFIQ::FingerprintImageData &fingerprintImage);

	virtual std::string getModuleID();

	virtual void initModule() {};

	static std::vector<std::string> getAllFeatureIDs();
	static const std::string speedFeatureIDGroup;

    private:
	int blocksize;
	double threshold;
	int slantedBlockSizeX, slantedBlockSizeY;
	bool padFlag; // used by getRotatedBlock
};
}}

#endif /* FDAFEATURE_H */
