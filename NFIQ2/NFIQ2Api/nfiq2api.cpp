#include <string>
#include <memory>
#include "NFIQ2Algorithm.h"
#include "NFIQException.h"
#include "nfiq2api.h"

// external vars for the version values
extern char product_name[128];
extern char product_copyright[128];
extern char product_vendor[128];
extern int version_major;
extern int version_minor;
extern int version_evolution;
extern int version_build;

std::string g_modulePath;
std::unique_ptr<NFIQ::NFIQ2Algorithm> g_nfiq2;

// static object to load the algorithm only once (random forest init!)

#ifdef _WIN32
extern "C" int APIENTRY DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved )
{
  if( fdwReason == DLL_PROCESS_ATTACH )
  {
    // retrieve and store the path of the DLL
    char buffer[MAX_PATH];
    int n = GetModuleFileName( hinstDLL, buffer, sizeof( buffer ) );
    if( n > 0 && n < ( int )sizeof( buffer ) )
    {
      char* c = strrchr( buffer, '\\' );
      if( c != NULL )
      {
        *c = 0;
        g_modulePath =  buffer;
      }
    }
  }
  return TRUE;
}
#elif __linux
#include <cstring>
#include <dlfcn.h>
#include <libgen.h>
#include <link.h>

void __attribute__( ( constructor ) ) my_load( void );
void __attribute__( ( destructor ) ) my_unload( void );

void my_load( void )
{
  // iterate thru all loaded  modules
  using UnknownStruct = struct unknown_struct
  {
    void*  pointers[3];
    struct unknown_struct* ptr;
  };
  using LinkMap = struct link_map;

  auto* handle = dlopen( NULL, RTLD_NOW );
  auto* p = reinterpret_cast<UnknownStruct*>( handle )->ptr;
  auto* map = reinterpret_cast<LinkMap*>( p->ptr );

  while( map )
  {
    if( strstr( map->l_name, "libNfiq2Api" ) )
    {
      char* path = nullptr;
      path = realpath( map->l_name, nullptr );
      if( path != nullptr )
      {
        // make sure g_modulePath is initialized before (__attribute__((init_priority(101))))
        g_modulePath = dirname( path );
        free( path );
      }
      break;
    }
    map = map->l_next;
  }
}
#endif

#include <cstdlib>

extern "C" {
  DLLEXPORT void STDCALL GetNfiq2Version( int* major, int* minor, int* evolution, int* increment, const char** ocv )
  {
    *major = version_major;
    *minor = version_minor;
    *evolution = version_evolution;
    *increment = version_build;
#if CV_MAJOR_VERSION == 3
    std::stringstream ss;
    ss << cv::getVersionMajor() << "." << cv::getVersionMinor() << "." << cv::getVersionRevision();
    static char buf[128];
    memset(buf, 0, 128);
    strncpy(buf, ss.str().c_str(), ss.str().length());
    *ocv = buf;
#else
    const char* m = nullptr;
    cvGetModuleInfo( nullptr, ocv, &m);
#endif    
  }
  DLLEXPORT void STDCALL InitNfiq2()
  {
    try
    {
      if( g_nfiq2.get() == nullptr )
      {
        g_nfiq2 = std::unique_ptr<NFIQ::NFIQ2Algorithm>( new NFIQ::NFIQ2Algorithm() );
      }
    }
    catch( std::exception& exc )
    {
      std::cerr << "NFIQ2 ERROR => " << exc.what() << std::endl;
    }
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


