#include <nfiq2.hpp>
#include <nfiq2_modelinfo.hpp>

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
NFIQ::NFIQ2Algorithm::computeQualityScore(
    const NFIQ::FingerprintImageData &rawImage) const
{
	return (this->pimpl->computeQualityScore(rawImage));
}

unsigned int
NFIQ::NFIQ2Algorithm::computeQualityScore(
    const std::vector<std::shared_ptr<NFIQ::QualityFeatures::BaseFeature>>
	&features) const
{
	return (this->pimpl->computeQualityScore(features));
}

std::string
NFIQ::NFIQ2Algorithm::getParameterHash() const
{
	return (this->pimpl->getParameterHash());
}

NFIQ::NFIQ2Algorithm::~NFIQ2Algorithm() = default;
