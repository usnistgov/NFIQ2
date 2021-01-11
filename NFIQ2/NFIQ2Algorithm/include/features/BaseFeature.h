#ifndef BASEFEATURE_H
#define BASEFEATURE_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <stdlib.h>

#include "include/nfiq2/fingerprintimagedata.h"
#include "include/nfiq2/interfacedefinitions.h"

#include <list>
#include <string>
#include <vector>

class BaseFeature {
    public:
	BaseFeature(bool bOutputSpeed,
	    std::list<NFIQ::QualityFeatureSpeed> &speedValues)
	    : m_bOutputSpeed(bOutputSpeed)
	    , m_lSpeedValues(speedValues) {};
	virtual ~BaseFeature() {};

	bool m_bOutputSpeed;
	std::list<NFIQ::QualityFeatureSpeed> &m_lSpeedValues;
};

#endif

/******************************************************************************/
