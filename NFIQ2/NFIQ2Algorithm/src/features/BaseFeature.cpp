#include <features/BaseFeature.h>
#include <nfiq2_interfacedefinitions.hpp>

#include <vector>

NFIQ::QualityFeatures::BaseFeature::BaseFeature() = default;

NFIQ::QualityFeatures::BaseFeature::~BaseFeature() {};

NFIQ::QualityFeatureSpeed
NFIQ::QualityFeatures::BaseFeature::getSpeed() const
{
	return this->speed;
}

void
NFIQ::QualityFeatures::BaseFeature::setSpeed(
    const NFIQ::QualityFeatureSpeed &featureSpeed)
{
	this->speed = featureSpeed;
}
