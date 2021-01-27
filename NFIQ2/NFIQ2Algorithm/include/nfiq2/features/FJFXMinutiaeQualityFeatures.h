#ifndef FINGERJETFXMINUTIAEQUALITYFEATURE_H
#define FINGERJETFXMINUTIAEQUALITYFEATURE_H

#include <nfiq2/features/BaseFeature.h>
#include <nfiq2/features/FingerJetFXFeature.h>
#include <nfiq2/fingerprintimagedata.hpp>
#include <nfiq2/interfacedefinitions.hpp>
#include <stdio.h>
#include <stdlib.h>

#include "FRFXLL.h"

#include <list>
#include <string>
#include <vector>

#if defined LINUX || defined __ANDROID__ || __APPLE__
#include <sys/types.h>
#include <sys/queue.h>
#include <sys/stat.h>

#include <unistd.h>
#endif

#include <errno.h>
#include <fcntl.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdint.h>

/* Ideal Standard Deviation of pixel values in a neighborhood. */
#define IDEALSTDEV 64
/* Ideal Mean of pixel values in a neighborhood. */
#define IDEALMEAN 127

class FJFXMinutiaeQualityFeature : BaseFeature {
    public:
	struct MinutiaData {
		int x;		///< x-coordinate from top-left corner
		int y;		///< y-coordinate from top-left corner
		double quality; ///< computed minutiae quality value
	};

	FJFXMinutiaeQualityFeature(bool bOutputSpeed,
	    std::list<NFIQ::QualityFeatureSpeed> &speedValues)
	    : BaseFeature(bOutputSpeed, speedValues) {};
	virtual ~FJFXMinutiaeQualityFeature();

	std::list<NFIQ::QualityFeatureResult> computeFeatureData(
	    const NFIQ::FingerprintImageData &fingerprintImage,
	    std::vector<FingerJetFXFeature::Minutia> &minutiaData,
	    bool &templateCouldBeExtracted);

	std::string getModuleID();

	void initModule() { /* not needed here */ };

	static std::list<std::string> getAllFeatureIDs();
	static const std::string speedFeatureIDGroup;

    private:
	std::vector<MinutiaData> computeMuMinQuality(
	    std::vector<FingerJetFXFeature::Minutia> &minutiaData, int bs,
	    const NFIQ::FingerprintImageData &fingerprintImage);

	std::vector<MinutiaData> computeOCLMinQuality(
	    std::vector<FingerJetFXFeature::Minutia> &minutiaData, int bs,
	    const NFIQ::FingerprintImageData &fingerprintImage);

	double computeMMBBasedOnCOM(
	    std::vector<FingerJetFXFeature::Minutia> &minutiaData, int bs,
	    const NFIQ::FingerprintImageData &fingerprintImage,
	    unsigned int regionSize);
};

#endif

/******************************************************************************/
