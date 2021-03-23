#include "nfiq2_results_impl.h"

#include <string>
#include <vector>

NFIQ::NFIQ2Results::Impl::Impl()
{
}

NFIQ::NFIQ2Results::Impl::Impl(
    std::vector<NFIQ::ActionableQualityFeedback> actionableQuality,
    std::vector<NFIQ::QualityFeatureData> qualityfeatureData,
    std::vector<NFIQ::QualityFeatureSpeed> qualityFeatureSpeed,
    unsigned int qualityScore)
    : actionableQuality_ { actionableQuality }
    , qualityfeatureData_ { qualityfeatureData }
    , qualityFeatureSpeed_ { qualityFeatureSpeed }
{
	if (qualityScore < 1 || qualityScore > 100) {
		const std::string errStr { "Invalid quality score: " +
			std::to_string(qualityScore) +
			". Valid scores are between 1 and 100" };
		throw NFIQ::NFIQException(
		    NFIQ::e_Error_InvalidNFIQ2Score, errStr);
	}
	this->qualityScore_ = qualityScore;
}

NFIQ::NFIQ2Results::Impl::~Impl()
{
}

void
NFIQ::NFIQ2Results::Impl::setActionable(
    std::vector<NFIQ::ActionableQualityFeedback> actionableQuality)
{
	this->actionableQuality_ = actionableQuality;
}
void
NFIQ::NFIQ2Results::Impl::setQuality(
    std::vector<NFIQ::QualityFeatureData> qualityfeatureData)
{
	this->qualityfeatureData_ = qualityfeatureData;
}
void
NFIQ::NFIQ2Results::Impl::setSpeed(
    std::vector<NFIQ::QualityFeatureSpeed> qualityFeatureSpeed)
{
	this->qualityFeatureSpeed_ = qualityFeatureSpeed;
}

void
NFIQ::NFIQ2Results::Impl::setScore(unsigned int qualityScore)
{
	if (qualityScore < 1 || qualityScore > 100) {
		const std::string errStr { "Invalid quality score: " +
			std::to_string(qualityScore) +
			". Valid scores are between 1 and 100" };
		throw NFIQ::NFIQException(
		    NFIQ::e_Error_InvalidNFIQ2Score, errStr);
	}
	this->qualityScore_ = qualityScore;
}

std::vector<NFIQ::ActionableQualityFeedback>
NFIQ::NFIQ2Results::Impl::getActionable() const
{
	return this->actionableQuality_;
}
std::vector<NFIQ::QualityFeatureData>
NFIQ::NFIQ2Results::Impl::getQuality() const
{
	return this->qualityfeatureData_;
}
std::vector<NFIQ::QualityFeatureSpeed>
NFIQ::NFIQ2Results::Impl::getSpeed() const
{
	return this->qualityFeatureSpeed_;
}

unsigned int
NFIQ::NFIQ2Results::Impl::getScore() const
{
	return this->qualityScore_;
}
