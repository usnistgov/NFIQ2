#include <nfiq2/nfiq2_results.hpp>

#include "nfiq2_results_impl.h"

NFIQ::NFIQ2Results::NFIQ2Results()
    : pimpl { new NFIQ::NFIQ2Results::Impl() }
{
}

NFIQ::NFIQ2Results::NFIQ2Results(
    const std::vector<NFIQ::ActionableQualityFeedback> &actionableQuality,
    const std::vector<NFIQ::QualityFeatureData> &qualityfeatureData,
    const std::vector<NFIQ::QualityFeatureSpeed> &qualityFeatureSpeed,
    const unsigned int qualityScore)
    : pimpl { new NFIQ::NFIQ2Results::Impl(actionableQuality,
	  qualityfeatureData, qualityFeatureSpeed, qualityScore) }
{
}

NFIQ::NFIQ2Results::~NFIQ2Results() = default;

void
NFIQ::NFIQ2Results::setActionable(
    const std::vector<NFIQ::ActionableQualityFeedback> &actionableQuality)
{
	this->pimpl->setActionable(actionableQuality);
}
void
NFIQ::NFIQ2Results::setQuality(
    const std::vector<NFIQ::QualityFeatureData> &qualityfeatureData)
{
	this->pimpl->setQuality(qualityfeatureData);
}
void
NFIQ::NFIQ2Results::setSpeed(
    const std::vector<NFIQ::QualityFeatureSpeed> &qualityFeatureSpeed)
{
	this->pimpl->setSpeed(qualityFeatureSpeed);
}

void
NFIQ::NFIQ2Results::setScore(const unsigned int qualityScore)
{
	this->pimpl->setScore(qualityScore);
}

std::vector<NFIQ::ActionableQualityFeedback>
NFIQ::NFIQ2Results::getActionable() const
{
	return (this->pimpl->getActionable());
}
std::vector<NFIQ::QualityFeatureData>
NFIQ::NFIQ2Results::getQuality() const
{
	return (this->pimpl->getQuality());
}
std::vector<NFIQ::QualityFeatureSpeed>
NFIQ::NFIQ2Results::getSpeed() const
{
	return (this->pimpl->getSpeed());
}
unsigned int
NFIQ::NFIQ2Results::getScore() const
{
	return (this->pimpl->getScore());
}
