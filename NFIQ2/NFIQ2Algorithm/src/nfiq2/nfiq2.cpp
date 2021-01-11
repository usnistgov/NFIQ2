#include "include/nfiq2/nfiq2.h"
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

std::vector<std::string>
NFIQ::NFIQ2Algorithm::getAllActionableIdentifiers()
{
	return NFIQ::NFIQ2Algorithm::Impl::getAllActionableIdentifiers();
}

std::vector<std::string>
NFIQ::NFIQ2Algorithm::getAllQualityFeatureIDs()
{
	return NFIQ::NFIQ2Algorithm::Impl::getAllQualityFeatureIDs();
}

std::vector<std::string>
NFIQ::NFIQ2Algorithm::getAllSpeedFeatureGroups()
{
	return NFIQ::NFIQ2Algorithm::Impl::getAllSpeedFeatureGroups();
}

std::string
NFIQ::NFIQ2Algorithm::getParameterHash() const
{
	return (this->pimpl->getParameterHash());
}

NFIQ::NFIQ2Algorithm::~NFIQ2Algorithm() = default;
