#ifndef NFIQ2_QUALITYMODULES_MU_H_
#define NFIQ2_QUALITYMODULES_MU_H_

#include <nfiq2_constants.hpp>
#include <nfiq2_fingerprintimagedata.hpp>
#include <quality_modules/Module.h>

#include <string>
#include <vector>

namespace NFIQ2 { namespace QualityMeasures {

class Mu : public Algorithm {
    public:
	Mu(const NFIQ2::FingerprintImageData &fingerprintImage);
	virtual ~Mu();

	std::string getName() const override;

	/** @throw NFIQ2::Exception
	 * Sigma has not yet been calculated.
	 */
	double getSigma() const;

	static std::vector<std::string> getNativeQualityMeasureIDs();

    private:
	std::unordered_map<std::string, double> computeFeatureData(
	    const NFIQ2::FingerprintImageData &fingerprintImage);

	bool sigmaComputed { false };
	double sigma {};
};

}}

#endif /* NFIQ2_QUALITYMODULES_MU_H_ */

/******************************************************************************/
