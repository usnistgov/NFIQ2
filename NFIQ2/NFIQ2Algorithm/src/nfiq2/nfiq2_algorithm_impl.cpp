#include <features/FDAFeature.h>
#include <features/FJFXMinutiaeQualityFeatures.h>
#include <features/FeatureFunctions.h>
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

#include "nfiq2_algorithm_impl.hpp"
#include <iomanip>
#include <string>
#include <vector>

NFIQ2::Algorithm::Impl::Impl()
    : initialized { false }
{
#ifdef NFIQ2_EMBED_RANDOM_FOREST_PARAMETERS
	// init RF module that takes some time to load the parameters
	this->m_parameterHash = m_RandomForestML.initModule();
	this->initialized = true;
#endif
}

NFIQ2::Algorithm::Impl::Impl(const std::string &fileName,
    const std::string &fileHash)
    : initialized { false }
{
#ifdef NFIQ2_EMBED_RANDOM_FOREST_PARAMETERS
	throw Exception { NFIQ2::ErrorCode::BadArguments,
		"Refusing to initialize random forest parameters with external "
		"file because the NFIQ 2 library was built with embedded "
		"random forest parameters." };
#endif

	// init RF module that takes some time to load the parameters
	try {
		this->m_parameterHash = m_RandomForestML.initModule(fileName,
		    fileHash);
		this->initialized = true;
	} catch (const cv::Exception &e) {
		throw Exception(NFIQ2::ErrorCode::BadArguments,
		    "Could not initialize random forest parameters with "
		    "external file. Most likely, the file does not exist. "
		    "Check the path (" +
			fileName + ") and hash (" + fileHash +
			") (initial error: " + e.msg + ").");
	} catch (const NFIQ2::Exception &e) {
		throw Exception(NFIQ2::ErrorCode::BadArguments,
		    "Could not initialize random forest parameters with "
		    "external file. Most likely, the hash is not correct. "
		    "Check the path (" +
			fileName + ") and hash (" + fileHash +
			") (initial error: " + e.what() + ").");
	}
}

NFIQ2::Algorithm::Impl::~Impl() = default;

double
NFIQ2::Algorithm::Impl::getQualityPrediction(
    const std::unordered_map<std::string, double> &features) const
{
	this->throwIfUninitialized();

	double quality {};
	m_RandomForestML.evaluate(features, quality);

	return quality;
}

unsigned int
NFIQ2::Algorithm::Impl::computeUnifiedQualityScore(
    const std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	&features) const
{
	this->throwIfUninitialized();

	const std::unordered_map<std::string, double> quality =
	    NFIQ2::QualityMeasures::getNativeQualityMeasures(features);

	if (quality.size() == 0) {
		// no features have been computed
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FeatureCalculationError,
		    "No features have been computed");
	}

	// ---------------------
	// compute quality score
	// ---------------------

	double qualityScore {};
	try {
		qualityScore = getQualityPrediction(quality);
	} catch (const NFIQ2::Exception &) {
		throw;
	}

	return (unsigned int)qualityScore;
}

unsigned int
NFIQ2::Algorithm::Impl::computeUnifiedQualityScore(
    const NFIQ2::FingerprintImageData &rawImage) const
{
	this->throwIfUninitialized();

	// --------------------------------------------------------
	// compute quality features (including actionable feedback)
	// --------------------------------------------------------

	std::vector<std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>>
	    modules {};
	try {
		modules = NFIQ2::QualityMeasures::computeNativeQualityMeasures(
		    rawImage);
	} catch (const NFIQ2::Exception &) {
		throw;
	} catch (const std::exception &e) {
		/*
		 * Nothing should get here, but computeNativeQualityMeasures()
		 * calls a lot of code...
		 */
		throw NFIQ2::Exception(NFIQ2::ErrorCode::UnknownError,
		    e.what());
	}

	const std::unordered_map<std::string, double> quality =
	    NFIQ2::QualityMeasures::getNativeQualityMeasures(modules);

	if (quality.size() == 0) {
		// no features have been computed
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FeatureCalculationError,
		    "No features have been computed");
	}

	// ---------------------
	// compute quality score
	// ---------------------

	double qualityScore {};
	try {
		qualityScore = getQualityPrediction(quality);
	} catch (const NFIQ2::Exception &) {
		throw;
	}

	return (unsigned int)qualityScore;
}

unsigned int
NFIQ2::Algorithm::Impl::computeUnifiedQualityScore(
    const std::unordered_map<std::string, double> &features) const
{
	this->throwIfUninitialized();

	return (unsigned int)getQualityPrediction(features);
}

std::unordered_map<std::string, unsigned int>
NFIQ2::Algorithm::Impl::getQualityBlockValues(
    const std::unordered_map<std::string, double> &nativeQualityMeasureValues)
{
	std::unordered_map<std::string, unsigned int> ret {};
	for (const auto &it : nativeQualityMeasureValues) {
		ret[it.first] = getQualityBlockValue(it.first, it.second);
	}

	return (ret);
}

unsigned int
NFIQ2::Algorithm::Impl::getQualityBlockValue(
    const std::string &featureIdentifier,
    const double nativeQualityMeasureValue)
{
	return (QualityMeasures::getQualityBlockValue(featureIdentifier,
	    nativeQualityMeasureValue));
}

std::string
NFIQ2::Algorithm::Impl::getParameterHash() const
{
	this->throwIfUninitialized();

	return (this->m_parameterHash);
}

void
NFIQ2::Algorithm::Impl::throwIfUninitialized() const
{
	if (!this->isInitialized())
		throw NFIQ2::Exception { NFIQ2::ErrorCode::MachineLearningError,
			"Random forest parameters were not loaded" };
}

bool
NFIQ2::Algorithm::Impl::isInitialized() const
{
	return (this->initialized);
}

bool
NFIQ2::Algorithm::Impl::isEmbedded() const
{
#ifdef NFIQ2_EMBED_RANDOM_FOREST_PARAMETERS
	return (true);
#else
	return (false);
#endif
}

unsigned int
NFIQ2::Algorithm::Impl::getEmbeddedFCT() const
{
	if (!this->isEmbedded())
		throw NFIQ2::Exception { NFIQ2::ErrorCode::NoDataAvailable,
			"Random forest parameters were not embedded" };

#ifdef NFIQ2_EMBEDDED_RANDOM_FOREST_PARAMETERS_FCT
	return (NFIQ2_EMBEDDED_RANDOM_FOREST_PARAMETERS_FCT);
#else
	throw NFIQ2::Exception { NFIQ2::ErrorCode::NoDataAvailable,
		"Random forest parameters did not specify FCT" };
#endif
}
