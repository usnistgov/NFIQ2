#include "nfiq2_qualityfeatures_impl.h"

#include <list>
#include <string>
#include <vector>

std::vector<std::string>
NFIQ::QualityFeatures::getAllActionableIdentifiers()
{
	return NFIQ::QualityFeatures::Impl::getAllActionableIdentifiers();
}

std::vector<std::string>
NFIQ::QualityFeatures::getAllQualityFeatureIDs()
{
	return NFIQ::QualityFeatures::Impl::getAllQualityFeatureIDs();
}

std::vector<std::string>
NFIQ::QualityFeatures::getAllSpeedFeatureGroups()
{
	return NFIQ::QualityFeatures::Impl::getAllSpeedFeatureGroups();
}

std::list<NFIQ::QualityFeatureData>
NFIQ::QualityFeatures::computeQualityFeatures(
    const NFIQ::FingerprintImageData &rawImage, bool bComputeActionableQuality,
    std::list<NFIQ::ActionableQualityFeedback> &actionableQuality,
    bool bOutputSpeed, std::list<NFIQ::QualityFeatureSpeed> &speedValues)
{
	return NFIQ::QualityFeatures::Impl::computeQualityFeatures(rawImage,
	    bComputeActionableQuality, actionableQuality, bOutputSpeed,
	    speedValues);
}
