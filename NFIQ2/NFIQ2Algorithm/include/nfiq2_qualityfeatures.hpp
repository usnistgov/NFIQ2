#ifndef NFIQ2_QUALITYFEATURES_HPP_
#define NFIQ2_QUALITYFEATURES_HPP_

#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_interfacedefinitions.hpp>

#include <memory>
#include <string>
#include <unordered_map>

namespace NFIQ2 { namespace QualityFeatures {

/* Forward declaration. */
class BaseFeature;

/**
 * @brief
 * Obtain all actionable quality feedback identifiers.
 *
 * @return
 * Vector of strings containing all actionable quality feedback
 * identifiers.
 */
std::vector<std::string> getAllActionableIdentifiers();

/**
 * @brief
 * Obtain all quality feature IDs from quality modules.
 *
 * @return
 * Vector of strings containing all quality feature IDs.
 */
std::vector<std::string> getAllQualityFeatureIDs();

/**
 * @brief
 * Obtain all speed feature groups from quality modules.
 *
 * @return
 * Vector of strings containing all speed feature groups.
 */
std::vector<std::string> getAllSpeedFeatureGroups();

/**
 * @brief
 * Obtain computed quality feature data from a fingerprint image.
 *
 * @param rawImage
 * Fingerprint image in raw format.
 *
 * @return
 * A vector if BaseFeature modules containing computed feature data.
 */
std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
computeQualityFeatures(const NFIQ2::FingerprintImageData &rawImage);

/**
 * @brief
 * Obtain actionable quality feedback from a vector of features.
 *
 * @param features
 * A vector of BaseFeatures obtained from a raw fingerprint image.
 *
 * @return
 * A map of string, actionable quality feedback pairs.
 */
std::unordered_map<std::string, NFIQ2::ActionableQualityFeedback>
getActionableQualityFeedback(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
	&features);

/**
 * @brief
 * Obtain actionable quality feedback from a fingerprint image.
 *
 * @param rawImage
 * Fingerprint image in raw format.
 *
 * @return
 * A map of string, actionable quality feedback pairs.
 */
std::unordered_map<std::string, NFIQ2::ActionableQualityFeedback>
getActionableQualityFeedback(const NFIQ2::FingerprintImageData &rawImage);

/**
 * @brief
 * Obtain quality feature data from a vector of features.
 *
 * @param features
 * A vector of BaseFeatures obtained from a raw fingerprint image.
 *
 * @return
 * A map of string, quality feature data pairs.
 */
std::unordered_map<std::string, double> getQualityFeatureData(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
	&features);

/**
 * @brief
 * Obtain quality feature data from a fingerprint image.
 *
 * @param rawImage
 * Fingerprint image in raw format.
 *
 * @return
 * A map of string, quality feature data pairs.
 */
std::unordered_map<std::string, double> getQualityFeatureData(
    const NFIQ2::FingerprintImageData &rawImage);

/**
 * @brief
 * Obtain quality feature speeds from a vector of features.
 *
 * @param features
 * A vector of BaseFeatures obtained from a raw fingerprint image.
 *
 * @return
 * A map of string, quality feature speed pairs.
 */
std::unordered_map<std::string, NFIQ2::QualityFeatureSpeed>
getQualityFeatureSpeeds(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
	&features);

}}

#endif /* NFIQ2_QUALITYFEATURES_HPP_ */
