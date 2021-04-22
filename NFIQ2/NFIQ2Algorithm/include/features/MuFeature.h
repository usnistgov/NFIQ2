#ifndef MUFEATURE_H
#define MUFEATURE_H

#include <features/BaseFeature.h>
#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_interfacedefinitions.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdlib.h>

#include <list>
#include <string>
#include <vector>

namespace NFIQ { namespace QualityFeatures {

class MuFeature : public BaseFeature {
    public:
	MuFeature(const NFIQ::FingerprintImageData &fingerprintImage);
	virtual ~MuFeature();

	std::string getModuleName() const override;

	/** @throw NFIQ::NFIQException
	 * Sigma has not yet been calculated.
	 */
	double getSigma() const;

	static std::vector<std::string> getAllFeatureIDs();
	static const std::string speedFeatureIDGroup;
	static const std::string moduleName;

    private:
	std::vector<NFIQ::QualityFeatureResult> computeFeatureData(
	    const NFIQ::FingerprintImageData &fingerprintImage);

	bool sigmaComputed { false };
	double sigma {};
};

}}

#endif

/******************************************************************************/
