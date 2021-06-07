#include <features/BaseFeature.h>
#include <nfiq2_constants.hpp>

#include <vector>

NFIQ2::QualityFeatures::BaseFeature::BaseFeature() = default;

NFIQ2::QualityFeatures::BaseFeature::~BaseFeature() = default;

double
NFIQ2::QualityFeatures::BaseFeature::getSpeed() const
{
	return this->speed;
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::BaseFeature::getFeatures() const
{
	return this->features;
}

void
NFIQ2::QualityFeatures::BaseFeature::setSpeed(const double featureSpeed)
{
	this->speed = featureSpeed;
}

void
NFIQ2::QualityFeatures::BaseFeature::setFeatures(
    const std::unordered_map<std::string, double> &featureResult)
{
	this->features = featureResult;
}
