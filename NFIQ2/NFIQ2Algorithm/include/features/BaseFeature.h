#ifndef BASEFEATURE_H
#define BASEFEATURE_H

#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_interfacedefinitions.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <list>
#include <string>
#include <vector>

namespace NFIQ { namespace QualityFeatures {

class BaseFeature {
    public:
	BaseFeature();

	virtual ~BaseFeature();

	/** @return Identifier for this particular feature */
	virtual std::string getModuleName() const = 0;

	/** @return computed quality feature speed */
	virtual NFIQ::QualityFeatureSpeed getSpeed() const;

	/** @return computed quality features */
	virtual std::vector<NFIQ::QualityFeatureResult> getFeatures() const;

    protected:
	void setSpeed(const NFIQ::QualityFeatureSpeed &featureSpeed);

	void setFeatures(
	    const std::vector<NFIQ::QualityFeatureResult> &featureResult);

    private:
	NFIQ::QualityFeatureSpeed speed {};

	std::vector<NFIQ::QualityFeatureResult> features {};
};

}}

#endif

/******************************************************************************/
