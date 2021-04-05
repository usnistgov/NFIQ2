#ifndef MUFEATURE_H
#define MUFEATURE_H

#include <features/BaseFeature.h>
#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_interfacedefinitions.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <string>
#include <vector>

namespace NFIQ { namespace QualityFeatures {

class MuFeature : BaseFeature {
    public:
	MuFeature()
	    : BaseFeature() {};
	virtual ~MuFeature();

	std::vector<NFIQ::QualityFeatureResult> computeFeatureData(
	    const NFIQ::FingerprintImageData &fingerprintImage) override;

	std::vector<NFIQ::QualityFeatureSpeed> getSpeedValues() const override;

	std::string getModuleName() const override;

	/** @throw NFIQ::NFIQException
	 * Sigma has not yet been calculated.
	 */
	double getSigma() const;

	void initModule() { /* not needed here */ };

	static std::vector<std::string> getAllFeatureIDs();
	static const std::string speedFeatureIDGroup;
	static const std::string moduleName;

    private:
	bool sigmaComputed { false };
	double sigma {};
};

}}

#endif

/******************************************************************************/
