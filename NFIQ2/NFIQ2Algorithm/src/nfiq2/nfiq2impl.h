#ifndef NFIQ2ALGORITHMIMPL_H_
#define NFIQ2ALGORITHMIMPL_H_

#include <nfiq2/fingerprintimagedata.hpp>
#include <nfiq2/interfacedefinitions.hpp>
#include <nfiq2/nfiq2.hpp>
#include <nfiq2/nfiqexception.hpp>
#include <nfiq2/prediction/RandomForestML.h>

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
	/**
	 * @throws NFIQException
	 * Failure to compute (OpenCV reason contained within message string).
	 */
	double getQualityPrediction(
	    std::list<NFIQ::QualityFeatureData> &featureVector) const;

	NFIQ::Prediction::RandomForestML m_RandomForestML;
	std::string m_parameterHash {};
};

class NFIQ2Results::Impl {
    public:
	Impl();
	Impl(std::vector<NFIQ::ActionableQualityFeedback> actionableQuality,
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
	std::vector<NFIQ::ActionableQualityFeedback> actionableQuality_ {};
	std::vector<NFIQ::QualityFeatureData> qualityfeatureData_ {};
	std::vector<NFIQ::QualityFeatureSpeed> qualityFeatureSpeed_ {};
};
} // namespace NFIQ

#endif

/******************************************************************************/
