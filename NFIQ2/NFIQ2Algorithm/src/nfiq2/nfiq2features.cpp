#include "nfiq2featuresimpl.h"

#include <list>
#include <string>
#include <vector>

std::vector<std::string>
NFIQ::FeatureCalculation::getAllActionableIdentifiers()
{
	return NFIQ::FeatureCalculation::Impl::getAllActionableIdentifiers();
}

std::vector<std::string>
NFIQ::FeatureCalculation::getAllQualityFeatureIDs()
{
	return NFIQ::FeatureCalculation::Impl::getAllQualityFeatureIDs();
}

std::vector<std::string>
NFIQ::FeatureCalculation::getAllSpeedFeatureGroups()
{
	return NFIQ::FeatureCalculation::Impl::getAllSpeedFeatureGroups();
}

std::list<NFIQ::QualityFeatureData>
NFIQ::FeatureCalculation::computeQualityFeatures(
    const NFIQ::FingerprintImageData &rawImage, bool bComputeActionableQuality,
    std::list<NFIQ::ActionableQualityFeedback> &actionableQuality,
    bool bOutputSpeed, std::list<NFIQ::QualityFeatureSpeed> &speedValues)
{
	return NFIQ::FeatureCalculation::Impl::computeQualityFeatures(rawImage,
	    bComputeActionableQuality, actionableQuality, bOutputSpeed,
	    speedValues);
}
