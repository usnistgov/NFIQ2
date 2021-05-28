#ifndef RANDOMFORESTML_H
#define RANDOMFORESTML_H

#include <nfiq2_constants.hpp>
#include <opencv2/ml.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace NFIQ2 { namespace Prediction {

/**
 * This class handles the Random Forest Machine Learning model used
 * to generate NFIQ 2 quality scores.
 */
class RandomForestML {

    public:
	/** Default constructor of RandomForestML. */
	RandomForestML();

	/** Destructor. */
	virtual ~RandomForestML();

	/** Returns the Random Forest Module name. */
	std::string getModuleName() const;

#ifdef NFIQ2_EMBED_RANDOM_FOREST_PARAMETERS
	/** Initializes module when parameters are embedded. */
	std::string initModule();
#endif

	/** Initialize model (When not using embedded parameters). */
	std::string initModule(
	    const std::string &fileName, const std::string &fileHash);

	/**
	 * Compute NFIQ2 quality score based on model and provided
	 * QualityFeatureData.
	 */
	void evaluate(const std::unordered_map<std::string, double> &features,
	    double &qualityValue) const;

    private:
	/** OpenCV shared smart pointer referring to the RF model itself. */
	cv::Ptr<cv::ml::RTrees> m_pTrainedRF;
	/** Calculates the hash of the RandomForest parameters. */
	std::string calculateHashString(const std::string &s);
	/** Initialize model using string parameters. */
	void initModule(const std::string &params);

#ifdef NFIQ2_EMBED_RANDOM_FOREST_PARAMETERS
	/** Extracts string parameters when model is embedded. */
	std::string joinRFTrainedParamsString();
#endif
};

}}

#endif /* RANDOMFORESTML_H */
