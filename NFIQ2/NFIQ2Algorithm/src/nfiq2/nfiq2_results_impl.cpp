#include "nfiq2_results_impl.hpp"
#include <string>
#include <vector>

NFIQ2::NFIQ2Results::Impl::Impl() = default;

NFIQ2::NFIQ2Results::Impl::Impl(
    const std::vector<NFIQ2::ActionableQualityFeedback> &actionableQuality,
    const std::vector<NFIQ2::QualityFeatureData> &qualityFeatureData,
    const std::vector<NFIQ2::QualityFeatureSpeed> &qualityFeatureSpeed,
    const unsigned int qualityScore)
    : actionableQuality_ { actionableQuality }
    , qualityFeatureData_ { qualityFeatureData }
    , qualityFeatureSpeed_ { qualityFeatureSpeed }
    , qualityScore_ { NFIQ2::NFIQ2Results::checkScore(qualityScore) }
{
}

NFIQ2::NFIQ2Results::Impl::~Impl() = default;

unsigned int
NFIQ2::NFIQ2Results::Impl::checkScore(const unsigned int qualityScore)
{
	if (qualityScore > 100) {
		const std::string errStr { "Invalid quality score: " +
			std::to_string(qualityScore) +
			". Valid scores are between 0 and 100" };
		throw NFIQ2::NFIQException(
		    NFIQ2::ErrorCode::InvalidNFIQ2Score, errStr);
	}
	return qualityScore;
}

void
NFIQ2::NFIQ2Results::Impl::setActionableQualityFeedback(
    const std::vector<NFIQ2::ActionableQualityFeedback> &actionableQuality)
{
	this->actionableQuality_ = actionableQuality;
}
void
NFIQ2::NFIQ2Results::Impl::setQualityFeatures(
    const std::vector<NFIQ2::QualityFeatureData> &qualityFeatureData)
{
	this->qualityFeatureData_ = qualityFeatureData;
}
void
NFIQ2::NFIQ2Results::Impl::setQualityFeatureSpeed(
    const std::vector<NFIQ2::QualityFeatureSpeed> &qualityFeatureSpeed)
{
	this->qualityFeatureSpeed_ = qualityFeatureSpeed;
}

void
NFIQ2::NFIQ2Results::Impl::setScore(const unsigned int qualityScore)
{
	this->qualityScore_ = NFIQ2::NFIQ2Results::checkScore(qualityScore);
}

std::vector<NFIQ2::ActionableQualityFeedback>
NFIQ2::NFIQ2Results::Impl::getActionableQualityFeedback() const
{
	return this->actionableQuality_;
}
std::vector<NFIQ2::QualityFeatureData>
NFIQ2::NFIQ2Results::Impl::getQualityFeatures() const
{
	return this->qualityFeatureData_;
}
std::vector<NFIQ2::QualityFeatureSpeed>
NFIQ2::NFIQ2Results::Impl::getQualityFeatureSpeed() const
{
	return this->qualityFeatureSpeed_;
}

unsigned int
NFIQ2::NFIQ2Results::Impl::getScore() const
{
	return this->qualityScore_;
}
