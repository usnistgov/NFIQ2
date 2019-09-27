#include <string>
#include <memory>
#include "NFIQ2Algorithm.h"
#include "NFIQException.h"
#include "nfiq2api.h"

#include <opencv2/core/version.hpp>

// external vars for the version values
extern char product_name[128];
extern char product_copyright[128];
extern char product_vendor[128];
extern int version_major;
extern int version_minor;
extern int version_evolution;
extern int version_build;

// static object to load the algorithm only once (random forest init!)
std::unique_ptr<NFIQ::NFIQ2Algorithm> g_nfiq2;

#include <cstdlib>

extern "C" {
  DLLEXPORT void STDCALL GetNfiq2Version( int* major, int* minor, int* evolution, int* increment, const char** ocv )
  {
    *major = version_major;
    *minor = version_minor;
    *evolution = version_evolution;
    *increment = version_build;
#if CV_MAJOR_VERSION <= 2
    const char* m = nullptr;
    cvGetModuleInfo( nullptr, ocv, &m );
#else
    std::stringstream ss;
    ss << cv::getVersionMajor() << "." << cv::getVersionMinor() << "." << cv::getVersionRevision();
    static char buf[128];
    memset(buf, 0, 128);
    strncpy(buf, ss.str().c_str(), ss.str().length());
    *ocv = buf;
#endif
  }
  DLLEXPORT const char* STDCALL InitNfiq2()
  {
    try
    {
      if( g_nfiq2.get() == nullptr )
      {
#       ifdef EMBED_RANDOMFOREST_PARAMETERS
        g_nfiq2 = std::unique_ptr<NFIQ::NFIQ2Algorithm>( new NFIQ::NFIQ2Algorithm() );
#       else
        g_nfiq2 = std::unique_ptr<NFIQ::NFIQ2Algorithm>( new NFIQ::NFIQ2Algorithm("nfiq2rf.yaml", "0xccd75820b48c19f1645ef5e9c481c592") );
#       endif
        return g_nfiq2->getParameterHash().c_str();
      }
    }
    catch( std::exception& exc )
    {
      std::cerr << "NFIQ2 ERROR => " << exc.what() << std::endl;
    }
    return nullptr;
  }
  DLLEXPORT int STDCALL ComputeNfiq2Score( int fpos, const unsigned char* pixels, int size, int width, int height, int ppi )
  {
    try
    {
      if( g_nfiq2.get() != nullptr )
      {
        NFIQ::FingerprintImageData rawImage( pixels, size, width, height, fpos, ppi );
        std::list<NFIQ::ActionableQualityFeedback> actionableQuality;
        std::list<NFIQ::QualityFeatureData> featureVector;
        std::list<NFIQ::QualityFeatureSpeed> featureTimings;
        int qualityScore = ( int )g_nfiq2->computeQualityScore( rawImage,
                           true, actionableQuality,
                           false, featureVector,
                           false, featureTimings );
        return qualityScore;
      }
    }
    catch( const NFIQ::NFIQException& exc )
    {
      std::cerr << "NFIQ2 ERROR => Return code [" << exc.getReturnCode() << "]: " << exc.getErrorMessage() << std::endl;
      return 255;
    }
    catch( std::exception& exc )
    {
      std::cerr << "NFIQ2 ERROR => " << exc.what() << std::endl;
      return -1;
    }
    return -2; // not initialized
  }
}


