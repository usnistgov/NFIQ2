#include <nfiq2_algorithm.hpp>
#include <nfiq2_modelinfo.hpp>

#include "nfiq2_algorithm_impl.hpp"

#ifdef EMBED_RANDOMFOREST_PARAMETERS
NFIQ2::Algorithm::Algorithm()
    : pimpl { new NFIQ2::Algorithm::Impl() }
{
}
#endif

NFIQ2::Algorithm::Algorithm(
    const std::string &fileName, const std::string &fileHash)
    : pimpl { new NFIQ2::Algorithm::Impl(fileName, fileHash) }
{
}

NFIQ2::Algorithm::Algorithm(const NFIQ2::ModelInfo &modelInfoObj)
    : NFIQ2::Algorithm { modelInfoObj.getModelPath(),
	    modelInfoObj.getModelHash() }
{
}

unsigned int
NFIQ2::Algorithm::computeQualityScore(
    const NFIQ2::FingerprintImageData &rawImage) const
{
	return (this->pimpl->computeQualityScore(rawImage));
}

unsigned int
NFIQ2::Algorithm::computeQualityScore(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
	&features) const
{
	return (this->pimpl->computeQualityScore(features));
}

unsigned int
NFIQ2::Algorithm::computeQualityScore(
    const std::unordered_map<std::string, NFIQ2::QualityFeatureData> &features)
    const
{
	return (this->pimpl->computeQualityScore(features));
}

std::string
NFIQ2::Algorithm::getParameterHash() const
{
	return (this->pimpl->getParameterHash());
}

NFIQ2::Algorithm::~Algorithm() = default;
