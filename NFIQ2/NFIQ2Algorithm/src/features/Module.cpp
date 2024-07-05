#include <features/Module.h>
#include <nfiq2_constants.hpp>

#include <vector>

NFIQ2::QualityMeasures::Algorithm::Algorithm() = default;

NFIQ2::QualityMeasures::Algorithm::~Algorithm() = default;

double
NFIQ2::QualityMeasures::Algorithm::getSpeed() const
{
	return this->speed;
}

std::unordered_map<std::string, double>
NFIQ2::QualityMeasures::Algorithm::getFeatures() const
{
	return this->features;
}

void
NFIQ2::QualityMeasures::Algorithm::setSpeed(const double featureSpeed)
{
	this->speed = featureSpeed;
}

void
NFIQ2::QualityMeasures::Algorithm::setFeatures(
    const std::unordered_map<std::string, double> &featureResult)
{
	this->features = featureResult;
}
