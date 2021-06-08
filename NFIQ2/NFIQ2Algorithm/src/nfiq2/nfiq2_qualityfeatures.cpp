#include "nfiq2_qualityfeatures_impl.hpp"
#include <list>
#include <memory>
#include <string>
#include <vector>

std::vector<std::string>
NFIQ2::QualityFeatures::getActionableQualityFeedbackIDs()
{
	return NFIQ2::QualityFeatures::Impl::getActionableQualityFeedbackIDs();
}

std::vector<std::string>
NFIQ2::QualityFeatures::getQualityFeatureIDs()
{
	return NFIQ2::QualityFeatures::Impl::getQualityFeatureIDs();
}

std::vector<std::string>
NFIQ2::QualityFeatures::getQualityModuleIDs()
{
	return NFIQ2::QualityFeatures::Impl::getQualityModuleIDs();
}

std::vector<std::shared_ptr<NFIQ2::QualityFeatures::Module>>
NFIQ2::QualityFeatures::computeQualityModules(
    const NFIQ2::FingerprintImageData &rawImage)
{
	return NFIQ2::QualityFeatures::Impl::computeQualityModules(rawImage);
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::getActionableQualityFeedback(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::Module>> &modules)
{
	return NFIQ2::QualityFeatures::Impl::getActionableQualityFeedback(
	    modules);
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::computeActionableQualityFeedback(
    const NFIQ2::FingerprintImageData &rawImage)
{
	return NFIQ2::QualityFeatures::Impl::computeActionableQualityFeedback(
	    rawImage);
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::getQualityFeatureValues(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::Module>> &modules)
{
	return NFIQ2::QualityFeatures::Impl::getQualityFeatureValues(modules);
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::computeQualityFeatures(
    const NFIQ2::FingerprintImageData &rawImage)
{
	return NFIQ2::QualityFeatures::Impl::computeQualityFeatures(rawImage);
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::getQualityModuleSpeeds(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::Module>> &modules)
{
	return NFIQ2::QualityFeatures::Impl::getQualityModuleSpeeds(modules);
}

std::unordered_map<std::string, std::shared_ptr<NFIQ2::QualityFeatures::Module>>
NFIQ2::QualityFeatures::getQualityModules(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::Module>> &modules)
{
	return NFIQ2::QualityFeatures::Impl::getQualityModules(modules);
}
