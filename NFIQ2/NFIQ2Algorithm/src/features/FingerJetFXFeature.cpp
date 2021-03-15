#include <nfiq2/features/FingerJetFXFeature.h>
#include <nfiq2/nfiqexception.hpp>
#include <nfiq2/timer.hpp>
#include <string.h>

#include <sstream>

using namespace NFIQ;

FingerJetFXFeature::~FingerJetFXFeature()
{
}

const std::string FingerJetFXFeature::speedFeatureIDGroup = "Minutiae";

std::pair<unsigned int, unsigned int>
FingerJetFXFeature::centerOfMinutiaeMass(
    const std::vector<FingerJetFXFeature::Minutia> &minutiaData)
{
	unsigned int lx { 0 }, ly { 0 };
	for (const auto &m : minutiaData) {
		lx += m.x;
		ly += m.y;
	}
	return std::make_pair<unsigned int, unsigned int>(
	    lx / minutiaData.size(), ly / minutiaData.size());
}

std::string
FingerJetFXFeature::parseFRFXLLError(const FRFXLL_RESULT fxRes)
{
	switch (fxRes) {
	case FRFXLL_ERR_FB_TOO_SMALL_AREA:
		return "FRFXLL_ERR_FB_TOO_SMALL_AREA: Fingerprint area is too "
		       "small. Most likely this is bacause the tip of the "
		       "finger is presented.";
	case FRFXLL_ERR_INVALID_PARAM:
		return "FRFXLL_ERR_INVALID_PARAM: One or more of the "
		       "parameters is invalid.";
	case FRFXLL_ERR_NO_MEMORY:
		return "FRFXLL_ERR_NO_MEMORY: There is not enough memory to "
		       "perform the function.";
	case FRFXLL_ERR_MORE_DATA:
		return "FRFXLL_ERR_MORE_DATA: More data is available.";
	case FRFXLL_ERR_INTERNAL:
		return "FRFXLL_ERR_INTERNAL: An unknown internal error has "
		       "occurred.";
	case FRFXLL_ERR_INVALID_BUFFER:
		return "FRFXLL_ERR_INVALID_BUFFER: The image buffer is too "
		       "small for in-place processing.";
	case FRFXLL_ERR_INVALID_HANDLE:
		return "FRFXLL_ERR_INVALID_HANDLE: The specified handle is "
		       "invalid.";
	case FRFXLL_ERR_INVALID_IMAGE:
		return "FRFXLL_ERR_INVALID_IMAGE: The image buffer is invalid.";
	case FRFXLL_ERR_INVALID_DATA:
		return "FRFXLL_ERR_INVALID_DATA: Supplied data is invalid.";
	case FRFXLL_ERR_NO_FP:
		return "FRFXLL_ERR_NO_FP: The specified finger or view is not "
		       "present.";
	default:
		return "Unknown FRFXLL Error";
	}
}

std::list<NFIQ::QualityFeatureResult>
FingerJetFXFeature::computeFeatureData(
    const NFIQ::FingerprintImageData fingerprintImage,
    std::vector<FingerJetFXFeature::Minutia> &minutiaData,
    bool &templateCouldBeExtracted)
{
	templateCouldBeExtracted = false;

	std::list<NFIQ::QualityFeatureResult> featureDataList;

	// make local copy of fingerprint image
	// since FJFX somehow transforms the input image
	NFIQ::FingerprintImageData localFingerprintImage(
	    fingerprintImage.m_ImageWidth, fingerprintImage.m_ImageHeight,
	    fingerprintImage.m_FingerCode, fingerprintImage.m_ImageDPI);
	// copy data now
	localFingerprintImage.resize(fingerprintImage.size());
	memcpy((void *)localFingerprintImage.data(), fingerprintImage.data(),
	    fingerprintImage.size());

	NFIQ::QualityFeatureData fd_min_cnt;
	fd_min_cnt.featureID = "FingerJetFX_MinutiaeCount";
	fd_min_cnt.featureDataType = NFIQ::e_QualityFeatureDataTypeDouble;
	fd_min_cnt.featureDataDouble = 0;
	NFIQ::QualityFeatureResult res_min_cnt;
	res_min_cnt.featureData = fd_min_cnt;
	res_min_cnt.returnCode = 0;

	NFIQ::QualityFeatureData fd_min_cnt_comrect200x200;
	fd_min_cnt_comrect200x200.featureID =
	    "FingerJetFX_MinCount_COMMinRect200x200";
	fd_min_cnt_comrect200x200.featureDataType =
	    NFIQ::e_QualityFeatureDataTypeDouble;
	fd_min_cnt_comrect200x200.featureDataDouble = 0;
	NFIQ::QualityFeatureResult res_min_cnt_comrect200x200;
	res_min_cnt_comrect200x200.featureData = fd_min_cnt_comrect200x200;
	res_min_cnt_comrect200x200.returnCode = 0;

	NFIQ::Timer timer;
	timer.startTimer();

	// create context for feature extraction
	// the created context function is modified to override default settings
	FRFXLL_HANDLE hCtx = NULL, hFeatureSet = NULL;
	if (!FRFXLL_SUCCESS(createContext(&hCtx))) {
		throw NFIQ::NFIQException(
		    NFIQ::
			e_Error_FeatureCalculationError_FJFX_CannotCreateContext,
		    "Cannot create context of feature extraction (create "
		    "context failed).");
	}
	if (hCtx == NULL) {
		throw NFIQ::NFIQException(
		    NFIQ::
			e_Error_FeatureCalculationError_FJFX_CannotCreateContext,
		    "Cannot create context of feature extraction (hCtx is "
		    "NULL).");
	}

	// extract feature set
	const FRFXLL_RESULT fxRes = FRFXLLCreateFeatureSetFromRaw(hCtx,
	    (unsigned char *)localFingerprintImage.data(),
	    localFingerprintImage.size(), localFingerprintImage.m_ImageWidth,
	    localFingerprintImage.m_ImageHeight,
	    localFingerprintImage.m_ImageDPI, FRFXLL_FEX_ENABLE_ENHANCEMENT,
	    &hFeatureSet);
	if (!FRFXLL_SUCCESS(fxRes)) {
		FRFXLLCloseHandle(&hCtx);
		throw NFIQ::NFIQException(
		    NFIQ::
			e_Error_FeatureCalculationError_FJFX_CannotCreateFeatureSet,
		    "Could not create feature set from raw data: " +
			FingerJetFXFeature::parseFRFXLLError(fxRes));
	}

	// close handle
	FRFXLLCloseHandle(&hCtx);
	if (hFeatureSet == NULL) {
		throw NFIQ::NFIQException(
		    NFIQ::
			e_Error_FeatureCalculationError_FJFX_CannotCreateFeatureSet,
		    "Feature set creation failed. Feature set is null.");
	}

	unsigned int minCnt { 0 };
	const FRFXLL_RESULT fxResMin = FRFXLLGetMinutiaInfo(
	    hFeatureSet, &minCnt, nullptr);
	if (!FRFXLL_SUCCESS(fxResMin)) {
		FRFXLLCloseHandle(&hFeatureSet);
		throw NFIQ::NFIQException(
		    NFIQ::
			e_Error_FeatureCalculationError_FJFX_NoFeatureSetCreated,
		    "Failed to obtain Minutia Info from feature set: " +
			FingerJetFXFeature::parseFRFXLLError(fxResMin));
	}

	std::unique_ptr<FRFXLL_Basic_19794_2_Minutia[]> mdata {};
	try {
		mdata.reset(new FRFXLL_Basic_19794_2_Minutia[minCnt]);
	} catch (const std::bad_alloc &) {
		FRFXLLCloseHandle(&hFeatureSet);
		throw NFIQ::NFIQException(NFIQ::e_Error_NotEnoughMemory,
		    "Could not allocate space for extracted minutiae records.");
	}

	const FRFXLL_RESULT fxResData = FRFXLLGetMinutiae(
	    hFeatureSet, BASIC_19794_2_MINUTIA_STRUCT, &minCnt, mdata.get());
	if (!FRFXLL_SUCCESS(fxResData)) {
		FRFXLLCloseHandle(&hFeatureSet);
		throw NFIQ::NFIQException(
		    NFIQ::
			e_Error_FeatureCalculationError_FJFX_NoFeatureSetCreated,
		    "Failed to parse Minutia Data into 19794 Minutia Struct: " +
			FingerJetFXFeature::parseFRFXLLError(fxResData));
	}

	minutiaData.clear();
	minutiaData.reserve(minCnt);
	for (int i = 0; i < minCnt; i++) {
		minutiaData.emplace_back(static_cast<unsigned int>(mdata[i].x),
		    static_cast<unsigned int>(mdata[i].y),
		    static_cast<unsigned int>(mdata[i].a),
		    static_cast<unsigned int>(mdata[i].q),
		    static_cast<unsigned int>(static_cast<
			std::underlying_type<FRXLL_MINUTIA_TYPE>::type>(
			mdata[i].t)));
	}

	// close handle
	FRFXLLCloseHandle(&hFeatureSet);

	templateCouldBeExtracted = true;

	if (minCnt == 0) {
		// return features
		fd_min_cnt_comrect200x200.featureDataDouble =
		    0; // no minutiae found
		res_min_cnt_comrect200x200.featureData =
		    fd_min_cnt_comrect200x200;
		res_min_cnt_comrect200x200.returnCode = 0;
		featureDataList.push_back(res_min_cnt_comrect200x200);

		fd_min_cnt.featureDataDouble = 0; // no minutiae found
		res_min_cnt.returnCode = 0;
		res_min_cnt.featureData = fd_min_cnt;
		featureDataList.push_back(res_min_cnt);

		if (m_bOutputSpeed) {
			NFIQ::QualityFeatureSpeed speed;
			speed.featureIDGroup =
			    FingerJetFXFeature::speedFeatureIDGroup;
			speed.featureIDs.push_back("FingerJetFX_MinutiaeCount");
			speed.featureIDs.push_back(
			    "FingerJetFX_MinCount_COMMinRect200x200");
			speed.featureSpeed = timer.endTimerAndGetElapsedTime();
			m_lSpeedValues.push_back(speed);
		}

		return featureDataList;
	}

	// compute ROI and return features
	std::vector<FingerJetFXFeature::Object> vecRectDimensions;
	FingerJetFXFeature::Object rect200x200;
	rect200x200.comType = e_COMType_MinutiaeLocation;
	rect200x200.width = 200;
	rect200x200.height = 200;
	vecRectDimensions.push_back(rect200x200);

	FingerJetFXFeature::FJFXROIResults roiResults = computeROI(
	    minutiaData, 32, fingerprintImage, vecRectDimensions);
	double noOfMinInRect200x200 = 0;
	for (unsigned int i = 0;
	     i < roiResults.vecNoOfMinutiaeInRectangular.size(); i++) {
		if (roiResults.vecNoOfMinutiaeInRectangular.at(i).comType ==
		    e_COMType_MinutiaeLocation) {
			if (roiResults.vecNoOfMinutiaeInRectangular.at(i)
				    .width == 200 &&
			    roiResults.vecNoOfMinutiaeInRectangular.at(i)
				    .height == 200) {
				noOfMinInRect200x200 =
				    roiResults.vecNoOfMinutiaeInRectangular
					.at(i)
					.noOfMinutiae;
			}
		}
	}

	// return features
	fd_min_cnt_comrect200x200.featureDataDouble = noOfMinInRect200x200;
	res_min_cnt_comrect200x200.featureData = fd_min_cnt_comrect200x200;
	res_min_cnt_comrect200x200.returnCode = 0;
	featureDataList.push_back(res_min_cnt_comrect200x200);

	fd_min_cnt.featureDataDouble = minCnt;
	res_min_cnt.returnCode = 0;
	res_min_cnt.featureData = fd_min_cnt;
	featureDataList.push_back(res_min_cnt);

	if (m_bOutputSpeed) {
		NFIQ::QualityFeatureSpeed speed;
		speed.featureIDGroup = FingerJetFXFeature::speedFeatureIDGroup;
		speed.featureIDs.push_back("FingerJetFX_MinutiaeCount");
		speed.featureIDs.push_back(
		    "FingerJetFX_MinCount_COMMinRect200x200");
		speed.featureSpeed = timer.endTimerAndGetElapsedTime();
		m_lSpeedValues.push_back(speed);
	}
	return featureDataList;
}

std::string
FingerJetFXFeature::getModuleID()
{
	return "NFIQ2_FingerJetFX";
}

std::list<std::string>
FingerJetFXFeature::getAllFeatureIDs()
{
	std::list<std::string> featureIDs;
	featureIDs.push_back("FingerJetFX_MinCount_COMMinRect200x200");
	featureIDs.push_back("FingerJetFX_MinutiaeCount");
	return featureIDs;
}

FRFXLL_RESULT
FingerJetFXFeature::createContext(FRFXLL_HANDLE_PT phContext)
{
	FRFXLL_RESULT rc = FRFXLL_OK;

	FRFXLL_HANDLE h_context = NULL;

	rc = FRFXLLCreateLibraryContext(&h_context);
	if (FRFXLL_SUCCESS(rc)) {
		*phContext = h_context;
	}

	return rc;
}

FingerJetFXFeature::FJFXROIResults
FingerJetFXFeature::computeROI(
    const std::vector<FingerJetFXFeature::Minutia> &minutiaData, int bs,
    const NFIQ::FingerprintImageData &fingerprintImage,
    std::vector<FingerJetFXFeature::Object> vecRectDimensions)
{
	unsigned int fpHeight = fingerprintImage.m_ImageHeight;
	unsigned int fpWidth = fingerprintImage.m_ImageWidth;

	FingerJetFXFeature::FJFXROIResults roiResults;
	roiResults.chosenBlockSize = bs;

	// compute Centre of Mass based on minutiae
	std::tie(roiResults.centreOfMassMinutiae.x,
	    roiResults.centreOfMassMinutiae.y) =
	    FingerJetFXFeature::centerOfMinutiaeMass(minutiaData);

	// get number of minutiae that lie inside a block defined by the Centre
	// of Mass (COM)

	// 1. rectangular block with height and width centered around COM
	for (unsigned int i = 0; i < vecRectDimensions.size(); i++) {
		int centre_x = 0, centre_y = 0;
		if (vecRectDimensions.at(i).comType ==
		    e_COMType_MinutiaeLocation) {
			centre_x = roiResults.centreOfMassMinutiae.x;
			centre_y = roiResults.centreOfMassMinutiae.y;
		}

		unsigned int comRectHeightHalf =
		    vecRectDimensions.at(i).height / 2; // round down
		unsigned int comRectWidthHalf = vecRectDimensions.at(i).width /
		    2; // round down
		// check boundaries
		int startY = (centre_y - comRectHeightHalf);
		int startX = (centre_x - comRectWidthHalf);
		int endY = (centre_y + comRectHeightHalf);
		int endX = (centre_x + comRectWidthHalf);
		if (startY < 0) {
			startY = 0;
		}
		if (startX < 0) {
			startX = 0;
		}
		if (endY >= (int)fpHeight) {
			endY = (fpHeight - 1);
		}
		if (endX >= (int)fpWidth) {
			endX = (fpWidth - 1);
		}

		unsigned int noOfMinutiaeInRect = 0;
		for (const auto &m : minutiaData) {
			if (m.x >= startX && m.x <= endX && m.y >= startY &&
			    m.y <= endY) {
				// minutia is inside rectangular
				noOfMinutiaeInRect++;
			}
		}

		FingerJetFXFeature::MinutiaeInObjectInfo minObjInfo;
		minObjInfo.height = vecRectDimensions.at(i).height;
		minObjInfo.width = vecRectDimensions.at(i).width;
		minObjInfo.noOfMinutiae = noOfMinutiaeInRect;
		minObjInfo.comType = vecRectDimensions.at(i).comType;
		roiResults.vecNoOfMinutiaeInRectangular.push_back(minObjInfo);
	}

	return roiResults;
}
