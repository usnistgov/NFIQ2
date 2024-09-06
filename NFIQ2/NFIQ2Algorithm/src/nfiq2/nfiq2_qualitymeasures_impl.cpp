#include <nfiq2_exception.hpp>
#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_qualitymeasures.hpp>
#include <quality_modules/FDA.h>
#include <quality_modules/FJFXMinutiaeQuality.h>
#include <quality_modules/FingerJetFX.h>
#include <quality_modules/ImgProcROI.h>
#include <quality_modules/LCS.h>
#include <quality_modules/Module.h>
#include <quality_modules/Mu.h>
#include <quality_modules/OCLHistogram.h>
#include <quality_modules/OF.h>
#include <quality_modules/QualityMap.h>
#include <quality_modules/RVUPHistogram.h>

#include "nfiq2_qualitymeasures_impl.hpp"
#include <iomanip>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

const char NFIQ2::Identifiers::ActionableQualityFeedback::
    EmptyImageOrContrastTooLow[] { "EmptyImageOrContrastTooLow" };
const char NFIQ2::Identifiers::ActionableQualityFeedback::UniformImage[] {
	"UniformImage"
};
const char NFIQ2::Identifiers::ActionableQualityFeedback::
    FingerprintImageWithMinutiae[] { "FingerprintImageWithMinutiae" };
const char NFIQ2::Identifiers::ActionableQualityFeedback::
    SufficientFingerprintForeground[] { "SufficientFingerprintForeground" };
const double
    NFIQ2::Thresholds::ActionableQualityFeedback::EmptyImageOrContrastTooLow {
	    250.0
    };
const double NFIQ2::Thresholds::ActionableQualityFeedback::UniformImage { 1.0 };
const double
    NFIQ2::Thresholds::ActionableQualityFeedback::FingerprintImageWithMinutiae {
	    5.0
    };
const double NFIQ2::Thresholds::ActionableQualityFeedback::
    SufficientFingerprintForeground { 50000.0 };

std::unordered_map<std::string, double>
NFIQ2::QualityMeasures::Impl::getNativeQualityMeasureAlgorithmSpeeds(
    const std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	&features)
{
	std::vector<std::string> speedIdentifiers =
	    NFIQ2::QualityMeasures::getNativeQualityMeasureAlgorithmIDs();

	std::unordered_map<std::string, double> speedMap {};

	for (std::vector<std::string>::size_type i = 0;
	     i < speedIdentifiers.size(); i++) {
		speedMap[speedIdentifiers.at(i)] = features.at(i)->getSpeed();
	}

	return speedMap;
}

std::unordered_map<std::string, double>
NFIQ2::QualityMeasures::Impl::computeNativeQualityMeasures(
    const NFIQ2::FingerprintImageData &rawImage)
{
	return NFIQ2::QualityMeasures::getNativeQualityMeasures(
	    NFIQ2::QualityMeasures::computeNativeQualityMeasureAlgorithms(
		rawImage));
}

std::unordered_map<std::string, double>
NFIQ2::QualityMeasures::Impl::getNativeQualityMeasures(
    const std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	&features)
{
	std::unordered_map<std::string, double> quality {};

	for (const auto &feature : features) {
		const auto moduleFeatures = feature->getFeatures();
		quality.insert(moduleFeatures.cbegin(), moduleFeatures.cend());
	}

	return quality;
}

std::unordered_map<std::string, double>
NFIQ2::QualityMeasures::Impl::computeActionableQualityFeedback(
    const NFIQ2::FingerprintImageData &rawImage)
{
	return NFIQ2::QualityMeasures::getActionableQualityFeedback(
	    NFIQ2::QualityMeasures::computeNativeQualityMeasureAlgorithms(
		rawImage));
}

std::unordered_map<std::string, double>
NFIQ2::QualityMeasures::Impl::getActionableQualityFeedback(
    const std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	&features)
{
	std::unordered_map<std::string, double> actionableMap {};

	/* Pre-populate the map */
	for (const auto &id : getActionableQualityFeedbackIDs()) {
		actionableMap[id] =
		    std::numeric_limits<double>::signaling_NaN();
	}

	for (const auto &feature : features) {
		if (feature->getName() ==
		    Identifiers::QualityMeasureAlgorithms::Contrast) {
			// Uniform and Contrast
			const std::shared_ptr<Mu> muFeatureModule =
			    std::dynamic_pointer_cast<Mu>(feature);

			// check for uniform image by using the Sigma value
			bool isUniformImage = false;
			actionableMap[Identifiers::ActionableQualityFeedback::
				UniformImage] = muFeatureModule->getSigma();
			isUniformImage =
			    (actionableMap[Identifiers::
				     ActionableQualityFeedback::UniformImage] <
					Thresholds::ActionableQualityFeedback::
					    UniformImage ?
				    true :
				    false);

			// Mu is computed always since it is used as feature
			// anyway
			bool isEmptyImage = false;
			for (const auto &muFeature :
			    muFeatureModule->getFeatures()) {
				if (muFeature.first ==
				    Identifiers::QualityMeasures::Contrast::
					ImageMean) {
					actionableMap[Identifiers::
						ActionableQualityFeedback::
						    EmptyImageOrContrastTooLow] =
					    muFeature.second;
					isEmptyImage =
					    (actionableMap[Identifiers::
						     ActionableQualityFeedback::
							 EmptyImageOrContrastTooLow] >
							Thresholds::
							    ActionableQualityFeedback::
								EmptyImageOrContrastTooLow ?
						    true :
						    false);
				}
			}

			if (isEmptyImage || isUniformImage) {
				// empty image or uniform image has been
				// detected return empty feature vector feature
				// values will not be computed in that case
				return actionableMap;
			}

		} else if (feature->getName() ==
		    Identifiers::QualityMeasureAlgorithms::MinutiaeCount) {
			// Minutiae
			const std::shared_ptr<FingerJetFX> fjfxFeatureModule =
			    std::dynamic_pointer_cast<FingerJetFX>(feature);

			for (const auto &fjfxFeature :
			    fjfxFeatureModule->getFeatures()) {
				if (fjfxFeature.first ==
				    Identifiers::QualityMeasures::Minutiae::
					Count) {
					// return informative feature about
					// number of minutiae
					actionableMap[Identifiers::
						ActionableQualityFeedback::
						    FingerprintImageWithMinutiae] =
					    fjfxFeature.second;
				}
			}

		} else if (feature->getName().compare(
			       Identifiers::QualityMeasureAlgorithms::
				   RegionOfInterestMean) == 0) {
			// FP Foreground
			const std::shared_ptr<ImgProcROI> roiFeatureModule =
			    std::dynamic_pointer_cast<ImgProcROI>(feature);

			// add ROI information to actionable quality feedback
			// absolute number of ROI pixels (foreground)
			actionableMap[Identifiers::ActionableQualityFeedback::
				SufficientFingerprintForeground] =
			    roiFeatureModule->getImgProcResults().noOfROIPixels;
		}
	}

	return actionableMap;
}

// Defined in 32-bit linux operating systems with floating point
// mode as a paramenter, otherwise this parameter is unused.
#if defined(__linux) && defined(__i386__)
void
NFIQ2::QualityMeasures::Impl::setFPU(unsigned int mode)
{
	asm("fldcw %0" : : "m"(*&mode));
}
#else
void
NFIQ2::QualityMeasures::Impl::setFPU(unsigned int)
{
}
#endif

std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
NFIQ2::QualityMeasures::Impl::computeNativeQualityMeasureAlgorithms(
    const NFIQ2::FingerprintImageData &rawImage)
{
	/* use double-precision rounding for 32-bit linux */
	setFPU(0x27F);

	const NFIQ2::FingerprintImageData croppedImage =
	    rawImage.copyRemovingNearWhiteFrame();

	std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	    features {};

	features.push_back(std::make_shared<FDA>(croppedImage));

	std::shared_ptr<FingerJetFX> fjfxFeatureModule =
	    std::make_shared<FingerJetFX>(croppedImage);
	features.push_back(fjfxFeatureModule);

	features.push_back(std::make_shared<FJFXMinutiaeQuality>(croppedImage,
	    fjfxFeatureModule->getMinutiaData()));

	std::shared_ptr<ImgProcROI> roiFeatureModule =
	    std::make_shared<ImgProcROI>(croppedImage);
	features.push_back(roiFeatureModule);

	features.push_back(std::make_shared<LCS>(croppedImage));

	features.push_back(std::make_shared<Mu>(croppedImage));

	features.push_back(std::make_shared<OCLHistogram>(croppedImage));

	features.push_back(std::make_shared<OF>(croppedImage));

	features.push_back(std::make_shared<QualityMap>(croppedImage,
	    roiFeatureModule->getImgProcResults()));

	features.push_back(std::make_shared<RVUPHistogram>(croppedImage));

	return features;
}

std::unordered_map<std::string,
    std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
NFIQ2::QualityMeasures::Impl::getNativeQualityMeasureAlgorithms(
    const std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	&features)
{
	std::unordered_map<std::string,
	    std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	    ret {};
	for (const auto &feature : features)
		ret[feature->getName()] = feature;

	return ret;
}

std::vector<std::string>
NFIQ2::QualityMeasures::Impl::getActionableQualityFeedbackIDs()
{
	static const std::vector<std::string> actionableIdentifiers {
		NFIQ2::Identifiers::ActionableQualityFeedback::UniformImage,
		NFIQ2::Identifiers::ActionableQualityFeedback::
		    EmptyImageOrContrastTooLow,
		NFIQ2::Identifiers::ActionableQualityFeedback::
		    FingerprintImageWithMinutiae,
		Identifiers::ActionableQualityFeedback::
		    SufficientFingerprintForeground
	};

	return actionableIdentifiers;
}

std::vector<std::string>
NFIQ2::QualityMeasures::Impl::getNativeQualityMeasureIDs()
{
	const std::vector<std::vector<std::string>> vov {
		FDA::getNativeQualityMeasureIDs(),
		FingerJetFX::getNativeQualityMeasureIDs(),
		FJFXMinutiaeQuality::getNativeQualityMeasureIDs(),
		ImgProcROI::getNativeQualityMeasureIDs(),
		LCS::getNativeQualityMeasureIDs(),
		Mu::getNativeQualityMeasureIDs(),
		OCLHistogram::getNativeQualityMeasureIDs(),
		OF::getNativeQualityMeasureIDs(),
		QualityMap::getNativeQualityMeasureIDs(),
		RVUPHistogram::getNativeQualityMeasureIDs()
	};

	std::vector<std::string> qualityFeatureIDs {};

	for (auto &vec : vov) {
		qualityFeatureIDs.insert(qualityFeatureIDs.end(), vec.cbegin(),
		    vec.cend());
	}

	return qualityFeatureIDs;
}

std::vector<std::string>
NFIQ2::QualityMeasures::Impl::getNativeQualityMeasureAlgorithmIDs()
{
	static const std::vector<std::string> ids {
		Identifiers::QualityMeasureAlgorithms::FrequencyDomainAnalysis,
		Identifiers::QualityMeasureAlgorithms::MinutiaeCount,
		Identifiers::QualityMeasureAlgorithms::MinutiaeQuality,
		Identifiers::QualityMeasureAlgorithms::RegionOfInterestMean,
		Identifiers::QualityMeasureAlgorithms::LocalClarity,
		Identifiers::QualityMeasureAlgorithms::Contrast,
		Identifiers::QualityMeasureAlgorithms::OrientationCertainty,
		Identifiers::QualityMeasureAlgorithms::OrientationFlow,
		Identifiers::QualityMeasureAlgorithms::
		    RegionOfInterestCoherence,
		Identifiers::QualityMeasureAlgorithms::RidgeValleyUniformity
	};

	return ids;
}
