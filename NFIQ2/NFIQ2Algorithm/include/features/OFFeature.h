#ifndef OF_FEATURE_H
#define OF_FEATURE_H

#include <stdio.h>
#include <stdlib.h>

#include "include/FingerprintImageData.h"
#include "include/InterfaceDefinitions.h"
#include "include/features/BaseFeature.h"

#include <list>
#include <string>
#include <vector>

/**
******************************************************************************
* @class OFFeature
* @brief NFIQ2 Orientation Flow Quality Feature
******************************************************************************/

static double OFHISTLIMITS[9] = { 1.715e-2, 3.5e-2, 5.57e-2, 8.1e-2, 1.15e-1,
	1.718e-1, 2.569e-1, 4.758e-1, 7.48e-1 };

class OFFeature : BaseFeature {
    public:
	OFFeature(bool bOutputSpeed,
	    std::list<NFIQ::QualityFeatureSpeed> &speedValues)
	    : BaseFeature(bOutputSpeed, speedValues)
	    , blocksize(16)
	    , slantedBlockSizeX(32)
	    , slantedBlockSizeY(16)
	    , threshold(0.1)
	    , angleMin(4.0) {};
	virtual ~OFFeature();

	virtual std::list<NFIQ::QualityFeatureResult> computeFeatureData(
	    const NFIQ::FingerprintImageData &fingerprintImage);

	virtual std::string getModuleID();

	virtual void initModule() {};

	static std::list<std::string> getAllFeatureIDs();
	static const std::string speedFeatureIDGroup;

    private:
	int blocksize; /*!< Processing is done in subblocks of this size. */
	int slantedBlockSizeX; /*!< Size of the rotated block in the x dimension
				*/
	int slantedBlockSizeY; /*!< Size of the rotated block in the y dimension
				*/
	double threshold;      /*!< Threshold for differentiating
				  foreground/background      blocks */
	double angleMin; /*!< Minimum angle change inclusion in the quality
			    measure */
};

#endif

/******************************************************************************/
