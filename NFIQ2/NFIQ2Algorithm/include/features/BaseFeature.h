#ifndef BASEFEATURE_H
#define BASEFEATURE_H

#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_interfacedefinitions.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <string>
#include <vector>

namespace NFIQ { namespace QualityFeatures {

class BaseFeature {
    public:
	BaseFeature(bool bOutputSpeed,
	    std::vector<NFIQ::QualityFeatureSpeed> &speedValues)
	    : m_bOutputSpeed(bOutputSpeed)
	    , m_lSpeedValues(speedValues) {};

	BaseFeature() {};

	virtual ~BaseFeature() {};

	/** @return Identifier for this particular feature */
	virtual std::string getModuleName() const = 0;

	/** @return vector of computed quality features */
	virtual std::vector<NFIQ::QualityFeatureResult> computeFeatureData(
	    const NFIQ::FingerprintImageData &fingerprintImage) = 0;

	/** @return vector of computed speed values */
	virtual std::vector<NFIQ::QualityFeatureSpeed>
	getSpeedValues() const = 0;

	bool m_bOutputSpeed;
	std::vector<NFIQ::QualityFeatureSpeed> m_lSpeedValues {};
};

}}

#endif

/******************************************************************************/
