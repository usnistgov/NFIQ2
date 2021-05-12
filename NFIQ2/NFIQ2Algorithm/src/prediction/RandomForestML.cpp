#include <nfiq2_exception.hpp>
#include <prediction/RandomForestML.h>

/*
 * If we're embedding parameters, figure out which to embed based on the
 * provided FRCT.
 */
#ifdef NFIQ2_EMBED_RANDOM_FOREST_PARAMETERS
#ifdef NFIQ2_EMBEDDED_RANDOM_FOREST_PARAMETERS_FCT
/* FRCT == Unknown */
#if NFIQ2_EMBEDDED_RANDOM_FOREST_PARAMETERS_FCT == 0
#include <prediction/RandomForestTrainedParams.h>
/* FRCT == scanned ink on paper */
#elif NFIQ2_EMBEDDED_RANDOM_FOREST_PARAMETERS_FCT == 2
#include <prediction/RandomForestTrainedParams.h>
/* FRCT == Optical: total internal reflection (bright field) */
#elif NFIQ2_EMBEDDED_RANDOM_FOREST_PARAMETERS_FCT == 3
#include <prediction/RandomForestTrainedParams.h>
/* Unsupported */
#else
#error Value of NFIQ2_EMBEDDED_RANDOM_FOREST_PARAMETERS_FCT is not supported.
#endif /* NFIQ2_EMBEDDED_RANDOM_FOREST_PARAMETERS_FCT */
#else
#include <prediction/RandomForestTrainedParams.h>
#endif /* NFIQ2_EMBEDDED_RANDOM_FOREST_PARAMETERS_FCT */
#endif /* NFIQ2_EMBED_RANDOM_FOREST_PARAMETERS */

#include "digestpp.hpp"
#include <cmath>
#include <ctime>
#include <numeric> // std::accumulate

std::string
NFIQ2::Prediction::RandomForestML::calculateHashString(const std::string &s)
{
	// calculate and compare the hash
	digestpp::md5 hasher;
	std::stringstream ss;
	ss << std::hex
	   << hasher.absorb((const char *)s.c_str(), s.length()).hexdigest();
	return ss.str();
}

void
NFIQ2::Prediction::RandomForestML::initModule(const std::string &params)
{
	// create file storage with parameters in memory
	cv::FileStorage fs(params.c_str(),
	    cv::FileStorage::READ | cv::FileStorage::MEMORY |
		cv::FileStorage::FORMAT_YAML);
	// now import data structures
	m_pTrainedRF = cv::ml::RTrees::create();
	m_pTrainedRF->read(cv::FileNode(fs["my_random_trees"]));
}

#ifdef NFIQ2_EMBED_RANDOM_FOREST_PARAMETERS
std::string
NFIQ2::Prediction::RandomForestML::joinRFTrainedParamsString()
{
	unsigned int size = sizeof(g_strRandomForestTrainedParams) /
	    sizeof(g_strRandomForestTrainedParams[0]);
	std::string result = "";
	for (unsigned int i = 0; i < size; i++) {
		result.append(g_strRandomForestTrainedParams[i]);
	}
	return result;
}
#endif

NFIQ2::Prediction::RandomForestML::RandomForestML()
{
}

NFIQ2::Prediction::RandomForestML::~RandomForestML()
{
	if (!m_pTrainedRF.empty()) {
		m_pTrainedRF->clear();
	}
}

#ifdef NFIQ2_EMBED_RANDOM_FOREST_PARAMETERS
std::string
NFIQ2::Prediction::RandomForestML::initModule()
{
	try {
		// get parameters from string
		// accumulate it to a single string
		// and decode base64
		NFIQ2::Data data;
		std::string params = joinRFTrainedParamsString();
		unsigned int size = params.size();
		data.fromBase64String(params);
		params = "";
		params.assign((const char *)data.data(), data.size());
		initModule(params);
		return calculateHashString(params);
	} catch (const cv::Exception &e) {
		throw Exception(NFIQ2::ErrorCode::UnknownError, e.msg);
	} catch (...) {
		throw;
	}
}
#endif

std::string
NFIQ2::Prediction::RandomForestML::initModule(
    const std::string &fileName, const std::string &fileHash)
{
	std::ifstream input(fileName);
	std::string params((std::istreambuf_iterator<char>(input)),
	    std::istreambuf_iterator<char>());
	initModule(params);
	// calculate and compare the hash
	std::string hash = calculateHashString(params);
	if (fileHash.compare(hash) != 0) {
		m_pTrainedRF->clear();
		throw NFIQ2::Exception(NFIQ2::ErrorCode::InvalidConfiguration,
		    "The trained network could not be initialized! "
		    "Error: " +
			hash);
	}
	return hash;
}

void
NFIQ2::Prediction::RandomForestML::evaluate(
    const std::unordered_map<std::string, NFIQ2::QualityFeatureData> &features,
    double &qualityValue) const
{
	/**
	   The following ordering of feature keys is critical to the
	   correct computation of NFIQ 2 scores. Any modification to this
	   ordering will result in incorrectly generated NFIQ 2 scores. This is
	   based on the training model currently in use and may be updated in
	   the future.
	*/
	static const std::vector<std::string> rfFeatureOrder { "FDA_Bin10_0",
		"FDA_Bin10_1", "FDA_Bin10_2", "FDA_Bin10_3", "FDA_Bin10_4",
		"FDA_Bin10_5", "FDA_Bin10_6", "FDA_Bin10_7", "FDA_Bin10_8",
		"FDA_Bin10_9", "FDA_Bin10_Mean", "FDA_Bin10_StdDev",
		"FingerJetFX_MinCount_COMMinRect200x200",
		"FingerJetFX_MinutiaeCount", "FJFXPos_Mu_MinutiaeQuality_2",
		"FJFXPos_OCL_MinutiaeQuality_80", "ImgProcROIArea_Mean",
		"LCS_Bin10_0", "LCS_Bin10_1", "LCS_Bin10_2", "LCS_Bin10_3",
		"LCS_Bin10_4", "LCS_Bin10_5", "LCS_Bin10_6", "LCS_Bin10_7",
		"LCS_Bin10_8", "LCS_Bin10_9", "LCS_Bin10_Mean",
		"LCS_Bin10_StdDev", "MMB", "Mu", "OCL_Bin10_0", "OCL_Bin10_1",
		"OCL_Bin10_2", "OCL_Bin10_3", "OCL_Bin10_4", "OCL_Bin10_5",
		"OCL_Bin10_6", "OCL_Bin10_7", "OCL_Bin10_8", "OCL_Bin10_9",
		"OCL_Bin10_Mean", "OCL_Bin10_StdDev", "OF_Bin10_0",
		"OF_Bin10_1", "OF_Bin10_2", "OF_Bin10_3", "OF_Bin10_4",
		"OF_Bin10_5", "OF_Bin10_6", "OF_Bin10_7", "OF_Bin10_8",
		"OF_Bin10_9", "OF_Bin10_Mean", "OF_Bin10_StdDev",
		"OrientationMap_ROIFilter_CoherenceRel",
		"OrientationMap_ROIFilter_CoherenceSum", "RVUP_Bin10_0",
		"RVUP_Bin10_1", "RVUP_Bin10_2", "RVUP_Bin10_3", "RVUP_Bin10_4",
		"RVUP_Bin10_5", "RVUP_Bin10_6", "RVUP_Bin10_7", "RVUP_Bin10_8",
		"RVUP_Bin10_9", "RVUP_Bin10_Mean", "RVUP_Bin10_StdDev" };

	std::vector<NFIQ2::QualityFeatureData> featureVector {};
	for (const auto &i : rfFeatureOrder) {
		featureVector.push_back(features.at(i));
	}

	try {
		if (m_pTrainedRF.empty() || !m_pTrainedRF->isTrained() ||
		    !m_pTrainedRF->isClassifier()) {
			throw NFIQ2::Exception(
			    NFIQ2::ErrorCode::InvalidConfiguration,
			    "The trained network could not be loaded for "
			    "prediction!");
		}

		// copy data to structure
		cv::Mat sample_data = cv::Mat(
		    1, featureVector.size(), CV_32FC1);
		std::vector<NFIQ2::QualityFeatureData>::const_iterator it_feat;
		unsigned int counterFeatures = 0;
		for (it_feat = featureVector.begin();
		     it_feat != featureVector.end(); it_feat++) {
			if (it_feat->featureDataType ==
			    e_QualityFeatureDataTypeDouble) {
				sample_data.at<float>(0, counterFeatures) =
				    (float)it_feat->featureDataDouble;
			} else {
				sample_data.at<float>(
				    0, counterFeatures) = 0.0f;
			}
			counterFeatures++;
		}

		// returns probability that between 0 and 1 that result belongs
		// to second class
		float prob = m_pTrainedRF->predict(
		    sample_data, cv::noArray(), cv::ml::StatModel::RAW_OUTPUT);
		// return quality value
		qualityValue = (int)(prob + 0.5);

	} catch (const cv::Exception &e) {
		throw Exception(NFIQ2::ErrorCode::MachineLearningError, e.msg);
	}
}

const std::string NFIQ2::Prediction::RandomForestML::moduleName {
	"NFIQ2_RandomForest"
};

std::string
NFIQ2::Prediction::RandomForestML::getModuleName() const
{
	return moduleName;
}
