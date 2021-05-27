#ifndef NFIQ2_INTERFACEDEFINITIONS_HPP_
#define NFIQ2_INTERFACEDEFINITIONS_HPP_

#include <nfiq2_data.hpp>

#include <string>
#include <vector>

namespace NFIQ2 {
/** Identifiers for actionable feedback values. */
namespace ActionableQualityFeedbackIDs {
/**
 * The image is blank or the contrast is too low.
 *
 * @details
 * The mean gray level appears white.
 */
extern const std::string EmptyImageOrContrastTooLow;
/** Standard deviation of gray levels in image indicates uniformity. */
extern const std::string UniformImage;
/**
 * Number of minutia in image.
 *
 * @note
 * Equivalent to QualityFeatures::Minutiae::Count.
 */
extern const std::string FingerprintImageWithMinutiae;
/** Number of pixels in the computed foreground. */
extern const std::string SufficientFingerprintForeground;
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
 * Bins for the histogram are defined in the NFIQ 2 Report.
 */
namespace Histogram {
/** First of ten histogram bin boundaries. */
extern const char Bin0[];
/** Second of ten histogram bin boundaries. */
extern const char Bin1[];
/** Third of ten histogram bin boundaries. */
extern const char Bin2[];
/** Fourth of ten histogram bin boundaries. */
extern const char Bin3[];
/** Fifth of ten histogram bin boundaries. */
extern const char Bin4[];
/** Sixth of ten histogram bin boundaries. */
extern const char Bin5[];
/** Seventh of ten histogram bin boundaries. */
extern const char Bin6[];
/** Eighth of ten histogram bin boundaries. */
extern const char Bin7[];
/** Ninth of ten histogram bin boundaries. */
extern const char Bin8[];
/** Tenth of ten histogram bin boundaries. */
extern const char Bin9[];
}

/** Mean of local quality value. */
extern const char Mean[];
/** Standard deviation of local quality values. */
extern const char StdDev[];
}
/**
 * Measures relative to minutiae detected within the image using the integrated
 * feature extractor, FingerJet FX OSE.
 */
namespace Minutiae {
/** Number of minutiae in the image. */
extern const char Count[];

/**
 * Number of minutiae lying in a 200x200 pixel rectangle centered at the center
 * of mass of the locations of all detected minutia.
 */
extern const char CountCOM[];

/**
 * Percentage of minutiae whose quality value, as determined by Grayscale::Mean
 * of a 32x32 pixel region centered on the minutiae, is between 0-0.5.
 *
 * @see Grayscale::Mean
 */
extern const char QualityMu2[];

/**
 * Percentage of minutiae whose quality value, as determined by the Orientation
 * Certainty Level of a 32x32 pixel region centered on the minutiae, is above
 * 80.
 *
 * @see OrientationCertainty
 */
extern const char QualityOCL80[];
}

/** Measures based on the foreground area of the image. */
namespace RegionOfInterest {
/**
 * Mean grayscale value of the number of 32x32 pixel regions having at least 1
 * pixel in the ROI.
 */
extern const char Mean[];

/** Sum of coherence values over all 16x16 pixel regions within the ROI. */
extern const char CoherenceSum[];

/**
 * Average of coherence values over all 16x16 pixel regions within the ROI.
 *
 * @details
 * This is computed as RegionOfInterest::CoherenceSum / (number of 16x16 pixel
 * regions that contain at least 1 pixel within the ROI).
 *
 * @see CoherenceSum
 */
extern const char CoherenceMean[];
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
extern const char Bin0[];
/** Second of ten histogram bin boundaries. */
extern const char Bin1[];
/** Third of ten histogram bin boundaries. */
extern const char Bin2[];
/** Fourth of ten histogram bin boundaries. */
extern const char Bin3[];
/** Fifth of ten histogram bin boundaries. */
extern const char Bin4[];
/** Sixth of ten histogram bin boundaries. */
extern const char Bin5[];
/** Seventh of ten histogram bin boundaries. */
extern const char Bin6[];
/** Eighth of ten histogram bin boundaries. */
extern const char Bin7[];
/** Ninth of ten histogram bin boundaries. */
extern const char Bin8[];
/** Tenth of ten histogram bin boundaries. */
extern const char Bin9[];
}

/** Mean of local quality value. */
extern const char Mean[];
/** Standard deviation of local quality values. */
extern const char StdDev[];
}

/** Measure of the gray levels of the image. */
namespace Grayscale {
/** Arithmetic mean of the image. */
extern const char Mean[];
/** Arithmetic mean of per-block means of the image. */
extern const char MeanBlock[];
}

/**
 * Measure of the strength of the energy concentration along the dominant
 * ridge flow orientation.
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
extern const char Bin0[];
/** Second of ten histogram bin boundaries. */
extern const char Bin1[];
/** Third of ten histogram bin boundaries. */
extern const char Bin2[];
/** Fourth of ten histogram bin boundaries. */
extern const char Bin3[];
/** Fifth of ten histogram bin boundaries. */
extern const char Bin4[];
/** Sixth of ten histogram bin boundaries. */
extern const char Bin5[];
/** Seventh of ten histogram bin boundaries. */
extern const char Bin6[];
/** Eighth of ten histogram bin boundaries. */
extern const char Bin7[];
/** Ninth of ten histogram bin boundaries. */
extern const char Bin8[];
/** Tenth of ten histogram bin boundaries. */
extern const char Bin9[];
}

/** Mean of local quality value. */
extern const char Mean[];
/** Standard deviation of local quality values. */
extern const char StdDev[];
}

/**
 * Measure of the ridge flow continuity, based on the absolute orientation
 * difference between a block in its 8-neighborhood.
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
extern const char Bin0[];
/** Second of ten histogram bin boundaries. */
extern const char Bin1[];
/** Third of ten histogram bin boundaries. */
extern const char Bin2[];
/** Fourth of ten histogram bin boundaries. */
extern const char Bin3[];
/** Fifth of ten histogram bin boundaries. */
extern const char Bin4[];
/** Sixth of ten histogram bin boundaries. */
extern const char Bin5[];
/** Seventh of ten histogram bin boundaries. */
extern const char Bin6[];
/** Eighth of ten histogram bin boundaries. */
extern const char Bin7[];
/** Ninth of ten histogram bin boundaries. */
extern const char Bin8[];
/** Tenth of ten histogram bin boundaries. */
extern const char Bin9[];
}

/** Mean of local quality value. */
extern const char Mean[];
/** Standard deviation of local quality values. */
extern const char StdDev[];
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
extern const char Bin0[];
/** Second of ten histogram bin boundaries. */
extern const char Bin1[];
/** Third of ten histogram bin boundaries. */
extern const char Bin2[];
/** Fourth of ten histogram bin boundaries. */
extern const char Bin3[];
/** Fifth of ten histogram bin boundaries. */
extern const char Bin4[];
/** Sixth of ten histogram bin boundaries. */
extern const char Bin5[];
/** Seventh of ten histogram bin boundaries. */
extern const char Bin6[];
/** Eighth of ten histogram bin boundaries. */
extern const char Bin7[];
/** Ninth of ten histogram bin boundaries. */
extern const char Bin8[];
/** Tenth of ten histogram bin boundaries. */
extern const char Bin9[];
}

/** Mean of local quality value. */
extern const char Mean[];
/** Standard deviation of local quality values. */
extern const char StdDev[];
}
}

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
