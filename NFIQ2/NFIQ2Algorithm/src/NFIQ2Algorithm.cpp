#include "include/NFIQ2Algorithm.h"
#include "NFIQ2AlgorithmImpl.h"

#ifdef EMBED_RANDOMFOREST_PARAMETERS
NFIQ::NFIQ2Algorithm::NFIQ2Algorithm ()
{
  this->pimpl.reset( new NFIQ::NFIQ2Algorithm::Impl() );
}
#endif

NFIQ::NFIQ2Algorithm::NFIQ2Algorithm (
  const std::string& fileName,
  const std::string& fileHash )
{
  this->pimpl.reset( new NFIQ::NFIQ2Algorithm::Impl( fileName, fileHash ) );
}

unsigned int
NFIQ::NFIQ2Algorithm::computeQualityScore (
  NFIQ::FingerprintImageData rawImage,
  bool bComputeActionableQuality,
  std::list<NFIQ::ActionableQualityFeedback>& actionableQuality,
  bool bOutputFeatures,
  std::list<NFIQ::QualityFeatureData>& qualityFeatureData,
  bool bOutputSpeed,
  std::list<NFIQ::QualityFeatureSpeed>& qualityFeatureSpeed )
{
  return ( this->pimpl->computeQualityScore(
             rawImage,
             bComputeActionableQuality,
             actionableQuality,
             bOutputFeatures,
             qualityFeatureData,
             bOutputSpeed,
             qualityFeatureSpeed ) );
}

std::string
NFIQ::NFIQ2Algorithm::getParameterHash ()
const
{
  return ( this->pimpl->getParameterHash() );
}

NFIQ::NFIQ2Algorithm::~NFIQ2Algorithm () = default;
