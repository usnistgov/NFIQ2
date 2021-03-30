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
	/**
	 * @fn computeQualityScore
	 * @brief Computes the quality score from the input fingerprint image
	 * data
	 * @throws NFIQException
	 * Failure to compute (reason contained within message string).
	 * @param rawImage fingerprint image in raw format
	 * @param bComputeActionableQuality if to compute actionable quality
	 * flags or not
	 * @param actionableQuality compute actionable quality values
	 * @param bOutputFeatures if to output feature values
	 * @param qualityfeatureData list of computed feature data values
	 * @param bOutputSpeed if to output speed of computed features
	 * @param qualityFeatureSpeed list of feature computation speed
	 * @return achieved quality score
	 */
	unsigned int computeQualityScore(
	    const NFIQ::FingerprintImageData &rawImage,
	    bool bComputeActionableQuality,
	    std::vector<NFIQ::ActionableQualityFeedback> &actionableQuality,
	    bool bOutputFeatures,
	    std::vector<NFIQ::QualityFeatureData> &qualityFeatureData,
	    bool bOutputSpeed,
	    std::vector<NFIQ::QualityFeatureSpeed> &qualityFeatureSpeed) const;

	/**
	 * @throws NFIQException
	 * Failure to compute (OpenCV reason contained within message string).
	 */
	double getQualityPrediction(
	    const std::vector<NFIQ::QualityFeatureData> &featureVector) const;

	NFIQ::Prediction::RandomForestML m_RandomForestML;
	std::string m_parameterHash {};
};
} // namespace NFIQ

#endif

/******************************************************************************/
