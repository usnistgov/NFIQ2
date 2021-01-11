#ifndef RVUPHISTOGRAMFEATURE_H
#define RVUPHISTOGRAMFEATURE_H

#include <nfiq2/features/BaseFeature.h>
#include <nfiq2/fingerprintimagedata.hpp>
#include <nfiq2/interfacedefinitions.hpp>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <string>
#include <vector>

static double RVUPHISTLIMITS[9] = { 0.5, 0.667, 0.8, 1, 1.25, 1.5, 2, 24, 30 };

class RVUPHistogramFeature : BaseFeature {
    public:
	RVUPHistogramFeature(bool bOutputSpeed,
	    std::list<NFIQ::QualityFeatureSpeed> &speedValues)
	    : BaseFeature(bOutputSpeed, speedValues)
	    , blocksize(32)
	    , threshold(0.1)
	    , slantedBlockSizeX(32)
	    , slantedBlockSizeY(32 / 2)
	    , padFlag(true) {};
	virtual ~RVUPHistogramFeature();

	virtual std::list<NFIQ::QualityFeatureResult> computeFeatureData(
	    const NFIQ::FingerprintImageData &fingerprintImage);

	virtual std::string getModuleID();

	virtual void initModule() {};

	static std::list<std::string> getAllFeatureIDs();
	static const std::string speedFeatureIDGroup;

    protected:
	int blocksize;
	double threshold;
	int slantedBlockSizeX, slantedBlockSizeY;
	int screenRes;
	bool padFlag;
};

#endif

/******************************************************************************/
