#include "nfiq2_results_impl.h"

#include <string>
#include <vector>

NFIQ::NFIQ2Results::Impl::Impl() = default;

NFIQ::NFIQ2Results::Impl::Impl(
    const std::vector<NFIQ::ActionableQualityFeedback> &actionableQuality,
    const std::vector<NFIQ::QualityFeatureData> &qualityFeatureData,
    const std::vector<NFIQ::QualityFeatureSpeed> &qualityFeatureSpeed,
    const unsigned int qualityScore)
    : actionableQuality_ { actionableQuality }
    , qualityFeatureData_ { qualityFeatureData }
    , qualityFeatureSpeed_ { qualityFeatureSpeed }
    , qualityScore_ { NFIQ::NFIQ2Results::checkScore(qualityScore) }
{
}

NFIQ::NFIQ2Results::Impl::~Impl() = default;

unsigned int
NFIQ::NFIQ2Results::Impl::checkScore(const unsigned int qualityScore)
{
	if (qualityScore > 100) {
		const std::string errStr { "Invalid quality score: " +
			std::to_string(qualityScore) +
			". Valid scores are between 0 and 100" };
		throw NFIQ::NFIQException(
		    NFIQ::e_Error_InvalidNFIQ2Score, errStr);
	}
	return qualityScore;
}

void
NFIQ::NFIQ2Results::Impl::setActionableQualityFeedback(
    const std::vector<NFIQ::ActionableQualityFeedback> &actionableQuality)
{
	this->actionableQuality_ = actionableQuality;
}
void
NFIQ::NFIQ2Results::Impl::setQualityFeatures(
    const std::vector<NFIQ::QualityFeatureData> &qualityFeatureData)
{
	this->qualityFeatureData_ = qualityFeatureData;
}
void
NFIQ::NFIQ2Results::Impl::setQualityFeatureSpeed(
    const std::vector<NFIQ::QualityFeatureSpeed> &qualityFeatureSpeed)
{
	this->qualityFeatureSpeed_ = qualityFeatureSpeed;
}

void
NFIQ::NFIQ2Results::Impl::setScore(const unsigned int qualityScore)
{
	this->qualityScore_ = NFIQ::NFIQ2Results::checkScore(qualityScore);
}

std::vector<NFIQ::ActionableQualityFeedback>
NFIQ::NFIQ2Results::Impl::getActionableQualityFeedback() const
{
	return this->actionableQuality_;
}
std::vector<NFIQ::QualityFeatureData>
NFIQ::NFIQ2Results::Impl::getQualityFeatures() const
{
	return this->qualityFeatureData_;
}
std::vector<NFIQ::QualityFeatureSpeed>
NFIQ::NFIQ2Results::Impl::getQualityFeatureSpeed() const
{
	return this->qualityFeatureSpeed_;
}

unsigned int
NFIQ::NFIQ2Results::Impl::getScore() const
{
	return this->qualityScore_;
}
