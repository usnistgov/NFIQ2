#ifndef NFIQ2_NFIQ2_RESULTS_H_
#define NFIQ2_NFIQ2_RESULTS_H_

#include <nfiq2/interfacedefinitions.hpp>

#include <memory>
#include <string>
#include <vector>

namespace NFIQ {

class NFIQ2Results {
    public:
	NFIQ2Results();
	NFIQ2Results(const std::vector<NFIQ::ActionableQualityFeedback>
			 &actionableQuality,
	    const std::vector<NFIQ::QualityFeatureData> &qualityFeatureData,
	    const std::vector<NFIQ::QualityFeatureSpeed> &qualityFeatureSpeed,
	    const unsigned int qualityScore);
	~NFIQ2Results();

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
	class Impl;
	std::shared_ptr<NFIQ2Results::Impl> pimpl;
	std::vector<NFIQ::ActionableQualityFeedback> actionableQuality_ {};
	std::vector<NFIQ::QualityFeatureData> qualityFeatureData_ {};
	std::vector<NFIQ::QualityFeatureSpeed> qualityFeatureSpeed_ {};
	unsigned int qualityScore_ {};
};
}

#endif
