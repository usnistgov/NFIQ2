#include <nfiq2_results.hpp>

#include "nfiq2_results_impl.hpp"

NFIQ::NFIQ2Results::NFIQ2Results()
    : pimpl { new NFIQ::NFIQ2Results::Impl() }
{
}

NFIQ::NFIQ2Results::NFIQ2Results(
    const std::vector<NFIQ::ActionableQualityFeedback>
	&actionableQualityFeedback,
    const std::vector<NFIQ::QualityFeatureData> &qualityFeatureData,
    const std::vector<NFIQ::QualityFeatureSpeed> &qualityFeatureSpeed,
    const unsigned int qualityScore)
    : pimpl { new NFIQ::NFIQ2Results::Impl(actionableQualityFeedback,
	  qualityFeatureData, qualityFeatureSpeed, qualityScore) }
{
}

NFIQ::NFIQ2Results::~NFIQ2Results() = default;

unsigned int
NFIQ::NFIQ2Results::checkScore(const unsigned int qualityScore)
{
	return NFIQ::NFIQ2Results::Impl::checkScore(qualityScore);
}

void
NFIQ::NFIQ2Results::setActionableQualityFeedback(
    const std::vector<NFIQ::ActionableQualityFeedback> &actionableQuality)
{
	this->pimpl->setActionableQualityFeedback(actionableQuality);
}
void
NFIQ::NFIQ2Results::setQualityFeatures(
    const std::vector<NFIQ::QualityFeatureData> &qualityFeatureData)
{
	this->pimpl->setQualityFeatures(qualityFeatureData);
}
void
NFIQ::NFIQ2Results::setQualityFeatureSpeed(
    const std::vector<NFIQ::QualityFeatureSpeed> &qualityFeatureSpeed)
{
	this->pimpl->setQualityFeatureSpeed(qualityFeatureSpeed);
}

void
NFIQ::NFIQ2Results::setScore(const unsigned int qualityScore)
{
	this->pimpl->setScore(qualityScore);
}

std::vector<NFIQ::ActionableQualityFeedback>
NFIQ::NFIQ2Results::getActionableQualityFeedback() const
{
	return (this->pimpl->getActionableQualityFeedback());
}
std::vector<NFIQ::QualityFeatureData>
NFIQ::NFIQ2Results::getQualityFeatures() const
{
	return (this->pimpl->getQualityFeatures());
}
std::vector<NFIQ::QualityFeatureSpeed>
NFIQ::NFIQ2Results::getQualityFeatureSpeed() const
{
	return (this->pimpl->getQualityFeatureSpeed());
}
unsigned int
NFIQ::NFIQ2Results::getScore() const
{
	return (this->pimpl->getScore());
}
