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
#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_nfiqexception.hpp>
#include <nfiq2_qualityfeatures.hpp>
#include <nfiq2_timer.hpp>
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
    const std::unordered_map<std::string, NFIQ::QualityFeatureData> &featureMap) const
{
	const double utility {};
	double deviation {};
	double quality {};
	m_RandomForestML.evaluate(featureMap, utility, quality, deviation);

	return quality;
}

unsigned int
NFIQ2Algorithm::Impl::computeQualityScore(
    const std::vector<std::shared_ptr<NFIQ::QualityFeatures::BaseFeature>>
	&features) const
{

	const std::unordered_map<std::string, NFIQ::QualityFeatureData> featureMap =
	    NFIQ::QualityFeatures::getQualityFeatureData(features);

	if (featureMap.size() == 0) {
		// no features have been computed
		throw NFIQ::NFIQException(e_Error_FeatureCalculationError,
		    "No features have been computed");
	}

	// ---------------------
	// compute quality score
	// ---------------------

	double qualityScore {};
	try {
		qualityScore = getQualityPrediction(featureMap);
	} catch (const NFIQ::NFIQException &) {
		throw;
	}

	return (unsigned int)qualityScore;
}

unsigned int
NFIQ2Algorithm::Impl::computeQualityScore(
    const NFIQ::FingerprintImageData &rawImage) const
{

	// --------------------------------------------------------
	// compute quality features (including actionable feedback)
	// --------------------------------------------------------

	std::vector<std::shared_ptr<NFIQ::QualityFeatures::BaseFeature>>
	    features {};
	try {
		features = NFIQ::QualityFeatures::computeQualityFeatures(
		    rawImage);
	} catch (const NFIQ::NFIQException &) {
		throw;
	} catch (const std::exception &e) {
		/*
		 * Nothing should get here, but computeQualityFeatures() calls
		 * a lot of code...
		 */
		throw NFIQ::NFIQException(e_Error_UnknownError, e.what());
	}

	const std::unordered_map<std::string, NFIQ::QualityFeatureData> featureMap =
	    NFIQ::QualityFeatures::getQualityFeatureData(features);

	if (featureMap.size() == 0) {
		// no features have been computed
		throw NFIQ::NFIQException(e_Error_FeatureCalculationError,
		    "No features have been computed");
	}

	// ---------------------
	// compute quality score
	// ---------------------

	double qualityScore {};
	try {
		qualityScore = getQualityPrediction(featureMap);
	} catch (const NFIQ::NFIQException &) {
		throw;
	}

	return (unsigned int)qualityScore;
}

unsigned int 
NFIQ2Algorithm::Impl::computeQualityScore(
    const std::unordered_map<std::string, NFIQ::QualityFeatureData> &featureMap) const 
{
   	return (unsigned int)getQualityPrediction(featureMap);
}

std::string
NFIQ2Algorithm::Impl::getParameterHash() const
{
	return (this->m_parameterHash);
}
