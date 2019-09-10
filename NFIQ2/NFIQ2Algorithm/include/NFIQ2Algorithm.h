#ifndef NFIQ2ALGORITHM_H
#define NFIQ2ALGORITHM_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <list>

#include "include/NFIQException.h"
#include "include/FingerprintImageData.h"
#include "include/InterfaceDefinitions.h"

#include "RandomForestML.h"

namespace NFIQ
{
	static const std::string ActionableQualityFeedbackIdentifier_EmptyImageOrContrastTooLow = "EmptyImageOrContrastTooLow";
	static const std::string ActionableQualityFeedbackIdentifier_UniformImage = "UniformImage";
	static const std::string ActionableQualityFeedbackIdentifier_FingerprintImageWithMinutiae = "FingerprintImageWithMinutiae";
	static const std::string ActionableQualityFeedbackIdentifier_SufficientFingerprintForeground = "SufficientFingerprintForeground";

	static const double ActionableQualityFeedbackThreshold_EmptyImageOrContrastTooLow = 250.0;
	static const double ActionableQualityFeedbackThreshold_UniformImage = 1.0;
	static const double ActionableQualityFeedbackThreshold_FingerprintImageWithMinutiae = 5.0; // minimum 5 minutiae shall be found
	static const double ActionableQualityFeedbackThreshold_SufficientFingerprintForeground = 50000.0; // minimum foreground pixels

	/**
	* This type represents a structure for actionable quality feedback
	*/
	typedef struct actionable_quality_feedback_t
	{
		std::string		identifier;		///< ID of the actionable quality
		double			actionableQualityValue;	///< actionable quality value
	} ActionableQualityFeedback;

	/**
	******************************************************************************
	* @class NFIQ2Algorithm
	* @brief This class serves as a wrapper to return quality scores for a
	* fingerprint image
	******************************************************************************/
	class NFIQ2Algorithm 
	{
	public:
		/******************************************************************************/
		// --- Constructor / Destructor --- //
		/******************************************************************************/

		/**
		* @brief Default constructor of NFIQ2Algorithm
		*/
		NFIQ2Algorithm();

		/**
		* @brief Destructor
		*/
		virtual ~NFIQ2Algorithm();

		/******************************************************************************/
		// --- Public functions --- //
		/******************************************************************************/

		/**
		* @fn computeQualityScore
		* @brief Computes the quality score from the input fingerprint image data
		* @param rawImage fingerprint image in raw format
		* @param bComputeActionableQuality if to compute actionable quality flags or not
		* @param actionableQuality compute actionable quality values
		* @param bOutputFeatures if to output feature values
		* @param qualityfeatureData list of computed feature data values
		* @param bOutputSpeed if to output speed of computed features
		* @param qualityFeatureSpeed list of feature computation speed
		* @return achieved quality score
		*/
		unsigned int computeQualityScore(
			NFIQ::FingerprintImageData rawImage, 
			bool bComputeActionableQuality, std::list<NFIQ::ActionableQualityFeedback> & actionableQuality,
			bool bOutputFeatures, std::list<NFIQ::QualityFeatureData> & qualityFeatureData,
			bool bOutputSpeed, std::list<NFIQ::QualityFeatureSpeed> & qualityFeatureSpeed);

	private:
		std::list<NFIQ::QualityFeatureData> computeQualityFeatures(
			const NFIQ::FingerprintImageData & rawImage,
			bool bComputeActionableQuality, std::list<NFIQ::ActionableQualityFeedback> & actionableQuality,
			bool bOutputSpeed, std::list<NFIQ::QualityFeatureSpeed> & speedValues);
		double getQualityPrediction(std::list<NFIQ::QualityFeatureData> & featureVector);

		RandomForestML m_RandomForestML;
	};
}

#endif

/******************************************************************************/
