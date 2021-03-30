#ifndef NFIQ2_INTERFACEDEFINITIONS_HPP_
#define NFIQ2_INTERFACEDEFINITIONS_HPP_

#include <nfiq2_data.hpp>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <string>
#include <vector>

namespace NFIQ {
static const std::string
    ActionableQualityFeedbackIdentifier_EmptyImageOrContrastTooLow {
	    "EmptyImageOrContrastTooLow"
    };
static const std::string ActionableQualityFeedbackIdentifier_UniformImage {
	"UniformImage"
};
static const std::string
    ActionableQualityFeedbackIdentifier_FingerprintImageWithMinutiae {
	    "FingerprintImageWithMinutiae"
    };
static const std::string
    ActionableQualityFeedbackIdentifier_SufficientFingerprintForeground {
	    "SufficientFingerprintForeground"
    };

static const double
    ActionableQualityFeedbackThreshold_EmptyImageOrContrastTooLow { 250.0 };
static const double ActionableQualityFeedbackThreshold_UniformImage { 1.0 };
/** Minimum 5 minutiae shall be found */
static const double
    ActionableQualityFeedbackThreshold_FingerprintImageWithMinutiae { 5.0 };
/** Minimum foreground pixels */
static const double
    ActionableQualityFeedbackThreshold_SufficientFingerprintForeground {
	    50000.0
    };

struct actionable_quality_feedback_t {
	std::string identifier;
	double actionableQualityValue;
};
using ActionableQualityFeedback = actionable_quality_feedback_t;

/**
 * This type represents the type of quality feature data.
 */
typedef enum feature_data_type_e {
	e_QualityFeatureDataTypeUnknown = 0, ///< Type is not known
	e_QualityFeatureDataTypeDouble =
	    1, ///< Feature has floating point format
	e_QualityFeatureDataTypeBinary = 2 ///< Feature has binary data format
} QualityFeatureDataType;

/**
 * This type represents the ID of an image
 */
typedef struct image_id_t {
	std::string subjectID;	   ///< User/subject ID of image
	uint8_t fingerCode;	   ///< ISO finger code of fingerprint in image
	uint8_t acquisitionNumber; ///< Acquisition number of image
} ImageID;

/**
 * This type represents a structure for quality feature data
 */
typedef struct feature_data_t {
	std::string featureID; ///< The unique ID of the feature data
	NFIQ::QualityFeatureDataType
	    featureDataType;	      ///< The type of feature data
	double featureDataDouble;     ///< The feature value in floating point
				      ///< format (if type is
				      ///< e_QualityFeatureDataTypeDouble)
	NFIQ::Data featureDataBinary; ///< The feature value in binary data
				      ///< format (if type is
				      ///< e_QualityFeatureDataTypeBinary)
} QualityFeatureData;

/**
 * This type represents a structure for timing information of features
 */
typedef struct feature_speed_t {
	std::string featureIDGroup; ///< The name of the feature group
	std::vector<std::string>
	    featureIDs;	     ///< The unique IDs of the features that
			     ///< are used for determing the speed
	double featureSpeed; ///< The speed of feature data computation in
			     ///< milliseconds
} QualityFeatureSpeed;

/**
 * This type represents a structure for database information
 */
typedef struct database_information_t {
	std::string databaseID;	    ///< ID of the database
	uint32_t databaseDimension; ///< number of fingerprints which are
				    ///< stored in the original database
} DatabaseInformation;

/**
 * This type represents the result of a quality feature extraction
 */
typedef struct quality_feature_result_t {
	NFIQ::QualityFeatureData featureData; ///< The quality feature data
	uint32_t returnCode; ///< The return code of the quality feature
			     ///< extraction operation
} QualityFeatureResult;

/**
 * This type represents the result of a comparison scores computation
 */
typedef struct comparison_result_t {
	NFIQ::ImageID referenceImageID; ///< The image ID of the reference image
	NFIQ::ImageID probeImageID;	///< The image ID of the probe image
	double comparisonScore;		///< The comparison score
	uint32_t returnCode; ///< The return code of the comparison operation
} ComparisonResult;

/**
 * This type represents the result of a utility value calculation
 */
typedef struct utility_result_t {
	double utilityValue; ///< The utility value
	uint32_t returnCode; ///< The return code of the utility computation
			     ///< operation
} UtilityResult;

/**
 * This type represents a sample for utility computation (image ID + utility
 * result)
 */
typedef struct utility_sample_t {
	std::string databaseID; ///< The database ID where the fingerprint
				///< image is stored
	NFIQ::ImageID fingerprintImageID; ///< The ID of the fingerprint image
	NFIQ::UtilityResult
	    utilityResult; ///< The result of the utility
			   ///< computation (value + return code)
} UtilitySample;

/**
 * This type represents the combination of a provider and a list of utility
 * samples
 */
typedef struct utility_provider_content_t {
	std::string providerID; ///< The provider ID for which the utility was
				///< computed
	std::vector<NFIQ::UtilitySample>
	    utilitySamples; ///< A list of utility values and its
			    ///< information (imageID, return code)
} UtilityProviderContent;

/**
 * This type represents the content of an utility exchange file
 */
typedef struct utility_content_t {
	std::string databaseID;		      ///< The ID of the database
	std::string utilityID;		      ///< The utility ID
	std::vector<std::string> providerIDs; ///< A list of provider IDs
	std::vector<NFIQ::UtilitySample>
	    samples; ///< Results of the utility computation for images of
		     ///< the database
} UtilityContent;

/**
 * This type represents the content of a quality feature exchange file
 */
typedef struct quality_feature_sample_t {
	NFIQ::ImageID fingerprintImageID; ///< The ID of the fingerprint image
	NFIQ::QualityFeatureResult
	    featureResult; ///< The result of the quality
			   ///< feature computation (value
			   ///< + return code)
} QualityFeatureSample;

/**
 * This type represents the result of a quality feature exchange file
 */
typedef struct quality_feature_content_t {
	std::string databaseID; ///< The ID of the database
	std::string featureID;	///< The feature ID
	std::vector<NFIQ::QualityFeatureSample>
	    samples; ///< Results of the quality feature computation for
		     ///< images of the database
} QualityFeatureContent;

/**
 * This type represents the structure of a probe result for comparison scores
 * computation
 */
typedef struct comparison_probe_result_t {
	NFIQ::ImageID probeImageID; ///< The ID of the probe/verification
				    ///< fingerprint image
	std::string
	    comparisonType; ///< The comparison type: "g" ... Genuine
			    ///< comparison, "i" ... Impostor comparison,
			    ///< "x"
			    ///< ... Cross-comparison between same person
	NFIQ::ComparisonResult
	    comparisonResult; ///< Achieved score and return code between
			      ///< probe and reference image
} ComparisonProbeResult;

/**
 * This type represents the structure of a comparison scores sample
 */
typedef struct comparison_scores_sample_t {
	NFIQ::ImageID referenceImageID; ///< The ID of the reference/enrolment
					///< fingerprint image
	std::vector<NFIQ::ComparisonProbeResult>
	    probes; ///< A list of probe images + results for which
		    ///< comparisons where conducted
} ComparisonScoresSample;

/**
 * This type represents the content of a comparison scores exchange file
 */
typedef struct comparison_scores_content_t {
	std::string databaseID; ///< The ID of the database
	std::string providerID; ///< The ID of the provider used for comparison
				///< scores computation
	std::vector<NFIQ::ComparisonScoresSample>
	    references; ///< Results of the comparison scores computation
			///< for this reference fingerprint image
} ComparisonScoresContent;

/**
 * This type represents the structure of a sample needed for training
 */
typedef struct training_sample_t {
	std::vector<NFIQ::QualityFeatureData>
	    featureDataVector; ///< The quality feature data vector used
			       ///< for training
	double utilityValue;   ///< The assigned utility value
	double weight;	       ///< The weight of the current value [0..1]
} TrainingSample;

/**
 * This type represents the result of a Machine Learning evaluation run
 */
typedef struct evaluation_result_t {
	std::string databaseID;		  ///< The ID of the database
	NFIQ::ImageID fingerprintImageID; ///< The ID of the fingerprint image
	double utilityValue;		  ///< The utility value
	double qualityScore; ///< The quality score for the fingerprint image
	double
	    deviation; ///< The deviation of the quality score and utility value
} EvaluationResult;

/**
 * This type represents the weight assigned to a fingerprint image ID
 */
typedef struct weight_image_id_t {
	NFIQ::ImageID fingerprintImageID; ///< The ID of the fingerprint image
	double weight;			  ///< The set (pattern) weight [0..1].
} WeightImageID;

/**
 * This type represents a list of weight image IDs for a dedicated database =
 * training ID
 */
typedef struct training_id_t {
	std::string databaseID; ///< The ID of the database
	std::vector<NFIQ::WeightImageID>
	    imageIDs; ///< A list of image IDs + their weights
} TrainingID;

/**
 * This type represents the image ID and an assigned database ID
 */
typedef struct database_image_id {
	NFIQ::ImageID fingerprintImageID; ///< The ID of the fingerprint image
	std::string
	    databaseID; ///< The ID of the database where image is stored
} DatabaseImageID;

/**
 * This type represents a list of image IDs and an assigned database ID for
 * evaluation
 */
typedef struct evaluation_id {
	std::string
	    databaseID; ///< The ID of the database where image is stored
	std::vector<NFIQ::ImageID> imageIDs; ///< A list of image IDs
} EvaluationID;
} // namespace NFIQ

#endif

/******************************************************************************/
