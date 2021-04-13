#ifndef NFIQ2_NFIQ2_QUALITYFEATURES_IMPL_H_
#define NFIQ2_NFIQ2_QUALITYFEATURES_IMPL_H_

#include <features/BaseFeature.h>
#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_qualityfeatures.hpp>

#include <list>
#include <memory>
#include <string>
#include <vector>

namespace NFIQ { namespace QualityFeatures { namespace Impl {
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
 * Fingerprint image in raw format
 * @param bComputeActionableQuality
 * If to compute actionable quality flags or not
 * @param actionableQuality
 * Compute actionable quality values
 * @param bOutputSpeed
 * If to output speed of computed features
 * @param speedValues
 * List of feature computation speed
 *
 * @return
 * A list of fingerprint quality feature data. This can be used with model
 * paramenters to produce a final NFIQ 2 score.
 */
std::vector<NFIQ::QualityFeatureData> computeQualityFeatures(
    const NFIQ::FingerprintImageData &rawImage, bool bComputeActionableQuality,
    std::vector<NFIQ::ActionableQualityFeedback> &actionableQuality,
    bool bOutputSpeed, std::vector<NFIQ::QualityFeatureSpeed> &speedValues);

/**
 * @brief
 * Obtain computed quality feature data from a fingerprint image.
 *
 * @param rawImage
 * Fingerprint image in raw format
 *
 * @return
 * A vector if BaseFeature modules containing computed feature data
 */
std::vector<std::shared_ptr<NFIQ::QualityFeatures::BaseFeature>>
getComputedQualityFeatures(const NFIQ::FingerprintImageData &rawImage);

/**
 * @brief
 * Obtain actionable quality feedback from a vector of features
 *
 * @param features
 * A vector of BaseFeatures obtained from a raw fingerprint image
 *
 * @return
 * A vector of actionable quality feedback
 */
std::vector<NFIQ::ActionableQualityFeedback> getActionableQualityFeedback(
    const std::vector<std::shared_ptr<NFIQ::QualityFeatures::BaseFeature>>
	&features);

/**
 * @brief
 * Obtain actionable quality feedback from a fingerprint image
 *
 * @param rawImage
 * Fingerprint image in raw format
 *
 * @return
 * A vector of actionable quality feedback
 */
std::vector<NFIQ::ActionableQualityFeedback> getActionableQualityFeedback(
    const NFIQ::FingerprintImageData &rawImage);

std::vector<NFIQ::QualityFeatureData> getQualityFeatures(
    const std::vector<std::shared_ptr<NFIQ::QualityFeatures::BaseFeature>>
	&features);

std::vector<NFIQ::QualityFeatureData> getQualityFeatures(
    const NFIQ::FingerprintImageData &rawImage);

std::vector<NFIQ::QualityFeatureSpeed> getQualityFeatureSpeeds(
    const std::vector<std::shared_ptr<NFIQ::QualityFeatures::BaseFeature>>
	&features);

std::vector<NFIQ::QualityFeatureSpeed> getQualityFeatureSpeeds(
    const NFIQ::FingerprintImageData &rawImage);

}}}

#endif
