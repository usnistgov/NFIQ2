#ifndef NFIQ2_NFIQ2_RESULTS_IMPL_H_
#define NFIQ2_NFIQ2_RESULTS_IMPL_H_

#include <nfiq2/nfiq2_results.hpp>

#include "nfiq2_results_impl.h"

#include <vector>

namespace NFIQ {
class NFIQ2Results::Impl {
    public:
	Impl();
	Impl(std::vector<NFIQ::ActionableQualityFeedback> actionableQuality,
	    std::vector<NFIQ::QualityFeatureData> qualityfeatureData,
	    std::vector<NFIQ::QualityFeatureSpeed> qualityFeatureSpeed,
	    unsigned int qualityScore);
	virtual ~Impl();

	void setActionable(
	    std::vector<NFIQ::ActionableQualityFeedback> actionableQuality);
	void setQuality(
	    std::vector<NFIQ::QualityFeatureData> qualityfeatureData);
	void setSpeed(
	    std::vector<NFIQ::QualityFeatureSpeed> qualityFeatureSpeed);
	void setScore(unsigned int qualityScore);

	std::vector<NFIQ::ActionableQualityFeedback> getActionable() const;
	std::vector<NFIQ::QualityFeatureData> getQuality() const;
	std::vector<NFIQ::QualityFeatureSpeed> getSpeed() const;
	unsigned int getScore() const;

    private:
	std::vector<NFIQ::ActionableQualityFeedback> actionableQuality_ {};
	std::vector<NFIQ::QualityFeatureData> qualityfeatureData_ {};
	std::vector<NFIQ::QualityFeatureSpeed> qualityFeatureSpeed_ {};
	unsigned int qualityScore_ {};
};
}

#endif
