// This file should be in the include path in front of stlport, to replaces user_config.h from the stlport

#define _STLP_VERBOSE_AUTO_LINK
#define _STLP_NO_IOSTREAMS 1

#if (_WIN32_WCE == 0x420 || _WIN32_WCE == 0x400)
# define _STLP_GLOBAL_NEW_HANDLER 1
# define _STLP_NO_CSTD_FUNCTION_IMPORTS
//# define _STLP_NEW_PLATFORM_SDK
# if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
#   define _STLP_NATIVE_C_INCLUDE_PATH ..\include
#   define _STLP_NATIVE_CPP_RUNTIME_INCLUDE_PATH ..\include
#   define _STLP_NATIVE_CPP_C_INCLUDE_PATH ..\include
//#   define _STLP_HAS_NO_EXCEPTIONS
# endif
#endif

#ifndef _STLP_SHOW_MSG_AND_ABORT
# define _STLP_SHOW_MSG_AND_ABORT(ex,msg) 
#endif

#if defined(_MS_SUA_)
#   define _STLP_NO_CSTD_FUNCTION_IMPORTS 1
#   define _STLP_NATIVE_C_INCLUDE_PATH ../include
#   define _STLP_NATIVE_CPP_RUNTIME_INCLUDE_PATH ../../include/c++
#   define _STLP_NATIVE_CPP_C_INCLUDE_PATH ../../include/c++
#elif defined(__GNUC__)
//#define _STLP_NO_LIBSTDCPP
#  define _STLP_MAKE_HEADER(path, header) <path/header>
#  if __GNUC_PATCHLEVEL__
#    define _STLP_NATIVE_INCLUDE_PATH ../../../include/c++/__GNUC__.__GNUC_MINOR__.__GNUC_PATCHLEVEL__
#  else
#    define _STLP_NATIVE_INCLUDE_PATH ../../../include/c++/__GNUC__.__GNUC_MINOR__
#  endif
#  if __GNUC__ <= 3
#    define _STLP_HAS_GLOBAL_C_MATH_FUNCTIONS 1
#    define _STLP_DONT_SIMULATE_PARTIAL_SPEC_FOR_TYPE_TRAITS 1
#  endif
#  define _STLP_NATIVE_CPP_C_INCLUDE_PATH _STLP_NATIVE_INCLUDE_PATH
#  define _STLP_NATIVE_CPP_C_HEADER(header)  _STLP_MAKE_HEADER(_STLP_NATIVE_CPP_C_INCLUDE_PATH,header)
#  ifdef __avr32__
#    define _STLP_HAS_NO_NEW_C_HEADERS
#    define _STLP_USE_GLIBC 
#  endif
#  ifdef __xtensa__
     typedef long off_t;
#  endif
#endif

#define _STLP_DONT_USE_EXCEPTIONS 1
#define _STLP_NO_EXCEPTIONS
//#define _STLP_EXTERN_RANGE_ERRORS
//
//#ifdef __cplusplus
//
//namespace stlp_std { //_STLP_BEGIN_NAMESPACE
//
//inline void __stl_throw_runtime_error(const char* __msg) {}
//inline void __stl_throw_range_error(const char* __msg) {}
//inline void __stl_throw_out_of_range(const char* __msg) {}
//inline void __stl_throw_length_error(const char* __msg) {}
//inline void __stl_throw_invalid_argument(const char* __msg) {}
//inline void __stl_throw_overflow_error(const char* __msg) {}
//
//} //_STLP_END_NAMESPACE
//
//#endif

//#ifdef __cplusplus
//extern "C"
//#endif
//int puts(const char * msg) {}
//#define _STLP_ABORT() ((void)(0))
//
