#ifndef RANDOMFORESTML_H
#define RANDOMFORESTML_H

#include <nfiq2_interfacedefinitions.hpp>
#include <opencv2/ml.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace NFIQ2 { namespace Prediction {

class RandomForestML {
    public:
	RandomForestML();
	virtual ~RandomForestML();

	static const std::string moduleName;
	std::string getModuleName() const;

#ifdef NFIQ2_EMBED_RANDOM_FOREST_PARAMETERS
	std::string initModule();
#endif
	std::string initModule(
	    const std::string &fileName, const std::string &fileHash);

	void evaluate(
	    const std::unordered_map<std::string, NFIQ2::QualityFeatureData>
		&features,
	    double &qualityValue) const;

    private:
	cv::Ptr<cv::ml::RTrees> m_pTrainedRF;

	std::string calculateHashString(const std::string &s);
	void initModule(const std::string &params);
#ifdef NFIQ2_EMBED_RANDOM_FOREST_PARAMETERS
	std::string joinRFTrainedParamsString();
#endif
};

}}

#endif

/******************************************************************************/
