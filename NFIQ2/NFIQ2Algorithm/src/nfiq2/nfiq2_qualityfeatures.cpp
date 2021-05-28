#include "nfiq2_qualityfeatures_impl.hpp"
#include <list>
#include <memory>
#include <string>
#include <vector>

std::vector<std::string>
NFIQ2::QualityFeatures::getAllActionableQualityFeedbackIDs()
{
	return NFIQ2::QualityFeatures::Impl::
	    getAllActionableQualityFeedbackIDs();
}

std::vector<std::string>
NFIQ2::QualityFeatures::getAllQualityFeatureIDs()
{
	return NFIQ2::QualityFeatures::Impl::getAllQualityFeatureIDs();
}

std::vector<std::string>
NFIQ2::QualityFeatures::getAllQualityModuleIDs()
{
	return NFIQ2::QualityFeatures::Impl::getAllQualityModuleIDs();
}

std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
NFIQ2::QualityFeatures::computeQualityFeatures(
    const NFIQ2::FingerprintImageData &rawImage)
{
	return NFIQ2::QualityFeatures::Impl::computeQualityFeatures(rawImage);
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::getActionableQualityFeedback(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
	&features)
{
	return NFIQ2::QualityFeatures::Impl::getActionableQualityFeedback(
	    features);
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::getActionableQualityFeedback(
    const NFIQ2::FingerprintImageData &rawImage)
{
	return NFIQ2::QualityFeatures::Impl::getActionableQualityFeedback(
	    rawImage);
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::getQualityFeatureData(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
	&features)
{
	return NFIQ2::QualityFeatures::Impl::getQualityFeatureData(features);
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::getQualityFeatureData(
    const NFIQ2::FingerprintImageData &rawImage)
{
	return NFIQ2::QualityFeatures::Impl::getQualityFeatureData(rawImage);
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::getQualityFeatureSpeeds(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
	&features)
{
	return NFIQ2::QualityFeatures::Impl::getQualityFeatureSpeeds(features);
}
