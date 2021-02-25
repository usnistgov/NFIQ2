#ifndef NFIQ2API_H_
#define NFIQ2API_H_

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

#ifdef __cplusplus
extern "C" {
#endif

DLLEXPORT void STDCALL GetNfiq2Version(
    int *major, int *minor, int *patch, const char **ocv);
DLLEXPORT const char *STDCALL InitNfiq2(char **hash);
DLLEXPORT int STDCALL ComputeNfiq2Score(int fpos, const unsigned char *pixels,
    int size, int width, int height, int ppi);

#ifdef __cplusplus
}
#endif

#endif /* NFIQ2API_H_ */
