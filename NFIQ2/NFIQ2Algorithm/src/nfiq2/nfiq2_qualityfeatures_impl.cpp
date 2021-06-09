#include <features/FDAFeature.h>
#include <features/FJFXMinutiaeQualityFeatures.h>
#include <features/FingerJetFXFeature.h>
#include <features/ImgProcROIFeature.h>
#include <features/LCSFeature.h>
#include <features/Module.h>
#include <features/MuFeature.h>
#include <features/OCLHistogramFeature.h>
#include <features/OFFeature.h>
#include <features/QualityMapFeatures.h>
#include <features/RVUPHistogramFeature.h>
#include <nfiq2_exception.hpp>
#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_qualityfeatures.hpp>

#include "nfiq2_qualityfeatures_impl.hpp"
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
NFIQ2::QualityFeatures::Impl::getQualityModuleSpeeds(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::Module>>
	&features)
{
	std::vector<std::string> speedIdentifiers =
	    NFIQ2::QualityFeatures::getQualityModuleIDs();

	std::unordered_map<std::string, double> speedMap {};

	for (std::vector<std::string>::size_type i = 0;
	     i < speedIdentifiers.size(); i++) {
		speedMap[speedIdentifiers.at(i)] = features.at(i)->getSpeed();
	}

	return speedMap;
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::Impl::computeQualityFeatures(
    const NFIQ2::FingerprintImageData &rawImage)
{
	return NFIQ2::QualityFeatures::getQualityFeatureValues(
	    NFIQ2::QualityFeatures::computeQualityModules(rawImage));
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::Impl::getQualityFeatureValues(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::Module>>
	&features)
{
	std::vector<std::string> qualityIdentifiers =
	    NFIQ2::QualityFeatures::Impl::getQualityFeatureIDs();

	std::unordered_map<std::string, double> quality {};

	for (const auto &feature : features) {
		const auto moduleFeatures = feature->getFeatures();
		quality.insert(moduleFeatures.cbegin(), moduleFeatures.cend());
	}

	return quality;
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::Impl::computeActionableQualityFeedback(
    const NFIQ2::FingerprintImageData &rawImage)
{
	return NFIQ2::QualityFeatures::getActionableQualityFeedback(
	    NFIQ2::QualityFeatures::computeQualityModules(rawImage));
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::Impl::getActionableQualityFeedback(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::Module>>
	&features)
{
	std::unordered_map<std::string, double> actionableMap {};

	/* Pre-populate the map */
	for (const auto &id : getActionableQualityFeedbackIDs()) {
		actionableMap[id] =
		    std::numeric_limits<double>::signaling_NaN();
	}

	for (const auto &feature : features) {
		if (feature->getModuleName() ==
		    Identifiers::QualityModules::Contrast) {
			// Uniform and Contrast
			const std::shared_ptr<MuFeature> muFeatureModule =
			    std::dynamic_pointer_cast<MuFeature>(feature);

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
				    Identifiers::QualityFeatures::Contrast::
					Mean) {
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

		} else if (feature->getModuleName() ==
		    Identifiers::QualityModules::MinutiaeCount) {
			// Minutiae
			const std::shared_ptr<FingerJetFXFeature>
			    fjfxFeatureModule =
				std::dynamic_pointer_cast<FingerJetFXFeature>(
				    feature);

			for (const auto &fjfxFeature :
			    fjfxFeatureModule->getFeatures()) {
				if (fjfxFeature.first ==
				    Identifiers::QualityFeatures::Minutiae::
					Count) {
					// return informative feature about
					// number of minutiae
					actionableMap[Identifiers::
						ActionableQualityFeedback::
						    FingerprintImageWithMinutiae] =
					    fjfxFeature.second;
				}
			}

		} else if (feature->getModuleName().compare(Identifiers::
				   QualityModules::RegionOfInterestMean) == 0) {
			// FP Foreground
			const std::shared_ptr<ImgProcROIFeature>
			    roiFeatureModule =
				std::dynamic_pointer_cast<ImgProcROIFeature>(
				    feature);

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
NFIQ2::QualityFeatures::Impl::setFPU(unsigned int mode)
{
	asm("fldcw %0" : : "m"(*&mode));
}
#else
void
NFIQ2::QualityFeatures::Impl::setFPU(unsigned int)
{
}
#endif

std::vector<std::shared_ptr<NFIQ2::QualityFeatures::Module>>
NFIQ2::QualityFeatures::Impl::computeQualityModules(
    const NFIQ2::FingerprintImageData &rawImage)
{
	/* use double-precision rounding for 32-bit linux */
	setFPU(0x27F);

	const NFIQ2::FingerprintImageData croppedImage =
	    rawImage.copyRemovingNearWhiteFrame();

	std::vector<std::shared_ptr<NFIQ2::QualityFeatures::Module>>
	    features {};

	features.push_back(std::make_shared<FDAFeature>(croppedImage));

	std::shared_ptr<FingerJetFXFeature> fjfxFeatureModule =
	    std::make_shared<FingerJetFXFeature>(croppedImage);
	features.push_back(fjfxFeatureModule);

	features.push_back(std::make_shared<FJFXMinutiaeQualityFeature>(
	    croppedImage, fjfxFeatureModule->getMinutiaData()));

	std::shared_ptr<ImgProcROIFeature> roiFeatureModule =
	    std::make_shared<ImgProcROIFeature>(croppedImage);
	features.push_back(roiFeatureModule);

	features.push_back(std::make_shared<LCSFeature>(croppedImage));

	features.push_back(std::make_shared<MuFeature>(croppedImage));

	features.push_back(std::make_shared<OCLHistogramFeature>(croppedImage));

	features.push_back(std::make_shared<OFFeature>(croppedImage));

	features.push_back(std::make_shared<QualityMapFeatures>(
	    croppedImage, roiFeatureModule->getImgProcResults()));

	features.push_back(
	    std::make_shared<RVUPHistogramFeature>(croppedImage));

	return features;
}

std::unordered_map<std::string, std::shared_ptr<NFIQ2::QualityFeatures::Module>>
NFIQ2::QualityFeatures::Impl::getQualityModules(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::Module>>
	&features)
{
	std::unordered_map<std::string,
	    std::shared_ptr<NFIQ2::QualityFeatures::Module>>
	    ret {};
	for (const auto &feature : features)
		ret[feature->getModuleName()] = feature;

	return ret;
}

std::vector<std::string>
NFIQ2::QualityFeatures::Impl::getActionableQualityFeedbackIDs()
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
NFIQ2::QualityFeatures::Impl::getQualityFeatureIDs()
{
	const std::vector<std::vector<std::string>> vov {
		FDAFeature::getQualityFeatureIDs(),
		FingerJetFXFeature::getQualityFeatureIDs(),
		FJFXMinutiaeQualityFeature::getQualityFeatureIDs(),
		ImgProcROIFeature::getQualityFeatureIDs(),
		LCSFeature::getQualityFeatureIDs(),
		MuFeature::getQualityFeatureIDs(),
		OCLHistogramFeature::getQualityFeatureIDs(),
		OFFeature::getQualityFeatureIDs(),
		QualityMapFeatures::getQualityFeatureIDs(),
		RVUPHistogramFeature::getQualityFeatureIDs()
	};

	std::vector<std::string> qualityFeatureIDs {};

	for (auto &vec : vov) {
		qualityFeatureIDs.insert(
		    qualityFeatureIDs.end(), vec.cbegin(), vec.cend());
	}

	return qualityFeatureIDs;
}

std::vector<std::string>
NFIQ2::QualityFeatures::Impl::getQualityModuleIDs()
{
	static const std::vector<std::string> ids {
		Identifiers::QualityModules::FrequencyDomainAnalysis,
		Identifiers::QualityModules::MinutiaeCount,
		Identifiers::QualityModules::MinutiaeQuality,
		Identifiers::QualityModules::RegionOfInterestMean,
		Identifiers::QualityModules::LocalClarity,
		Identifiers::QualityModules::Contrast,
		Identifiers::QualityModules::OrientationCertainty,
		Identifiers::QualityModules::OrientationFlow,
		Identifiers::QualityModules::RegionOfInterestCoherence,
		Identifiers::QualityModules::RidgeValleyUniformity
	};

	return ids;
}
