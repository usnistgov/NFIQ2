#ifndef NFIQ2_INTERFACEDEFINITIONS_HPP_
#define NFIQ2_INTERFACEDEFINITIONS_HPP_

#include <nfiq2_data.hpp>

#include <string>
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

/** Identifiers of individual quality features. */
namespace QualityFeatureIDs {
/** Frequency of the sinusoid following the ridge-valley structure. */
namespace FrequencyDomainAnalysis {
/**
 * Local quality values represented as a histogram.
 *
 * @details
 * Bins for the histogram are defined in the NFIQ 2 Report
 */
namespace Histogram {
/** First of ten histogram bin boundaries. */
static const std::string Bin0 { "FDA_Bin10_0" };
/** Second of ten histogram bin boundaries. */
static const std::string Bin1 { "FDA_Bin10_1" };
/** Third of ten histogram bin boundaries. */
static const std::string Bin2 { "FDA_Bin10_2" };
/** Fourth of ten histogram bin boundaries. */
static const std::string Bin3 { "FDA_Bin10_3" };
/** Fifth of ten histogram bin boundaries. */
static const std::string Bin4 { "FDA_Bin10_4" };
/** Sixth of ten histogram bin boundaries. */
static const std::string Bin5 { "FDA_Bin10_5" };
/** Seventh of ten histogram bin boundaries. */
static const std::string Bin6 { "FDA_Bin10_6" };
/** Eighth of ten histogram bin boundaries. */
static const std::string Bin7 { "FDA_Bin10_7" };
/** Ninth of ten histogram bin boundaries. */
static const std::string Bin8 { "FDA_Bin10_8" };
/** Tenth of ten histogram bin boundaries. */
static const std::string Bin9 { "FDA_Bin10_9" };
}

/** Mean of local quality value. */
static const std::string Mean { "FDA_Bin10_Mean" };
/** Standard deviation of local quality values. */
static const std::string StdDev { "FDA_Bin10_StdDev" };
}

/**
 * Measures relative to minutiae detected within the image using
 * the integrated feature extractor, FingerJet FX OSE.
 */
namespace Minutiae {
/** Number of minutiae in the image. */
static const std::string Count { "FingerJetFX_MinutiaeCount" };

/**
 * Number of minutiae lying in a 200x200 pixel rectangle
 * centered at the center of mass of the locations of all
 * detected minutia.
 */
static const std::string CountCOM { "FingerJetFX_MinCount_COMMinRect200x200" };

/**
 * Percentage of minutiae whose quality value, as determined by
 * Grayscale::Mean of a 32x32 pixel region centered on the
 * minutiae, is between 0-0.5.
 *
 * @see Grayscale::Mean
 */
static const std::string QualityMu2 { "FJFXPos_Mu_MinutiaeQuality_2" };

/**
 * Percentage of minutiae whose quality value, as determined by
 * the Orientation Certainty Level of a 32x32 pixel region
 * centered on the minutiae, is above 80.
 *
 * @see OrientationCertainty
 */
static const std::string QualityOCL80 { "FJFXPos_OCL_MinutiaeQuality_80" };
}

/** Measures based on the foreground area of the image. */
namespace RegionOfInterest {
/**
 * Mean grayscale value of the number of 32x32 pixel regions
 * having at least 1 pixel in the ROI.
 */
static const std::string Mean { "ImgProcROIArea_Mean" };

/**
 * Sum of coherence values over all 16x16 pixel regions within
 * the ROI.
 */
static const std::string CoherenceSum {
	"OrientationMap_ROIFilter_CoherenceSum"
};

/**
 * Average of coherence values over all 16x16 pixel regions
 * within the ROI.
 *
 * @details
 * This is computed as RegionOfInterest::CoherenceSum / (number
 * of 16x16 pixel regions that contain at least 1 pixel within
 * the ROI).
 *
 * @see CoherenceSum
 */
static const std::string CoherenceMean {
	"OrientationMap_ROIFilter_CoherenceRel"
};
}

/** Measure of the clarity of ridge and valleys. */
namespace LocalClarity {
/**
 * Local quality values represented as a histogram.
 *
 * @details
 * Bins for the histogram are defined in the NFIQ 2 Report
 */
namespace Histogram {
/** First of ten histogram bin boundaries. */
static const std::string Bin0 { "LCS_Bin10_0" };
/** Second of ten histogram bin boundaries. */
static const std::string Bin1 { "LCS_Bin10_1" };
/** Third of ten histogram bin boundaries. */
static const std::string Bin2 { "LCS_Bin10_2" };
/** Fourth of ten histogram bin boundaries. */
static const std::string Bin3 { "LCS_Bin10_3" };
/** Fifth of ten histogram bin boundaries. */
static const std::string Bin4 { "LCS_Bin10_4" };
/** Sixth of ten histogram bin boundaries. */
static const std::string Bin5 { "LCS_Bin10_5" };
/** Seventh of ten histogram bin boundaries. */
static const std::string Bin6 { "LCS_Bin10_6" };
/** Eighth of ten histogram bin boundaries. */
static const std::string Bin7 { "LCS_Bin10_7" };
/** Ninth of ten histogram bin boundaries. */
static const std::string Bin8 { "LCS_Bin10_8" };
/** Tenth of ten histogram bin boundaries. */
static const std::string Bin9 { "LCS_Bin10_9" };
}

/** Mean of local quality value. */
static const std::string Mean { "LCS_Bin10_Mean" };
/** Standard deviation of local quality values. */
static const std::string StdDev { "LCS_Bin10_StdDev" };
}

/** Measure of the gray levels of the image. */
namespace Grayscale {
/** Arithmetic mean of the image. */
static const std::string Mean { "Mu" };
/** Arithmetic mean of per-block means of the image. */
static const std::string MeanBlock { "MMB" };
}

/**
 * Measure of the strength of the energy concentration along the
 * dominant ridge flow orientation.
 */
namespace OrientationCertainty {
/**
 * Local quality values represented as a histogram.
 *
 * @details
 * Bins for the histogram are defined in the NFIQ 2 Report
 */
namespace Histogram {
/** First of ten histogram bin boundaries. */
static const std::string Bin0 { "OCL_Bin10_0" };
/** Second of ten histogram bin boundaries. */
static const std::string Bin1 { "OCL_Bin10_1" };
/** Third of ten histogram bin boundaries. */
static const std::string Bin2 { "OCL_Bin10_2" };
/** Fourth of ten histogram bin boundaries. */
static const std::string Bin3 { "OCL_Bin10_3" };
/** Fifth of ten histogram bin boundaries. */
static const std::string Bin4 { "OCL_Bin10_4" };
/** Sixth of ten histogram bin boundaries. */
static const std::string Bin5 { "OCL_Bin10_5" };
/** Seventh of ten histogram bin boundaries. */
static const std::string Bin6 { "OCL_Bin10_6" };
/** Eighth of ten histogram bin boundaries. */
static const std::string Bin7 { "OCL_Bin10_7" };
/** Ninth of ten histogram bin boundaries. */
static const std::string Bin8 { "OCL_Bin10_8" };
/** Tenth of ten histogram bin boundaries. */
static const std::string Bin9 { "OCL_Bin10_9" };
}

/** Mean of local quality value. */
static const std::string Mean { "OCL_Bin10_Mean" };
/** Standard deviation of local quality values. */
static const std::string StdDev { "OCL_Bin10_StdDev" };
}

/**
 * Measure of the ridge flow continuity, based on the absolute
 * orientation difference between a block in its 8-neighborhood.
 */
namespace OrientationFlow {
/**
 * Local quality values represented as a histogram.
 *
 * @details
 * Bins for the histogram are defined in the NFIQ 2 Report
 */
namespace Histogram {
/** First of ten histogram bin boundaries. */
static const std::string Bin0 { "OF_Bin10_0" };
/** Second of ten histogram bin boundaries. */
static const std::string Bin1 { "OF_Bin10_1" };
/** Third of ten histogram bin boundaries. */
static const std::string Bin2 { "OF_Bin10_2" };
/** Fourth of ten histogram bin boundaries. */
static const std::string Bin3 { "OF_Bin10_3" };
/** Fifth of ten histogram bin boundaries. */
static const std::string Bin4 { "OF_Bin10_4" };
/** Sixth of ten histogram bin boundaries. */
static const std::string Bin5 { "OF_Bin10_5" };
/** Seventh of ten histogram bin boundaries. */
static const std::string Bin6 { "OF_Bin10_6" };
/** Eighth of ten histogram bin boundaries. */
static const std::string Bin7 { "OF_Bin10_7" };
/** Ninth of ten histogram bin boundaries. */
static const std::string Bin8 { "OF_Bin10_8" };
/** Tenth of ten histogram bin boundaries. */
static const std::string Bin9 { "OF_Bin10_9" };
}

/** Mean of local quality value. */
static const std::string Mean { "OF_Bin10_Mean" };
/** Standard deviation of local quality values. */
static const std::string StdDev { "OF_Bin10_StdDev" };
}

/** Measure of the consistency of ridge and valley widths. */
namespace RidgeValleyUniformity {
/**
 * Local quality values represented as a histogram.
 *
 * @details
 * Bins for the histogram are defined in the NFIQ 2 Report
 */
namespace Histogram {
/** First of ten histogram bin boundaries. */
static const std::string Bin0 { "RVUP_Bin10_0" };
/** Second of ten histogram bin boundaries. */
static const std::string Bin1 { "RVUP_Bin10_1" };
/** Third of ten histogram bin boundaries. */
static const std::string Bin2 { "RVUP_Bin10_2" };
/** Fourth of ten histogram bin boundaries. */
static const std::string Bin3 { "RVUP_Bin10_3" };
/** Fifth of ten histogram bin boundaries. */
static const std::string Bin4 { "RVUP_Bin10_4" };
/** Sixth of ten histogram bin boundaries. */
static const std::string Bin5 { "RVUP_Bin10_5" };
/** Seventh of ten histogram bin boundaries. */
static const std::string Bin6 { "RVUP_Bin10_6" };
/** Eighth of ten histogram bin boundaries. */
static const std::string Bin7 { "RVUP_Bin10_7" };
/** Ninth of ten histogram bin boundaries. */
static const std::string Bin8 { "RVUP_Bin10_8" };
/** Tenth of ten histogram bin boundaries. */
static const std::string Bin9 { "RVUP_Bin10_9" };
}

/** Mean of local quality value. */
static const std::string Mean { "RVUP_Bin10_Mean" };
/** Standard deviation of local quality values. */
static const std::string StdDev { "RVUP_Bin10_StdDev" };
}
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
