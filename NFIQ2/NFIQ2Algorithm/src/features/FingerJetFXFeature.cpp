#include <features/FingerJetFXFeature.h>
#include <nfiq2_exception.hpp>
#include <nfiq2_timer.hpp>

#include <cstring>
#include <memory>
#include <sstream>
#include <tuple>

const char NFIQ2::Identifiers::QualityModules::MinutiaeCount[] {
	"MinutiaeCount"
};
const char NFIQ2::Identifiers::QualityFeatures::Minutiae::Count[] {
	"FingerJetFX_MinutiaeCount"
};
const char NFIQ2::Identifiers::QualityFeatures::Minutiae::CountCOM[] {
	"FingerJetFX_MinCount_COMMinRect200x200"
};

NFIQ2::QualityFeatures::FingerJetFXFeature::FingerJetFXFeature(
    const NFIQ2::FingerprintImageData &fingerprintImage)
{
	this->setFeatures(computeFeatureData(fingerprintImage));
}

NFIQ2::QualityFeatures::FingerJetFXFeature::~FingerJetFXFeature() = default;

std::pair<unsigned int, unsigned int>
NFIQ2::QualityFeatures::FingerJetFXFeature::centerOfMinutiaeMass(
    const std::vector<FingerJetFXFeature::Minutia> &minutiaData)
{
	unsigned int lx { 0 }, ly { 0 };
	for (const auto &m : minutiaData) {
		lx += m.x;
		ly += m.y;
	}
	return std::make_pair<unsigned int, unsigned int>(
	    static_cast<unsigned int>(lx / minutiaData.size()),
	    static_cast<unsigned int>(ly / minutiaData.size()));
}

std::string
NFIQ2::QualityFeatures::FingerJetFXFeature::parseFRFXLLError(
    const FRFXLL_RESULT fxRes)
{
	switch (fxRes) {
	case FRFXLL_ERR_FB_TOO_SMALL_AREA:
		return "FRFXLL_ERR_FB_TOO_SMALL_AREA: Fingerprint area is too "
		       "small. Most likely this is because the tip of the "
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

std::vector<NFIQ2::QualityFeatures::FingerJetFXFeature::Minutia>
NFIQ2::QualityFeatures::FingerJetFXFeature::getMinutiaData() const
{
	return (this->minutiaData_);
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::FingerJetFXFeature::computeFeatureData(
    const NFIQ2::FingerprintImageData &fingerprintImage)
{
	std::unordered_map<std::string, double> featureDataList;

	// make local copy of fingerprint image
	// since FJFX somehow transforms the input image
	NFIQ2::FingerprintImageData localFingerprintImage(
	    fingerprintImage.width, fingerprintImage.height,
	    fingerprintImage.fingerCode, fingerprintImage.ppi);
	// copy data now
	localFingerprintImage.resize(fingerprintImage.size());
	memcpy((void *)localFingerprintImage.data(), fingerprintImage.data(),
	    fingerprintImage.size());

	std::pair<std::string, double> fd_min_cnt;
	fd_min_cnt = std::make_pair(
	    Identifiers::QualityFeatures::Minutiae::Count, 0);

	std::pair<std::string, double> fd_min_cnt_comrect200x200;
	fd_min_cnt_comrect200x200 = std::make_pair(
	    Identifiers::QualityFeatures::Minutiae::CountCOM, 0);

	NFIQ2::Timer timer;
	timer.start();

	// create context for feature extraction
	// the created context function is modified to override default settings
	FRFXLL_HANDLE hCtx = NULL, hFeatureSet = NULL;
	if (!FRFXLL_SUCCESS(createContext(&hCtx))) {
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FJFX_CannotCreateContext,
		    "Cannot create context of feature extraction (create "
		    "context failed).");
	}
	if (hCtx == NULL) {
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FJFX_CannotCreateContext,
		    "Cannot create context of feature extraction (hCtx is "
		    "NULL).");
	}

	// extract feature set
	const FRFXLL_RESULT fxRes = FRFXLLCreateFeatureSetFromRaw(hCtx,
	    (unsigned char *)localFingerprintImage.data(),
	    localFingerprintImage.size(), localFingerprintImage.width,
	    localFingerprintImage.height, localFingerprintImage.ppi,
	    FRFXLL_FEX_ENABLE_ENHANCEMENT, &hFeatureSet);
	if (!FRFXLL_SUCCESS(fxRes)) {
		FRFXLLCloseHandle(&hCtx);
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FJFX_CannotCreateFeatureSet,
		    "Could not create feature set from raw data: " +
			FingerJetFXFeature::parseFRFXLLError(fxRes));
	}

	// close handle
	FRFXLLCloseHandle(&hCtx);
	if (hFeatureSet == NULL) {
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FJFX_CannotCreateFeatureSet,
		    "Feature set creation failed. Feature set is null.");
	}

	unsigned int minCnt { 0 };
	const FRFXLL_RESULT fxResMin = FRFXLLGetMinutiaInfo(
	    hFeatureSet, &minCnt, nullptr);
	if (!FRFXLL_SUCCESS(fxResMin)) {
		FRFXLLCloseHandle(&hFeatureSet);
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FJFX_NoFeatureSetCreated,
		    "Failed to obtain Minutia Info from feature set: " +
			FingerJetFXFeature::parseFRFXLLError(fxResMin));
	}

	std::unique_ptr<FRFXLL_Basic_19794_2_Minutia[]> mdata {};
	try {
		mdata.reset(new FRFXLL_Basic_19794_2_Minutia[minCnt]);
	} catch (const std::bad_alloc &) {
		FRFXLLCloseHandle(&hFeatureSet);
		throw NFIQ2::Exception(NFIQ2::ErrorCode::NotEnoughMemory,
		    "Could not allocate space for extracted minutiae records.");
	}

	const FRFXLL_RESULT fxResData = FRFXLLGetMinutiae(
	    hFeatureSet, BASIC_19794_2_MINUTIA_STRUCT, &minCnt, mdata.get());
	if (!FRFXLL_SUCCESS(fxResData)) {
		FRFXLLCloseHandle(&hFeatureSet);
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FJFX_NoFeatureSetCreated,
		    "Failed to parse Minutia Data into 19794 Minutia Struct: " +
			FingerJetFXFeature::parseFRFXLLError(fxResData));
	}

	this->minutiaData_.clear();
	this->minutiaData_.reserve(minCnt);
	for (unsigned int i = 0; i < minCnt; i++) {
		this->minutiaData_.emplace_back(
		    static_cast<unsigned int>(mdata[i].x),
		    static_cast<unsigned int>(mdata[i].y),
		    static_cast<unsigned int>(mdata[i].a),
		    static_cast<unsigned int>(mdata[i].q),
		    static_cast<unsigned int>(static_cast<
			std::underlying_type<FRXLL_MINUTIA_TYPE>::type>(
			mdata[i].t)));
	}

	// close handle
	FRFXLLCloseHandle(&hFeatureSet);

	if (minCnt == 0) {
		// return features
		fd_min_cnt_comrect200x200.second = 0; // no minutiae found
		featureDataList[fd_min_cnt_comrect200x200.first] =
		    fd_min_cnt_comrect200x200.second;

		fd_min_cnt.second = 0; // no minutiae found
		featureDataList[fd_min_cnt.first] = fd_min_cnt.second;

		this->setSpeed(timer.stop());

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
	    32, fingerprintImage, vecRectDimensions);
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
	fd_min_cnt_comrect200x200.second = noOfMinInRect200x200;
	featureDataList[fd_min_cnt_comrect200x200.first] =
	    fd_min_cnt_comrect200x200.second;

	fd_min_cnt.second = minCnt;
	featureDataList[fd_min_cnt.first] = fd_min_cnt.second;

	this->setSpeed(timer.stop());

	return featureDataList;
}

std::string
NFIQ2::QualityFeatures::FingerJetFXFeature::getModuleName() const
{
	return NFIQ2::Identifiers::QualityModules::MinutiaeCount;
}

std::vector<std::string>
NFIQ2::QualityFeatures::FingerJetFXFeature::getQualityFeatureIDs()
{
	return { Identifiers::QualityFeatures::Minutiae::CountCOM,
		Identifiers::QualityFeatures::Minutiae::Count };
}

FRFXLL_RESULT
NFIQ2::QualityFeatures::FingerJetFXFeature::createContext(
    FRFXLL_HANDLE_PT phContext)
{
	FRFXLL_RESULT rc = FRFXLL_OK;

	FRFXLL_HANDLE h_context = NULL;

	rc = FRFXLLCreateLibraryContext(&h_context);
	if (FRFXLL_SUCCESS(rc)) {
		*phContext = h_context;
	}

	return rc;
}

NFIQ2::QualityFeatures::FingerJetFXFeature::FJFXROIResults
NFIQ2::QualityFeatures::FingerJetFXFeature::computeROI(int bs,
    const NFIQ2::FingerprintImageData &fingerprintImage,
    std::vector<FingerJetFXFeature::Object> vecRectDimensions)
{
	unsigned int fpHeight = fingerprintImage.height;
	unsigned int fpWidth = fingerprintImage.width;

	FingerJetFXFeature::FJFXROIResults roiResults;
	roiResults.chosenBlockSize = bs;

	// compute Centre of Mass based on minutiae
	std::tie(roiResults.centreOfMassMinutiae.x,
	    roiResults.centreOfMassMinutiae.y) =
	    FingerJetFXFeature::centerOfMinutiaeMass(this->minutiaData_);

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
		for (const auto &m : this->minutiaData_) {
			if (m.x >= static_cast<unsigned int>(startX) &&
			    m.x <= static_cast<unsigned int>(endX) &&
			    m.y >= static_cast<unsigned int>(startY) &&
			    m.y <= static_cast<unsigned int>(endY)) {
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
