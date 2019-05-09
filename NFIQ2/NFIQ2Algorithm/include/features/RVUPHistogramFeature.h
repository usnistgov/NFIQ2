#ifndef RVUPHISTOGRAMFEATURE_H
#define RVUPHISTOGRAMFEATURE_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <list>

#include "include/InterfaceDefinitions.h"
#include "include/FingerprintImageData.h"
#include "include/features/BaseFeature.h"

static double RVUPHISTLIMITS[9] = { 0.5, 0.667, 0.8, 1, 1.25, 1.5, 2, 24, 30 };

class RVUPHistogramFeature : BaseFeature
{
public:
	RVUPHistogramFeature(bool bOutputSpeed, std::list<NFIQ::QualityFeatureSpeed> & speedValues)
		: BaseFeature(bOutputSpeed, speedValues)
		, blocksize(32), threshold(0.1), slantedBlockSizeX(32), slantedBlockSizeY(32/2), padFlag(true)
	{
	};
	virtual ~RVUPHistogramFeature();

	virtual std::list<NFIQ::QualityFeatureResult> computeFeatureData(
		const NFIQ::FingerprintImageData & fingerprintImage);

	virtual std::string getModuleID();

	virtual void initModule() {};

	virtual std::list<std::string> getAllFeatureIDs();

protected:
	int blocksize;
	double threshold;
	int slantedBlockSizeX, slantedBlockSizeY;
	int screenRes;
	bool padFlag;
};


#endif

/******************************************************************************/
