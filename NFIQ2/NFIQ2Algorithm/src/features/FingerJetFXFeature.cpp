#include <nfiq2/features/FingerJetFXFeature.h>
#include <nfiq2/nfiqexception.hpp>
#include <nfiq2/timer.hpp>
#include <string.h>

#include <sstream>

#define CBEFF (0x00330502)

using namespace NFIQ;

int cMem = 0;

static void *
m_malloc(size_t size, void *_)
{
	++cMem;
	return malloc(size);
}
static void
m_free(void *p, void *_)
{
	free(p);
	--cMem;
}

FingerJetFXFeature::~FingerJetFXFeature()
{
}

const std::string FingerJetFXFeature::speedFeatureIDGroup = "Minutiae";

std::list<NFIQ::QualityFeatureResult>
FingerJetFXFeature::computeFeatureData(
    const NFIQ::FingerprintImageData fingerprintImage,
    unsigned char templateData[], size_t &templateSize,
    bool &templateCouldBeExtracted)
{
	templateCouldBeExtracted = false;

#ifndef WITHOUT_BIOMDI_SUPPORT
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
	if (createContext(&hCtx) != FRFXLL_OK) {
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
	FRFXLL_RESULT fxRes = FRFXLLCreateFeatureSetInPlaceFromRaw(hCtx,
	    (unsigned char *)localFingerprintImage.data(),
	    localFingerprintImage.size(), localFingerprintImage.m_ImageWidth,
	    localFingerprintImage.m_ImageHeight,
	    localFingerprintImage.m_ImageDPI, FRFXLL_FEX_ENABLE_ENHANCEMENT,
	    &hFeatureSet);
	if (fxRes != FRFXLL_OK) {
		FRFXLLCloseHandle(&hCtx);

		// return features
		fd_min_cnt_comrect200x200.featureDataDouble =
		    0; // no minutiae found
		res_min_cnt_comrect200x200.featureData =
		    fd_min_cnt_comrect200x200;
		res_min_cnt_comrect200x200.returnCode = fxRes;
		featureDataList.push_back(res_min_cnt_comrect200x200);

		fd_min_cnt.featureDataDouble = 0; // no minutiae found
		res_min_cnt.returnCode = fxRes;
		res_min_cnt.featureData = fd_min_cnt;
		featureDataList.push_back(res_min_cnt);

		return featureDataList;
	}

	// close handle
	FRFXLLCloseHandle(&hCtx);
	if (hFeatureSet == NULL) {
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

		return featureDataList;
	}

	// export ISO template from handle

	unsigned short dpcm =
	    ((unsigned int)localFingerprintImage.m_ImageDPI * 100 + 50) / 254;
	FRFXLL_OUTPUT_PARAM_ISO_ANSI param = {
		sizeof(FRFXLL_OUTPUT_PARAM_ISO_ANSI), CBEFF,
		localFingerprintImage.m_FingerCode, 0, dpcm, dpcm,
		(unsigned short)localFingerprintImage.m_ImageWidth,
		(unsigned short)localFingerprintImage.m_ImageHeight, 0, 0,
		0 /*live-scan plain*/
	};
	if (FRFXLLExport(hFeatureSet, FRFXLL_DT_ISO_FEATURE_SET, &param,
		templateData, &templateSize) != FRFXLL_OK) {
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

		return featureDataList;
	}

	templateCouldBeExtracted = true;

	// close handle
	FRFXLLCloseHandle(&hFeatureSet);

	// parse ISO template
	// validity check
	if (templateSize < 28) {
		throw NFIQ::NFIQException(
		    NFIQ::
			e_Error_FeatureCalculationError_FJFX_ISOTemplateTooSmall,
		    "Created ISO template is too small.");
	}

	struct finger_minutiae_record *fmr;
	if (new_fmr(FMR_STD_ISO, &fmr) < 0) {
		throw NFIQ::NFIQException(
		    NFIQ::
			e_Error_FeatureCalculationError_FJFX_CannotAllocateFMR,
		    "Could not allocate FMR.");
	}

	BDB bdb;
	INIT_BDB(&bdb, templateData, templateSize);
	if (scan_fmr(&bdb, fmr) != READ_OK) {
		if (fmr != NULL) {
			free_fmr(fmr);
		}
		throw NFIQ::NFIQException(
		    NFIQ::e_Error_FeatureCalculationError_FJFX_CannotInitBDB,
		    "Could not init BDB.");
	}

	// Get all of the minutiae records
	int rcount = get_fvmr_count(fmr);
	if (rcount == 0) {
		if (fmr != NULL) {
			free_fmr(fmr);
		}
		throw NFIQ::NFIQException(
		    NFIQ::
			e_Error_FeatureCalculationError_FJFX_NoMinutiaeRecords,
		    "No minutiae records found.");
	}
	struct finger_view_minutiae_record **fvmrs = NULL;
	fvmrs = (struct finger_view_minutiae_record **)malloc(
	    rcount * sizeof(struct finger_view_minutiae_record **));
	if (get_fvmrs(fmr, fvmrs) != rcount) {
		if (fvmrs) {
			free(fvmrs);
		}
		if (fmr != NULL) {
			free_fmr(fmr);
		}
		throw NFIQ::NFIQException(
		    NFIQ::
			e_Error_FeatureCalculationError_FJFX_CannotGetMinutiaeRecords,
		    "Cannot get minutiae records.");
	}

	// get number of minutiae
	int minCnt = get_fmd_count(fvmrs[0]);
	if (minCnt == 0) {
		if (fvmrs) {
			free(fvmrs);
		}
		if (fmr != NULL) {
			free_fmr(fmr);
		}

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

	struct finger_minutiae_data **fmds;
	fmds = (struct finger_minutiae_data **)malloc(
	    minCnt * sizeof(struct finger_minutiae_data **));

	if (get_fmds(fvmrs[0], fmds) != minCnt) {
		if (fmds) {
			free(fmds);
		}
		if (fvmrs) {
			free(fvmrs);
		}
		if (fmr != NULL) {
			free_fmr(fmr);
		}
		throw NFIQ::NFIQException(
		    NFIQ::
			e_Error_FeatureCalculationError_FJFX_CannotGetMinutiaeData,
		    "Cannot get minutiae data.");
	}

	// compute ROI and return features
	std::vector<FingerJetFXFeature::Object> vecRectDimensions;
	FingerJetFXFeature::Object rect200x200;
	rect200x200.comType = e_COMType_MinutiaeLocation;
	rect200x200.width = 200;
	rect200x200.height = 200;
	vecRectDimensions.push_back(rect200x200);

	FingerJetFXFeature::FJFXROIResults roiResults = computeROI(
	    fmds, minCnt, 32, fingerprintImage, vecRectDimensions);
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

	if (fmds) {
		free(fmds);
	}

	if (fvmrs) {
		free(fvmrs);
	}

	if (fmr != NULL) {
		free_fmr(fmr);
	}

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
#else
	throw NFIQ::NFIQException(NFIQ::e_Error_FunctionNotImplemented,
	    "libbiomdi support is not enabled. FJFX feature "
	    "computation is not possible.");
#endif
}

std::string
FingerJetFXFeature::getModuleID()
{
	return "NFIQ2_FingerJetFX";
}

std::list<std::string>
FingerJetFXFeature::getAllFeatureIDs()
{
#ifndef WITHOUT_BIOMDI_SUPPORT
	std::list<std::string> featureIDs;
	featureIDs.push_back("FingerJetFX_MinCount_COMMinRect200x200");
	featureIDs.push_back("FingerJetFX_MinutiaeCount");
	return featureIDs;
#else
	// return empty feature list
	std::list<std::string> featureIDs;
	return featureIDs;
#endif
}

FRFXLL_RESULT
FingerJetFXFeature::createContext(FRFXLL_HANDLE_PT phContext)
{
	FRFXLL_RESULT rc = FRFXLL_OK;

	FRFXLL_CONTEXT_INIT ctx_init = {
		sizeof(FRFXLL_CONTEXT_INIT), // length
		NULL,			     // heapContext
		&m_malloc,		     // malloc
		&m_free,		     // free
	};
	FRFXLL_HANDLE h_context = NULL;

	rc = FRFXLLCreateContext(&ctx_init, &h_context);
	if (FRFXLL_SUCCESS(rc)) {
		*phContext = h_context;
	}

	return rc;
}

#ifndef WITHOUT_BIOMDI_SUPPORT

FingerJetFXFeature::FJFXROIResults
FingerJetFXFeature::computeROI(struct finger_minutiae_data **fmds,
    unsigned int minCount, int bs,
    const NFIQ::FingerprintImageData &fingerprintImage,
    std::vector<FingerJetFXFeature::Object> vecRectDimensions)
{
	unsigned int fpHeight = fingerprintImage.m_ImageHeight;
	unsigned int fpWidth = fingerprintImage.m_ImageWidth;

	FingerJetFXFeature::FJFXROIResults roiResults;
	roiResults.chosenBlockSize = bs;

	// compute Centre of Mass based on minutiae
	int x = 0, y = 0;
	find_center_of_minutiae_mass(fmds, minCount, &x, &y);

	roiResults.centreOfMassMinutiae.x = x;
	roiResults.centreOfMassMinutiae.y = y;

	// get number of minutiae that lie inside a block defined by the Centre
	// of Mass (COM)

	// 1. rectangular block with height and width centered around COM
	for (unsigned int i = 0; i < vecRectDimensions.size(); i++) {
		int centre_x = 0, centre_y = 0;
		if (vecRectDimensions.at(i).comType ==
		    e_COMType_MinutiaeLocation) {
			centre_x = x;
			centre_y = y;
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
		for (unsigned int k = 0; k < minCount; k++) {
			if (fmds[k]->x_coord >= startX &&
			    fmds[k]->x_coord <= endX &&
			    fmds[k]->y_coord >= startY &&
			    fmds[k]->y_coord <= endY) {
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

#endif
