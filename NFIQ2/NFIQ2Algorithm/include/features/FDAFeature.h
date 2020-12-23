#ifndef FDAFEATURE_H
#define FDAFEATURE_H
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <string>
#include <vector>

#include "include/FingerprintImageData.h"
#include "include/InterfaceDefinitions.h"
#include "include/features/BaseFeature.h"

/**
******************************************************************************
* @class FDAFeature
* @brief NFIQ2 Frequency Domain Analysis Quality Feature
******************************************************************************/

static double FDAHISTLIMITS[9] = {0.268, 0.304, 0.33, 0.355, 0.38,
                                  0.407, 0.44,  0.50, 1};

class FDAFeature : BaseFeature {
   public:
    FDAFeature(bool bOutputSpeed,
               std::list<NFIQ::QualityFeatureSpeed>& speedValues)
        : BaseFeature(bOutputSpeed, speedValues),
          blocksize(32),
          threshold(0.1),
          slantedBlockSizeX(32),
          slantedBlockSizeY(16),
          padFlag(true){};

    virtual ~FDAFeature();
    virtual std::list<NFIQ::QualityFeatureResult>
    computeFeatureData(const NFIQ::FingerprintImageData& fingerprintImage);

    virtual std::string
    getModuleID();

    virtual void
    initModule(){};

    static std::list<std::string>
    getAllFeatureIDs();
    static const std::string speedFeatureIDGroup;

   private:
    int blocksize;
    double threshold;
    int slantedBlockSizeX, slantedBlockSizeY;
    bool padFlag;  // used by getRotatedBlock
};

#endif /* FDAFEATURE_H */
