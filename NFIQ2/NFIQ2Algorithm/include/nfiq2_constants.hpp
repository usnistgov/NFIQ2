/*
 * This file is part of NIST Fingerprint Image Quality (NFIQ) 2. For more
 * information on this project, refer to:
 *   - https://nist.gov/services-resources/software/nfiq2
 *   - https://github.com/usnistgov/NFIQ2
 *
 * This work is in the public domain. For complete licensing details, refer to:
 *   - https://github.com/usnistgov/NFIQ2/blob/master/LICENSE.md
 */

#ifndef NFIQ2_CONSTANTS_HPP_
#define NFIQ2_CONSTANTS_HPP_

#include <nfiq2_data.hpp>

#include <string>
#include <vector>

namespace NFIQ2 {
/** Identification strings for various objects. */
namespace Identifiers {

/** CBEFF quality algorithm identifiers. */
namespace CBEFF {
/** Quality algorithm vendor */
extern const unsigned int Vendor;

/** Unified quality score (NFIQ 2.0). */
extern const unsigned int NFIQ2Rev0;
/** Unified quality score (NFIQ 2.1). */
extern const unsigned int NFIQ2Rev1;
/** Unified quality score (NFIQ 2.2). */
extern const unsigned int NFIQ2Rev2;
/** Unified quality score (NFIQ 2.3). */
extern const unsigned int NFIQ2Rev3;
/** Unified quality score (ISO/IEC 29794-4). */
extern const unsigned int UnifiedQualityScore;

/** Orientation certainty quality algorithm identifiers. */
namespace OrientationCertainty {
/** Mean of local orientation certainty level. */
extern const unsigned int Mean;
/** Standard deviation of local orientation certainty level. */
extern const unsigned int StdDev;
}

/** Local clarity quality algorithm identifiers. */
namespace LocalClarity {
/** Mean of local clarity score. */
extern const unsigned int Mean;
/** Standard deviation of local clarity score. */
extern const unsigned int StdDev;
}

/** Frequency domain analysis quality algorithm identifiers. */
namespace FrequencyDomainAnalysis {
/** Mean of local frequency domain analysis. */
extern const unsigned int Mean;
/** Standard deviation of local frequency domain analysis. */
extern const unsigned int StdDev;
}

/** Ridge valley uniformity quality algorithm identifiers. */
namespace RidgeValleyUniformity {
/** Mean of local ridge valley uniformity. */
extern const unsigned int Mean;
/** Standard deviation of local ridge valley uniformity. */
extern const unsigned int StdDev;
}

/** Orientation flow quality algorithm identifiers. */
namespace OrientationFlow {
/** Mean of local orientation flow. */
extern const unsigned int Mean;
/** Standard deviation of orientation flow. */
extern const unsigned int StdDev;
}

/** Image contrast quality algorithm identifiers. */
namespace Contrast {
/** Average contrast ("MU"). */
extern const unsigned int Mean;
/** Average of average constrasts ("MMB"). */
extern const unsigned int MeanBlock;
}

/** Minutie-based quality algorithm identifiers. */
namespace Minutiae {
/** Minutiae count. */
extern const unsigned int Count;
/** Minutiae count in center of mass. */
extern const unsigned int CountCOM;
/** Minutiae quality based on image mean. */
extern const unsigned int QualityMu2;
/** Minutiae quality based on orientation certainty level. */
extern const unsigned int QualityOCL80;
}

/** Region of interest quality algorithm identifiers. */
namespace RegionOfInterest {
/** Region of interest image mean. */
extern const unsigned int Mean;
/** Region of interest orientation map coherence sum. */
extern const unsigned int CoherenceSum;
/** Region of interest relative orientation map coherence sum. */
extern const unsigned int CoherenceMean;
}

/** Radial power spectrum. */
extern const unsigned int RadialPowerSpectrum;
/** Gabor quality score. */
extern const unsigned int Gabor;
}

/**
 * Identifiers for interpretation of quality features that may indicated
 * corrective measures for subsequent captures of the same subject.
 */
namespace ActionableQualityFeedback {
/**
 * The image is blank or the contrast is too low.
 *
 * @details
 * The mean gray level appears white.
 */
extern const char EmptyImageOrContrastTooLow[];
/** Standard deviation of gray levels in image indicates uniformity. */
extern const char UniformImage[];
/**
 * Number of minutia in image.
 *
 * @note
 * Equivalent to QualityFeatures::Features::Minutiae::Count.
 */
extern const char FingerprintImageWithMinutiae[];
/** Number of pixels in the computed foreground. */
extern const char SufficientFingerprintForeground[];
} /* ActionableQualityFeedback */

/** Modules that combine features to compute a quality score. */
namespace PredictionModules {
/** Identifier for the RandomForest prediction module. */
extern const char RandomForest[];
} /* Identifiers::PredictionModules */

/** Modules that compute one or more QualityFeatures. */
namespace QualityModules {
/** Identifier for the FrequencyDomainAnalysis feature module. */
extern const char FrequencyDomainAnalysis[];
/** Identifier for the MinutiaeCount feature module. */
extern const char MinutiaeCount[];
/** Identifier for the MinutiaeQuality feature module. */
extern const char MinutiaeQuality[];
/** Identifier for the LocalClarity feature module. */
extern const char LocalClarity[];
/** Identifier for the Contrast feature module. */
extern const char Contrast[];
/** Identifier for the OrientationCertainty feature module. */
extern const char OrientationCertainty[];
/** Identifier for the OrientationFlow feature module. */
extern const char OrientationFlow[];
/** Identifier for the RegionOfInterestMean feature module. */
extern const char RegionOfInterestMean[];
/** Identifier for the RegionOfInterestCoherence feature module. */
extern const char RegionOfInterestCoherence[];
/** Identifier for the RidgeValleyUniformity feature module. */
extern const char RidgeValleyUniformity[];
} /* Identifiers::QualityModules */

/** Identifiers of individual quality features and modules. */
namespace QualityFeatures {
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
 * Percentage of minutiae whose quality value, as determined by Contrast::Mean
 * of a local region centered on the minutiae, is between 0-0.5.
 *
 * @see Contrast::Mean
 */
extern const char QualityMu2[];

/**
 * Percentage of minutiae whose quality value, as determined by the Orientation
 * Certainty Level of a local region centered on the minutiae, is above
 * 80.
 *
 * @see OrientationCertainty
 */
extern const char QualityOCL80[];
}

/** Measures based on the foreground area of the image. */
namespace RegionOfInterest {
/**
 * Mean grayscale value of the number of local regions having at least 1 pixel
 * in the ROI.
 */
extern const char Mean[];

/** Sum of coherence values over all local regions within the ROI. */
extern const char CoherenceSum[];

/**
 * Average of coherence values over all local regions within the ROI.
 *
 * @details
 * This is computed as RegionOfInterest::CoherenceSum / (number of local regions
 * that contain at least 1 pixel within the ROI).
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
namespace Contrast {
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
} /* Identifiers::QualityFeatures::RidgeValleyUniformity */
} /* Identifiers::QualityFeatures */
} /* Identifiers */

/** Threshold constants. */
namespace Thresholds {
/**
 * Thresholds corresponding to Identifiers::ActionableQualityFeedback.
 *
 * @details
 * Comparing the values of Identifiers::ActionableQualityFeedback with
 * Thresholds::ActionableQualityFeedback may indicate a corrective action for
 * subsequent captures of the same subject.
 */
namespace ActionableQualityFeedback {
/** Threshold value for IDs::EmptyImageOrContrastTooLow. */
extern const double EmptyImageOrContrastTooLow;
/** Threshold value for IDs::UniformImage. */
extern const double UniformImage;
/** Threshold value for IDs::FingerprintImageWithMinutiae. */
extern const double FingerprintImageWithMinutiae;
/** Threshold value for IDs::SufficientFingerprintForeground. */
extern const double SufficientFingerprintForeground;
} /* Thresholds::ActionableQualityFeedback */
} /* Thresholds */

/** Block size constants. */
namespace Sizes {
/**
 * Number of pixels (in width and height) comprising a local region.
 *
 * @details
 * As defined by ISO/IEC 29794-4:202X, Section 5.1.2, "The size for each local
 * region shall be 32 x 32 pixels, which is sufficient to cover 2 clear ridges."
 */
const unsigned int LocalRegionSquare { 32 };
/** Width after rotating local region to vertically align ridges. */
const unsigned int VerticallyAlignedLocalRegionWidth { 32 };
/** Height after rotating local region to vertically align ridges. */
const unsigned int VerticallyAlignedLocalRegionHeight { 16 };
}

} /* NFIQ2 */

#endif /* NFIQ2_CONSTANTS_HPP_ */
