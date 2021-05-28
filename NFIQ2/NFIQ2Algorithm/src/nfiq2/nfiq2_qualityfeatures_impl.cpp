#include <features/BaseFeature.h>
#include <features/FDAFeature.h>
#include <features/FJFXMinutiaeQualityFeatures.h>
#include <features/FingerJetFXFeature.h>
#include <features/ImgProcROIFeature.h>
#include <features/LCSFeature.h>
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

const char NFIQ2::ActionableQualityFeedback::IDs::EmptyImageOrContrastTooLow[] {
	"EmptyImageOrContrastTooLow"
};
const char NFIQ2::ActionableQualityFeedback::IDs::UniformImage[] {
	"UniformImage"
};
const char
    NFIQ2::ActionableQualityFeedback::IDs::FingerprintImageWithMinutiae[] {
	    "FingerprintImageWithMinutiae"
    };
const char
    NFIQ2::ActionableQualityFeedback::IDs::SufficientFingerprintForeground[] {
	    "SufficientFingerprintForeground"
    };
const double
    NFIQ2::ActionableQualityFeedback::Thresholds::EmptyImageOrContrastTooLow {
	    250.0
    };
const double NFIQ2::ActionableQualityFeedback::Thresholds::UniformImage { 1.0 };
const double
    NFIQ2::ActionableQualityFeedback::Thresholds::FingerprintImageWithMinutiae {
	    5.0
    };
const double NFIQ2::ActionableQualityFeedback::Thresholds::
    SufficientFingerprintForeground { 50000.0 };

std::unordered_map<std::string, NFIQ2::QualityFeatureSpeed>
NFIQ2::QualityFeatures::Impl::getQualityFeatureSpeeds(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
	&features)
{
	std::vector<std::string> speedIdentifiers =
	    NFIQ2::QualityFeatures::getAllSpeedFeatureGroups();

	std::unordered_map<std::string, NFIQ2::QualityFeatureSpeed> speedMap {};

	for (std::vector<std::string>::size_type i = 0;
	     i < speedIdentifiers.size(); i++) {
		speedMap[speedIdentifiers.at(i)] = features.at(i)->getSpeed();
	}

	return speedMap;
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::Impl::getQualityFeatureData(
    const NFIQ2::FingerprintImageData &rawImage)
{
	return NFIQ2::QualityFeatures::getQualityFeatureData(
	    NFIQ2::QualityFeatures::computeQualityFeatures(rawImage));
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::Impl::getQualityFeatureData(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
	&features)
{
	std::vector<std::string> qualityIdentifiers =
	    NFIQ2::QualityFeatures::Impl::getAllQualityFeatureIDs();

	std::unordered_map<std::string, double> quality {};

	for (const auto &feature : features) {
		const auto moduleFeatures = feature->getFeatures();
		quality.insert(moduleFeatures.cbegin(), moduleFeatures.cend());
	}

	return quality;
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::Impl::getActionableQualityFeedback(
    const NFIQ2::FingerprintImageData &rawImage)
{
	return NFIQ2::QualityFeatures::getActionableQualityFeedback(
	    NFIQ2::QualityFeatures::computeQualityFeatures(rawImage));
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::Impl::getActionableQualityFeedback(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
	&features)
{
	std::unordered_map<std::string, double> actionableMap {};

	for (const auto &feature : features) {
		if (feature->getModuleName() ==
		    QualityFeatures::Modules::Grayscale) {
			// Uniform and Contrast
			const std::shared_ptr<MuFeature> muFeatureModule =
			    std::dynamic_pointer_cast<MuFeature>(feature);

			// check for uniform image by using the Sigma value
			bool isUniformImage = false;
			actionableMap
			    [ActionableQualityFeedback::IDs::UniformImage] =
				muFeatureModule->getSigma();
			isUniformImage =
			    (actionableMap[ActionableQualityFeedback::IDs::
				     UniformImage] <
					ActionableQualityFeedback::Thresholds::
					    UniformImage ?
					  true :
					  false);

			// Mu is computed always since it is used as feature
			// anyway
			bool isEmptyImage = false;
			for (const auto &muFeature :
			    muFeatureModule->getFeatures()) {
				if (muFeature.first ==
				    QualityFeatureIDs::Grayscale::Mean) {
					actionableMap
					    [ActionableQualityFeedback::IDs::
						    EmptyImageOrContrastTooLow] =
						muFeature.second;
					isEmptyImage =
					    (actionableMap[ActionableQualityFeedback::
						     IDs::
							 EmptyImageOrContrastTooLow] >
							ActionableQualityFeedback::
							    Thresholds::
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
		    QualityFeatures::Modules::MinutiaeCount) {
			// Minutiae
			const std::shared_ptr<FingerJetFXFeature>
			    fjfxFeatureModule =
				std::dynamic_pointer_cast<FingerJetFXFeature>(
				    feature);

			for (const auto &fjfxFeature :
			    fjfxFeatureModule->getFeatures()) {
				if (fjfxFeature.first ==
				    QualityFeatureIDs::Minutiae::Count) {
					// return informative feature about
					// number of minutiae
					actionableMap
					    [ActionableQualityFeedback::IDs::
						    FingerprintImageWithMinutiae] =
						fjfxFeature.second;
				}
			}

		} else if (feature->getModuleName().compare(QualityFeatures::
				   Modules::RegionOfInterestMean) == 0) {
			// FP Foreground
			const std::shared_ptr<ImgProcROIFeature>
			    roiFeatureModule =
				std::dynamic_pointer_cast<ImgProcROIFeature>(
				    feature);

			// add ROI information to actionable quality feedback
			// absolute number of ROI pixels (foreground)
			actionableMap[ActionableQualityFeedback::IDs::
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

std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
NFIQ2::QualityFeatures::Impl::computeQualityFeatures(
    const NFIQ2::FingerprintImageData &rawImage)
{
	/* use double-precision rounding for 32-bit linux */
	setFPU(0x27F);

	const NFIQ2::FingerprintImageData croppedImage =
	    rawImage.removeWhiteFrameAroundFingerprint();

	std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
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

std::vector<std::string>
NFIQ2::QualityFeatures::Impl::getAllActionableIdentifiers()
{
	static const std::vector<std::string> actionableIdentifiers {
		NFIQ2::ActionableQualityFeedback::IDs::UniformImage,
		NFIQ2::ActionableQualityFeedback::IDs::
		    EmptyImageOrContrastTooLow,
		NFIQ2::ActionableQualityFeedback::IDs::
		    FingerprintImageWithMinutiae,
		ActionableQualityFeedback::IDs::SufficientFingerprintForeground
	};

	return actionableIdentifiers;
}

std::vector<std::string>
NFIQ2::QualityFeatures::Impl::getAllQualityFeatureIDs()
{
	const std::vector<std::vector<std::string>> vov {
		FDAFeature::getAllFeatureIDs(),
		FingerJetFXFeature::getAllFeatureIDs(),
		FJFXMinutiaeQualityFeature::getAllFeatureIDs(),
		ImgProcROIFeature::getAllFeatureIDs(),
		LCSFeature::getAllFeatureIDs(), MuFeature::getAllFeatureIDs(),
		OCLHistogramFeature::getAllFeatureIDs(),
		OFFeature::getAllFeatureIDs(),
		QualityMapFeatures::getAllFeatureIDs(),
		RVUPHistogramFeature::getAllFeatureIDs()
	};

	std::vector<std::string> qualityFeatureIDs {};

	for (auto &vec : vov) {
		qualityFeatureIDs.insert(
		    qualityFeatureIDs.end(), vec.cbegin(), vec.cend());
	}

	return qualityFeatureIDs;
}

std::vector<std::string>
NFIQ2::QualityFeatures::Impl::getAllSpeedFeatureGroups()
{
	static const std::vector<std::string> speedFeatureGroups {
		FDAFeature::SpeedFeatureIDGroup,
		FingerJetFXFeature::SpeedFeatureIDGroup,
		FJFXMinutiaeQualityFeature::SpeedFeatureIDGroup,
		ImgProcROIFeature::SpeedFeatureIDGroup,
		LCSFeature::SpeedFeatureIDGroup, MuFeature::SpeedFeatureIDGroup,
		OCLHistogramFeature::SpeedFeatureIDGroup,
		OFFeature::SpeedFeatureIDGroup,
		QualityMapFeatures::SpeedFeatureIDGroup,
		RVUPHistogramFeature::SpeedFeatureIDGroup
	};

	return speedFeatureGroups;
}
