#ifndef NFIQ2_ALGORITHM_IMPL_HPP_
#define NFIQ2_ALGORITHM_IMPL_HPP_

#include <nfiq2_algorithm.hpp>
#include <nfiq2_constants.hpp>
#include <nfiq2_exception.hpp>
#include <nfiq2_fingerprintimagedata.hpp>
#include <prediction/RandomForestML.h>

#include <fstream>
#include <list>
#include <string>
#include <vector>

namespace NFIQ2 {

/**
 * Internal implementation of Applies trained random forest parameters to
 * quality features, computing an overall quality score (i.e., NFIQ2).
 */
class Algorithm::Impl {
    public:
	/**
	 * @brief
	 * Default constructor of Algorithm.
	 *
	 * @note
	 * May load from parameters compiled into source code, in which case
	 * this can be slow.
	 */
	Impl();

	/**
	 * @brief
	 * Constructor that loads random forest parameters from disk.
	 *
	 * @param fileName
	 * The file path containing the random forest model.
	 * @param fileHash
	 * The md5 checksum of the provided file.
	 */
	Impl(const std::string &fileName, const std::string &fileHash);

	/** Destructor. */
	virtual ~Impl();

	/**
	 * @brief
	 * Computes the quality score from the provided fingerprint image data.
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
	 * Computes the quality score from a vector of extracted `features`
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
	unsigned int computeQualityScore(
	    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::Module>>
		&features) const;

	/**
	 * @brief
	 * Computes the quality score from a map of extracted image
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
	    const std::unordered_map<std::string, double> &features) const;

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
	 * Obtain if the random forest parameters are embedded in the library
	 * or located externally.
	 *
	 * @return
	 * true if random forest parameters are embedded, false otherwise.
	 */
	bool isEmbedded() const;

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
	 * Retrieves FR capture technology.
	 *
	 * @return
	 * Embedded friction ridge capture technology specified.
	 */
	unsigned int getEmbeddedFCT() const;

    private:
	/** Indicates whether random forest parameters have been loaded. */
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
	    const std::unordered_map<std::string, double> &features) const;

	/**
	 * @brief
	 * Throw an exception if random forest parameters have not been
	 * loaded.
	 *
	 * @throw NFIQ2::Exception
	 * Random forest parameters have not been loaded.
	 */
	void throwIfUninitialized() const;

	/** RandomForest parameters. */
	NFIQ2::Prediction::RandomForestML m_RandomForestML;

	/** RandomForest parameter md5 hash. */
	std::string m_parameterHash {};
};
} // namespace NFIQ2

#endif /* NFIQ2_ALGORITHM_IMPL_HPP_ */
