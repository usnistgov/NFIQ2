#ifndef BASEFEATURE_H
#define BASEFEATURE_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <list>

#include <stdint.h>
#include <InterfaceDefinitions.h>
#include <FingerprintImageData.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class BaseFeature
{

public:
	BaseFeature(bool bOutputSpeed, std::list<NFIQ::QualityFeatureSpeed> & speedValues)
		: m_lSpeedValues(speedValues)
		, m_bOutputSpeed(bOutputSpeed)
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
