#ifndef RVUPHISTOGRAMFEATURE_H
#define RVUPHISTOGRAMFEATURE_H

#include <features/BaseFeature.h>
#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_interfacedefinitions.hpp>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <string>
#include <vector>

namespace NFIQ { namespace QualityFeatures {

static double RVUPHISTLIMITS[9] = { 0.5, 0.667, 0.8, 1, 1.25, 1.5, 2, 24, 30 };

class RVUPHistogramFeature : BaseFeature {
    public:
	RVUPHistogramFeature()
	    : BaseFeature()
	    , blocksize(32)
	    , threshold(0.1)
	    , slantedBlockSizeX(32)
	    , slantedBlockSizeY(32 / 2)
	    , padFlag(true) {};
	virtual ~RVUPHistogramFeature();

	std::vector<NFIQ::QualityFeatureResult> computeFeatureData(
	    const NFIQ::FingerprintImageData &fingerprintImage) override;

	std::vector<NFIQ::QualityFeatureSpeed> getSpeedValues() const override;

	std::string getModuleName() const override;

	virtual void initModule() {};

	static std::vector<std::string> getAllFeatureIDs();
	static const std::string speedFeatureIDGroup;
	static const std::string moduleName;

    protected:
	int blocksize;
	double threshold;
	int slantedBlockSizeX, slantedBlockSizeY;
	int screenRes;
	bool padFlag;
};

}}

#endif

/******************************************************************************/
