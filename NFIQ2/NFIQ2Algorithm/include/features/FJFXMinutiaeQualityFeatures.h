#ifndef FINGERJETFXMINUTIAEQUALITYFEATURE_H
#define FINGERJETFXMINUTIAEQUALITYFEATURE_H

#include <features/FingerJetFXFeature.h>
#include <features/Module.h>
#include <nfiq2_constants.hpp>
#include <nfiq2_fingerprintimagedata.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "FRFXLL.h"

#include <string>
#include <vector>

namespace NFIQ2 { namespace QualityFeatures {

/* Ideal Standard Deviation of pixel values in a neighborhood. */
#define IDEALSTDEV 64
/* Ideal Mean of pixel values in a neighborhood. */
#define IDEALMEAN 127

class FJFXMinutiaeQualityFeature : public Module {
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

	std::string getModuleName() const override;

	static std::vector<std::string> getQualityFeatureIDs();

	/** @throw NFIQ2::Exception
	 * Template could not be extracted.
	 */
	std::vector<FingerJetFXFeature::Minutia> getMinutiaData() const;

    private:
	std::unordered_map<std::string, double> computeFeatureData(
	    const NFIQ2::FingerprintImageData &fingerprintImage);

	std::vector<FingerJetFXFeature::Minutia> minutiaData_ {};
	std::vector<MinutiaData> computeMuMinQuality(
	    int bs, const NFIQ2::FingerprintImageData &fingerprintImage);

	std::vector<MinutiaData> computeOCLMinQuality(
	    int bs, const NFIQ2::FingerprintImageData &fingerprintImage);

	double computeMMBBasedOnCOM(int bs,
	    const NFIQ2::FingerprintImageData &fingerprintImage,
	    unsigned int regionSize);
};

}}
#endif

/******************************************************************************/
