#ifndef NFIQ2_ALGORITHM_IMPL_HPP_
#define NFIQ2_ALGORITHM_IMPL_HPP_

#include <nfiq2_algorithm.hpp>
#include <nfiq2_exception.hpp>
#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_interfacedefinitions.hpp>
#include <prediction/RandomForestML.h>

#include <fstream>
#include <list>
#include <string>
#include <vector>

namespace NFIQ2 {

/**
 * 	This class serves as a wrapper to return quality scores
 * 	for a fingerprint image.
 */
class Algorithm::Impl {
    public:
	/** Default constructor of Impl. */
	Impl();

	/** Constructor using model filename and the file's hash. */
	Impl(const std::string &fileName, const std::string &fileHash);

	/** Destructor. */
	virtual ~Impl();

	/**
	 * @brief
	 * Computes the quality score from the input fingerprint image data.
	 *
	 * @param rawImage
	 * Fingerprint image in raw format.
	 *
	 * @return
	 * Computed quality score.
	 *
	 * @throw Exception
	 * Called before random forest parameters were loaded.
	 */
	unsigned int computeQualityScore(
	    const NFIQ2::FingerprintImageData &rawImage) const;

	/**
	 * @brief
	 * Computes the quality score from a vector of extracted feature
	 * from a cropped fingerprint image.
	 *
	 * @param features
	 * Vector of computed feature metrics that contain quality
	 * information for a fingerprint image.
	 *
	 * @return
	 * Computed quality score.
	 *
	 * @throw Exception
	 * Called before random forest parameters were loaded.
	 */
	unsigned int computeQualityScore(const std::vector<
	    std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>> &features)
	    const;

	/**
	 * @brief
	 * Computes the quality score from the extracted image
	 * quality feature data.
	 *
	 * @param features
	 * Map of string, quality feature data pairs.
	 *
	 * @return
	 * Computed quality score.
	 *
	 * @throw Exception
	 * Called before random forest parameters were loaded.
	 */
	unsigned int computeQualityScore(
	    const std::unordered_map<std::string, NFIQ2::QualityFeatureData>
		&features) const;

	/**
	 * @brief
	 * Obtain MD5 checksum of Random Forest parameter file loaded.
	 *
	 * @return
	 * MD5 checksum of the Random Forest parameter file loaded.
	 *
	 * @throw Exception
	 * Called before random forest parameters were loaded.
	 */
	std::string getParameterHash() const;

	/**
	 * @brief
	 * Determine if random forest parameters have been loaded.
	 *
	 * @return
	 * true if some set of random forest parameters have been loaded, false
	 * otherwise.
	 */
	bool isInitialized() const;

    private:
	/** Indicates whether or not random forest parameters have been loaded.
	 */
	bool initialized { false };

	/**
	 * @brief
	 * Retrieves NFIQ 2 quality score from a map of feature data.
	 *
	 * @param features
	 * Map of string, QualityFeatureData pairs.
	 *
	 * @return
	 * Computed NFIQ 2 quality score.
	 *
	 * @throws Exception
	 * Failure to compute (OpenCV reason contained within message string) or
	 * called before random forest parameters loaded.
	 */
	double getQualityPrediction(
	    const std::unordered_map<std::string, NFIQ2::QualityFeatureData>
		&features) const;

	/**
	 * @brief
	 * Throw an exception if random forest parameters have not been
	 * loaded.
	 *
	 * @throw NFIQ2::Exception
	 * Random forest parameters have not been loaded.
	 */
	void throwIfUninitialized() const;

	/** Private member storing RandomForest parameters. */
	NFIQ2::Prediction::RandomForestML m_RandomForestML;

	/** Private member storing RandomForest parameter md5 hash. */
	std::string m_parameterHash {};
};
} // namespace NFIQ2

#endif /* NFIQ2_ALGORITHM_IMPL_HPP_ */
