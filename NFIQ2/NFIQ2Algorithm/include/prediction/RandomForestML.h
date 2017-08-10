#ifndef RANDOMFORESTML_H
#define RANDOMFORESTML_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <list>

#include <stdint.h>
#include <InterfaceDefinitions.h>

#include <opencv/cv.h>
#include <opencv/ml.h>

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
	CvRTrees* m_pTrainedRF;
	std::string joinRFTrainedParamsString();
};


#endif

/******************************************************************************/
