#ifndef FINGERJETFXFEATURE_H
#define FINGERJETFXFEATURE_H

#include <features/BaseFeature.h>
#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_interfacedefinitions.hpp>
#include <stdio.h>
#include <stdlib.h>

#include "FRFXLL.h"

#include <list>
#include <string>
#include <vector>

#if defined LINUX || defined __ANDROID__ || defined __APPLE__
#include <sys/types.h>
#include <sys/queue.h>
#include <sys/stat.h>

#include <unistd.h>
#endif

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>

namespace NFIQ { namespace QualityFeatures {

class FingerJetFXFeature : public BaseFeature {
    public:
	typedef enum com_type {
		e_COMType_MinutiaeLocation = 1,
		e_COMType_PixelGrayValues = 2
	} COMType;

	struct Minutia {
		unsigned int x; ///< x-coordinate from top-left corner
		unsigned int y; ///< y-coordinate from top-left corner
		unsigned int
		    angle; ///< angle in degrees (full range 0..360 degrees)
		unsigned int quality; ///< minutiae quality
		unsigned int type;    ///< minutiae type

		Minutia(unsigned int x_, unsigned int y_, unsigned int angle_,
		    unsigned int quality_, unsigned int type_)
		    : x(x_)
		    , y(y_)
		    , angle(angle_)
		    , quality(quality_)
		    , type(type_)
		{
		}
	};

	struct Point {
		unsigned int x; ///< x-coordinate
		unsigned int y; ///< x-coordinate
	};

	struct Object {
		COMType comType;     ///< type of requested centre of mass that
				     ///< shall be used for computation
		unsigned int width;  ///< width of object
		unsigned int height; ///< height of object
	};

	struct MinutiaeBlockInfo {
		unsigned int x;	     ///< x-coordinate from top-left corner
		unsigned int y;	     ///< y-coordinate from top-left corner
		unsigned int width;  ///< width of block
		unsigned int height; ///< height of block
		std::vector<Minutia>
		    vecMinutiae; ///< detected minutiae in block
	};

	struct MinutiaeInObjectInfo {
		COMType comType; ///< type of centre of mass that was used for
				 ///< computation
		unsigned int
		    width; ///< width of rectangular or diameter of circle
		unsigned int height; ///< height of rectangular of diameter of
				     ///< circle
		unsigned int noOfMinutiae; ///< number of minutiae inside the
					   ///< defined object (e.g.
					   ///< rectangular, circle, ...)
	};

	struct FJFXROIResults {
		unsigned int
		    chosenBlockSize; ///< the input block size in pixels
		unsigned int noOfCompleteBlocks; ///< the overall number of
						 ///< complete blocks (with full
						 ///< block size) in the image
		unsigned int noOfAllBlocks;   ///< the overall number of blocks
					      ///< in the image
		Point centreOfMassMinutiae;   ///< Centre of Mass based on
					      ///< minutiae coordinates
		Point centreOfMassGrayValues; ///< Centre of Mass based on
					      ///< pixel grayvalues
		std::vector<MinutiaeBlockInfo>
		    vecROIBlocks; ///< the detected ROI (that contain at
				  ///< least one minutia) blocks with
				  ///< position, size and number of
				  ///< minutiae
		std::vector<MinutiaeInObjectInfo>
		    vecNoOfMinutiaeInRectangular; ///< vector of the number
						  ///< of minutiae that are
						  ///< inside the defined
						  ///< rectangular
		std::vector<MinutiaeInObjectInfo>
		    vecNoOfMinutiaeInCircle; ///< vector of the number of
					     ///< minutiae that are inside
					     ///< the defined circle
	};

	FingerJetFXFeature();
	virtual ~FingerJetFXFeature();

	std::vector<NFIQ::QualityFeatureResult> computeFeatureData(
	    const NFIQ::FingerprintImageData &fingerprintImage) override;

	std::string getModuleName() const override;

	static std::vector<std::string> getAllFeatureIDs();
	static const std::string speedFeatureIDGroup;
	static const std::string moduleName;

	static std::pair<unsigned int, unsigned int> centerOfMinutiaeMass(
	    const std::vector<FingerJetFXFeature::Minutia> &minutiaData);

	static std::string parseFRFXLLError(const FRFXLL_RESULT fxRes);

	/** @throw NFIQ::NFIQException
	 * Template could not be extracted.
	 */
	std::vector<FingerJetFXFeature::Minutia> getMinutiaData() const;

	bool getTemplateStatus() const;

    private:
	FRFXLL_RESULT
	createContext(FRFXLL_HANDLE_PT phContext);

	std::vector<FingerJetFXFeature::Minutia> minutiaData_ {};
	bool templateCouldBeExtracted_ { false };

	FJFXROIResults computeROI(int bs,
	    const NFIQ::FingerprintImageData &fingerprintImage,
	    std::vector<FingerJetFXFeature::Object> vecRectDimensions);
};
}}

#endif

/******************************************************************************/
