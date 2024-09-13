#ifndef NFIQ2_QUALITYMODULES_LCS_H_
#define NFIQ2_QUALITYMODULES_LCS_H_

#include <nfiq2_constants.hpp>
#include <nfiq2_fingerprintimagedata.hpp>
#include <quality_modules/Module.h>

#include <string>
#include <vector>

namespace NFIQ2 { namespace QualityMeasures {

static double LCSHISTLIMITS[9] = { 0, 0.70, 0.74, 0.77, 0.79, 0.81, 0.83, 0.85,
	0.87 };

class LCS : public Algorithm {
    public:
	LCS(const NFIQ2::FingerprintImageData &fingerprintImage);
	virtual ~LCS();

	std::string getName() const override;

	static std::vector<std::string> getNativeQualityMeasureIDs();

    private:
	std::unordered_map<std::string, double> computeFeatureData(
	    const NFIQ2::FingerprintImageData &fingerprintImage);

	const int blocksize { Sizes::LocalRegionSquare };
	const double threshold { .1 };
	const int scannerRes { 500 };
	const bool padFlag { false };
};

}}

#endif /* NFIQ2_QUALITYMODULES_LCS_H_ */

/******************************************************************************/
