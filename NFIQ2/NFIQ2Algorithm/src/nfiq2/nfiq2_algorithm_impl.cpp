#include <features/FDAFeature.h>
#include <features/FJFXMinutiaeQualityFeatures.h>
#include <features/FingerJetFXFeature.h>
#include <features/ImgProcROIFeature.h>
#include <features/LCSFeature.h>
#include <features/MuFeature.h>
#include <features/OCLHistogramFeature.h>
#include <features/OFFeature.h>
#include <features/QualityMapFeatures.h>
#include <features/RVUPHistogramFeature.h>
#include <nfiq2_exception.hpp>
#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_qualityfeatures.hpp>
#include <nfiq2_timer.hpp>
#include <string.h>

#include "nfiq2_algorithm_impl.hpp"
#include <iomanip>
#include <list>
#include <string>
#include <vector>

#ifdef EMBED_RANDOMFOREST_PARAMETERS
NFIQ2::NFIQ2Algorithm::Impl::Impl()
{
	// init RF module that takes some time to load the parameters
	this->m_parameterHash = m_RandomForestML.initModule();
}
#endif

NFIQ2::NFIQ2Algorithm::Impl::Impl(
    const std::string &fileName, const std::string &fileHash)
{
	// init RF module that takes some time to load the parameters
	try {
		this->m_parameterHash = m_RandomForestML.initModule(
		    fileName, fileHash);
	} catch (const cv::Exception &e) {
		throw NFIQException(NFIQ2::ErrorCode::BadArguments,
		    "Could not initialize random forest parameters with "
		    "external file. Most likely, the file does not exist. "
		    "Check the path (" +
			fileName + ") and hash (" + fileHash +
			") (initial error: " + e.msg + ").");
	} catch (const NFIQ2::NFIQException &e) {
		throw NFIQException(NFIQ2::ErrorCode::BadArguments,
		    "Could not initialize random forest parameters with "
		    "external file. Most likely, the hash is not correct. "
		    "Check the path (" +
			fileName + ") and hash (" + fileHash +
			") (initial error: " + e.what() + ").");
	}
}

NFIQ2::NFIQ2Algorithm::Impl::~Impl()
{
}

double
NFIQ2::NFIQ2Algorithm::Impl::getQualityPrediction(
    const std::unordered_map<std::string, NFIQ2::QualityFeatureData> &features)
    const
{
	const double utility {};
	double deviation {};
	double quality {};
	m_RandomForestML.evaluate(features, utility, quality, deviation);

	return quality;
}

unsigned int
NFIQ2::NFIQ2Algorithm::Impl::computeQualityScore(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
	&features) const
{

	const std::unordered_map<std::string, NFIQ2::QualityFeatureData>
	    quality = NFIQ2::QualityFeatures::getQualityFeatureData(features);

	if (quality.size() == 0) {
		// no features have been computed
		throw NFIQ2::NFIQException(
		    NFIQ2::ErrorCode::FeatureCalculationError,
		    "No features have been computed");
	}

	// ---------------------
	// compute quality score
	// ---------------------

	double qualityScore {};
	try {
		qualityScore = getQualityPrediction(quality);
	} catch (const NFIQ2::NFIQException &) {
		throw;
	}

	return (unsigned int)qualityScore;
}

unsigned int
NFIQ2::NFIQ2Algorithm::Impl::computeQualityScore(
    const NFIQ2::FingerprintImageData &rawImage) const
{

	// --------------------------------------------------------
	// compute quality features (including actionable feedback)
	// --------------------------------------------------------

	std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
	    features {};
	try {
		features = NFIQ2::QualityFeatures::computeQualityFeatures(
		    rawImage);
	} catch (const NFIQ2::NFIQException &) {
		throw;
	} catch (const std::exception &e) {
		/*
		 * Nothing should get here, but computeQualityFeatures() calls
		 * a lot of code...
		 */
		throw NFIQ2::NFIQException(
		    NFIQ2::ErrorCode::UnknownError, e.what());
	}

	const std::unordered_map<std::string, NFIQ2::QualityFeatureData>
	    quality = NFIQ2::QualityFeatures::getQualityFeatureData(features);

	if (quality.size() == 0) {
		// no features have been computed
		throw NFIQ2::NFIQException(
		    NFIQ2::ErrorCode::FeatureCalculationError,
		    "No features have been computed");
	}

	// ---------------------
	// compute quality score
	// ---------------------

	double qualityScore {};
	try {
		qualityScore = getQualityPrediction(quality);
	} catch (const NFIQ2::NFIQException &) {
		throw;
	}

	return (unsigned int)qualityScore;
}

unsigned int
NFIQ2::NFIQ2Algorithm::Impl::computeQualityScore(
    const std::unordered_map<std::string, NFIQ2::QualityFeatureData> &features)
    const
{
	return (unsigned int)getQualityPrediction(features);
}

std::string
NFIQ2::NFIQ2Algorithm::Impl::getParameterHash() const
{
	return (this->m_parameterHash);
}
