#ifndef LCSFEATURE_H
#define LCSFEATURE_H

#include <features/BaseFeature.h>
#include <nfiq2/fingerprintimagedata.h>
#include <nfiq2/interfacedefinitions.h>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <string>
#include <vector>

static double LCSHISTLIMITS[9] = { 0, 0.70, 0.74, 0.77, 0.79, 0.81, 0.83, 0.85,
	0.87 };

class LCSFeature : BaseFeature {
    public:
	LCSFeature(bool bOutputSpeed,
	    std::list<NFIQ::QualityFeatureSpeed> &speedValues)
	    : BaseFeature(bOutputSpeed, speedValues)
	    , blocksize(32)
	    , threshold(0.1)
	    , scannerRes(500)
	    , padFlag(false) {};
	virtual ~LCSFeature();

	virtual std::list<NFIQ::QualityFeatureResult> computeFeatureData(
	    const NFIQ::FingerprintImageData &fingerprintImage);

	virtual std::string getModuleID();

	virtual void initModule() {};

	static std::list<std::string> getAllFeatureIDs();
	static const std::string speedFeatureIDGroup;

    protected:
	int blocksize;
	double threshold;
	int scannerRes;
	bool padFlag;
};

#endif

/******************************************************************************/
