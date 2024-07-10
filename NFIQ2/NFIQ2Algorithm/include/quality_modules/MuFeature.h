#ifndef MUFEATURE_H
#define MUFEATURE_H

#include <nfiq2_constants.hpp>
#include <nfiq2_fingerprintimagedata.hpp>
#include <quality_modules/Module.h>

#include <string>
#include <vector>

namespace NFIQ2 { namespace QualityMeasures {

class MuFeature : public Algorithm {
    public:
	MuFeature(const NFIQ2::FingerprintImageData &fingerprintImage);
	virtual ~MuFeature();

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

#endif

/******************************************************************************/
