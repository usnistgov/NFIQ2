#include <features/Module.h>
#include <nfiq2_constants.hpp>

#include <vector>

NFIQ2::QualityFeatures::Module::Module() = default;

NFIQ2::QualityFeatures::Module::~Module() = default;

double
NFIQ2::QualityFeatures::Module::getSpeed() const
{
	return this->speed;
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::Module::getFeatures() const
{
	return this->features;
}

void
NFIQ2::QualityFeatures::Module::setSpeed(const double featureSpeed)
{
	this->speed = featureSpeed;
}

void
NFIQ2::QualityFeatures::Module::setFeatures(
    const std::unordered_map<std::string, double> &featureResult)
{
	this->features = featureResult;
}
