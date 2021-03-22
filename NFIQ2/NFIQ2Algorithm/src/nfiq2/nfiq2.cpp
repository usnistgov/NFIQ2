#include <nfiq2/modelinfo.hpp>
#include <nfiq2/nfiq2.hpp>

#include "nfiq2impl.h"

#ifdef EMBED_RANDOMFOREST_PARAMETERS
NFIQ::NFIQ2Algorithm::NFIQ2Algorithm()
    : pimpl { new NFIQ::NFIQ2Algorithm::Impl() }
{
}
#endif

NFIQ::NFIQ2Algorithm::NFIQ2Algorithm(
    const std::string &fileName, const std::string &fileHash)
    : pimpl { new NFIQ::NFIQ2Algorithm::Impl(fileName, fileHash) }
{
}

NFIQ::NFIQ2Algorithm::NFIQ2Algorithm(const NFIQ::ModelInfo &modelInfoObj)
    : NFIQ::NFIQ2Algorithm { modelInfoObj.getModelPath(),
	    modelInfoObj.getModelHash() }
{
}

unsigned int
NFIQ::NFIQ2Algorithm::computeQualityScore(NFIQ::FingerprintImageData rawImage,
    bool bComputeActionableQuality,
    std::list<NFIQ::ActionableQualityFeedback> &actionableQuality,
    bool bOutputFeatures,
    std::list<NFIQ::QualityFeatureData> &qualityFeatureData, bool bOutputSpeed,
    std::list<NFIQ::QualityFeatureSpeed> &qualityFeatureSpeed) const
{
	return (this->pimpl->computeQualityScore(rawImage,
	    bComputeActionableQuality, actionableQuality, bOutputFeatures,
	    qualityFeatureData, bOutputSpeed, qualityFeatureSpeed));
}

unsigned int
NFIQ::NFIQ2Algorithm::computeQualityScore(
    NFIQ::FingerprintImageData rawImage) const
{
	return (this->pimpl->computeQualityScore(rawImage));
}

NFIQ::NFIQ2Results
NFIQ::NFIQ2Algorithm::computeQualityFeaturesAndScore(
    NFIQ::FingerprintImageData rawImage) const
{
	return (this->pimpl->computeQualityFeaturesAndScore(rawImage));
}

std::string
NFIQ::NFIQ2Algorithm::getParameterHash() const
{
	return (this->pimpl->getParameterHash());
}

NFIQ::NFIQ2Algorithm::~NFIQ2Algorithm() = default;

NFIQ::NFIQ2Results::NFIQ2Results()
    : pimpl { new NFIQ::NFIQ2Results::Impl() }
{
}

NFIQ::NFIQ2Results::NFIQ2Results(
    std::vector<NFIQ::ActionableQualityFeedback> actionableQuality,
    std::vector<NFIQ::QualityFeatureData> qualityfeatureData,
    std::vector<NFIQ::QualityFeatureSpeed> qualityFeatureSpeed)
    : pimpl { new NFIQ::NFIQ2Results::Impl(
	  actionableQuality, qualityfeatureData, qualityFeatureSpeed) }
{
}

void
NFIQ::NFIQ2Results::setActionable(
    std::vector<NFIQ::ActionableQualityFeedback> actionableQuality)
{
	this->pimpl->setActionable(actionableQuality);
}
void
NFIQ::NFIQ2Results::setQuality(
    std::vector<NFIQ::QualityFeatureData> qualityfeatureData)
{
	this->pimpl->setQuality(qualityfeatureData);
}
void
NFIQ::NFIQ2Results::setSpeed(
    std::vector<NFIQ::QualityFeatureSpeed> qualityFeatureSpeed)
{
	this->pimpl->setSpeed(qualityFeatureSpeed);
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
