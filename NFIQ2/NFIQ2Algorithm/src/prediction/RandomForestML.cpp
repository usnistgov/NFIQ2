#include <nfiq2_nfiqexception.hpp>
#include <prediction/RandomForestML.h>

#ifdef EMBED_RANDOMFOREST_PARAMETERS
#include <prediction/RandomForestTrainedParams.h>
#endif

#if defined WINDOWS || defined WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <float.h>
#include <math.h>
#include <time.h>

/*
 * FIXME: Issue on GitHub Actions where it appears NOMINMAX isn't getting set,
 *        which breaks digestpp.hpp
 */
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include "digestpp.hpp"
#include <numeric> // std::accumulate

using namespace NFIQ;

std::string
NFIQ::Prediction::RandomForestML::calculateHashString(const std::string &s)
{
	// calculate and compare the hash
	digestpp::md5 hasher;
	std::stringstream ss;
	ss << std::hex
	   << hasher.absorb((const char *)s.c_str(), s.length()).hexdigest();
	return ss.str();
}

void
NFIQ::Prediction::RandomForestML::initModule(const std::string &params)
{
	// create file storage with parameters in memory
	cv::FileStorage fs(params.c_str(),
	    cv::FileStorage::READ | cv::FileStorage::MEMORY |
		cv::FileStorage::FORMAT_YAML);
	// now import data structures
#if CV_MAJOR_VERSION <= 2
	m_pTrainedRF = new cv::CvRTrees();
	m_pTrainedRF->read(
	    fs.fs, cv::cvGetFileNodeByName(fs.fs, NULL, "my_random_trees"));
#else
	m_pTrainedRF = cv::ml::RTrees::create();
	m_pTrainedRF->read(cv::FileNode(fs["my_random_trees"]));
#endif
}

#ifdef EMBED_RANDOMFOREST_PARAMETERS
std::string
NFIQ::Prediction::RandomForestML::joinRFTrainedParamsString()
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

NFIQ::Prediction::RandomForestML::RandomForestML()
{
#if CV_MAJOR_VERSION <= 2
	m_pTrainedRF = NULL;
#endif
}

NFIQ::Prediction::RandomForestML::~RandomForestML()
{
#if CV_MAJOR_VERSION <= 2
	if (m_pTrainedRF != nullptr) {
		m_pTrainedRF->clear();
		delete m_pTrainedRF;
		m_pTrainedRF = NULL;
	}
#else
	if (!m_pTrainedRF.empty()) {
		m_pTrainedRF->clear();
	}
#endif
}

#ifdef EMBED_RANDOMFOREST_PARAMETERS
std::string
NFIQ::Prediction::RandomForestML::initModule()
{
	try {
		// get parameters from string
		// accumulate it to a single string
		// and decode base64
		NFIQ::Data data;
		std::string params = joinRFTrainedParamsString();
		unsigned int size = params.size();
		data.fromBase64String(params);
		params = "";
		params.assign((const char *)data.data(), data.size());
		initModule(params);
		return calculateHashString(params);
	} catch (const cv::Exception &e) {
		throw NFIQException(e_Error_UnknownError, e.msg);
	} catch (...) {
		throw;
	}
}
#endif

std::string
NFIQ::Prediction::RandomForestML::initModule(
    const std::string &fileName, const std::string &fileHash)
{
	std::ifstream input(fileName);
	std::string params((std::istreambuf_iterator<char>(input)),
	    std::istreambuf_iterator<char>());
	initModule(params);
	// calculate and compare the hash
	std::string hash = calculateHashString(params);
	if (fileHash.compare(hash) != 0) {
#if CV_MAJOR_VERSION <= 2
		m_pTrainedRF->clear();
		delete m_pTrainedRF;
		m_pTrainedRF = NULL;
#else
		m_pTrainedRF->clear();
#endif
		throw NFIQ::NFIQException(NFIQ::e_Error_InvalidConfiguration,
		    "The trained network could not be initialized! "
		    "Error: " +
			hash);
	}
	return hash;
}

void
NFIQ::Prediction::RandomForestML::evaluate(
    const std::unordered_map<std::string, NFIQ::QualityFeatureData> &features,
    const double &utilityValue, double &qualityValue, double &deviation) const
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

	std::vector<NFIQ::QualityFeatureData> featureVector {};
	for (const auto &i : rfFeatureOrder) {
		featureVector.push_back(features.at(i));
	}

	try {
#if CV_MAJOR_VERSION <= 2
		if (m_pTrainedRF == nullptr) {
			throw NFIQ::NFIQException(
			    NFIQ::e_Error_InvalidConfiguration,
			    "The trained network could not be loaded for "
			    "prediction!");
		}
#else
		if (m_pTrainedRF.empty() || !m_pTrainedRF->isTrained() ||
		    !m_pTrainedRF->isClassifier()) {
			throw NFIQ::NFIQException(
			    NFIQ::e_Error_InvalidConfiguration,
			    "The trained network could not be loaded for "
			    "prediction!");
		}
#endif

		deviation = 0.0; // ignore deviation here

		// copy data to structure
		cv::Mat sample_data = cv::Mat(
		    1, featureVector.size(), CV_32FC1);
		std::vector<NFIQ::QualityFeatureData>::const_iterator it_feat;
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

#if CV_MAJOR_VERSION <= 2
		// returns probability that between 0 and 1 that result belongs
		// to second class
		float prob = m_pTrainedRF->predict_prob(sample_data, cv::Mat());
		// return quality value
		qualityValue = (int)((prob * 100) + 0.5);
#else
		// returns probability that between 0 and 1 that result belongs
		// to second class
		float prob = m_pTrainedRF->predict(
		    sample_data, cv::noArray(), cv::ml::StatModel::RAW_OUTPUT);
		// return quality value
		qualityValue = (int)(prob + 0.5);
#endif

	} catch (const cv::Exception &e) {
		throw NFIQException(e_Error_MachineLearningError, e.msg);
	}
}

const std::string NFIQ::Prediction::RandomForestML::moduleName {
	"NFIQ2_RandomForest"
};

std::string
NFIQ::Prediction::RandomForestML::getModuleName() const
{
	return moduleName;
}
