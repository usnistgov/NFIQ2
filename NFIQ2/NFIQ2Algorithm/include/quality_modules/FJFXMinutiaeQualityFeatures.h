#ifndef FINGERJETFXMINUTIAEQUALITYFEATURE_H
#define FINGERJETFXMINUTIAEQUALITYFEATURE_H

#include <nfiq2_constants.hpp>
#include <nfiq2_fingerprintimagedata.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <quality_modules/FingerJetFXFeature.h>
#include <quality_modules/Module.h>

#include "FRFXLL.h"

#include <string>
#include <vector>

namespace NFIQ2 { namespace QualityMeasures {

/* Ideal Standard Deviation of pixel values in a neighborhood. */
#define IDEALSTDEV 64
/* Ideal Mean of pixel values in a neighborhood. */
#define IDEALMEAN 127

class FJFXMinutiaeQualityFeature : public Algorithm {
    public:
	struct MinutiaData {
		int x;		///< x-coordinate from top-left corner
		int y;		///< y-coordinate from top-left corner
		double quality; ///< computed minutiae quality value
	};

	FJFXMinutiaeQualityFeature(
	    const NFIQ2::FingerprintImageData &fingerprintImage,
	    const std::vector<FingerJetFXFeature::Minutia> &minutiaData);

	virtual ~FJFXMinutiaeQualityFeature();

	std::string getName() const override;

	static std::vector<std::string> getNativeQualityMeasureIDs();

	/** @throw NFIQ2::Exception
	 * Template could not be extracted.
	 */
	std::vector<FingerJetFXFeature::Minutia> getMinutiaData() const;

    private:
	std::unordered_map<std::string, double> computeFeatureData(
	    const NFIQ2::FingerprintImageData &fingerprintImage);

	std::vector<FingerJetFXFeature::Minutia> minutiaData_ {};
	std::vector<MinutiaData> computeMuMinQuality(int bs,
	    const NFIQ2::FingerprintImageData &fingerprintImage);

	std::vector<MinutiaData> computeOCLMinQuality(int bs,
	    const NFIQ2::FingerprintImageData &fingerprintImage);

	double computeMMBBasedOnCOM(int bs,
	    const NFIQ2::FingerprintImageData &fingerprintImage,
	    unsigned int regionSize);
};

}}
#endif

/******************************************************************************/
