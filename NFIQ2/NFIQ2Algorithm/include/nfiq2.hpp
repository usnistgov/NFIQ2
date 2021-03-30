#ifndef NFIQ2_HPP_
#define NFIQ2_HPP_

#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_interfacedefinitions.hpp>
#include <nfiq2_modelinfo.hpp>
#include <nfiq2_results.hpp>

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

	/**
	 * @fn computeQualityScore
	 * @brief Computes the quality score from the input fingerprint image
	 * data
	 * @param rawImage fingerprint image in raw format
	 * @param qualityfeatureData list of computed feature data values
	 * @return achieved quality score
	 */
	unsigned int computeQualityScore(
	    const NFIQ::FingerprintImageData &rawImage) const;

	/**
	 * @fn computeQualityScore
	 * @brief Computes the quality score from the extracted image
	 * quality feature data
	 * @param qualityfeatureData list of computed feature data values
	 * @return achieved quality score
	 */
	unsigned int computeQualityScore(
	    const std::vector<NFIQ::QualityFeatureData> &qualityFeatureData)
	    const;

	/**
	 * @fn computeQualityFeaturesAndScore
	 * @brief Computes the quality score from the input fingerprint image
	 * data
	 * @param rawImage fingerprint image in raw format
	 * @return Object containing score, actionable, quality and speed data
	 */
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
