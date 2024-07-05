#include "nfiq2_qualityfeatures_impl.hpp"
#include <list>
#include <memory>
#include <string>
#include <vector>

std::vector<std::string>
NFIQ2::QualityMeasures::getActionableQualityFeedbackIDs()
{
	return NFIQ2::QualityMeasures::Impl::getActionableQualityFeedbackIDs();
}

std::vector<std::string>
NFIQ2::QualityMeasures::getQualityFeatureIDs()
{
	return NFIQ2::QualityMeasures::Impl::getQualityFeatureIDs();
}

std::vector<std::string>
NFIQ2::QualityMeasures::getQualityModuleIDs()
{
	return NFIQ2::QualityMeasures::Impl::getQualityModuleIDs();
}

std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
NFIQ2::QualityMeasures::computeQualityModules(
    const NFIQ2::FingerprintImageData &rawImage)
{
	return NFIQ2::QualityMeasures::Impl::computeQualityModules(rawImage);
}

std::unordered_map<std::string, double>
NFIQ2::QualityMeasures::getActionableQualityFeedback(
    const std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	&modules)
{
	return NFIQ2::QualityMeasures::Impl::getActionableQualityFeedback(
	    modules);
}

std::unordered_map<std::string, double>
NFIQ2::QualityMeasures::computeActionableQualityFeedback(
    const NFIQ2::FingerprintImageData &rawImage)
{
	return NFIQ2::QualityMeasures::Impl::computeActionableQualityFeedback(
	    rawImage);
}

std::unordered_map<std::string, double>
NFIQ2::QualityMeasures::getNativeQualityMeasures(
    const std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	&modules)
{
	return NFIQ2::QualityMeasures::Impl::getNativeQualityMeasures(modules);
}

std::unordered_map<std::string, double>
NFIQ2::QualityMeasures::computeQualityMeasures(
    const NFIQ2::FingerprintImageData &rawImage)
{
	return NFIQ2::QualityMeasures::Impl::computeQualityMeasures(rawImage);
}

std::unordered_map<std::string, double>
NFIQ2::QualityMeasures::getQualityModuleSpeeds(
    const std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	&modules)
{
	return NFIQ2::QualityMeasures::Impl::getQualityModuleSpeeds(modules);
}

std::unordered_map<std::string,
    std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
NFIQ2::QualityMeasures::getQualityModules(
    const std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	&modules)
{
	return NFIQ2::QualityMeasures::Impl::getQualityModules(modules);
}
