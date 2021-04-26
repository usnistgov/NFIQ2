#ifndef NFIQ2_RESULTS_HPP_
#define NFIQ2_RESULTS_HPP_

#include <nfiq2_interfacedefinitions.hpp>

#include <memory>
#include <string>
#include <vector>

namespace NFIQ2 {

class NFIQ2Results {
    public:
	NFIQ2Results();
	NFIQ2Results(const std::vector<NFIQ2::ActionableQualityFeedback>
			 &actionableQuality,
	    const std::vector<NFIQ2::QualityFeatureData> &qualityFeatureData,
	    const std::vector<NFIQ2::QualityFeatureSpeed> &qualityFeatureSpeed,
	    const unsigned int qualityScore);
	~NFIQ2Results();

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
	class Impl;
	std::shared_ptr<NFIQ2Results::Impl> pimpl;
	std::vector<NFIQ2::ActionableQualityFeedback> actionableQuality_ {};
	std::vector<NFIQ2::QualityFeatureData> qualityFeatureData_ {};
	std::vector<NFIQ2::QualityFeatureSpeed> qualityFeatureSpeed_ {};
	unsigned int qualityScore_ {};
};
}

#endif
