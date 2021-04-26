#ifndef NFIQ2_RESULTS_IMPL_HPP_
#define NFIQ2_RESULTS_IMPL_HPP_

#include <nfiq2_results.hpp>

#include "nfiq2_results_impl.hpp"
#include <vector>

namespace NFIQ2 {
class NFIQ2Results::Impl {
    public:
	Impl();
	Impl(const std::vector<NFIQ2::ActionableQualityFeedback>
		 &actionableQuality,
	    const std::vector<NFIQ2::QualityFeatureData> &qualityFeatureData,
	    const std::vector<NFIQ2::QualityFeatureSpeed> &qualityFeatureSpeed,
	    const unsigned int qualityScore);
	~Impl();

	static unsigned int checkScore(const unsigned int qualityScore);

	void setActionableQualityFeedback(
	    const std::vector<NFIQ2::ActionableQualityFeedback>
		&actionableQuality);
	void setQualityFeatures(
	    const std::vector<NFIQ2::QualityFeatureData> &qualityFeatureData);
	void setQualityFeatureSpeed(
	    const std::vector<NFIQ2::QualityFeatureSpeed> &qualityFeatureSpeed);
	void setScore(const unsigned int qualityScore);

	std::vector<NFIQ2::ActionableQualityFeedback>
	getActionableQualityFeedback() const;
	std::vector<NFIQ2::QualityFeatureData> getQualityFeatures() const;
	std::vector<NFIQ2::QualityFeatureSpeed> getQualityFeatureSpeed() const;
	unsigned int getScore() const;

    private:
	std::vector<NFIQ2::ActionableQualityFeedback> actionableQuality_ {};
	std::vector<NFIQ2::QualityFeatureData> qualityFeatureData_ {};
	std::vector<NFIQ2::QualityFeatureSpeed> qualityFeatureSpeed_ {};
	unsigned int qualityScore_ {};
};
}

#endif
