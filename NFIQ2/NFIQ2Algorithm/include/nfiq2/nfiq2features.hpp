#ifndef NFIQ2_NFIQ2FEATURES_H_
#define NFIQ2_NFIQ2FEATURES_H_

#include <nfiq2/fingerprintimagedata.hpp>
#include <nfiq2/interfacedefinitions.hpp>

#include <list>
#include <memory>
#include <string>

namespace NFIQ { namespace QualityFeatures {
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
std::list<NFIQ::QualityFeatureData> computeQualityFeatures(
    const NFIQ::FingerprintImageData &rawImage, bool bComputeActionableQuality,
    std::list<NFIQ::ActionableQualityFeedback> &actionableQuality,
    bool bOutputSpeed, std::list<NFIQ::QualityFeatureSpeed> &speedValues);

}}

#endif
