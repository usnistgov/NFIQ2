#ifndef NFIQ2ALGORITHM_H_
#define NFIQ2ALGORITHM_H_

#include "include/FingerprintImageData.h"
#include "include/InterfaceDefinitions.h"
#include "include/nfiq2_version.h"

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
	~NFIQ2Algorithm();

	/**
	 * @brief
	 * Computes the quality score from the input fingerprint image
	 * data.
	 *
	 * @param rawImage
	 * Fingerprint image in raw format
	 * @param bComputeActionableQuality
	 * If to compute actionable quality flags or not
	 * @param actionableQuality
	 * Compute actionable quality values
	 * @param bOutputFeatures
	 * If to output feature values
	 * @param qualityfeatureData
	 * List of computed feature data values
	 * @param bOutputSpeed
	 * If to output speed of computed features
	 * @param qualityFeatureSpeed
	 * List of feature computation speed
	 *
	 * @return
	 * Achieved quality score
	 */
	unsigned int computeQualityScore(NFIQ::FingerprintImageData rawImage,
	    bool bComputeActionableQuality,
	    std::list<NFIQ::ActionableQualityFeedback> &actionableQuality,
	    bool bOutputFeatures,
	    std::list<NFIQ::QualityFeatureData> &qualityFeatureData,
	    bool bOutputSpeed,
	    std::list<NFIQ::QualityFeatureSpeed> &qualityFeatureSpeed) const;

	/**
	 * @brief
	 * Obtain all actionable quality feedback identifiers.
	 *
	 * @return
	 * Vector of strings containing all actionable quality feedback
	 * identifiers.
	 */
	static std::vector<std::string> getAllActionableIdentifiers();

	/**
	 * @brief
	 * Obtain all quality feature IDs from quality modules.
	 *
	 * @return
	 * Vector of strings containing all quality feature IDs.
	 */
	static std::vector<std::string> getAllQualityFeatureIDs();

	/**
	 * @brief
	 * Obtain all speed feature groups from quality modules.
	 *
	 * @return
	 * Vector of strings containing all speed feature groups.
	 */
	static std::vector<std::string> getAllSpeedFeatureGroups();

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
