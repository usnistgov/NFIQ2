#include <features/BaseFeature.h>
#include <nfiq2_interfacedefinitions.hpp>

#include <vector>

NFIQ::QualityFeatures::BaseFeature::BaseFeature() = default;

NFIQ::QualityFeatures::BaseFeature::~BaseFeature() {};

std::vector<NFIQ::QualityFeatureSpeed>
NFIQ::QualityFeatures::BaseFeature::getSpeedValues() const
{
	return this->speedValues;
}

void
NFIQ::QualityFeatures::BaseFeature::appendSpeedValues(
    const NFIQ::QualityFeatureSpeed &speedFeature)
{
	this->speedValues.push_back(speedFeature);
}
