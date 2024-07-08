/*
 * This file is part of NIST Fingerprint Image Quality (NFIQ) 2. For more
 * information on this project, refer to:
 *   - https://nist.gov/services-resources/software/nfiq2
 *   - https://github.com/usnistgov/NFIQ2
 *
 * This work is in the public domain. For complete licensing details, refer to:
 *   - https://github.com/usnistgov/NFIQ2/blob/master/LICENSE.md
 */

#ifndef NFIQ2_QUALITYFEATURES_HPP_
#define NFIQ2_QUALITYFEATURES_HPP_

#include <nfiq2_constants.hpp>
#include <nfiq2_fingerprintimagedata.hpp>

#include <memory>
#include <string>
#include <unordered_map>

namespace NFIQ2 { namespace QualityMeasures {

/**
 * @brief
 * Controls the computation of one or more quality measures.
 *
 * @note
 * Forward declaration into public interface.
 */
class Algorithm;

/******************************************************************************/

/*
 * Obtain identifier constants.
 */

/**
 * @brief
 * Obtain all actionable quality feedback identifiers.
 *
 * @return
 * Vector of strings containing all actionable quality feedback identifiers.
 *
 * @see Identifiers::ActionableQualityFeedback
 */
std::vector<std::string> getActionableQualityFeedbackIDs();

/**
 * @brief
 * Obtain all native quality measure algorithm identifiers.
 *
 * @return
 * Vector of strings with all quality module identifiers.
 *
 * @see Identifiers::QualityModules
 */
std::vector<std::string> getNativeQualityMeasureAlgorithmIDs();

/**
 * @brief
 * Obtain all quality measure identifiers from quality measure algorithms.
 *
 * @return
 * Vector of strings containing all quality feature identifiers.
 *
 * @see Identifiers::QualityMeasures
 */
std::vector<std::string> getQualityFeatureIDs();

/******************************************************************************/

/**
 * @addtogroup compute
 * Compute unified quality scoress, native quality measures, and
 * actionable quality feedback.
 * @{
 */

/**
 * @brief
 * Compute native quality measures.
 *
 * @param rawImage
 * Fingerprint image in raw format.
 *
 * @return
 * A vector of evaluated native quality measure algorithms.
 */
std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
computeNativeQualityMeasureAlgorithms(
    const NFIQ2::FingerprintImageData &rawImage);

/**
 * @brief
 * Compute quality feature values.
 *
 * @param rawImage
 * Fingerprint image in raw format.
 *
 * @return
 * A map of quality measure algorithm identifiers to native quality measures.
 *
 * @see Identifiers::QualityMeasures
 */
std::unordered_map<std::string, double> computeQualityMeasures(
    const NFIQ2::FingerprintImageData &rawImage);

/**
 * @brief
 * Compute actionable quality feedback.
 *
 * @param rawImage
 * Fingerprint image in raw format.
 *
 * @return
 * A map of actionable quality identifiers to actionable quality values.
 *
 * @see Identifiers::ActionableQualityFeedback
 * @see Thresholds::ActionableQualityFeedback
 */
std::unordered_map<std::string, double> computeActionableQualityFeedback(
    const NFIQ2::FingerprintImageData &rawImage);

/**@}**************************************************************************/

/*
 * Extract values in other formats.
 */

/**
 * @brief
 * Obtain actionable quality feedback from computed quality modules.
 *
 * @param algorithms
 * Computed native quality measure algorithms.
 *
 * @return
 * A map of actionable quality identifiers to actionable quality values.
 *
 * @see Identifiers::ActionableQualityFeedback
 * @see Thresholds::ActionableQualityFeedback
 */
std::unordered_map<std::string, double> getActionableQualityFeedback(
    const std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	&algorithms);

/**
 * @brief
 * Obtain native quality measures from computed native quality measure
 * algorithms.
 *
 * @param algorithms
 * Computed native quality measure algorithms.
 *
 * @return
 * A map of quality feature identifiers to quality feature values.
 *
 * @see Identifiers::QualityMeasures
 */
std::unordered_map<std::string, double> getNativeQualityMeasures(
    const std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	&algorithms);

/**
 * @brief
 * Obtain native quality measure algorithms organized as a map.
 *
 * @param algorithms
 * Computed native quality measure algorithms.
 *
 * @return
 * Algorithms from `algorithms` in a map with the algorithms's identifier as the
 * key.
 *
 * @see Identifiers::QualityModules
 */
std::unordered_map<std::string,
    std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
getNativeQualityMeasureAlgorithms(
    const std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	&algorithms);

/**
 * @brief
 * Obtain time elapsed during native quality measure algorithm's computation, in
 * milliseconds.
 *
 * @param algorithms
 * Computed native quality measure algorithms.
 *
 * @return
 * A map of quality measure algorithm identifiers to the elapsed time in
 * milliseconds for the quality measure's computation.
 *
 * @see Identifiers::QualityModules
 */
std::unordered_map<std::string, double> getNativeQualityMeasureAlgorithmSpeeds(
    const std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	&algorithms);
}}

#endif /* NFIQ2_QUALITYFEATURES_HPP_ */
