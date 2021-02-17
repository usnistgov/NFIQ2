#ifdef _WIN32
#include <windows.h>
#define DLLEXPORT __declspec(dllexport)
#define STDCALL __stdcall
#else
#define DLLEXPORT __attribute__((visibility("default")))
#define STDCALL
#endif

#ifdef _MSC_VER
#define __attribute__(X)
#endif

extern "C" DLLEXPORT void STDCALL GetNfiq2Version(
    int *major, int *minor, int *patch, const char **ocv);
extern "C" DLLEXPORT const char *STDCALL InitNfiq2(char **hash);
extern "C" DLLEXPORT int STDCALL ComputeNfiq2Score(int fpos,
    const unsigned char *pixels, int size, int width, int height, int ppi);
