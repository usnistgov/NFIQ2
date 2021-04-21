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
#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_nfiqexception.hpp>
#include <nfiq2_qualityfeatures.hpp>

#include "nfiq2_qualityfeatures_impl.h"

#include <iomanip>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

std::unordered_map<std::string, NFIQ::QualityFeatureSpeed>
NFIQ::QualityFeatures::Impl::getQualityFeatureSpeeds(
    const std::vector<std::shared_ptr<NFIQ::QualityFeatures::BaseFeature>>
	&features)
{
	std::vector<std::string> speedIdentifiers =
	    NFIQ::QualityFeatures::getAllSpeedFeatureGroups();

	std::unordered_map<std::string, NFIQ::QualityFeatureSpeed> speedMap {};

	for (auto i = 0; i < speedIdentifiers.size(); i++) {
		speedMap[speedIdentifiers.at(i)] = features.at(i)->getSpeed();
	}

	return speedMap;
}

std::unordered_map<std::string, NFIQ::QualityFeatureData>
NFIQ::QualityFeatures::Impl::getQualityFeatureData(
    const NFIQ::FingerprintImageData &rawImage)
{
	return NFIQ::QualityFeatures::getQualityFeatureData(
	    NFIQ::QualityFeatures::computeQualityFeatures(rawImage));
}

std::unordered_map<std::string, NFIQ::QualityFeatureData>
NFIQ::QualityFeatures::Impl::getQualityFeatureData(
    const std::vector<std::shared_ptr<NFIQ::QualityFeatures::BaseFeature>>
	&features)
{
	std::vector<std::string> qualityIdentifiers =
	    NFIQ::QualityFeatures::Impl::getAllQualityFeatureIDs();

	std::unordered_map<std::string, NFIQ::QualityFeatureData> quality {};

	auto counter = 0;
	for (const auto &feature : features) {
		for (auto &result : feature->getFeatures()) {
			if (result.returnCode == 0) {
				quality[qualityIdentifiers.at(counter)] =
				    result.featureData;
			} else {
				result.featureData.featureDataDouble = 0;
				quality[qualityIdentifiers.at(counter)] =
				    result.featureData;
			}
			counter++;
		}
	}

	return quality;
}

std::unordered_map<std::string, NFIQ::ActionableQualityFeedback>
NFIQ::QualityFeatures::Impl::getActionableQualityFeedback(
    const NFIQ::FingerprintImageData &rawImage)
{
	return NFIQ::QualityFeatures::getActionableQualityFeedback(
	    NFIQ::QualityFeatures::computeQualityFeatures(rawImage));
}

std::unordered_map<std::string, NFIQ::ActionableQualityFeedback>
NFIQ::QualityFeatures::Impl::getActionableQualityFeedback(
    const std::vector<std::shared_ptr<NFIQ::QualityFeatures::BaseFeature>>
	&features)
{
	std::unordered_map<std::string, NFIQ::ActionableQualityFeedback>
	    actionableMap {};

	for (const auto feature : features) {
		if (feature->getModuleName().compare("NFIQ2_Mu") == 0) {
			// Uniform and Contrast
			const std::shared_ptr<MuFeature> muFeatureModule =
			    std::dynamic_pointer_cast<MuFeature>(feature);

			std::vector<NFIQ::QualityFeatureResult> muFeatures =
			    muFeatureModule->getFeatures();

			std::vector<NFIQ::QualityFeatureResult>::iterator
			    it_muFeatures;
			// check for uniform image by using the Sigma value
			bool isUniformImage = false;
			NFIQ::ActionableQualityFeedback fbUniform;
			fbUniform.actionableQualityValue =
			    muFeatureModule->getSigma();
			fbUniform.identifier = NFIQ::
			    ActionableQualityFeedbackIdentifier::UniformImage;
			isUniformImage = (fbUniform.actionableQualityValue <
				    ActionableQualityFeedbackThreshold::
					UniformImage ?
				      true :
				      false);
			actionableMap[ActionableQualityFeedbackIdentifier::
				UniformImage] = fbUniform;

			// Mu is computed always since it is used as feature
			// anyway
			bool isEmptyImage = false;
			for (it_muFeatures = muFeatures.begin();
			     it_muFeatures != muFeatures.end();
			     ++it_muFeatures) {
				if (it_muFeatures->featureData.featureID
					.compare("Mu") == 0) {
					NFIQ::ActionableQualityFeedback fb;
					fb.actionableQualityValue =
					    it_muFeatures->featureData
						.featureDataDouble;
					fb.identifier = NFIQ::
					    ActionableQualityFeedbackIdentifier::
						EmptyImageOrContrastTooLow;
					isEmptyImage = (fb.actionableQualityValue >
						    ActionableQualityFeedbackThreshold::
							EmptyImageOrContrastTooLow ?
						      true :
						      false);
					actionableMap
					    [ActionableQualityFeedbackIdentifier::
						    EmptyImageOrContrastTooLow] =
						fb;
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

			std::vector<NFIQ::QualityFeatureResult> fjfxFeatures =
			    fjfxFeatureModule->getFeatures();

			std::vector<NFIQ::QualityFeatureResult>::iterator
			    it_fjfxFeatures;

			for (it_fjfxFeatures = fjfxFeatures.begin();
			     it_fjfxFeatures != fjfxFeatures.end();
			     ++it_fjfxFeatures) {
				if (it_fjfxFeatures->featureData.featureID
					.compare("FingerJetFX_MinutiaeCount") ==
				    0) {
					// return informative feature about
					// number of minutiae
					NFIQ::ActionableQualityFeedback fb;
					fb.actionableQualityValue =
					    it_fjfxFeatures->featureData
						.featureDataDouble;
					fb.identifier = NFIQ::
					    ActionableQualityFeedbackIdentifier::
						FingerprintImageWithMinutiae;
					actionableMap
					    [ActionableQualityFeedbackIdentifier::
						    FingerprintImageWithMinutiae] =
						fb;
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
			NFIQ::ActionableQualityFeedback fb_roi;
			fb_roi.actionableQualityValue =
			    roiFeatureModule->getImgProcResults()
				.noOfROIPixels; // absolute number of ROI pixels
						// (foreground)
			fb_roi.identifier =
			    NFIQ::ActionableQualityFeedbackIdentifier::
				SufficientFingerprintForeground;
			actionableMap[ActionableQualityFeedbackIdentifier::
				SufficientFingerprintForeground] = fb_roi;
		}
	}

	return actionableMap;
}

std::vector<std::shared_ptr<NFIQ::QualityFeatures::BaseFeature>>
NFIQ::QualityFeatures::Impl::computeQualityFeatures(
    const NFIQ::FingerprintImageData &rawImage)
{
	const NFIQ::FingerprintImageData croppedImage =
	    rawImage.removeWhiteFrameAroundFingerprint();

	std::vector<std::shared_ptr<NFIQ::QualityFeatures::BaseFeature>>
	    features {};

	features.push_back(std::make_shared<FDAFeature>(croppedImage));

	std::shared_ptr<FingerJetFXFeature> fjfxFeatureModule =
	    std::make_shared<FingerJetFXFeature>(croppedImage);
	features.push_back(fjfxFeatureModule);

	features.push_back(std::make_shared<FJFXMinutiaeQualityFeature>(
	    croppedImage, fjfxFeatureModule->getMinutiaData(),
	    fjfxFeatureModule->getTemplateStatus()));

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
NFIQ::QualityFeatures::Impl::getAllActionableIdentifiers()
{
	static const std::vector<std::string> actionableIdentifiers {
		NFIQ::ActionableQualityFeedbackIdentifier::UniformImage,
		NFIQ::ActionableQualityFeedbackIdentifier::
		    EmptyImageOrContrastTooLow,
		NFIQ::ActionableQualityFeedbackIdentifier::
		    FingerprintImageWithMinutiae,
		ActionableQualityFeedbackIdentifier::
		    SufficientFingerprintForeground
	};

	return actionableIdentifiers;
}

std::vector<std::string>
NFIQ::QualityFeatures::Impl::getAllQualityFeatureIDs()
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
		    qualityFeatureIDs.cend(), vec.cbegin(), vec.cend());
	}

	return qualityFeatureIDs;
}

std::vector<std::string>
NFIQ::QualityFeatures::Impl::getAllSpeedFeatureGroups()
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
