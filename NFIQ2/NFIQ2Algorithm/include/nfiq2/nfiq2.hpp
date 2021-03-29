#ifndef NFIQ2_NFIQ2_H_
#define NFIQ2_NFIQ2_H_

#include <nfiq2/fingerprintimagedata.hpp>
#include <nfiq2/interfacedefinitions.hpp>
#include <nfiq2/modelinfo.hpp>
#include <nfiq2/nfiq2_results.hpp>

#include <list>
#include <memory>
#include <string>

namespace NFIQ {
/** Wrapper to return quality scores for a fingerprint image */
class NFIQ2Algorithm {
    public:
#ifdef EMBED_RANDOMFOREST_PARAMETERS
	NFIQ2Algorithm();
#endif
	NFIQ2Algorithm(
	    const std::string &fileName, const std::string &fileHash);
	NFIQ2Algorithm(const NFIQ::ModelInfo &modelInfoObj);
	~NFIQ2Algorithm();

	unsigned int computeQualityScore(
	    const NFIQ::FingerprintImageData &rawImage) const;

	unsigned int computeQualityScore(
	    const std::vector<NFIQ::QualityFeatureData> &qualityFeatureData)
	    const;

	NFIQ::NFIQ2Results computeQualityFeaturesAndScore(
	    const NFIQ::FingerprintImageData &rawImage) const;

	/**
	 * @brief
	 * Obtain MD5 checksum of Random Forest parameter file loaded.
	 *
	 * @return
	 * MD5 checksum of the Random Forest parameter file loaded.
	 */
	std::string getParameterHash() const;

    private:
	class Impl;
	const std::unique_ptr<const NFIQ2Algorithm::Impl> pimpl;

	NFIQ2Algorithm(const NFIQ2Algorithm &) = delete;
	NFIQ2Algorithm &operator=(const NFIQ2Algorithm &) = delete;
};
} // namespace NFIQ

#endif
