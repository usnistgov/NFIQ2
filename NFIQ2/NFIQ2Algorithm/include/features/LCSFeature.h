#ifndef LCSFEATURE_H
#define LCSFEATURE_H

#include <features/Module.h>
#include <nfiq2_constants.hpp>
#include <nfiq2_fingerprintimagedata.hpp>

#include <string>
#include <vector>

namespace NFIQ2 { namespace QualityFeatures {

static double LCSHISTLIMITS[9] = { 0, 0.70, 0.74, 0.77, 0.79, 0.81, 0.83, 0.85,
	0.87 };

class LCSFeature : public Module {
    public:
	LCSFeature(const NFIQ2::FingerprintImageData &fingerprintImage);
	virtual ~LCSFeature();

	std::string getModuleName() const override;

	static std::vector<std::string> getQualityFeatureIDs();

    private:
	std::unordered_map<std::string, double> computeFeatureData(
	    const NFIQ2::FingerprintImageData &fingerprintImage);

	const int blocksize { 32 };
	const double threshold { .1 };
	const int scannerRes { 500 };
	const bool padFlag { false };
};

}}

#endif

/******************************************************************************/
