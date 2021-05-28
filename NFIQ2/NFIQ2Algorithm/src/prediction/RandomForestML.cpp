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

const char NFIQ2::Identifiers::Prediction::Modules::RandomForest[] {
	"NFIQ2_RandomForest"
};
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
    const std::unordered_map<std::string, double> &features,
    double &qualityValue) const
{
	/**
	   The following ordering of feature keys is critical to the
	   correct computation of NFIQ 2 scores. Any modification to this
	   ordering will result in incorrectly generated NFIQ 2 scores. This is
	   based on the training model currently in use and may be updated in
	   the future.
	*/
	static const std::vector<std::string> rfFeatureOrder {
		Identifiers::QualityFeatures::Features::
		    FrequencyDomainAnalysis::Histogram::Bin0,
		Identifiers::QualityFeatures::Features::
		    FrequencyDomainAnalysis::Histogram::Bin1,
		Identifiers::QualityFeatures::Features::
		    FrequencyDomainAnalysis::Histogram::Bin2,
		Identifiers::QualityFeatures::Features::
		    FrequencyDomainAnalysis::Histogram::Bin3,
		Identifiers::QualityFeatures::Features::
		    FrequencyDomainAnalysis::Histogram::Bin4,
		Identifiers::QualityFeatures::Features::
		    FrequencyDomainAnalysis::Histogram::Bin5,
		Identifiers::QualityFeatures::Features::
		    FrequencyDomainAnalysis::Histogram::Bin6,
		Identifiers::QualityFeatures::Features::
		    FrequencyDomainAnalysis::Histogram::Bin7,
		Identifiers::QualityFeatures::Features::
		    FrequencyDomainAnalysis::Histogram::Bin8,
		Identifiers::QualityFeatures::Features::
		    FrequencyDomainAnalysis::Histogram::Bin9,
		Identifiers::QualityFeatures::Features::
		    FrequencyDomainAnalysis::Mean,
		Identifiers::QualityFeatures::Features::
		    FrequencyDomainAnalysis::StdDev,

		Identifiers::QualityFeatures::Features::Minutiae::CountCOM,
		Identifiers::QualityFeatures::Features::Minutiae::Count,
		Identifiers::QualityFeatures::Features::Minutiae::QualityMu2,
		Identifiers::QualityFeatures::Features::Minutiae::QualityOCL80,

		Identifiers::QualityFeatures::Features::RegionOfInterest::Mean,

		Identifiers::QualityFeatures::Features::LocalClarity::
		    Histogram::Bin0,
		Identifiers::QualityFeatures::Features::LocalClarity::
		    Histogram::Bin1,
		Identifiers::QualityFeatures::Features::LocalClarity::
		    Histogram::Bin2,
		Identifiers::QualityFeatures::Features::LocalClarity::
		    Histogram::Bin3,
		Identifiers::QualityFeatures::Features::LocalClarity::
		    Histogram::Bin4,
		Identifiers::QualityFeatures::Features::LocalClarity::
		    Histogram::Bin5,
		Identifiers::QualityFeatures::Features::LocalClarity::
		    Histogram::Bin6,
		Identifiers::QualityFeatures::Features::LocalClarity::
		    Histogram::Bin7,
		Identifiers::QualityFeatures::Features::LocalClarity::
		    Histogram::Bin8,
		Identifiers::QualityFeatures::Features::LocalClarity::
		    Histogram::Bin9,
		Identifiers::QualityFeatures::Features::LocalClarity::Mean,
		Identifiers::QualityFeatures::Features::LocalClarity::StdDev,

		Identifiers::QualityFeatures::Features::Contrast::MeanBlock,
		Identifiers::QualityFeatures::Features::Contrast::Mean,

		Identifiers::QualityFeatures::Features::OrientationCertainty::
		    Histogram::Bin0,
		Identifiers::QualityFeatures::Features::OrientationCertainty::
		    Histogram::Bin1,
		Identifiers::QualityFeatures::Features::OrientationCertainty::
		    Histogram::Bin2,
		Identifiers::QualityFeatures::Features::OrientationCertainty::
		    Histogram::Bin3,
		Identifiers::QualityFeatures::Features::OrientationCertainty::
		    Histogram::Bin4,
		Identifiers::QualityFeatures::Features::OrientationCertainty::
		    Histogram::Bin5,
		Identifiers::QualityFeatures::Features::OrientationCertainty::
		    Histogram::Bin6,
		Identifiers::QualityFeatures::Features::OrientationCertainty::
		    Histogram::Bin7,
		Identifiers::QualityFeatures::Features::OrientationCertainty::
		    Histogram::Bin8,
		Identifiers::QualityFeatures::Features::OrientationCertainty::
		    Histogram::Bin9,
		Identifiers::QualityFeatures::Features::OrientationCertainty::
		    Mean,
		Identifiers::QualityFeatures::Features::OrientationCertainty::
		    StdDev,

		Identifiers::QualityFeatures::Features::OrientationFlow::
		    Histogram::Bin0,
		Identifiers::QualityFeatures::Features::OrientationFlow::
		    Histogram::Bin1,
		Identifiers::QualityFeatures::Features::OrientationFlow::
		    Histogram::Bin2,
		Identifiers::QualityFeatures::Features::OrientationFlow::
		    Histogram::Bin3,
		Identifiers::QualityFeatures::Features::OrientationFlow::
		    Histogram::Bin4,
		Identifiers::QualityFeatures::Features::OrientationFlow::
		    Histogram::Bin5,
		Identifiers::QualityFeatures::Features::OrientationFlow::
		    Histogram::Bin6,
		Identifiers::QualityFeatures::Features::OrientationFlow::
		    Histogram::Bin7,
		Identifiers::QualityFeatures::Features::OrientationFlow::
		    Histogram::Bin8,
		Identifiers::QualityFeatures::Features::OrientationFlow::
		    Histogram::Bin9,
		Identifiers::QualityFeatures::Features::OrientationFlow::Mean,
		Identifiers::QualityFeatures::Features::OrientationFlow::StdDev,

		Identifiers::QualityFeatures::Features::RegionOfInterest::
		    CoherenceMean,
		Identifiers::QualityFeatures::Features::RegionOfInterest::
		    CoherenceSum,

		Identifiers::QualityFeatures::Features::RidgeValleyUniformity::
		    Histogram::Bin0,
		Identifiers::QualityFeatures::Features::RidgeValleyUniformity::
		    Histogram::Bin1,
		Identifiers::QualityFeatures::Features::RidgeValleyUniformity::
		    Histogram::Bin2,
		Identifiers::QualityFeatures::Features::RidgeValleyUniformity::
		    Histogram::Bin3,
		Identifiers::QualityFeatures::Features::RidgeValleyUniformity::
		    Histogram::Bin4,
		Identifiers::QualityFeatures::Features::RidgeValleyUniformity::
		    Histogram::Bin5,
		Identifiers::QualityFeatures::Features::RidgeValleyUniformity::
		    Histogram::Bin6,
		Identifiers::QualityFeatures::Features::RidgeValleyUniformity::
		    Histogram::Bin7,
		Identifiers::QualityFeatures::Features::RidgeValleyUniformity::
		    Histogram::Bin8,
		Identifiers::QualityFeatures::Features::RidgeValleyUniformity::
		    Histogram::Bin9,
		Identifiers::QualityFeatures::Features::RidgeValleyUniformity::
		    Mean,
		Identifiers::QualityFeatures::Features::RidgeValleyUniformity::
		    StdDev
	};

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
		    1, rfFeatureOrder.size(), CV_32FC1);

		for (unsigned int i { 0 }; i < rfFeatureOrder.size(); ++i) {
			sample_data.at<float>(0, i) = features.at(
			    rfFeatureOrder[i]);
		}

		// returns probability that between 0 and 1 that result belongs
		// to second class
		float prob = m_pTrainedRF->predict(
		    sample_data, cv::noArray(), cv::ml::StatModel::RAW_OUTPUT);
		// return quality value
		qualityValue = (int)(prob + 0.5);

	} catch (const cv::Exception &e) {
		throw Exception(NFIQ2::ErrorCode::MachineLearningError, e.msg);
	} catch (const std::out_of_range &e) {
		throw Exception(
		    NFIQ2::ErrorCode::FeatureCalculationError, e.what());
	}
}

std::string
NFIQ2::Prediction::RandomForestML::getModuleName() const
{
	return Identifiers::Prediction::Modules::RandomForest;
}
