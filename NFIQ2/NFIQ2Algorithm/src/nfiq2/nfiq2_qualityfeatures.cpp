#include "nfiq2_qualityfeatures_impl.h"

#include <list>
#include <memory>
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

std::vector<std::shared_ptr<NFIQ::QualityFeatures::BaseFeature>>
NFIQ::QualityFeatures::computeQualityFeatures(
    const NFIQ::FingerprintImageData &rawImage)
{
	return NFIQ::QualityFeatures::Impl::computeQualityFeatures(rawImage);
}

std::vector<NFIQ::ActionableQualityFeedback>
NFIQ::QualityFeatures::getActionableQualityFeedback(
    const std::vector<std::shared_ptr<NFIQ::QualityFeatures::BaseFeature>>
	&features)
{
	return NFIQ::QualityFeatures::Impl::getActionableQualityFeedback(
	    features);
}

std::vector<NFIQ::ActionableQualityFeedback>
NFIQ::QualityFeatures::getActionableQualityFeedback(
    const NFIQ::FingerprintImageData &rawImage)
{
	return NFIQ::QualityFeatures::Impl::getActionableQualityFeedback(
	    rawImage);
}

std::vector<NFIQ::QualityFeatureData>
NFIQ::QualityFeatures::getQualityFeatureData(
    const std::vector<std::shared_ptr<NFIQ::QualityFeatures::BaseFeature>>
	&features)
{
	return NFIQ::QualityFeatures::Impl::getQualityFeatureData(features);
}

std::vector<NFIQ::QualityFeatureData>
NFIQ::QualityFeatures::getQualityFeatureData(
    const NFIQ::FingerprintImageData &rawImage)
{
	return NFIQ::QualityFeatures::Impl::getQualityFeatureData(rawImage);
}

std::vector<NFIQ::QualityFeatureSpeed>
NFIQ::QualityFeatures::getQualityFeatureSpeeds(
    const std::vector<std::shared_ptr<NFIQ::QualityFeatures::BaseFeature>>
	&features)
{
	return NFIQ::QualityFeatures::Impl::getQualityFeatureSpeeds(features);
}
