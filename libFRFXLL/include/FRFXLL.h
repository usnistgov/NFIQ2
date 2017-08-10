/*
    FingerJetFX OSE -- Fingerprint Feature Extractor, Open Source Edition

    Copyright (c) 2011 by DigitalPersona, Inc. All rights reserved.

    DigitalPersona, FingerJet, and FingerJetFX are registered trademarks 
    or trademarks of DigitalPersona, Inc. in the United States and other
    countries.

    FingerJetFX OSE is open source software that you may modify and/or
    redistribute under the terms of the GNU Lesser General Public License
    as published by the Free Software Foundation, either version 3 of the 
    License, or (at your option) any later version, provided that the 
    conditions specified in the COPYRIGHT.txt file provided with this 
    software are met.
 
    For more information, please visit digitalpersona.com/fingerjetfx.
*/ 
/*
      LIBRARY: FRFXLL.a - Fingerprint Feature Extractor - Low Level API

      ALGORITHM:      Alexander Ivanisov
                      Yi Chen
                      Salil Prabhakar
      IMPLEMENTATION: Alexander Ivanisov
                      Jacob Kaminsky
                      Lixin Wei
      DATE:           11/08/2011
*/

#ifndef __FRFXLL_h
#define __FRFXLL_h

#ifndef __FRFXLL_VER__
#define __FRFXLL_VER__ ((0x5<<8) | 0x2)
#endif //__FRFXLL_VER__


#include <stddef.h>
#include "FRFXLL_Results.h"
// if OK or warning
#define FRFXLL_SUCCESS(rc) ((rc) >= FRFXLL_OK)

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef void * FRFXLL_HANDLE;            ///< Handle to an object
typedef FRFXLL_HANDLE * FRFXLL_HANDLE_PT;  ///< Pointer to a handle to an object
typedef unsigned int FRFXLL_TIME;       ///< time in milliseconds
#define FRFXLL_INFINITE ((FRFXLL_TIME) -1)

#ifndef FRFXLL_EXPORT
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define FRFXLL_EXPORT                    // __attribute__((visibility("default")))
#else
#define FRFXLL_EXPORT
#endif
#endif // FRFXLL_EXPORT

typedef unsigned int FRFXLL_DATA_TYPE;
#define FRFXLL_DT_RAW_SAMPLE           ((FRFXLL_DATA_TYPE)0x00000001)  ///< Sample in RAW format represented as FRFXLL_RAW_SAMPLE or FRFXLL_RAW_CONST_SAMPLE; The second is for FRFXLLCreateFeatureSet only.
#define FRFXLL_DT_ANSI_381_SAMPLE      ((FRFXLL_DATA_TYPE)0x001B0401)  ///< Sample in ANSI 381-2004 data format
#define FRFXLL_DT_ISO_19794_4_SAMPLE   ((FRFXLL_DATA_TYPE)0x01010007)  ///< Sample in ISO/IEC 19794-4 data format

typedef struct tag_FRFXLL_SAMPLE {
  unsigned short width, height, dpi, reserved;
  unsigned char * pixels;
} FRFXLL_RAW_SAMPLE; // to be used with FRFXLLCreateFeatureSetInPlace

typedef struct tag_FRFXLL_RAW_CONST_SAMPLE {
  unsigned short width, height, dpi, reserved;
  const unsigned char * pixels;
} FRFXLL_RAW_CONST_SAMPLE;  // can be used with FRFXLLCreateFeatureSet

// Feature extraction flags
#define FRFXLL_FEX_DISABLE_ENHANCEMENT  (0x00000001U)       // About 2x faster but less accurate than default
#define FRFXLL_FEX_ENABLE_ENHANCEMENT   (0x00000002U)       // Slower and more accurate: currently same as default

typedef struct tag_FRFXLL_VERSION_INFO {
  unsigned  major;
  unsigned  minor;
  unsigned  revision;
  unsigned  build;
} FRFXLL_VERSION;

/**
Context initialization structure
*/
typedef struct tag_FRFXLL_CONTEXT_INIT {
  size_t length;        ///< Length of this structure: for extensibility
  void * heapContext;   ///< Context passed into heap functions. Heap should be ready to use prior to calling FRFXLLCreateContext
  void *    (*malloc)   (size_t size, void * heapContext);      ///< Function to allocate block of memory on the heap
  void      (*free)     (void * block, void * heapContext);     ///< Function to free block of memory on the heap
  unsigned int (*get_current_tick_in_ms)( void );               ///< Get current tick count im milliseconds, optional. If not supplied timeout parameter is not honored
  long (*interlocked_increment) (volatile long *);              ///< optional
  long (*interlocked_decrement) (volatile long *);              ///< optional
  long (*interlocked_exchange)  (volatile long *, long);        ///< optional
  long (*interlocked_compare_exchange) (volatile long *, long, long);  ///< optional
} FRFXLL_CONTEXT_INIT;
/**
Creates the fingerprint recognition context. <P>
The fingerprint recognition context is introduced in order to support environments without the global scope <P>
Note: for the function that accept multiple handles, all handles must reference objects that were created in the same context

\retval FRFXLL_OK                      The operation completed successfully.
\retval FRFXLL_ERR_INVALID_PARAM       Invalid parameter
\retval FRFXLL_ERR_NO_MEMORY           No enough memory to complete the operation
*/
FRFXLL_RESULT FRFXLL_EXPORT FRFXLLCreateContext(
  FRFXLL_CONTEXT_INIT * contextInit,  ///< [in] pointer to filled context initialization structure
  FRFXLL_HANDLE_PT phContext          ///< [out] pointer to where to put an open handle to created context
);

/**
  Create library context with the default initialization parameters
  !!! Note: using this function imposes c runtime and other platform dependencies !!!
  !!! If developing firmware use FRFXLLCreateContext() instead !!!

  \retval FRFXLL_OK                      The operation completed successfully.
  \retval FRFXLL_ERR_INVALID_PARAM       Invalid parameter
  \retval FRFXLL_ERR_NO_MEMORY           No enough memory to complete the operation
*/
FRFXLL_RESULT FRFXLL_EXPORT FRFXLLCreateLibraryContext(
  FRFXLL_HANDLE_PT phContext          ///< [out] pointer to where to put an open handle to created context
);

/** 
Retrieves information about the version of recognition engine library

\retval FRFXLL_OK                      The operation completed successfully.
\retval FRFXLL_ERR_INVALID_PARAM       Invalid parameter
*/
FRFXLL_RESULT FRFXLL_EXPORT FRFXLLGetLibraryVersion(
  FRFXLL_VERSION * pVersionInfo         ///< [out] version information
);

/**
Close handle

\retval FRFXLL_OK                      The operation completed successfully.
\retval FRFXLL_ERR_INVALID_HANDLE      Handle is invalid. Note, that it is not an error to close NULL handle.
*/
FRFXLL_RESULT FRFXLL_EXPORT FRFXLLCloseHandle(
  FRFXLL_HANDLE handle            ///< [in] Handle to close. Object is freed when the reference count is zero
);
/**
Duplicate handle

\retval FRFXLL_OK                  The operation completed successfully.
\retval FRFXLL_ERR_INVALID_PARAM   Invalid parameter, for example a invalid pointer to handle.
\retval FRFXLL_ERR_INVALID_HANDLE  Invalid handle.
\retval FRFXLL_ERR_NO_MEMORY       No enough memory to complete the operation
*/
FRFXLL_RESULT FRFXLL_EXPORT FRFXLLDuplicateHandle(
  FRFXLL_HANDLE handle,          ///< [in] Handle to copy
  FRFXLL_HANDLE_PT pHandle       ///< [out] Pointer where to put a new handle
);

/**
Feature extraction function
\note This function is not intended for processing the image as it is being read (streaming processing)

\retval FRFXLL_OK                        The operation completed successfully.
\retval FRFXLL_ERR_INVALID_PARAM         Invalid parameter, for example incorrect data type.
\retval FRFXLL_ERR_INVALID_HANDLE        Invalid context handle.
\retval FRFXLL_ERR_NO_MEMORY             No enough memory to complete the operation
\retval FRFXLL_ERR_FB_TOO_SMALL_AREA     Fingerprint area is too small
\retval FRFXLL_ERR_INVALID_IMAGE         Invalid image data
*/
FRFXLL_RESULT FRFXLL_EXPORT FRFXLLCreateFeatureSet(
  FRFXLL_HANDLE hContext,          ///< [in] Handle to a fingerprint recognition context
  const unsigned char fpData[],    ///< [in] sample
  size_t size,                     ///< [in] size of the sample buffer
  FRFXLL_DATA_TYPE dataType,       ///< [in] type of the sample, for instance image format
  unsigned int flags,              ///< [in] set to 0 for default or bitwise or of any of the FRFXLL_FEX_xxx flags
  FRFXLL_HANDLE_PT phFeatureSet    ///< [out] pointer to where to put an open handle to the feature set
);

/**
Feature extraction function that re-uses the image buffer for the intermediate data:
as the result function uses very small amount of memory, but the content of image buffer is not preserved
\note This function is not intended for processing the image as it is being read (streaming processing)

\retval FRFXLL_OK                        The operation completed successfully.
\retval FRFXLL_ERR_INVALID_PARAM         Invalid parameter, for example incorrect data type.
\retval FRFXLL_ERR_INVALID_HANDLE        Invalid context handle.
\retval FRFXLL_ERR_NO_MEMORY             No enough memory to complete the operation
\retval FRFXLL_ERR_FB_TOO_SMALL_AREA     Fingerprint area is too small
\retval FRFXLL_ERR_INVALID_IMAGE         Invalid image data
*/
FRFXLL_RESULT FRFXLL_EXPORT FRFXLLCreateFeatureSetInPlace(
  FRFXLL_HANDLE hContext,          ///< [in] Handle to a fingerprint recognition context
  unsigned char fpData[],          ///< [in] fingerprint sample, buffer is overridden during feature extraction to save memory
  size_t size,                     ///< [in] size of the sample buffer
  FRFXLL_DATA_TYPE dataType,       ///< [in] type of the sample, for instance image format
  unsigned int flags,              ///< [in] set to 0 for default or bitwise or of any of the FRFXLL_FEX_xxx flags
  FRFXLL_HANDLE_PT phFeatureSet    ///< [out] pointer to where to put an open handle to the feature set
);

/**
Feature extraction function using raw pixel array as an input
\note This function is not intended for processing the image as it is being read (streaming processing)

\retval FRFXLL_OK                        The operation completed successfully.
\retval FRFXLL_ERR_INVALID_PARAM         Invalid parameter, for example incorrect data type.
\retval FRFXLL_ERR_INVALID_HANDLE        Invalid context handle.
\retval FRFXLL_ERR_NO_MEMORY             No enough memory to complete the operation
\retval FRFXLL_ERR_FB_TOO_SMALL_AREA     Fingerprint area is too small
\retval FRFXLL_ERR_INVALID_IMAGE         Invalid image data
*/
FRFXLL_RESULT FRFXLLCreateFeatureSetFromRaw(
  FRFXLL_HANDLE hContext,          ///< [in] Handle to a fingerprint recognition context
  const unsigned char pixels[],    ///< [in] sample as 8bpp pixel array (no line padding for alignment)
  size_t size,                     ///< [in] size of the sample buffer
  unsigned int width,              ///< [in] width of the image
  unsigned int height,             ///< [in] heidht of the image
  unsigned int imageResolution,    ///< [in] image resolution [DPI]
  unsigned int flags,              ///< [in] Set to 0 for default or bitwise or of any of the FRFXLL_FEX_xxx flags
  FRFXLL_HANDLE_PT phFeatureSet    ///< [out] pointer to where to put an open handle to the feature set
);

/**
Feature extraction function using raw pixel array as an input that re-uses this array for the intermediate data:
as the result function uses very small amount of memory, but the content of pixel array is not preserved
\note This function is not intended for processing the image as it is being read (streaming processing)

\retval FRFXLL_OK                        The operation completed successfully.
\retval FRFXLL_ERR_INVALID_PARAM         Invalid parameter, for example incorrect data type.
\retval FRFXLL_ERR_INVALID_HANDLE        Invalid context handle.
\retval FRFXLL_ERR_NO_MEMORY             No enough memory to complete the operation
\retval FRFXLL_ERR_FB_TOO_SMALL_AREA     Fingerprint area is too small
\retval FRFXLL_ERR_INVALID_IMAGE         Invalid image data
*/
FRFXLL_RESULT FRFXLLCreateFeatureSetInPlaceFromRaw(
  FRFXLL_HANDLE hContext,          ///< [in] Handle to a fingerprint recognition context
  unsigned char pixels[],          ///< [in] sample as 8bpp pixel array (no line padding for alignment)
  size_t size,                     ///< [in] size of the sample buffer
  unsigned int width,              ///< [in] width of the image
  unsigned int height,             ///< [in] heidht of the image
  unsigned int imageResolution,    ///< [in] image resolution [DPI]
  unsigned int flags,              ///< [in] Set to 0 for default or bitwise or of any of the FRFXLL_FEX_xxx flags
  FRFXLL_HANDLE_PT phFeatureSet    ///< [out] pointer to where to put an open handle to the feature set
);

/* Data types for export */
#define FRFXLL_DT_ISO_FEATURE_SET     ((FRFXLL_DATA_TYPE)0x01018001)  ///< Fingerprint feature set in ISO data format.
#define FRFXLL_DT_ANSI_FEATURE_SET    ((FRFXLL_DATA_TYPE)0x001B8001)  ///< Fingerprint feature set in ANSI data format.

/* Structure for the feature set export parameters */
/* Certaim parameters can be unspecified by using one of the values below */
#define FRFXLL_CBEFF_NOT_SPECIFIED             0
#define FRFXLL_FINGER_POSITION_NOT_SPECIFIED   0xFFFF
#define FRFXLL_VIEW_NUMBER_NOT_SPECIFIED       0xFFFF
#define FRFXLL_RESOLUTION_NOT_SPECIFIED        0xFFFF
#define FRFXLL_IMAGE_SIZE_NOT_SPECIFIED        0xFFFF

typedef struct tag_FRFXLL_OUTPUT_PARAM_ISO_ANSI {
  size_t length;                     ///< size of this structure, for extensibility
  unsigned int    CBEFF;
  unsigned short  fingerPosition;
  unsigned short  viewNumber;
  unsigned short  resolutionX;       ///< in pixels per cm
  unsigned short  resolutionY;       ///< in pixels per cm
  unsigned short  imageSizeX;        ///< in pixels
  unsigned short  imageSizeY;        ///< in pixels
  unsigned char   rotation;          ///< rotation in degrees clockwise> * 128 / 180, keep in mind rounding
  unsigned char   fingerQuality;     ///< finger quality is a mandatory field ISO 19794-2 and ANSI 378, 
  unsigned char   impressionType;    ///< finger impression type
} FRFXLL_OUTPUT_PARAM_ISO_ANSI;

/**
Exports a fingerprint template, or a fingerprint feature set, or a verification alignment data, that allows to specify export parameters<p>

If the buffer specified by the pbData parameter is not large enough to hold the returned data, the function
returns FRFXLL_ERR_MORE_DATA code and stores the required buffer size, in bytes, in the variable pointed to
by pcbData.<p>

If NULL is input for pbData and pcbData is not NULL, FRFXLL_OK is returned, and the function returns the
size, in bytes, of the needed memory buffer in the variable pointed to by pcbData. This lets an application
determine the size of, and the best way to allocate, a buffer for the returned data.

\note When NULL is input for pbData to determine the size needed to ensure that the returned data fits in the
specified buffer, the second call to the function which populates the buffer with the desired data may not use
the whole buffer. After the second call, the actual size of the data returned is contained in pcbData. Use this
size when processing the data.

\retval FRFXLL_OK
\retval FRFXLL_ERR_INVALID_PARAM       Invalid parameter
\retval FRFXLL_ERR_INVALID_HANDLE      Handle is invalid
\retval FRFXLL_ERR_MORE_DATA           More data is available, when this code is returned the content 
                                   of the buffer pointed by pbData is undefined
*/
FRFXLL_RESULT FRFXLL_EXPORT FRFXLLExport(
  FRFXLL_HANDLE handle,          ///< [in] Handle to data object to export
  FRFXLL_DATA_TYPE dataType,     ///< [in] Type and format of data to export
  const FRFXLL_OUTPUT_PARAM_ISO_ANSI * parameters,     ///< [in] parameters structure, specific to the data type
  unsigned char pbData[],        ///< [out] Buffer where to export the data, optional
  size_t * pcbData               ///< [in/out] Pointer where to store the length of exported data, optional
);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __FRFXLL_h
