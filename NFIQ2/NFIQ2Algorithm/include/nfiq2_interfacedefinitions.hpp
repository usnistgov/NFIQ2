#ifndef NFIQ2_INTERFACEDEFINITIONS_HPP_
#define NFIQ2_INTERFACEDEFINITIONS_HPP_

#include <nfiq2_data.hpp>

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace NFIQ2 {
/** Contains strings corresponding to Actionable Feedback Scores. */
namespace ActionableQualityFeedbackIdentifier {

static const std::string EmptyImageOrContrastTooLow {
	"EmptyImageOrContrastTooLow"
};
static const std::string UniformImage { "UniformImage" };
static const std::string FingerprintImageWithMinutiae {
	"FingerprintImageWithMinutiae"
};
static const std::string SufficientFingerprintForeground {
	"SufficientFingerprintForeground"
};
}

/** Contains doubles corresponding to Actionable Feedback Thresholds. */
namespace ActionableQualityFeedbackThreshold {
static const double EmptyImageOrContrastTooLow { 250.0 };
static const double UniformImage { 1.0 };
static const double FingerprintImageWithMinutiae { 5.0 };
static const double SufficientFingerprintForeground { 50000.0 };
}

/** ActionableQualityFeedback structure. */
struct actionable_quality_feedback_t {
	/** Name of the actionable quality metric. */
	std::string identifier;
	/** Numerical value associated with the actionable quality metric. */
	double actionableQualityValue;
};
using ActionableQualityFeedback = actionable_quality_feedback_t;

/** This type represents the ID of an image */
typedef struct image_id_t {
	/** User/subject ID of image. */
	std::string subjectID;
	/** ISO finger code of fingerprint in image. */
	uint8_t fingerCode;
	/** Acquisition number of image. */
	uint8_t acquisitionNumber;
} ImageID;

/** This type represents a structure for timing information of features. */
typedef struct feature_speed_t {
	/** The name of the feature group. */
	std::string featureIDGroup;
	/**
	 * The unique IDs of the features that are used for determining the
	 * speed.
	 */
	std::vector<std::string> featureIDs;
	/** The speed of feature data computation in milliseconds. */
	double featureSpeed;
} QualityFeatureSpeed;

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
	/** The ID of the reference/enrollment fingerprint image. */
	NFIQ2::ImageID referenceImageID;
	/**
	 * 	A list of probe images + results for which comparisons
	 * 	where conducted.
	 */
	std::vector<NFIQ2::ComparisonProbeResult> probes;
} ComparisonScoresSample;
} // namespace NFIQ

#endif /* NFIQ2_INTERFACEDEFINITIONS_HPP_ */
