#ifndef RANDOMFORESTML_H
#define RANDOMFORESTML_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <list>

#include "include/InterfaceDefinitions.h"

#include <opencv2/core/version.hpp>
#if CV_MAJOR_VERSION <= 2
#include <opencv/cv.h>
#include <opencv/ml.h>
#else
#include <opencv2/ml.hpp>
#endif /* CV_MAJOR_VERSION */

class RandomForestML
{

public:
	RandomForestML();
	virtual ~RandomForestML();

	std::string getModuleID();

	void initModule();

	void evaluate(
		const std::list<NFIQ::QualityFeatureData> & featureVector,
		const double & utilityValue,
		double & qualityValue,
		double & deviation);

private:
#		if CV_MAJOR_VERSION <= 2
		CvRTrees* m_pTrainedRF;
#		else
		cv::Ptr<cv::ml::RTrees> m_pTrainedRF;
#		endif
	std::string joinRFTrainedParamsString();
};


#endif

/******************************************************************************/
