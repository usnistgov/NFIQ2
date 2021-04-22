#ifndef FINGERJETFXMINUTIAEQUALITYFEATURE_H
#define FINGERJETFXMINUTIAEQUALITYFEATURE_H

#include <features/BaseFeature.h>
#include <features/FingerJetFXFeature.h>
#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_interfacedefinitions.hpp>
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

class FJFXMinutiaeQualityFeature : public BaseFeature {
    public:
	struct MinutiaData {
		int x;		///< x-coordinate from top-left corner
		int y;		///< y-coordinate from top-left corner
		double quality; ///< computed minutiae quality value
	};

	FJFXMinutiaeQualityFeature(
	    const NFIQ::FingerprintImageData &fingerprintImage,
	    const std::vector<FingerJetFXFeature::Minutia> &minutiaData,
	    const bool templateCouldBeExtracted);

	virtual ~FJFXMinutiaeQualityFeature();

	std::string getModuleName() const override;

	static std::vector<std::string> getAllFeatureIDs();
	static const std::string speedFeatureIDGroup;
	static const std::string moduleName;

	/** @throw NFIQ::NFIQException
	 * Template could not be extracted.
	 */
	std::vector<FingerJetFXFeature::Minutia> getMinutiaData() const;

	bool getTemplateStatus() const;

    private:
	std::vector<NFIQ::QualityFeatureResult> computeFeatureData(
	    const NFIQ::FingerprintImageData &fingerprintImage);

	std::vector<FingerJetFXFeature::Minutia> minutiaData_ {};
	bool templateCouldBeExtracted_ { false };
	std::vector<MinutiaData> computeMuMinQuality(
	    int bs, const NFIQ::FingerprintImageData &fingerprintImage);

	std::vector<MinutiaData> computeOCLMinQuality(
	    int bs, const NFIQ::FingerprintImageData &fingerprintImage);

	double computeMMBBasedOnCOM(int bs,
	    const NFIQ::FingerprintImageData &fingerprintImage,
	    unsigned int regionSize);
};

}}
#endif

/******************************************************************************/
