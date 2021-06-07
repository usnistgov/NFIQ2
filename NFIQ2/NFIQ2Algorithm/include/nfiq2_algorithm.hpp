/*
 * This file is part of NIST Fingerprint Image Quality (NFIQ) 2. For more
 * information on this project, refer to:
 *   - https://nist.gov/services-resources/software/nfiq2
 *   - https://github.com/usnistgov/NFIQ2
 *
 * This work is in the public domain. For complete licensing details, refer to:
 *   - https://github.com/usnistgov/NFIQ2/blob/master/LICENSE.md
 */

#ifndef NFIQ2_ALGORITHM_HPP_
#define NFIQ2_ALGORITHM_HPP_

#include <nfiq2_constants.hpp>
#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_modelinfo.hpp>
#include <nfiq2_qualityfeatures.hpp>

#include <memory>
#include <string>
#include <unordered_map>

namespace NFIQ2 {

/**
 * Applies trained random forest parameters to quality features, computing an
 * overall quality score (i.e., NFIQ2).
 */
class Algorithm {
    public:
	/**
	 * @brief
	 * Default constructor of Algorithm.
	 *
	 * @note
	 * May load from parameters compiled into source code, in which case
	 * this can be slow.
	 */
	Algorithm();

	/**
	 * @brief
	 * Constructor that loads random forest parameters from disk.
	 *
	 * @param fileName
	 * The file path containing the random forest model.
	 * @param fileHash
	 * The md5 checksum of the provided file.
	 */
	Algorithm(const std::string &fileName, const std::string &fileHash);

	/**
	 * @brief
	 * Constructor using NFIQ2::ModelInfo to initialize the random forest.
	 *
	 * @param modelInfoObj
	 * Contains the random forest model and information about it.
	 */
	Algorithm(const NFIQ2::ModelInfo &modelInfoObj);

	/** Copy constructor. */
	Algorithm(const Algorithm &);

	/** Assignment operator. */
	Algorithm &operator=(const Algorithm &);

	/** Move constructor. */
	Algorithm(Algorithm &&) noexcept;

	/** Move assignment operator. */
	Algorithm &operator=(Algorithm &&) noexcept;

	/** Destructor. */
	~Algorithm();

	/**
	 * @brief
	 * Compute a NFIQ 2 quality score.
	 *
	 * @param rawImage
	 * Fingerprint image.
	 *
	 * @return
	 * Computed NFIQ 2 quality score.
	 *
	 * @throw Exception
	 * Called before random forest parameters were loaded.
	 *
	 * @ingroup compute
	 */
	unsigned int computeQualityScore(
	    const NFIQ2::FingerprintImageData &rawImage) const;

	/**
	 * @brief
	 * Compute a NFIQ 2 quality score.
	 *
	 * @param modules
	 * Computed quality modules.
	 *
	 * @return
	 * Computed NFIQ 2 quality score.
	 *
	 * @throw Exception
	 * Called before random forest parameters were loaded.
	 *
	 * @ingroup compute
	 * @see QualityFeatures::computeQualityModules
	 */
	unsigned int computeQualityScore(
	    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::Module>>
		&modules) const;

	/**
	 * @brief
	 * Compute a NFIQ 2 quality score.
	 *
	 * @param features
	 * Map of quality feature identifiers to quality feature values.
	 *
	 * @return
	 * Computed NFIQ 2 quality score.
	 *
	 * @throw Exception
	 * Called before random forest parameters were loaded.
	 *
	 * @ingroup compute
	 * @see QualityFeatures::computeQualityFeatures
	 */
	unsigned int computeQualityScore(
	    const std::unordered_map<std::string, double> &features) const;

	/**
	 * @brief
	 * Obtain MD5 checksum of random forest parameter file loaded.
	 *
	 * @return
	 * MD5 checksum of the random forest parameter file loaded.
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

	/**
	 * @brief
	 * Obtain if the random forest parameters are embedded in the library
	 * or located externally.
	 *
	 * @return
	 * true if random forest parameters are embedded, false otherwise.
	 */
	bool isEmbedded() const;

	/**
	 * @brief
	 * Obtain the friction ridge capture technology (FCT) specified for the
	 * embedded random forest parameters.
	 *
	 * @return
	 * Embedded FCT specified.
	 *
	 * @throw NFIQ2::Exception
	 * Parameters were not embedded or FCT was not specified.
	 */
	unsigned int getEmbeddedFCT() const;

    private:
	/** Pointer to Implementation class. */
	class Impl;

	/** Pointer to Implementation smart pointer. */
	std::unique_ptr<Algorithm::Impl> pimpl;
};
} // namespace NFIQ

#endif /* NFIQ2_ALGORITHM_HPP_ */
