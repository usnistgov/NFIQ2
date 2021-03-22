#include <nfiq2/features/FDAFeature.h>
#include <nfiq2/features/FJFXMinutiaeQualityFeatures.h>
#include <nfiq2/features/FingerJetFXFeature.h>
#include <nfiq2/features/ImgProcROIFeature.h>
#include <nfiq2/features/LCSFeature.h>
#include <nfiq2/features/MuFeature.h>
#include <nfiq2/features/OCLHistogramFeature.h>
#include <nfiq2/features/OFFeature.h>
#include <nfiq2/features/QualityMapFeatures.h>
#include <nfiq2/features/RVUPHistogramFeature.h>
#include <nfiq2/fingerprintimagedata.hpp>
#include <nfiq2/nfiq2_qualityfeatures.hpp>
#include <nfiq2/nfiqexception.hpp>
#include <nfiq2/timer.hpp>
#include <string.h>

#include "nfiq2impl.h"

#include <iomanip>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#if defined(__linux) && defined(__i386__)
void
set_fpu(unsigned int mode)
{
	asm("fldcw %0" : : "m"(*&mode));
}
#endif

using namespace NFIQ;
using namespace std;
using namespace cv;

#ifdef EMBED_RANDOMFOREST_PARAMETERS
NFIQ2Algorithm::Impl::Impl()
{
#if defined(__linux) && defined(__i386__)
	set_fpu(0x27F); /* use double-precision rounding */
#endif
	// init RF module that takes some time to load the parameters
	this->m_parameterHash = m_RandomForestML.initModule();
}
#endif

NFIQ2Algorithm::Impl::Impl(
    const std::string &fileName, const std::string &fileHash)
{
#if defined(__linux) && defined(__i386__)
	set_fpu(0x27F); /* use double-precision rounding */
#endif
	// init RF module that takes some time to load the parameters
	try {
		this->m_parameterHash = m_RandomForestML.initModule(
		    fileName, fileHash);
	} catch (const cv::Exception &e) {
		throw NFIQException(e_Error_BadArguments,
		    "Could not initialize random forest parameters with "
		    "external file. Most likely, the file does not exist. "
		    "Check the path (" +
			fileName + ") and hash (" + fileHash +
			") (initial error: " + e.msg + ").");
	} catch (const NFIQ::NFIQException &e) {
		throw NFIQException(e_Error_BadArguments,
		    "Could not initialize random forest parameters with "
		    "external file. Most likely, the hash is not correct. "
		    "Check the path (" +
			fileName + ") and hash (" + fileHash +
			") (initial error: " + e.what() + ").");
	}
}

NFIQ2Algorithm::Impl::~Impl()
{
}

double
NFIQ2Algorithm::Impl::getQualityPrediction(
    std::list<NFIQ::QualityFeatureData> &featureVector) const
{
	const double utility {};
	double deviation {};
	double quality {};
	m_RandomForestML.evaluate(featureVector, utility, quality, deviation);

	return quality;
}

unsigned int
NFIQ2Algorithm::Impl::computeQualityScore(NFIQ::FingerprintImageData rawImage,
    bool bComputeActionableQuality,
    std::list<NFIQ::ActionableQualityFeedback> &actionableQuality,
    bool bOutputFeatures,
    std::list<NFIQ::QualityFeatureData> &qualityFeatureData, bool bOutputSpeed,
    std::list<NFIQ::QualityFeatureSpeed> &qualityFeatureSpeed) const
{
	// crop image (white line removal) and use it for feature
	// computation
	NFIQ::FingerprintImageData croppedRawImage {};
	try {
		croppedRawImage = rawImage.removeWhiteFrameAroundFingerprint();
	} catch (const NFIQ::NFIQException &) {
		throw;
	}

	// --------------------------------------------------------
	// compute quality features (including actionable feedback)
	// --------------------------------------------------------

	std::list<NFIQ::QualityFeatureData> featureVector {};
	try {
		featureVector = NFIQ::QualityFeatures::computeQualityFeatures(
		    croppedRawImage, bComputeActionableQuality,
		    actionableQuality, bOutputSpeed, qualityFeatureSpeed);
	} catch (const NFIQ::NFIQException &) {
		throw;
	} catch (const std::exception &e) {
		/*
		 * Nothing should get here, but computeQualityFeatures() calls
		 * a lot of code...
		 */
		throw NFIQ::NFIQException(e_Error_UnknownError, e.what());
	}

	if (featureVector.size() == 0) {
		// no features have been computed
		throw NFIQ::NFIQException(e_Error_FeatureCalculationError,
		    "No features have been computed");
	}

	// ---------------------
	// compute quality score
	// ---------------------

	double qualityScore {};
	try {
		qualityScore = getQualityPrediction(featureVector);
	} catch (const NFIQ::NFIQException &) {
		throw;
	}

	// return feature vector if requested
	if (bOutputFeatures) {
		qualityFeatureData = featureVector;
	}

	return (unsigned int)qualityScore;
}

std::string
NFIQ2Algorithm::Impl::getParameterHash() const
{
	return (this->m_parameterHash);
}

unsigned int
NFIQ::NFIQ2Algorithm::Impl::computeQualityScore(
    NFIQ::FingerprintImageData rawImage) const
{

	std::list<NFIQ::ActionableQualityFeedback> actionableQuality {};
	std::list<NFIQ::QualityFeatureData> qualityFeatureData {};
	std::list<NFIQ::QualityFeatureSpeed> qualityFeatureSpeed {};

	return computeQualityScore(rawImage, true, actionableQuality, true,
	    qualityFeatureData, true, qualityFeatureSpeed);
}

NFIQ::NFIQ2Results
NFIQ::NFIQ2Algorithm::Impl::computeQualityFeaturesAndScore(
    NFIQ::FingerprintImageData rawImage) const
{
	std::list<NFIQ::ActionableQualityFeedback> actionableQuality {};
	std::list<NFIQ::QualityFeatureData> qualityfeatureData {};
	std::list<NFIQ::QualityFeatureSpeed> qualityFeatureSpeed {};

	// crop image (white line removal) and use it for feature
	// computation
	NFIQ::FingerprintImageData croppedRawImage {};
	try {
		croppedRawImage = rawImage.removeWhiteFrameAroundFingerprint();
	} catch (const NFIQ::NFIQException &) {
		throw;
	}

	// --------------------------------------------------------
	// compute quality features (including actionable feedback)
	// --------------------------------------------------------

	try {
		qualityfeatureData =
		    NFIQ::QualityFeatures::computeQualityFeatures(
			croppedRawImage, true, actionableQuality, true,
			qualityFeatureSpeed);
	} catch (const NFIQ::NFIQException &) {
		throw;
	} catch (const std::exception &e) {
		/*
		 * Nothing should get here, but computeQualityFeatures() calls
		 * a lot of code...
		 */
		throw NFIQ::NFIQException(e_Error_UnknownError, e.what());
	}

	if (qualityfeatureData.size() == 0) {
		// no features have been computed
		throw NFIQ::NFIQException(e_Error_FeatureCalculationError,
		    "No features have been computed");
	}

	// ---------------------
	// compute quality score
	// ---------------------

	double qualityScore {};
	try {
		qualityScore = getQualityPrediction(qualityfeatureData);
	} catch (const NFIQ::NFIQException &) {
		throw;
	}

	std::vector<NFIQ::ActionableQualityFeedback> actionableQualityVector {};
	actionableQualityVector.reserve(actionableQuality.size());
	std::copy(actionableQuality.begin(), actionableQuality.end(),
	    actionableQualityVector.begin());

	std::vector<NFIQ::QualityFeatureData> qualityfeatureDataVector {};
	qualityfeatureDataVector.reserve(qualityfeatureData.size());
	std::copy(qualityfeatureData.begin(), qualityfeatureData.end(),
	    qualityfeatureDataVector.begin());

	std::vector<NFIQ::QualityFeatureSpeed> qualityFeatureSpeedVector {};
	qualityFeatureSpeedVector.reserve(qualityFeatureSpeed.size());
	std::copy(qualityFeatureSpeed.begin(), qualityFeatureSpeed.end(),
	    qualityFeatureSpeedVector.begin());

	return NFIQ::NFIQ2Results(actionableQualityVector,
	    qualityfeatureDataVector, qualityFeatureSpeedVector, qualityScore);
}

NFIQ::NFIQ2Results::Impl::Impl()
{
}

NFIQ::NFIQ2Results::Impl::Impl(
    std::vector<NFIQ::ActionableQualityFeedback> actionableQuality,
    std::vector<NFIQ::QualityFeatureData> qualityfeatureData,
    std::vector<NFIQ::QualityFeatureSpeed> qualityFeatureSpeed,
    unsigned int qualityScore)
    : actionableQuality_ { actionableQuality }
    , qualityfeatureData_ { qualityfeatureData }
    , qualityFeatureSpeed_ { qualityFeatureSpeed }
{
	if (qualityScore < 1 || qualityScore > 100) {
		const std::string errStr { "Invalid quality score: " +
			to_string(qualityScore) +
			". Valid scores are between 1 and 100" };
		throw NFIQ::NFIQException(
		    NFIQ::e_Error_InvalidNFIQ2Score, errStr);
	}
	this->qualityScore_ = qualityScore;
}

void
NFIQ::NFIQ2Results::Impl::setActionable(
    std::vector<NFIQ::ActionableQualityFeedback> actionableQuality)
{
	this->actionableQuality_ = actionableQuality;
}
void
NFIQ::NFIQ2Results::Impl::setQuality(
    std::vector<NFIQ::QualityFeatureData> qualityfeatureData)
{
	this->qualityfeatureData_ = qualityfeatureData;
}
void
NFIQ::NFIQ2Results::Impl::setSpeed(
    std::vector<NFIQ::QualityFeatureSpeed> qualityFeatureSpeed)
{
	this->qualityFeatureSpeed_ = qualityFeatureSpeed;
}

void
NFIQ::NFIQ2Results::Impl::setScore(unsigned int qualityScore)
{
	if (qualityScore < 1 || qualityScore > 100) {
		const std::string errStr { "Invalid quality score: " +
			to_string(qualityScore) +
			". Valid scores are between 1 and 100" };
		throw NFIQ::NFIQException(
		    NFIQ::e_Error_InvalidNFIQ2Score, errStr);
	}
	this->qualityScore_ = qualityScore;
}

std::vector<NFIQ::ActionableQualityFeedback>
NFIQ::NFIQ2Results::Impl::getActionable() const
{
	return this->actionableQuality_;
}
std::vector<NFIQ::QualityFeatureData>
NFIQ::NFIQ2Results::Impl::getQuality() const
{
	return this->qualityfeatureData_;
}
std::vector<NFIQ::QualityFeatureSpeed>
NFIQ::NFIQ2Results::Impl::getSpeed() const
{
	return this->qualityFeatureSpeed_;
}

unsigned int
NFIQ::NFIQ2Results::Impl::getScore() const
{
	return this->qualityScore_;
}
