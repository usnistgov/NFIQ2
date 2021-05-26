#include <features/BaseFeature.h>
#include <nfiq2_interfacedefinitions.hpp>

#include <vector>

NFIQ2::QualityFeatures::BaseFeature::BaseFeature() = default;

NFIQ2::QualityFeatures::BaseFeature::~BaseFeature() = default;

NFIQ2::QualityFeatureSpeed
NFIQ2::QualityFeatures::BaseFeature::getSpeed() const
{
	return this->speed;
}

std::vector<NFIQ2::QualityFeatureData>
NFIQ2::QualityFeatures::BaseFeature::getFeatures() const
{
	return this->features;
}

void
NFIQ2::QualityFeatures::BaseFeature::setSpeed(
    const NFIQ2::QualityFeatureSpeed &featureSpeed)
{
	this->speed = featureSpeed;
}

void
NFIQ2::QualityFeatures::BaseFeature::setFeatures(
    const std::vector<NFIQ2::QualityFeatureData> &featureResult)
{
	this->features = featureResult;
}
