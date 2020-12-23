#ifndef MUFEATURE_H
#define MUFEATURE_H

#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <vector>

#include "include/FingerprintImageData.h"
#include "include/InterfaceDefinitions.h"
#include "include/features/BaseFeature.h"

class MuFeature : BaseFeature {
   public:
    MuFeature(bool bOutputSpeed,
              std::list<NFIQ::QualityFeatureSpeed>& speedValues)
        : BaseFeature(bOutputSpeed, speedValues){};
    virtual ~MuFeature();

    std::list<NFIQ::QualityFeatureResult>
    computeFeatureData(const NFIQ::FingerprintImageData& fingerprintImage,
                       double& sigma);

    std::string
    getModuleID();

    void
    initModule(){/* not needed here */};

    static std::list<std::string>
    getAllFeatureIDs();
    static const std::string speedFeatureIDGroup;
};

#endif

/******************************************************************************/
