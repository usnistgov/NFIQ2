#ifndef FINGERJETFXFEATURE_H
#define FINGERJETFXFEATURE_H

#include <stdio.h>
#include <stdlib.h>

#include "FRFXLL.h"
#include "include/FingerprintImageData.h"
#include "include/InterfaceDefinitions.h"
#include "include/features/BaseFeature.h"

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

#ifndef WITHOUT_BIOMDI_SUPPORT
#if defined WINDOWS || defined WIN32
#include <sys/queue.h>
#endif
extern "C" {
#include <biomdimacro.h>
#include <fmr.h>
}
#endif

class FingerJetFXFeature : BaseFeature {
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

	FingerJetFXFeature(bool bOutputSpeed,
	    std::list<NFIQ::QualityFeatureSpeed> &speedValues)
	    : BaseFeature(bOutputSpeed, speedValues) {};
	virtual ~FingerJetFXFeature();

	std::list<NFIQ::QualityFeatureResult> computeFeatureData(
	    const NFIQ::FingerprintImageData fingerprintImage,
	    unsigned char templateData[], size_t &templateSize,
	    bool &templateCouldBeExtracted);

	std::string getModuleID();

	void initModule() { /* not needed here */ };

	static std::list<std::string> getAllFeatureIDs();
	static const std::string speedFeatureIDGroup;

    private:
	FRFXLL_RESULT
	createContext(FRFXLL_HANDLE_PT phContext);

#ifndef WITHOUT_BIOMDI_SUPPORT
	FJFXROIResults computeROI(struct finger_minutiae_data **fmds,
	    unsigned int minCount, int bs,
	    const NFIQ::FingerprintImageData &fingerprintImage,
	    std::vector<Object> vecRectDimensions);
#endif
};

#endif

/******************************************************************************/
