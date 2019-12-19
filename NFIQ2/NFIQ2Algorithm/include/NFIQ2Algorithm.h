#ifndef NFIQ2ALGORITHM_H_
#define NFIQ2ALGORITHM_H_

#include <list>
#include <memory>
#include <string>

#include "include/FingerprintImageData.h"
#include "include/InterfaceDefinitions.h"

namespace NFIQ
{
	static const std::string 
	    ActionableQualityFeedbackIdentifier_EmptyImageOrContrastTooLow{
	    "EmptyImageOrContrastTooLow"};
	static const std::string
	    ActionableQualityFeedbackIdentifier_UniformImage{
	    "UniformImage"};
	static const std::string 
	    ActionableQualityFeedbackIdentifier_FingerprintImageWithMinutiae{
	    "FingerprintImageWithMinutiae"};
	static const std::string
	    ActionableQualityFeedbackIdentifier_SufficientFingerprintForeground{
	    "SufficientFingerprintForeground"};

	static const double 
	    ActionableQualityFeedbackThreshold_EmptyImageOrContrastTooLow{
	    250.0};
	static const double 
	    ActionableQualityFeedbackThreshold_UniformImage{1.0};
	/** Minimum 5 minutiae shall be found */
	static const double 
	    ActionableQualityFeedbackThreshold_FingerprintImageWithMinutiae{
	    5.0};
	/** Minimum foreground pixels */
	static const double 
	    ActionableQualityFeedbackThreshold_SufficientFingerprintForeground{
	    50000.0};

	struct actionable_quality_feedback_t
	{
		std::string identifier;
		double actionableQualityValue;	
	};
	using ActionableQualityFeedback = actionable_quality_feedback_t;

	/** Wrapper to return quality scores for a fingerprint image */
	class NFIQ2Algorithm 
	{
	public:
#ifdef EMBED_RANDOMFOREST_PARAMETERS
		NFIQ2Algorithm();
#endif
		NFIQ2Algorithm(
		    const std::string &fileName, 
		    const std::string &fileHash);
		~NFIQ2Algorithm();

		/**
		 * @brief
		 * Computes the quality score from the input fingerprint image 
		 * data.
		 *
		 * @param rawImage 
		 * Fingerprint image in raw format
		 * @param bComputeActionableQuality 
		 * If to compute actionable quality flags or not
		 * @param actionableQuality 
		 * Compute actionable quality values
		 * @param bOutputFeatures 
		 * If to output feature values
		 * @param qualityfeatureData 
		 * List of computed feature data values
		 * @param bOutputSpeed 
		 * If to output speed of computed features
		 * @param qualityFeatureSpeed 
		 * List of feature computation speed
		 *
		 * @return
		 * Achieved quality score
		 */
		unsigned int 
		computeQualityScore(
		    NFIQ::FingerprintImageData rawImage, 
		    bool bComputeActionableQuality, 
		    std::list<NFIQ::ActionableQualityFeedback> 
		        &actionableQuality,
		    bool bOutputFeatures, 
		    std::list<NFIQ::QualityFeatureData> &qualityFeatureData,
		    bool bOutputSpeed, 
		    std::list<NFIQ::QualityFeatureSpeed> &qualityFeatureSpeed);

		/**
		 * @brief
		 * Obtain MD5 checksum of Random Forest parameter file loaded.
		 *
		 * @return
		 * MD5 checksum of the Random Forest parameter file loaded.
		 */
		std::string
		getParameterHash()
		    const;

	private:
		class Impl;
		std::unique_ptr<NFIQ2Algorithm::Impl> pimpl;
	};
}

#endif
