#include <features/BaseFeature.h>
#include <nfiq2_interfacedefinitions.hpp>

#include <vector>

NFIQ::QualityFeatures::BaseFeature::BaseFeature() = default;

NFIQ::QualityFeatures::BaseFeature::~BaseFeature() = default;

NFIQ::QualityFeatureSpeed
NFIQ::QualityFeatures::BaseFeature::getSpeed() const
{
	return this->speed;
}

std::vector<NFIQ::QualityFeatureResult>
NFIQ::QualityFeatures::BaseFeature::getFeatures() const
{
	return this->features;
}

void
NFIQ::QualityFeatures::BaseFeature::setSpeed(
    const NFIQ::QualityFeatureSpeed &featureSpeed)
{
	this->speed = featureSpeed;
}

void
NFIQ::QualityFeatures::BaseFeature::setFeatures(
    const std::vector<NFIQ::QualityFeatureResult> &featureResult)
{
	this->features = featureResult;
}