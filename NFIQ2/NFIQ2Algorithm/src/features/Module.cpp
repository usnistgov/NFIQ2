#include <features/Module.h>
#include <nfiq2_constants.hpp>

#include <vector>

NFIQ2::QualityMeasures::Module::Module() = default;

NFIQ2::QualityMeasures::Module::~Module() = default;

double
NFIQ2::QualityMeasures::Module::getSpeed() const
{
	return this->speed;
}

std::unordered_map<std::string, double>
NFIQ2::QualityMeasures::Module::getFeatures() const
{
	return this->features;
}

void
NFIQ2::QualityMeasures::Module::setSpeed(const double featureSpeed)
{
	this->speed = featureSpeed;
}

void
NFIQ2::QualityMeasures::Module::setFeatures(
    const std::unordered_map<std::string, double> &featureResult)
{
	this->features = featureResult;
}
