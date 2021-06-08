#ifndef MUFEATURE_H
#define MUFEATURE_H

#include <features/Module.h>
#include <nfiq2_constants.hpp>
#include <nfiq2_fingerprintimagedata.hpp>

#include <string>
#include <vector>

namespace NFIQ2 { namespace QualityFeatures {

class MuFeature : public Module {
    public:
	MuFeature(const NFIQ2::FingerprintImageData &fingerprintImage);
	virtual ~MuFeature();

	std::string getModuleName() const override;

	/** @throw NFIQ2::Exception
	 * Sigma has not yet been calculated.
	 */
	double getSigma() const;

	static std::vector<std::string> getQualityFeatureIDs();

    private:
	std::unordered_map<std::string, double> computeFeatureData(
	    const NFIQ2::FingerprintImageData &fingerprintImage);

	bool sigmaComputed { false };
	double sigma {};
};

}}

#endif

/******************************************************************************/
