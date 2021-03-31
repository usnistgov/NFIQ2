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
#include <string>
#include <vector>

std::vector<NFIQ::QualityFeatureData>
NFIQ::QualityFeatures::Impl::computeQualityFeatures(
    const NFIQ::FingerprintImageData &rawImage, bool bComputeActionableQuality,
    std::vector<NFIQ::ActionableQualityFeedback> &actionableQuality,
    bool bOutputSpeed, std::vector<NFIQ::QualityFeatureSpeed> &speedValues)
{
	std::vector<NFIQ::QualityFeatureData> featureVector;

	// compute Mu at first since it is used to detect empty images
	// but the feature value for prediction will added later

	// compute contrast features
	// MMB
	// Mu
	MuFeature muFeatureModule(bOutputSpeed, speedValues);
	std::vector<NFIQ::QualityFeatureResult> muFeatures =
	    muFeatureModule.computeFeatureData(rawImage);

	// find Mu feature to get its value and return actionable feedback for
	// empty images
	std::vector<NFIQ::QualityFeatureResult>::iterator it_muFeatures;
	if (bComputeActionableQuality) {
		// check for uniform image by using the Sigma value
		bool isUniformImage = false;
		NFIQ::ActionableQualityFeedback fbUniform;
		fbUniform.actionableQualityValue = muFeatureModule.getSigma();
		fbUniform.identifier =
		    NFIQ::ActionableQualityFeedbackIdentifier_UniformImage;
		isUniformImage = (fbUniform.actionableQualityValue <
			    ActionableQualityFeedbackThreshold_UniformImage ?
			      true :
			      false);
		actionableQuality.push_back(fbUniform);

		// only return actionable feedback if so configured
		// Mu is computed always since it is used as feature anyway
		bool isEmptyImage = false;
		for (it_muFeatures = muFeatures.begin();
		     it_muFeatures != muFeatures.end(); ++it_muFeatures) {
			if (it_muFeatures->featureData.featureID.compare(
				"Mu") == 0) {
				NFIQ::ActionableQualityFeedback fb;
				fb.actionableQualityValue =
				    it_muFeatures->featureData
					.featureDataDouble;
				fb.identifier = NFIQ::
				    ActionableQualityFeedbackIdentifier_EmptyImageOrContrastTooLow;
				isEmptyImage = (fb.actionableQualityValue >
					    ActionableQualityFeedbackThreshold_EmptyImageOrContrastTooLow ?
					      true :
					      false);
				actionableQuality.push_back(fb);
			}
		}

		if (isEmptyImage || isUniformImage) {
			// empty image or uniform image has been detected
			// return empty feature vector
			// feature values will not be computed in that case
			return featureVector;
		}
	}

	// compute FDA features
	// FDA_Bin10_[0-9]
	// FDA_Bin10_Mean
	// FDA_Bin10_StdDev
	FDAFeature fdaFeatureModule(bOutputSpeed, speedValues);
	std::vector<NFIQ::QualityFeatureResult> fdaFeatures =
	    fdaFeatureModule.computeFeatureData(rawImage);

	// append to feature vector
	std::vector<NFIQ::QualityFeatureResult>::iterator it_fdaFeatures;
	for (it_fdaFeatures = fdaFeatures.begin();
	     it_fdaFeatures != fdaFeatures.end(); ++it_fdaFeatures) {
		if (it_fdaFeatures->returnCode == 0) {
			featureVector.push_back(it_fdaFeatures->featureData);
		} else {
			// feature extraction error is mapped to a score of 0
			it_fdaFeatures->featureData.featureDataDouble = 0;
			featureVector.push_back(it_fdaFeatures->featureData);
		}
	}

	// compute FJFX features
	// FingerJetFX_MinCount_COMMinRect200x200
	// FingerJetFX_MinutiaeCount
	FingerJetFXFeature fjfxFeatureModule(bOutputSpeed, speedValues);
	// this module returns the FJFX minutiae template to be used in other
	// modules

	std::vector<FingerJetFXFeature::Minutia> minutiaData {};
	bool templateCouldBeExtracted = false;
	std::vector<NFIQ::QualityFeatureResult> fjfxFeatures =
	    fjfxFeatureModule.computeFeatureData(
		rawImage, minutiaData, templateCouldBeExtracted);

	// append to feature vector
	std::vector<NFIQ::QualityFeatureResult>::iterator it_fjfxFeatures;
	for (it_fjfxFeatures = fjfxFeatures.begin();
	     it_fjfxFeatures != fjfxFeatures.end(); ++it_fjfxFeatures) {
		if (it_fjfxFeatures->returnCode == 0) {
			featureVector.push_back(it_fjfxFeatures->featureData);
		} else {
			// feature extraction error is mapped to a score of 0
			it_fjfxFeatures->featureData.featureDataDouble = 0;
			featureVector.push_back(it_fjfxFeatures->featureData);
		}
	}

	if (bComputeActionableQuality) {
		for (it_fjfxFeatures = fjfxFeatures.begin();
		     it_fjfxFeatures != fjfxFeatures.end(); ++it_fjfxFeatures) {
			if (it_fjfxFeatures->featureData.featureID.compare(
				"FingerJetFX_MinutiaeCount") == 0) {
				// return informative feature about number of
				// minutiae
				NFIQ::ActionableQualityFeedback fb;
				fb.actionableQualityValue =
				    it_fjfxFeatures->featureData
					.featureDataDouble;
				fb.identifier = NFIQ::
				    ActionableQualityFeedbackIdentifier_FingerprintImageWithMinutiae;
				actionableQuality.push_back(fb);
			}
		}
	}

	// compute FJFX minutiae quality features
	// FJFXPos_Mu_MinutiaeQuality_2
	// FJFXPos_OCL_MinutiaeQuality_80
	FJFXMinutiaeQualityFeature fjfxMinQualFeatureModule(
	    bOutputSpeed, speedValues);
	// this module uses the already computed FJFX minutiae template
	std::vector<NFIQ::QualityFeatureResult> fjfxMinQualFeatures =
	    fjfxMinQualFeatureModule.computeFeatureData(
		rawImage, minutiaData, templateCouldBeExtracted);

	// append to feature vector
	std::vector<NFIQ::QualityFeatureResult>::iterator
	    it_fjfxMinQualFeatures;
	for (it_fjfxMinQualFeatures = fjfxMinQualFeatures.begin();
	     it_fjfxMinQualFeatures != fjfxMinQualFeatures.end();
	     ++it_fjfxMinQualFeatures) {
		if (it_fjfxMinQualFeatures->returnCode == 0) {
			featureVector.push_back(
			    it_fjfxMinQualFeatures->featureData);
		} else {
			// feature extraction error is mapped to a score of 0
			it_fjfxMinQualFeatures->featureData.featureDataDouble =
			    0;
			featureVector.push_back(
			    it_fjfxMinQualFeatures->featureData);
		}
	}

	// compute ROI features
	// ImgProcROIArea_Mean
	ImgProcROIFeature roiFeatureModule(bOutputSpeed, speedValues);
	// this module returns ROI information to be used within other modules
	ImgProcROIFeature::ImgProcROIResults imgProcResults;
	std::vector<NFIQ::QualityFeatureResult> roiFeatures =
	    roiFeatureModule.computeFeatureData(rawImage, imgProcResults);

	// append to feature vector
	std::vector<NFIQ::QualityFeatureResult>::iterator it_roiFeatures;
	for (it_roiFeatures = roiFeatures.begin();
	     it_roiFeatures != roiFeatures.end(); ++it_roiFeatures) {
		if (it_roiFeatures->returnCode == 0) {
			featureVector.push_back(it_roiFeatures->featureData);
		} else {
			// feature extraction error is mapped to a score of 0
			it_roiFeatures->featureData.featureDataDouble = 0;
			featureVector.push_back(it_roiFeatures->featureData);
		}
	}

	// add ROI information to actionable quality feedback
	NFIQ::ActionableQualityFeedback fb_roi;
	fb_roi.actionableQualityValue =
	    imgProcResults.noOfROIPixels; // absolute number of ROI pixels
					  // (foreground)
	fb_roi.identifier = NFIQ::
	    ActionableQualityFeedbackIdentifier_SufficientFingerprintForeground;
	actionableQuality.push_back(fb_roi);

	// compute LCS features
	// LCS_Bin10_[0-9]
	// LCS_Bin10_Mean
	// LCS_Bin10_StdDev
	LCSFeature lcsFeatureModule(bOutputSpeed, speedValues);
	std::vector<NFIQ::QualityFeatureResult> lcsFeatures =
	    lcsFeatureModule.computeFeatureData(rawImage);

	// append to feature vector
	std::vector<NFIQ::QualityFeatureResult>::iterator it_lcsFeatures;
	for (it_lcsFeatures = lcsFeatures.begin();
	     it_lcsFeatures != lcsFeatures.end(); ++it_lcsFeatures) {
		if (it_lcsFeatures->returnCode == 0) {
			featureVector.push_back(it_lcsFeatures->featureData);
		} else {
			// feature extraction error is mapped to a score of 0
			it_lcsFeatures->featureData.featureDataDouble = 0;
			featureVector.push_back(it_lcsFeatures->featureData);
		}
	}

	// add contrast features (they are already computed above)
	// Mu
	// MMB

	// append to feature vector
	for (it_muFeatures = muFeatures.begin();
	     it_muFeatures != muFeatures.end(); ++it_muFeatures) {
		if (it_muFeatures->returnCode == 0) {
			featureVector.push_back(it_muFeatures->featureData);
		} else {
			// feature extraction error is mapped to a score of 0
			it_muFeatures->featureData.featureDataDouble = 0;
			featureVector.push_back(it_muFeatures->featureData);
		}
	}

	// compute OCL histogram features
	// OCL_Bin10_[0-9]
	// OCL_Bin10_Mean
	// OCL_Bin10_StdDev
	OCLHistogramFeature oclFeatureModule(bOutputSpeed, speedValues);
	std::vector<NFIQ::QualityFeatureResult> oclFeatures =
	    oclFeatureModule.computeFeatureData(rawImage);

	// append to feature vector
	std::vector<NFIQ::QualityFeatureResult>::iterator it_oclFeatures;
	for (it_oclFeatures = oclFeatures.begin();
	     it_oclFeatures != oclFeatures.end(); ++it_oclFeatures) {
		if (it_oclFeatures->returnCode == 0) {
			featureVector.push_back(it_oclFeatures->featureData);
		} else {
			// feature extraction error is mapped to a score of 0
			it_oclFeatures->featureData.featureDataDouble = 0;
			featureVector.push_back(it_oclFeatures->featureData);
		}
	}

	// compute OF features
	// OF_Bin10_[0-9]
	// OF_Bin10_Mean
	// OF_Bin10_StdDev
	OFFeature ofFeatureModule(bOutputSpeed, speedValues);
	std::vector<NFIQ::QualityFeatureResult> ofFeatures =
	    ofFeatureModule.computeFeatureData(rawImage);

	// append to feature vector
	std::vector<NFIQ::QualityFeatureResult>::iterator it_ofFeatures;
	for (it_ofFeatures = ofFeatures.begin();
	     it_ofFeatures != ofFeatures.end(); ++it_ofFeatures) {
		if (it_ofFeatures->returnCode == 0) {
			featureVector.push_back(it_ofFeatures->featureData);
		} else {
			// feature extraction error is mapped to a score of 0
			it_ofFeatures->featureData.featureDataDouble = 0;
			featureVector.push_back(it_ofFeatures->featureData);
		}
	}

	// compute quality map features
	// OrientationMap_ROIFilter_CoherenceRel
	// OrientationMap_ROIFilter_CoherenceSum
	QualityMapFeatures qmFeatureModule(bOutputSpeed, speedValues);
	std::vector<NFIQ::QualityFeatureResult> qmFeatures =
	    qmFeatureModule.computeFeatureData(rawImage, imgProcResults);

	// append to feature vector
	std::vector<NFIQ::QualityFeatureResult>::iterator it_qmFeatures;
	for (it_qmFeatures = qmFeatures.begin();
	     it_qmFeatures != qmFeatures.end(); ++it_qmFeatures) {
		if (it_qmFeatures->returnCode == 0) {
			featureVector.push_back(it_qmFeatures->featureData);
		} else {
			// feature extraction error is mapped to a score of 0
			it_qmFeatures->featureData.featureDataDouble = 0;
			featureVector.push_back(it_qmFeatures->featureData);
		}
	}

	// compute RVU features
	// RVUP_Bin10_[0-9]
	// RVUP_Bin10_Mean
	// RVUP_Bin10_StdDev
	RVUPHistogramFeature rvupFeatureModule(bOutputSpeed, speedValues);
	std::vector<NFIQ::QualityFeatureResult> rvupFeatures =
	    rvupFeatureModule.computeFeatureData(rawImage);

	// append to feature vector
	std::vector<NFIQ::QualityFeatureResult>::iterator it_rvupFeatures;
	for (it_rvupFeatures = rvupFeatures.begin();
	     it_rvupFeatures != rvupFeatures.end(); ++it_rvupFeatures) {
		if (it_rvupFeatures->returnCode == 0) {
			featureVector.push_back(it_rvupFeatures->featureData);
		} else {
			// feature extraction error is mapped to a score of 0
			it_rvupFeatures->featureData.featureDataDouble = 0;
			featureVector.push_back(it_rvupFeatures->featureData);
		}
	}

	return featureVector;
}

std::vector<std::string>
NFIQ::QualityFeatures::Impl::getAllActionableIdentifiers()
{
	static const std::vector<std::string> actionableIdentifiers {
		NFIQ::
		    ActionableQualityFeedbackIdentifier_EmptyImageOrContrastTooLow,
		NFIQ::ActionableQualityFeedbackIdentifier_UniformImage,
		NFIQ::
		    ActionableQualityFeedbackIdentifier_FingerprintImageWithMinutiae,
		ActionableQualityFeedbackIdentifier_SufficientFingerprintForeground
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
