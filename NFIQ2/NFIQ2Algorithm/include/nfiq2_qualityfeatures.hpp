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

namespace NFIQ2 { namespace QualityFeatures {

/* Forward declaration. */
class Module;

/**
 * @brief
 * Obtain all actionable quality feedback identifiers.
 *
 * @return
 * Vector of strings containing all actionable quality feedback
 * identifiers.
 */
std::vector<std::string> getActionableQualityFeedbackIDs();

/**
 * @brief
 * Obtain all quality module identifiers.
 *
 * @return
 * Vector of strings with all identifiers from Identifiers::QualityModules.
 */
std::vector<std::string> getQualityModuleIDs();

/**
 * @brief
 * Obtain all quality feature IDs from quality modules.
 *
 * @return
 * Vector of strings containing all quality feature IDs.
 */
std::vector<std::string> getQualityFeatureIDs();

/**
 * @brief
 * Obtain computed quality feature data from a fingerprint image.
 *
 * @param rawImage
 * Fingerprint image in raw format.
 *
 * @return
 * A vector of quality modules containing computed feature data.
 */
std::vector<std::shared_ptr<NFIQ2::QualityFeatures::Module>>
computeQualityModules(const NFIQ2::FingerprintImageData &rawImage);

/**
 * @brief
 * Obtain actionable quality feedback from a vector of features.
 *
 * @param modules
 * A vector of Modules obtained from a raw fingerprint image.
 *
 * @return
 * A map of string, actionable quality feedback pairs.
 *
 * @see computeQualityModules
 */
std::unordered_map<std::string, double> getActionableQualityFeedback(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::Module>>
	&modules);

/**
 * @brief
 * Compute actionable quality feedback from a fingerprint image.
 *
 * @param rawImage
 * Fingerprint image in raw format.
 *
 * @return
 * A map of string, actionable quality feedback pairs.
 */
std::unordered_map<std::string, double> computeActionableQualityFeedback(
    const NFIQ2::FingerprintImageData &rawImage);

/**
 * @brief
 * Obtain quality feature data from a vector of features.
 *
 * @param features
 * A vector of Modules obtained from a raw fingerprint image.
 *
 * @return
 * A map of string, quality feature data pairs.
 */
std::unordered_map<std::string, double> getQualityFeatureValues(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::Module>>
	&features);

/**
 * @brief
 * Compute and obtain quality feature values from a fingerprint image.
 *
 * @param rawImage
 * Fingerprint image in raw format.
 *
 * @return
 * A map of string, quality feature data pairs.
 */
std::unordered_map<std::string, double> computeQualityFeatures(
    const NFIQ2::FingerprintImageData &rawImage);

/**
 * @brief
 * Obtain quality modules organized as a map.
 *
 * @param modules
 * A vector of Modules obtained from a raw fingerprint image.
 *
 * @return
 * `features` in a map with `feature`'s identifier as the map key.
 */
std::unordered_map<std::string, std::shared_ptr<NFIQ2::QualityFeatures::Module>>
getQualityModules(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::Module>>
	&modules);

/**
 * @brief
 * Obtain quality feature speeds from a vector of features.
 *
 * @param modules
 * A vector of Modules obtained from a raw fingerprint image.
 *
 * @return
 * A map of Identifier::QualityModule, speed pairs.
 *
 * @see computeQualityModules
 */
std::unordered_map<std::string, double> getQualityModuleSpeeds(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::Module>>
	&modules);

}}

#endif /* NFIQ2_QUALITYFEATURES_HPP_ */
