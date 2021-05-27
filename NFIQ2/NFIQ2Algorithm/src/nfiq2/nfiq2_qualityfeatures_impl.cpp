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

const std::string
    NFIQ2::ActionableQualityFeedbackIDs::EmptyImageOrContrastTooLow {
	    "EmptyImageOrContrastTooLow"
    };
const std::string NFIQ2::ActionableQualityFeedbackIDs::UniformImage {
	"UniformImage"
};
const std::string
    NFIQ2::ActionableQualityFeedbackIDs::FingerprintImageWithMinutiae {
	    "FingerprintImageWithMinutiae"
    };
const std::string
    NFIQ2::ActionableQualityFeedbackIDs::SufficientFingerprintForeground {
	    "SufficientFingerprintForeground"
    };

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
		if (feature->getModuleName().compare("NFIQ2_Mu") == 0) {
			// Uniform and Contrast
			const std::shared_ptr<MuFeature> muFeatureModule =
			    std::dynamic_pointer_cast<MuFeature>(feature);

			std::unordered_map<std::string, double> muFeatures =
			    muFeatureModule->getFeatures();

			std::unordered_map<std::string, double>::iterator
			    it_muFeatures;
			// check for uniform image by using the Sigma value
			bool isUniformImage = false;
			actionableMap
			    [ActionableQualityFeedbackIDs::UniformImage] =
				muFeatureModule->getSigma();
			isUniformImage =
			    (actionableMap[ActionableQualityFeedbackIDs::
				     UniformImage] <
					ActionableQualityFeedbackThreshold::
					    UniformImage ?
					  true :
					  false);

			// Mu is computed always since it is used as feature
			// anyway
			bool isEmptyImage = false;
			for (it_muFeatures = muFeatures.begin();
			     it_muFeatures != muFeatures.end();
			     ++it_muFeatures) {
				if (it_muFeatures->first.compare("Mu") == 0) {
					actionableMap
					    [ActionableQualityFeedbackIDs::
						    EmptyImageOrContrastTooLow] =
						it_muFeatures->second;
					isEmptyImage =
					    (actionableMap[ActionableQualityFeedbackIDs::
						     EmptyImageOrContrastTooLow] >
							ActionableQualityFeedbackThreshold::
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

		} else if (feature->getModuleName().compare(
			       "NFIQ2_FingerJetFX") == 0) {
			// Minutiae
			const std::shared_ptr<FingerJetFXFeature>
			    fjfxFeatureModule =
				std::dynamic_pointer_cast<FingerJetFXFeature>(
				    feature);

			std::unordered_map<std::string, double> fjfxFeatures =
			    fjfxFeatureModule->getFeatures();

			std::unordered_map<std::string, double>::iterator
			    it_fjfxFeatures;

			for (it_fjfxFeatures = fjfxFeatures.begin();
			     it_fjfxFeatures != fjfxFeatures.end();
			     ++it_fjfxFeatures) {
				if (it_fjfxFeatures->first.compare(
					"FingerJetFX_MinutiaeCount") == 0) {
					// return informative feature about
					// number of minutiae
					actionableMap
					    [ActionableQualityFeedbackIDs::
						    FingerprintImageWithMinutiae] =
						it_fjfxFeatures->second;
				}
			}

		} else if (feature->getModuleName().compare(
			       "NFIQ2_ImgProcROI") == 0) {
			// FP Foreground
			const std::shared_ptr<ImgProcROIFeature>
			    roiFeatureModule =
				std::dynamic_pointer_cast<ImgProcROIFeature>(
				    feature);

			// add ROI information to actionable quality feedback
			// absolute number of ROI pixels (foreground)
			actionableMap[ActionableQualityFeedbackIDs::
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
		NFIQ2::ActionableQualityFeedbackIDs::UniformImage,
		NFIQ2::ActionableQualityFeedbackIDs::EmptyImageOrContrastTooLow,
		NFIQ2::ActionableQualityFeedbackIDs::
		    FingerprintImageWithMinutiae,
		ActionableQualityFeedbackIDs::SufficientFingerprintForeground
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
		FDAFeature::speedFeatureIDGroup,
		FingerJetFXFeature::speedFeatureIDGroup,
		FJFXMinutiaeQualityFeature::speedFeatureIDGroup,
		ImgProcROIFeature::speedFeatureIDGroup,
		LCSFeature::speedFeatureIDGroup, MuFeature::speedFeatureIDGroup,
		OCLHistogramFeature::speedFeatureIDGroup,
		OFFeature::speedFeatureIDGroup,
		QualityMapFeatures::speedFeatureIDGroup,
		RVUPHistogramFeature::speedFeatureIDGroup
	};

	return speedFeatureGroups;
}
