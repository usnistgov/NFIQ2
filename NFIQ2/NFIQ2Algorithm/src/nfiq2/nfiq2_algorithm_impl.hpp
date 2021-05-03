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

/**
 * @namespace NFIQ2
 *
 * @brief This namespace contains classes and functions that contribute to
 * the calculation of NFIQ 2 scores.
 */
namespace NFIQ2 {

/**
 * @class Impl
 *
 * @brief This class serves as a wrapper to return quality scores
 * for a fingerprint image
 */
class Algorithm::Impl {
    public:
	/**
	 * @brief Default constructor of Impl
	 */
	Impl();

	/**
	 * @brief Constructor of Impl using model filename and the file's
	 * hash
	 */
	Impl(const std::string &fileName, const std::string &fileHash);

	/**
	 * @brief Destructor
	 */
	virtual ~Impl();

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
	 * @brief Determine if random forest parameters have been loaded.
	 *
	 * @return true if some set of random forest parameters have been
	 * loaded, false otherwise.
	 */
	bool isInitialized() const;

    private:
	/**
	 * @var initialized
	 *
	 * @brief Indicates whether or not random forest parameters have been
	 * loaded.
	 */
	bool initialized { false };

	/**
	 * @fn getQualityPrediction
	 *
	 * @param features list of computed feature metrics that contain quality
	 * information for a fingerprint image
	 *
	 * @return Computed NFIQ 2 quality score
	 *
	 * @throws Exception
	 * Failure to compute (OpenCV reason contained within message string) or
	 * called before random forest parameters loaded.
	 */
	double getQualityPrediction(
	    const std::unordered_map<std::string, NFIQ2::QualityFeatureData>
		&features) const;

	/**
	 * @brief Throw an exception if random forest parameters have not been
	 * loaded.
	 *
	 * @throw NFIQ2::Exception
	 * Random forest parameters have not been loaded.
	 */
	void throwIfUninitialized() const;

	/**
	 * @var m_RandomForestML
	 *
	 * @brief Private member storing RandomForest parameters
	 */
	NFIQ2::Prediction::RandomForestML m_RandomForestML;

	/**
	 * @var m_paramenterHash
	 *
	 * @brief Private member storing RandomForest parameter md5 hash
	 */
	std::string m_parameterHash {};
};
} // namespace NFIQ2

#endif

/******************************************************************************/
