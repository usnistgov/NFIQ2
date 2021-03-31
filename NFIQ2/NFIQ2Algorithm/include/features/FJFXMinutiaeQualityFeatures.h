#ifndef FINGERJETFXMINUTIAEQUALITYFEATURE_H
#define FINGERJETFXMINUTIAEQUALITYFEATURE_H

#include <features/BaseFeature.h>
#include <features/FingerJetFXFeature.h>
#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_interfacedefinitions.hpp>
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

namespace NFIQ { namespace QualityFeatures {

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
	    std::vector<NFIQ::QualityFeatureSpeed> &speedValues)
	    : BaseFeature(bOutputSpeed, speedValues) {};
	virtual ~FJFXMinutiaeQualityFeature();

	std::vector<NFIQ::QualityFeatureResult> computeFeatureData(
	    const NFIQ::FingerprintImageData &fingerprintImage,
	    const std::vector<FingerJetFXFeature::Minutia> &minutiaData,
	    bool &templateCouldBeExtracted);

	std::string getModuleName() const override;

	void initModule() { /* not needed here */ };

	static std::vector<std::string> getAllFeatureIDs();
	static const std::string speedFeatureIDGroup;
	static const std::string moduleName;

    private:
	std::vector<MinutiaData> computeMuMinQuality(
	    const std::vector<FingerJetFXFeature::Minutia> &minutiaData, int bs,
	    const NFIQ::FingerprintImageData &fingerprintImage);

	std::vector<MinutiaData> computeOCLMinQuality(
	    const std::vector<FingerJetFXFeature::Minutia> &minutiaData, int bs,
	    const NFIQ::FingerprintImageData &fingerprintImage);

	double computeMMBBasedOnCOM(
	    const std::vector<FingerJetFXFeature::Minutia> &minutiaData, int bs,
	    const NFIQ::FingerprintImageData &fingerprintImage,
	    unsigned int regionSize);
};

}}
#endif

/******************************************************************************/
