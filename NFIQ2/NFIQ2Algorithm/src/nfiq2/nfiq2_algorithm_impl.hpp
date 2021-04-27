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
******************************************************************************
* @class Impl
* @brief This class serves as a wrapper to return quality scores for a
* fingerprint image
******************************************************************************/
class Algorithm::Impl {
    public:
	/******************************************************************************/
	// --- Constructor / Destructor --- //
	/******************************************************************************/

	/**
	 * @brief Default constructor of Impl
	 */
	Impl();
	Impl(const std::string &fileName, const std::string &fileHash);

	/**
	 * @brief Destructor
	 */
	virtual ~Impl();

	/******************************************************************************/
	// --- Public functions --- //
	/******************************************************************************/

	/**
	 * @fn computeQualityScore
	 * @brief Computes the quality score from the input fingerprint image
	 * data
	 * @param rawImage fingerprint image in raw format
	 * @return achieved quality score
	 */
	unsigned int computeQualityScore(
	    const NFIQ2::FingerprintImageData &rawImage) const;

	/**
	 * @fn computeQualityScore
	 * @brief Computes the quality score from a vector of extracted feature
	 * from a cropped fingerprint image
	 * @param features list of computed feature metrics that contain quality
	 * information for a fingerprint image
	 * @return achieved quality score
	 */
	unsigned int computeQualityScore(const std::vector<
	    std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>> &features)
	    const;

	/**
	 * @fn computeQualityScore
	 * @brief Computes the quality score from the extracted image
	 * quality feature data
	 * @param features map of string, quality feature data pairs
	 * @return achieved quality score
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
	 */
	std::string getParameterHash() const;

    private:
	bool initialized {};
	/**
	 * @throws Exception
	 * Failure to compute (OpenCV reason contained within message string).
	 */
	double getQualityPrediction(
	    const std::unordered_map<std::string, NFIQ2::QualityFeatureData>
		&features) const;

	NFIQ2::Prediction::RandomForestML m_RandomForestML;
	std::string m_parameterHash {};
};
} // namespace NFIQ2

#endif

/******************************************************************************/
