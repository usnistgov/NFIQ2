#ifndef NFIQ2ALGORITHMIMPL_H_
#define NFIQ2ALGORITHMIMPL_H_

#include <nfiq2.hpp>
#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_interfacedefinitions.hpp>
#include <nfiq2_nfiqexception.hpp>
#include <prediction/RandomForestML.h>

#include "nfiq2impl.h"

#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

namespace NFIQ {
/**
******************************************************************************
* @class Impl
* @brief This class serves as a wrapper to return quality scores for a
* fingerprint image
******************************************************************************/
class NFIQ2Algorithm::Impl {
    public:
	/******************************************************************************/
	// --- Constructor / Destructor --- //
	/******************************************************************************/

	/**
	 * @brief Default constructor of Impl
	 */
#ifdef EMBED_RANDOMFOREST_PARAMETERS
	Impl();
#endif
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
	    const NFIQ::FingerprintImageData &rawImage) const;

	/**
	 * @fn computeQualityScore
	 * @brief Computes the quality score from a vector of extracted feature
	 * from a cropped fingerprint image
	 * @param features list of computed feature metrics that contain quality
	 * information for a fingerprint image
	 * @return achieved quality score
	 */
	unsigned int computeQualityScore(const std::vector<
	    std::shared_ptr<NFIQ::QualityFeatures::BaseFeature>> &features)
	    const;

	/**
 	 * @fn computeQualityScore
 	 * @brief Computes the quality score from the extracted image
 	 * quality feature data
 	 * @param featureMap map of string, quality feature data pairs
 	 * @return achieved quality score
 	 */
 	unsigned int computeQualityScore(
 	    const std::unordered_map<std::string, NFIQ::QualityFeatureData> &featureMap)
 	    const;

	/**
	 * @brief
	 * Obtain MD5 checksum of Random Forest parameter file loaded.
	 *
	 * @return
	 * MD5 checksum of the Random Forest parameter file loaded.
	 */
	std::string getParameterHash() const;

    private:
	/**
	 * @throws NFIQException
	 * Failure to compute (OpenCV reason contained within message string).
	 */
	double getQualityPrediction(
	    const std::unordered_map<std::string, NFIQ::QualityFeatureData>
		&featureMap) const;

	NFIQ::Prediction::RandomForestML m_RandomForestML;
	std::string m_parameterHash {};
};
} // namespace NFIQ

#endif

/******************************************************************************/
