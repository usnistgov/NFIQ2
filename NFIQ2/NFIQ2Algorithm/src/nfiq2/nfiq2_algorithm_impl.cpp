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

#include "nfiq2_algorithm_impl.hpp"
#include <iomanip>
#include <string>
#include <vector>

#if defined(__linux) && defined(__i386__)
void
set_fpu(unsigned int mode)
{
	asm("fldcw %0" : : "m"(*&mode));
}
#endif

NFIQ2::Algorithm::Impl::Impl()
    : initialized { false }
{
#if defined(__linux) && defined(__i386__)
	set_fpu(0x27F); /* use double-precision rounding */
#endif

#ifdef EMBED_RANDOMFOREST_PARAMETERS
	// init RF module that takes some time to load the parameters
	this->m_parameterHash = m_RandomForestML.initModule();
	this->initialized = true;
#endif
}

NFIQ2::Algorithm::Impl::Impl(
    const std::string &fileName, const std::string &fileHash)
    : Impl()
{
	// init RF module that takes some time to load the parameters
	try {
		this->m_parameterHash = m_RandomForestML.initModule(
		    fileName, fileHash);
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
    const std::unordered_map<std::string, NFIQ2::QualityFeatureData> &features)
    const
{
	this->throwIfUninitialized();

	const double utility {};
	double deviation {};
	double quality {};
	m_RandomForestML.evaluate(features, utility, quality, deviation);

	return quality;
}

unsigned int
NFIQ2::Algorithm::Impl::computeQualityScore(
    const std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
	&features) const
{
	this->throwIfUninitialized();

	const std::unordered_map<std::string, NFIQ2::QualityFeatureData>
	    quality = NFIQ2::QualityFeatures::getQualityFeatureData(features);

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
NFIQ2::Algorithm::Impl::computeQualityScore(
    const NFIQ2::FingerprintImageData &rawImage) const
{
	this->throwIfUninitialized();

	// --------------------------------------------------------
	// compute quality features (including actionable feedback)
	// --------------------------------------------------------

	std::vector<std::shared_ptr<NFIQ2::QualityFeatures::BaseFeature>>
	    features {};
	try {
		features = NFIQ2::QualityFeatures::computeQualityFeatures(
		    rawImage);
	} catch (const NFIQ2::Exception &) {
		throw;
	} catch (const std::exception &e) {
		/*
		 * Nothing should get here, but computeQualityFeatures() calls
		 * a lot of code...
		 */
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::UnknownError, e.what());
	}

	const std::unordered_map<std::string, NFIQ2::QualityFeatureData>
	    quality = NFIQ2::QualityFeatures::getQualityFeatureData(features);

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
NFIQ2::Algorithm::Impl::computeQualityScore(
    const std::unordered_map<std::string, NFIQ2::QualityFeatureData> &features)
    const
{
	this->throwIfUninitialized();

	return (unsigned int)getQualityPrediction(features);
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
	return std::stoul(
	    std::string(NFIQ2_EMBEDDED_RANDOM_FOREST_PARAMETERS_FCT));
#else
	throw NFIQ2::Exception { NFIQ2::ErrorCode::NoDataAvailable,
		"Random forest parameters did not specify FCT" };
#endif
}
