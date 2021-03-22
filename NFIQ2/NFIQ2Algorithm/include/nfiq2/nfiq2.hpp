#ifndef NFIQ2_NFIQ2_H_
#define NFIQ2_NFIQ2_H_

#include <nfiq2/fingerprintimagedata.hpp>
#include <nfiq2/interfacedefinitions.hpp>
#include <nfiq2/modelinfo.hpp>

#include <list>
#include <memory>
#include <string>

namespace NFIQ {
class NFIQ2Results {
    public:
	NFIQ2Results();
	NFIQ2Results(
	    std::vector<NFIQ::ActionableQualityFeedback> actionableQuality,
	    std::vector<NFIQ::QualityFeatureData> qualityfeatureData,
	    std::vector<NFIQ::QualityFeatureSpeed> qualityFeatureSpeed);

	void setActionable(
	    std::vector<NFIQ::ActionableQualityFeedback> actionableQuality);
	void setQuality(
	    std::vector<NFIQ::QualityFeatureData> qualityfeatureData);
	void setSpeed(
	    std::vector<NFIQ::QualityFeatureSpeed> qualityFeatureSpeed);

	std::vector<NFIQ::ActionableQualityFeedback> getActionable() const;
	std::vector<NFIQ::QualityFeatureData> getQuality() const;
	std::vector<NFIQ::QualityFeatureSpeed> getSpeed() const;

    private:
	class Impl;
	std::unique_ptr<NFIQ2Results::Impl> pimpl;
	std::vector<NFIQ::ActionableQualityFeedback> actionableQuality_ {};
	std::vector<NFIQ::QualityFeatureData> qualityfeatureData_ {};
	std::vector<NFIQ::QualityFeatureSpeed> qualityFeatureSpeed_ {};
};

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
	 *
	 * @throws NFIQException
	 * Failure to compute (reason contained within message string).
	 *
	 * @return
	 * Achieved quality score
	 */
	// Change this so that only pass in bools - not the lists
	unsigned int computeQualityScore(NFIQ::FingerprintImageData rawImage,
	    bool bComputeActionableQuality,
	    std::list<NFIQ::ActionableQualityFeedback> &actionableQuality,
	    bool bOutputFeatures,
	    std::list<NFIQ::QualityFeatureData> &qualityFeatureData,
	    bool bOutputSpeed,
	    std::list<NFIQ::QualityFeatureSpeed> &qualityFeatureSpeed) const;

	unsigned int computeQualityScore(
	    NFIQ::FingerprintImageData rawImage) const;

	NFIQ::NFIQ2Results computeQualityFeaturesAndScore(
	    NFIQ::FingerprintImageData rawImage) const;

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
