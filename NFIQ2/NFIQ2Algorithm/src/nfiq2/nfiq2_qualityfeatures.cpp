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
NFIQ2::QualityMeasures::getNativeQualityMeasureAlgorithmIDs()
{
	return NFIQ2::QualityMeasures::Impl::
	    getNativeQualityMeasureAlgorithmIDs();
}

std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
NFIQ2::QualityMeasures::computeNativeQualityMeasures(
    const NFIQ2::FingerprintImageData &rawImage)
{
	return NFIQ2::QualityMeasures::Impl::computeNativeQualityMeasures(
	    rawImage);
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
NFIQ2::QualityMeasures::getNativeQualityMeasureAlgorithmSpeeds(
    const std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	&modules)
{
	return NFIQ2::QualityMeasures::Impl::
	    getNativeQualityMeasureAlgorithmSpeeds(modules);
}

std::unordered_map<std::string,
    std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
NFIQ2::QualityMeasures::getNativeQualityMeasureAlgorithms(
    const std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	&modules)
{
	return NFIQ2::QualityMeasures::Impl::getNativeQualityMeasureAlgorithms(
	    modules);
}
