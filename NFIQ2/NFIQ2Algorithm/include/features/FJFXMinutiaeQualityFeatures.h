#ifndef FINGERJETFXMINUTIAEQUALITYFEATURE_H
#define FINGERJETFXMINUTIAEQUALITYFEATURE_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <list>

#include <stdint.h>
#include <InterfaceDefinitions.h>
#include <FingerprintImageData.h>
#include <features/BaseFeature.h>

#include <FRFXLL.h>

#ifdef LINUX
#define _XOPEN_SOURCE   1
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/queue.h>

#include <unistd.h>
#endif

#include <errno.h>
#include <fcntl.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/* Ideal Standard Deviation of pixel values in a neighborhood. */
#define IDEALSTDEV  64
/* Ideal Mean of pixel values in a neighborhood. */
#define IDEALMEAN    127

#ifndef WITHOUT_BIOMDI_SUPPORT
extern "C"
{
#include <biomdimacro.h>
#include <fmr.h>
}
#if defined WINDOWS || defined WIN32
#include <sys/queue.h>
#endif
#endif

class FJFXMinutiaeQualityFeature : BaseFeature
{

public:

	struct MinutiaData
	{
		int x;	///< x-coordinate from top-left corner
		int y; ///< y-coordinate from top-left corner
		double quality; ///< computed minutiae quality value
	};

	FJFXMinutiaeQualityFeature(bool bOutputSpeed, std::list<NFIQ::QualityFeatureSpeed> & speedValues)
		: BaseFeature(bOutputSpeed, speedValues)
	{
	};
	virtual ~FJFXMinutiaeQualityFeature();

	std::list<NFIQ::QualityFeatureResult> computeFeatureData(
		const NFIQ::FingerprintImageData & fingerprintImage, unsigned char templateData[], size_t & templateSize, bool & templateCouldBeExtracted);

	std::string getModuleID();

	void initModule() { /* not needed here */ };

	std::list<std::string> getAllFeatureIDs();

private:

#ifndef WITHOUT_BIOMDI_SUPPORT
	std::vector<MinutiaData> computeMuMinQuality(struct finger_minutiae_data **fmds, unsigned int minCount, 
		int bs, const NFIQ::FingerprintImageData & fingerprintImage);

	std::vector<MinutiaData> computeOCLMinQuality(struct finger_minutiae_data **fmds, unsigned int minCount, 
		int bs, const NFIQ::FingerprintImageData & fingerprintImage);

	double computeMMBBasedOnCOM(struct finger_minutiae_data **fmds, unsigned int minCount, 
		int bs, const NFIQ::FingerprintImageData & fingerprintImage, unsigned int regionSize);

#endif
};


#endif

/******************************************************************************/
