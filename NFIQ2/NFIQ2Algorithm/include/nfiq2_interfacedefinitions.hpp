#ifndef NFIQ2_INTERFACEDEFINITIONS_HPP_
#define NFIQ2_INTERFACEDEFINITIONS_HPP_

#include <nfiq2_data.hpp>

#include <string>
#include <vector>

namespace NFIQ2 {
/** Contains strings corresponding to Actionable Feedback Scores. */
namespace ActionableQualityFeedbackIdentifier {

/** EmptyImageOrContrastTooLow. */
static const std::string EmptyImageOrContrastTooLow {
	"EmptyImageOrContrastTooLow"
};
/** UniformImage. */
static const std::string UniformImage { "UniformImage" };
/** FingerprintImageWithMinutiae. */
static const std::string FingerprintImageWithMinutiae {
	"FingerprintImageWithMinutiae"
};
/** SufficientFingerprintForeground. */
static const std::string SufficientFingerprintForeground {
	"SufficientFingerprintForeground"
};
}
/** Contains doubles corresponding to Actionable Feedback Thresholds. */
namespace ActionableQualityFeedbackThreshold {
/** Default value 250.0. */
static const double EmptyImageOrContrastTooLow { 250.0 };
/** Default Image of 1.0. */
static const double UniformImage { 1.0 };
/** Minimum 5 minutiae shall be found. */
static const double FingerprintImageWithMinutiae { 5.0 };
/** Minimum foreground pixels. */
static const double SufficientFingerprintForeground { 50000.0 };
}

/** ActionableQualityFeedback Struct. */
struct actionable_quality_feedback_t {
	std::string identifier;
	double actionableQualityValue;
};
using ActionableQualityFeedback = actionable_quality_feedback_t;

/** This type represents the type of quality feature data. */
typedef enum feature_data_type_e {
	/** Type is not known. */
	e_QualityFeatureDataTypeUnknown = 0,
	/** Feature has floating point format. */
	e_QualityFeatureDataTypeDouble = 1,
	/** Feature has binary data format. */
	e_QualityFeatureDataTypeBinary = 2
} QualityFeatureDataType;

/** This type represents the ID of an image */
typedef struct image_id_t {
	/** User/subject ID of image. */
	std::string subjectID;
	/** ISO finger code of fingerprint in image. */
	uint8_t fingerCode;
	/** Acquisition number of image. */
	uint8_t acquisitionNumber;
} ImageID;

/** This type represents a structure for quality feature data. */
typedef struct feature_data_t {
	/** The unique ID of the feature data. */
	std::string featureID;
	/** The type of feature data. */
	NFIQ2::QualityFeatureDataType featureDataType;
	/**
	 * The feature value in floating point format
	 * (if type is e_QualityFeatureDataTypeDouble).
	 */
	double featureDataDouble;
	/**
	 * The feature value in binary data format
	 * (if type is e_QualityFeatureDataTypeBinary).
	 */
	NFIQ2::Data featureDataBinary;
} QualityFeatureData;

/** This type represents a structure for timing information of features. */
typedef struct feature_speed_t {
	/** The name of the feature group. */
	std::string featureIDGroup;
	/**
	 * The unique IDs of the features that are used for determing the
	 * speed.
	 */
	std::vector<std::string> featureIDs;
	/** The speed of feature data computation in milliseconds. */
	double featureSpeed;
} QualityFeatureSpeed;

/** This type represents a structure for database information. */
typedef struct database_information_t {
	/** ID of the database. */
	std::string databaseID;
	/** Number of fingerprints which are tored in the original database. */
	uint32_t databaseDimension;
} DatabaseInformation;

/** This type represents the result of a quality feature extraction. */
typedef struct quality_feature_result_t {
	/** The quality feature data. */
	NFIQ2::QualityFeatureData featureData;
	/** The return code of the quality feature extraction operation. */
	uint32_t returnCode;
} QualityFeatureResult;

/** This type represents the result of a comparison scores computation. */
typedef struct comparison_result_t {
	/** The image ID of the reference image. */
	NFIQ2::ImageID referenceImageID;
	/** The image ID of the probe image. */
	NFIQ2::ImageID probeImageID;
	/** The comparison score. */
	double comparisonScore;
	/** The return code of the comparison operation */
	uint32_t returnCode;
} ComparisonResult;

/** This type represents the result of a utility value calculation. */
typedef struct utility_result_t {
	/** The utility value. */
	double utilityValue;
	/** The return code of the utility computation operation. */
	uint32_t returnCode;
} UtilityResult;

/**
 * This type represents a sample for utility computation (image ID + utility
 * result).
 */
typedef struct utility_sample_t {
	/** The database ID where the fingerprint image is stored. */
	std::string databaseID;
	/** The ID of the fingerprint image. */
	NFIQ2::ImageID fingerprintImageID;
	/** The result of the utility computation (value + return code). */
	NFIQ2::UtilityResult utilityResult;
} UtilitySample;

/**
 * This type represents the combination of a provider and a list of utility
 * samples.
 */
typedef struct utility_provider_content_t {
	/** The provider ID for which the utility was computed. */
	std::string providerID;
	/**
	 * A list of utility values and its information
	 * (imageID, return code).
	 */
	std::vector<NFIQ2::UtilitySample> utilitySamples;
} UtilityProviderContent;

/** This type represents the content of an utility exchange file. */
typedef struct utility_content_t {
	/** The ID of the database. */
	std::string databaseID;
	/** The utility ID. */
	std::string utilityID;
	/** A list of provider IDs. */
	std::vector<std::string> providerIDs;
	/** Results of the utility computation for images of the database */
	std::vector<NFIQ2::UtilitySample> samples;
} UtilityContent;

/** This type represents the content of a quality feature exchange file. */
typedef struct quality_feature_sample_t {
	/** The ID of the fingerprint image. */
	NFIQ2::ImageID fingerprintImageID;
	/**
	 * The result of the quality feature computation
	 * (value + return code).
	 */
	NFIQ2::QualityFeatureResult featureResult;
} QualityFeatureSample;

/** This type represents the result of a quality feature exchange file. */
typedef struct quality_feature_content_t {
	/** The ID of the database. */
	std::string databaseID;
	/** The feature ID. */
	std::string featureID;
	/**
	 * Results of the quality feature computation for images of the
	 * database.
	 */
	std::vector<NFIQ2::QualityFeatureSample> samples;
} QualityFeatureContent;

/**
 * This type represents the structure of a probe result for comparison scores
 * computation.
 */
typedef struct comparison_probe_result_t {
	/** The ID of the probe/verification fingerprint image. */
	NFIQ2::ImageID probeImageID;
	/**
	 *	The comparison type: "g" - Genuine comparison,
	 *	"i" ... Impostor comparison,
	 * 	"x" ... Cross-comparison between same person.
	 */
	std::string comparisonType;
	/** Achieved score and return code between probe and reference image. */
	NFIQ2::ComparisonResult comparisonResult;
} ComparisonProbeResult;

/** This type represents the structure of a comparison scores sample. */
typedef struct comparison_scores_sample_t {
	/** The ID of the reference/enrolment fingerprint image. */
	NFIQ2::ImageID referenceImageID;
	/**
	 * 	A list of probe images + results for which comparisons
	 * 	where conducted.
	 */
	std::vector<NFIQ2::ComparisonProbeResult> probes;
} ComparisonScoresSample;

/** This type represents the content of a comparison scores exchange file. */
typedef struct comparison_scores_content_t {
	/** The ID of the database. */
	std::string databaseID;
	/** The ID of the provider used for comparison scores computation. */
	std::string providerID;
	/**
	 *	Results of the comparison scores computation for
	 * 	this reference fingerprint image.
	 */
	std::vector<NFIQ2::ComparisonScoresSample> references;
} ComparisonScoresContent;

/** This type represents the structure of a sample needed for training. */
typedef struct training_sample_t {
	/** The quality feature data vector used for training. */
	std::vector<NFIQ2::QualityFeatureData> featureDataVector;
	/** The assigned utility value. */
	double utilityValue;
	/** The weight of the current value [0..1]. */
	double weight;
} TrainingSample;

/** This type represents the result of a Machine Learning evaluation run. */
typedef struct evaluation_result_t {
	/** The ID of the database. */
	std::string databaseID;
	/** The ID of the fingerprint image. */
	NFIQ2::ImageID fingerprintImageID;
	/** The utility value. */
	double utilityValue;
	/** The quality score for the fingerprint image. */
	double qualityScore;
	/** The deviation of the quality score and utility value. */
	double deviation;
} EvaluationResult;

/** This type represents the weight assigned to a fingerprint image ID. */
typedef struct weight_image_id_t {
	/** The ID of the fingerprint image. */
	NFIQ2::ImageID fingerprintImageID;
	/** The set (pattern) weight [0..1]. */
	double weight;
} WeightImageID;

/**
 * This type represents a list of weight image IDs for a dedicated database =
 * training ID.
 */
typedef struct training_id_t {
	/** The ID of the database. */
	std::string databaseID;
	/** A list of image IDs + their weights. */
	std::vector<NFIQ2::WeightImageID> imageIDs;
} TrainingID;

/** This type represents the image ID and an assigned database ID. */
typedef struct database_image_id {
	/** The ID of the fingerprint image. */
	NFIQ2::ImageID fingerprintImageID;
	/** The ID of the database where image is stored. */
	std::string databaseID;
} DatabaseImageID;

/**
 * This type represents a list of image IDs and an assigned database ID for
 * evaluation.
 */
typedef struct evaluation_id {
	/** The ID of the database where image is stored. */
	std::string databaseID;
	/** A list of image IDs. */
	std::vector<NFIQ2::ImageID> imageIDs;
} EvaluationID;
} // namespace NFIQ

#endif /* NFIQ2_INTERFACEDEFINITIONS_HPP_ */
