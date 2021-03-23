#ifndef NFIQ2_NFIQ2_RESULTS_H_
#define NFIQ2_NFIQ2_RESULTS_H_

#include <nfiq2/interfacedefinitions.hpp>

#include <string>
#include <vector>

namespace NFIQ {

class NFIQ2Results {
    public:
	NFIQ2Results();
	NFIQ2Results(
	    std::vector<NFIQ::ActionableQualityFeedback> actionableQuality,
	    std::vector<NFIQ::QualityFeatureData> qualityfeatureData,
	    std::vector<NFIQ::QualityFeatureSpeed> qualityFeatureSpeed,
	    unsigned int qualityScore);
	~NFIQ2Results();

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
	class Impl;
	std::shared_ptr<NFIQ2Results::Impl> pimpl;
	std::vector<NFIQ::ActionableQualityFeedback> actionableQuality_ {};
	std::vector<NFIQ::QualityFeatureData> qualityfeatureData_ {};
	std::vector<NFIQ::QualityFeatureSpeed> qualityFeatureSpeed_ {};
	unsigned int qualityScore_ {};
};
}

#endif
