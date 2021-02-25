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

#ifndef I_UNDERSTAND_THIS_NFIQ2_API_WILL_BE_REMOVED
#error "You have included nfiq2api.h. Please use the official C++ API. \
    An official C API will be added soon, but this is not it. You can track \
    progress on the C API at https://github.com/usnistgov/NFIQ2/issues/33. \
    If you understand and wish to bypass this error, define the symbol \
    I_UNDERSTAND_THIS_NFIQ2_API_WILL_BE_REMOVED in all compilation units that \
    include nfiq2api.h."
#endif /* I_UNDERSTAND_THIS_NFIQ2_API_WILL_BE_REMOVED */

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
