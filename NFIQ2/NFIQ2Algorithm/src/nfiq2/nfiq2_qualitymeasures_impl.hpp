#ifndef NFIQ2_QUALITYMEASURES_IMPL_HPP_
#define NFIQ2_QUALITYMEASURES_IMPL_HPP_

#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_qualitymeasures.hpp>
#include <quality_modules/Module.h>

#include <list>
#include <memory>
#include <string>
#include <vector>

namespace NFIQ2 { namespace QualityMeasures { namespace Impl {

std::vector<std::string> getActionableQualityFeedbackIDs();

std::vector<std::string> getNativeQualityMeasureIDs();

std::vector<std::string> getNativeQualityMeasureAlgorithmIDs();

/**
 * @brief
 * Updates the floating point precision mode used on 32-bit Linux
 * versions of NFIQ 2.
 *
 * @details
 * On 32-bit linux machines the floating point calculations are inconsistent
 * with other 32 bit or 64 bit operating systems. Executing a specific
 * assembly instruction fixes the issue and allows for score computation
 * to behave as expected. To learn more about this specific floating point
 * issue, please see the following website:
 * https://www.linuxtopia.org/online_books/an_introduction_to_gcc/gccintro_70.html
 *
 * @param mode
 * Floating point precision mode
 */
void setFPU(unsigned int mode);

std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
computeNativeQualityMeasureAlgorithms(
    const NFIQ2::FingerprintImageData &rawImage);

std::unordered_map<std::string, double> getActionableQualityFeedback(
    const std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	&algorithms);

std::unordered_map<std::string, double> computeActionableQualityFeedback(
    const NFIQ2::FingerprintImageData &rawImage);

std::unordered_map<std::string, double> getNativeQualityMeasures(
    const std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	&algorithms);

std::unordered_map<std::string, double> computeNativeQualityMeasures(
    const NFIQ2::FingerprintImageData &rawImage);

std::unordered_map<std::string, double> getNativeQualityMeasureAlgorithmSpeeds(
    const std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	&algorithms);

std::unordered_map<std::string,
    std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
getNativeQualityMeasureAlgorithms(
    const std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	&algorithms);
}}}

#endif /* NFIQ2_QUALITYMEASURES_IMPL_HPP_ */
