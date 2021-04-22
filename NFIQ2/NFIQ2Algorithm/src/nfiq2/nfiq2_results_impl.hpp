#ifndef NFIQ2_RESULTS_IMPL_HPP_
#define NFIQ2_RESULTS_IMPL_HPP_

#include <nfiq2_results.hpp>

#include "nfiq2_results_impl.hpp"
#include <vector>

namespace NFIQ {
class NFIQ2Results::Impl {
    public:
	Impl();
	Impl(const std::vector<NFIQ::ActionableQualityFeedback>
		 &actionableQuality,
	    const std::vector<NFIQ::QualityFeatureData> &qualityFeatureData,
	    const std::vector<NFIQ::QualityFeatureSpeed> &qualityFeatureSpeed,
	    const unsigned int qualityScore);
	~Impl();

	static unsigned int checkScore(const unsigned int qualityScore);

	void setActionableQualityFeedback(
	    const std::vector<NFIQ::ActionableQualityFeedback>
		&actionableQuality);
	void setQualityFeatures(
	    const std::vector<NFIQ::QualityFeatureData> &qualityFeatureData);
	void setQualityFeatureSpeed(
	    const std::vector<NFIQ::QualityFeatureSpeed> &qualityFeatureSpeed);
	void setScore(const unsigned int qualityScore);

	std::vector<NFIQ::ActionableQualityFeedback>
	getActionableQualityFeedback() const;
	std::vector<NFIQ::QualityFeatureData> getQualityFeatures() const;
	std::vector<NFIQ::QualityFeatureSpeed> getQualityFeatureSpeed() const;
	unsigned int getScore() const;

    private:
	std::vector<NFIQ::ActionableQualityFeedback> actionableQuality_ {};
	std::vector<NFIQ::QualityFeatureData> qualityFeatureData_ {};
	std::vector<NFIQ::QualityFeatureSpeed> qualityFeatureSpeed_ {};
	unsigned int qualityScore_ {};
};
}

#endif
