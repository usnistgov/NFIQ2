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

#ifndef FRFXLL_RC_DEFINED
#define FRFXLL_RC_DEFINED
 
#ifdef __cplusplus
extern "C" {
#endif

typedef int FRFXLL_RESULT;

/* Feedback codes should follow this FACILITY_DP_FEEDBACK << 16 | 0x8000 */
#define FRFXLL_IS_FEEDBACK(rc) (((rc) & 0x3FFF8000L) == 0x00048000L)

/**************** return codes common to different components *****************/
 // The following are message definitions.
//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +-+-+-+-+-+---------------------+-------------------------------+
//  |S|R|C|N|r|    Facility         |               Code            |
//  +-+-+-+-+-+---------------------+-------------------------------+
//
//  where
//
//      S - Severity - indicates success/fail
//
//          0 - Success
//          1 - Fail (COERROR)
//
//      R - reserved portion of the facility code, corresponds to NT's
//              second severity bit.
//
//      C - reserved portion of the facility code, corresponds to NT's
//              C field.
//
//      N - reserved portion of the facility code. Used to indicate a
//              mapped NT status value.
//
//      r - reserved portion of the facility code. Reserved for internal
//              use. Used to indicate FRFXLL_RESULT values that are not status
//              values, but are instead message ids for display strings.
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FRFXLL_FACILITY_WIN32                0x7
#define FRFXLL_FACILITY_SUCCESS              0x0
#define FRFXLL_FACILITY_ITF                  0x4
#define FRFXLL_FACILITY_DP_FEEDBACK          0x4
#define FRFXLL_FACILITY_DP_SPECIFIC          0x5BA


//
// Define the severity codes
//
#define FRFXLL_STATUS_SEVERITY_WARNING       0x1
#define FRFXLL_STATUS_SEVERITY_SUCCESS       0x0
#define FRFXLL_STATUS_SEVERITY_ERROR         0x2


//
// MessageId: FRFXLL_OK
//
// MessageText:
//
// The operation completed successfully.
//
#define FRFXLL_OK                            ((FRFXLL_RESULT)0x00000000L)

//
// MessageId: FRFXLL_ERR_FB_TOO_SMALL_AREA
//
// MessageText:
//
// User feedback: fingerprint area is too small. Most likely this is bacause the tip of the finger is presented.
//
#define FRFXLL_ERR_FB_TOO_SMALL_AREA         ((FRFXLL_RESULT)0x80048004L)

//
// MessageId: FRFXLL_ERR_INVALID_PARAM
//
// MessageText:
//
// One or more of the parameters is invalid. 
//
#define FRFXLL_ERR_INVALID_PARAM             ((FRFXLL_RESULT)0x80070057L)

//
// MessageId: FRFXLL_ERR_NO_MEMORY
//
// MessageText:
//
// There is not enough memory to perform the function.
//
#define FRFXLL_ERR_NO_MEMORY                 ((FRFXLL_RESULT)0x8007000EL)

//
// MessageId: FRFXLL_ERR_MORE_DATA
//
// MessageText:
//
// More data is available.
//
#define FRFXLL_ERR_MORE_DATA                 ((FRFXLL_RESULT)0x800700EAL)

//
// MessageId: FRFXLL_ERR_INTERNAL
//
// MessageText:
//
// An unknown internal error has occurred.
//
#define FRFXLL_ERR_INTERNAL                  ((FRFXLL_RESULT)0x8007054FL)

//
// MessageId: FRFXLL_ERR_INVALID_BUFFER
//
// MessageText:
//
// The image buffer is too small for in-place processing.
//
#define FRFXLL_ERR_INVALID_BUFFER            ((FRFXLL_RESULT)0x8007007AL)

//
// MessageId: FRFXLL_ERR_INVALID_HANDLE
//
// MessageText:
//
// The specified handle is invalid.
//
#define FRFXLL_ERR_INVALID_HANDLE            ((FRFXLL_RESULT)0x80070006L)

//
// MessageId: FRFXLL_ERR_INVALID_IMAGE
//
// MessageText:
//
// The image buffer is invalid. 
//
#define FRFXLL_ERR_INVALID_IMAGE             ((FRFXLL_RESULT)0x85BA0022L)

//
// MessageId: FRFXLL_ERR_INVALID_DATA
//
// MessageText:
//
// Supplied data is invalid.
//
#define FRFXLL_ERR_INVALID_DATA              ((FRFXLL_RESULT)0x85BA0024L)

//
// MessageId: FRFXLL_ERR_NO_FP
//
// MessageText:
//
// The specified finger or view is not present.
//
#define FRFXLL_ERR_NO_FP                     ((FRFXLL_RESULT)0x85BA010AL)

#ifdef __cplusplus
}
#endif

#endif
