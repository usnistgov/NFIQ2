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
#include <nfiq2/nfiq2features.hpp>
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
