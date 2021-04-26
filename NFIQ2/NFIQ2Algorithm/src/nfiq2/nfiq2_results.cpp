#include <nfiq2_results.hpp>

#include "nfiq2_results_impl.hpp"

NFIQ2::NFIQ2Results::NFIQ2Results()
    : pimpl { new NFIQ2::NFIQ2Results::Impl() }
{
}

NFIQ2::NFIQ2Results::NFIQ2Results(
    const std::vector<NFIQ2::ActionableQualityFeedback>
	&actionableQualityFeedback,
    const std::vector<NFIQ2::QualityFeatureData> &qualityFeatureData,
    const std::vector<NFIQ2::QualityFeatureSpeed> &qualityFeatureSpeed,
    const unsigned int qualityScore)
    : pimpl { new NFIQ2::NFIQ2Results::Impl(actionableQualityFeedback,
	  qualityFeatureData, qualityFeatureSpeed, qualityScore) }
{
}

NFIQ2::NFIQ2Results::~NFIQ2Results() = default;

unsigned int
NFIQ2::NFIQ2Results::checkScore(const unsigned int qualityScore)
{
	return NFIQ2::NFIQ2Results::Impl::checkScore(qualityScore);
}

void
NFIQ2::NFIQ2Results::setActionableQualityFeedback(
    const std::vector<NFIQ2::ActionableQualityFeedback> &actionableQuality)
{
	this->pimpl->setActionableQualityFeedback(actionableQuality);
}
void
NFIQ2::NFIQ2Results::setQualityFeatures(
    const std::vector<NFIQ2::QualityFeatureData> &qualityFeatureData)
{
	this->pimpl->setQualityFeatures(qualityFeatureData);
}
void
NFIQ2::NFIQ2Results::setQualityFeatureSpeed(
    const std::vector<NFIQ2::QualityFeatureSpeed> &qualityFeatureSpeed)
{
	this->pimpl->setQualityFeatureSpeed(qualityFeatureSpeed);
}

void
NFIQ2::NFIQ2Results::setScore(const unsigned int qualityScore)
{
	this->pimpl->setScore(qualityScore);
}

std::vector<NFIQ2::ActionableQualityFeedback>
NFIQ2::NFIQ2Results::getActionableQualityFeedback() const
{
	return (this->pimpl->getActionableQualityFeedback());
}
std::vector<NFIQ2::QualityFeatureData>
NFIQ2::NFIQ2Results::getQualityFeatures() const
{
	return (this->pimpl->getQualityFeatures());
}
std::vector<NFIQ2::QualityFeatureSpeed>
NFIQ2::NFIQ2Results::getQualityFeatureSpeed() const
{
	return (this->pimpl->getQualityFeatureSpeed());
}
unsigned int
NFIQ2::NFIQ2Results::getScore() const
{
	return (this->pimpl->getScore());
}
