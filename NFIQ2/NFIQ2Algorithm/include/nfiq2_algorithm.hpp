#ifndef NFIQ2_ALGORITHM_HPP_
#define NFIQ2_ALGORITHM_HPP_

#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_interfacedefinitions.hpp>
#include <nfiq2_modelinfo.hpp>
#include <nfiq2_qualityfeatures.hpp>

#include <memory>
#include <string>
#include <unordered_map>

/**
 * @namespace NFIQ2
 *
 * @brief This namespace contains classes and functions that contribute to
 * the calculation of NFIQ 2 scores.
 */
namespace NFIQ2 {

/**
 * @class Algorithm
 *
 * @brief This class serves as a wrapper to return quality scores
 * for a fingerprint image
 */
class Algorithm {
    public:
	/**
	 * @brief Default constructor of Algorithm
	 */
	Algorithm();

	/**
	 * @brief Constructor of Algorithm using model filename and the file's
	 * hash
	 */
	Algorithm(const std::string &fileName, const std::string &fileHash);

	/**
	 * @brief Constructor of Algorithm using a modelInfoObj object to
	 * initialize the random forest model
	 */
	Algorithm(const NFIQ2::ModelInfo &modelInfoObj);

	/**
	 * @brief Copy constructor
	 */
	Algorithm(const Algorithm &);

	/**
	 * @brief Assignment operator
	 */
	Algorithm &operator=(const Algorithm &);

	/**
	 * @brief Move constructor
	 */
	Algorithm(Algorithm &&) noexcept;

	/**
	 * @brief Move assignment operator
	 */
	Algorithm &operator=(Algorithm &&) noexcept;

	/**
	 * @brief Destructor
	 */
	~Algorithm();

	/**
	 * @fn computeQualityScore
	 *
	 * @brief Computes the quality score from the input fingerprint image
	 * data
	 *
	 * @param rawImage fingerprint image in raw format
	 *
	 * @return achieved quality score
	 *
	 * @throw Exception
	 * Called before random forest parameters were loaded
	 */
	unsigned int computeQualityScore(
	    const NFIQ2::FingerprintImageData &rawImage) const;

	/**
	 * @fn computeQualityScore
	 *
	 * @brief Computes the quality score from a vector of extracted feature
	 * from a cropped fingerprint image
	 *
	 * @param features list of computed feature metrics that contain quality
	 * information for a fingerprint image
	 *
	 * @return achieved quality score
	 *
	 * @throw Exception
	 * Called before random forest parameters were loaded
	 */
	unsigned int computeQualityScore(const std::vector<
	    std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>> &features)
	    const;

	/**
	 * @fn computeQualityScore
	 *
	 * @brief Computes the quality score from the extracted image
	 * quality feature data
	 *
	 * @param features map of string, quality feature data pairs
	 *
	 * @return achieved quality score
	 *
	 * @throw Exception
	 * Called before random forest parameters were loaded
	 */
	unsigned int computeQualityScore(
	    const std::unordered_map<std::string, NFIQ2::QualityFeatureData>
		&features) const;

	/**
	 * @fn getParameterHash
	 *
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
	 * @fn isInitialized
	 *
	 * @brief
	 * Determine if random forest parameters have been loaded.
	 *
	 * @return
	 * true if some set of random forest parameters have been loaded, false
	 * otherwise.
	 */
	bool isInitialized() const;

    private:
	/**
	 * @class Impl
	 *
	 * @brief Pointer to Implementation class
	 */
	class Impl;

	/**
	 * @var pimpl
	 *
	 * @brief Pointer to Implementation smart pointer
	 */
	std::unique_ptr<Algorithm::Impl> pimpl;
};
} // namespace NFIQ

#endif
