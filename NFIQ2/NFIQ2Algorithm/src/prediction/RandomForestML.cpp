#include <nfiq2/nfiqexception.hpp>
#include <nfiq2/prediction/RandomForestML.h>

#ifdef EMBED_RANDOMFOREST_PARAMETERS
#include <nfiq2/prediction/RandomForestTrainedParams.h>
#endif

#include <opencv2/core/version.hpp>

#if defined WINDOWS || defined WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <float.h>
#include <math.h>
#include <time.h>

#include "digestpp.hpp"
#include <numeric> // std::accumulate

using namespace NFIQ;
using namespace cv;

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
	FileStorage fs(params.c_str(),
	    FileStorage::READ | FileStorage::MEMORY | FileStorage::FORMAT_YAML);
	// now import data structures
#if CV_MAJOR_VERSION <= 2
	m_pTrainedRF = new CvRTrees();
	m_pTrainedRF->read(
	    fs.fs, cvGetFileNodeByName(fs.fs, NULL, "my_random_trees"));
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
    const std::vector<NFIQ::QualityFeatureData> &featureVector,
    const double &utilityValue, double &qualityValue, double &deviation) const
{
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
		Mat sample_data = Mat(1, featureVector.size(), CV_32FC1);
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
		float prob = m_pTrainedRF->predict_prob(sample_data, Mat());
		// return quality value
		qualityValue = (int)((prob * 100) + 0.5);
#else
		// returns probability that between 0 and 1 that result belongs
		// to second class
		float prob = m_pTrainedRF->predict(
		    sample_data, noArray(), cv::ml::StatModel::RAW_OUTPUT);
		// return quality value
		qualityValue = (int)(prob + 0.5);
#endif

	} catch (const cv::Exception &e) {
		throw NFIQException(e_Error_MachineLearningError, e.msg);
	}
}

std::string
NFIQ::Prediction::RandomForestML::getModuleID()
{
	return "NFIQ2_RandomForest";
}
