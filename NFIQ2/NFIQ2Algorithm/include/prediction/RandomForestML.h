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

#undef EMBED_RANDOMFOREST_PARAMETERS

class RandomForestML
{

  public:
    RandomForestML();
    virtual ~RandomForestML();

    std::string getModuleID();

# ifdef EMBED_RANDOMFOREST_PARAMETERS
    std::string initModule();
# endif
    std::string initModule( const std::string& fileName, const std::string& fileHash );

    void evaluate(
      const std::list<NFIQ::QualityFeatureData>& featureVector,
      const double& utilityValue,
      double& qualityValue,
      double& deviation ) const;

  private:
#   if CV_MAJOR_VERSION <= 2
    CvRTrees* m_pTrainedRF;
#   else
    cv::Ptr<cv::ml::RTrees> m_pTrainedRF;
#   endif

    std::string calculateHashString( const std::string& s );
    void initModule( const std::string& params );
# ifdef EMBED_RANDOMFOREST_PARAMETERS
    std::string joinRFTrainedParamsString();
# endif
};

#endif

/******************************************************************************/
