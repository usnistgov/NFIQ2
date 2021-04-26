#include "nfiq2_qualityfeatures_impl.hpp"
#include <list>
#include <memory>
#include <string>
#include <vector>

std::vector<std::string>
NFIQ2::QualityFeatures::getAllActionableIdentifiers()
{
	return NFIQ2::QualityFeatures::Impl::getAllActionableIdentifiers();
}

std::vector<std::string>
NFIQ2::QualityFeatures::getAllQualityFeatureIDs()
{
	return NFIQ2::QualityFeatures::Impl::getAllQualityFeatureIDs();
}

std::vector<std::string>
NFIQ2::QualityFeatures::getAllSpeedFeatureGroups()
{
	return NFIQ2::QualityFeatures::Impl::getAllSpeedFeatureGroups();
}

std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
NFIQ2::QualityFeatures::computeQualityFeatures(
    const NFIQ2::FingerprintImageData &rawImage)
{
	return NFIQ2::QualityFeatures::Impl::computeQualityFeatures(rawImage);
}

std::unordered_map<std::string, NFIQ2::ActionableQualityFeedback>
NFIQ2::QualityFeatures::getActionableQualityFeedback(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
	&features)
{
	return NFIQ2::QualityFeatures::Impl::getActionableQualityFeedback(
	    features);
}
std::unordered_map<std::string, NFIQ2::ActionableQualityFeedback>
NFIQ2::QualityFeatures::getActionableQualityFeedback(
    const NFIQ2::FingerprintImageData &rawImage)
{
	return NFIQ2::QualityFeatures::Impl::getActionableQualityFeedback(
	    rawImage);
}

std::unordered_map<std::string, NFIQ2::QualityFeatureData>
NFIQ2::QualityFeatures::getQualityFeatureData(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
	&features)
{
	return NFIQ2::QualityFeatures::Impl::getQualityFeatureData(features);
}

std::unordered_map<std::string, NFIQ2::QualityFeatureData>
NFIQ2::QualityFeatures::getQualityFeatureData(
    const NFIQ2::FingerprintImageData &rawImage)
{
	return NFIQ2::QualityFeatures::Impl::getQualityFeatureData(rawImage);
}

std::unordered_map<std::string, NFIQ2::QualityFeatureSpeed>
NFIQ2::QualityFeatures::getQualityFeatureSpeeds(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
	&features)
{
	return NFIQ2::QualityFeatures::Impl::getQualityFeatureSpeeds(features);
}
