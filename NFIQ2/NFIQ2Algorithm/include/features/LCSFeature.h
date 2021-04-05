#ifndef LCSFEATURE_H
#define LCSFEATURE_H

#include <features/BaseFeature.h>
#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_interfacedefinitions.hpp>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <string>
#include <vector>

namespace NFIQ { namespace QualityFeatures {

static double LCSHISTLIMITS[9] = { 0, 0.70, 0.74, 0.77, 0.79, 0.81, 0.83, 0.85,
	0.87 };

class LCSFeature : BaseFeature {
    public:
	LCSFeature(bool bOutputSpeed,
	    std::vector<NFIQ::QualityFeatureSpeed> &speedValues)
	    : BaseFeature(bOutputSpeed, speedValues)
	    , blocksize(32)
	    , threshold(0.1)
	    , scannerRes(500)
	    , padFlag(false) {};
	virtual ~LCSFeature();

	virtual std::vector<NFIQ::QualityFeatureResult> computeFeatureData(
	    const NFIQ::FingerprintImageData &fingerprintImage) override;

	std::string getModuleName() const override;

	virtual void initModule() {};

	static std::vector<std::string> getAllFeatureIDs();
	static const std::string speedFeatureIDGroup;
	static const std::string moduleName;

    protected:
	int blocksize;
	double threshold;
	int scannerRes;
	bool padFlag;
};

}}

#endif

/******************************************************************************/
