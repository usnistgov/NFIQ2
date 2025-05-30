#include <nfiq2_algorithm.hpp>
#include <nfiq2_modelinfo.hpp>

#include "nfiq2_algorithm_impl.hpp"

NFIQ2::Algorithm::Algorithm()
    : pimpl { new NFIQ2::Algorithm::Impl() }
{
}

NFIQ2::Algorithm::Algorithm(const std::string &fileName,
    const std::string &fileHash)
    : pimpl { new NFIQ2::Algorithm::Impl(fileName, fileHash) }
{
}

NFIQ2::Algorithm::Algorithm(const NFIQ2::ModelInfo &modelInfoObj)
    : NFIQ2::Algorithm { modelInfoObj.getModelPath(),
	    modelInfoObj.getModelHash() }
{
}

#ifdef __ANDROID__
NFIQ2::Algorithm::Algorithm(AAssetManager *assets, const std::string &fileName,
    const std::string &fileHash)
    : pimpl { new NFIQ2::Algorithm::Impl(assets, fileName, fileHash) }
{
}
#endif

NFIQ2::Algorithm::Algorithm(const Algorithm &rhs)
    : pimpl(new Impl(*rhs.pimpl))
{
}

NFIQ2::Algorithm &
NFIQ2::Algorithm::operator=(const Algorithm &rhs)
{
	*pimpl = *rhs.pimpl;
	return *this;
}

unsigned int
NFIQ2::Algorithm::computeUnifiedQualityScore(
    const NFIQ2::FingerprintImageData &rawImage) const
{
	return (this->pimpl->computeUnifiedQualityScore(rawImage));
}

unsigned int
NFIQ2::Algorithm::computeUnifiedQualityScore(
    const std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	&features) const
{
	return (this->pimpl->computeUnifiedQualityScore(features));
}

unsigned int
NFIQ2::Algorithm::computeUnifiedQualityScore(
    const std::unordered_map<std::string, double> &features) const
{
	return (this->pimpl->computeUnifiedQualityScore(features));
}

std::unordered_map<std::string, unsigned int>
NFIQ2::Algorithm::getQualityBlockValues(
    const std::unordered_map<std::string, double> &nativeQualityMeasureValues)
{
	return (Impl::getQualityBlockValues(nativeQualityMeasureValues));
}

unsigned int
NFIQ2::Algorithm::getQualityBlockValue(const std::string &featureIdentifier,
    const double nativeQualityMeasureValue)
{
	return (Impl::getQualityBlockValue(featureIdentifier,
	    nativeQualityMeasureValue));
}

std::string
NFIQ2::Algorithm::getParameterHash() const
{
	return (this->pimpl->getParameterHash());
}

bool
NFIQ2::Algorithm::isInitialized() const
{
	return (this->pimpl->isInitialized());
}

bool
NFIQ2::Algorithm::isEmbedded() const
{
	return (this->pimpl->isEmbedded());
}

unsigned int
NFIQ2::Algorithm::getEmbeddedFCT() const
{
	return (this->pimpl->getEmbeddedFCT());
}

NFIQ2::Algorithm::~Algorithm() = default;
NFIQ2::Algorithm::Algorithm(NFIQ2::Algorithm &&) noexcept = default;
NFIQ2::Algorithm &NFIQ2::Algorithm::operator=(Algorithm &&) noexcept = default;
