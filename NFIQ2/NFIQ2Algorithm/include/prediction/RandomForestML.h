#ifndef RANDOMFORESTML_H
#define RANDOMFORESTML_H

#include <nfiq2_interfacedefinitions.hpp>
#include <opencv2/ml.hpp>

#include <string>
#include <unordered_map>
#include <vector>

#undef EMBED_RANDOMFOREST_PARAMETERS

namespace NFIQ { namespace Prediction {

class RandomForestML {
    public:
	RandomForestML();
	virtual ~RandomForestML();

	static const std::string moduleName;
	std::string getModuleName() const;

#ifdef EMBED_RANDOMFOREST_PARAMETERS
	std::string initModule();
#endif
	std::string initModule(
	    const std::string &fileName, const std::string &fileHash);

	void evaluate(
	    const std::unordered_map<std::string, NFIQ::QualityFeatureData>
		&features,
	    const double &utilityValue, double &qualityValue,
	    double &deviation) const;

    private:
	cv::Ptr<cv::ml::RTrees> m_pTrainedRF;

	std::string calculateHashString(const std::string &s);
	void initModule(const std::string &params);
#ifdef EMBED_RANDOMFOREST_PARAMETERS
	std::string joinRFTrainedParamsString();
#endif
};

}}

#endif

/******************************************************************************/
