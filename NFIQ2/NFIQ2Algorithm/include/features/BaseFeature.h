#ifndef BASEFEATURE_H
#define BASEFEATURE_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <list>

#include "include/InterfaceDefinitions.h"
#include "include/FingerprintImageData.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class BaseFeature
{

public:
	BaseFeature(bool bOutputSpeed, std::list<NFIQ::QualityFeatureSpeed> & speedValues)
		: m_bOutputSpeed(bOutputSpeed)
		, m_lSpeedValues(speedValues)
	{
	};
	virtual ~BaseFeature()
	{
	};

	bool m_bOutputSpeed;
	std::list<NFIQ::QualityFeatureSpeed> & m_lSpeedValues;
};


#endif

/******************************************************************************/
