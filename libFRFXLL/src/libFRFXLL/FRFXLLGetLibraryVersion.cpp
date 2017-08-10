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
      LIBRARY: FRFXLL - Fingerprint Feature Extractor - Low Level API

      ALGORITHM:      Alexander Ivanisov
                      Yi Chen
                      Salil Prabhakar
      IMPLEMENTATION: Alexander Ivanisov
                      Jacob Kaminsky
                      Lixin Wei
      DATE:           11/08/2011
*/

#include <string.h>
#include <stddef.h>
#include "Object.h"
#include "version.h"

FRFXLL_RESULT FRFXLLGetLibraryVersion(
        FRFXLL_VERSION * pVersionInfo         ///< [out] version information
) {
  if (pVersionInfo == NULL) return CheckResult(FRFXLL_ERR_INVALID_PARAM);
  pVersionInfo->major = FRFXLL_MAJOR;
  pVersionInfo->minor = FRFXLL_MINOR;
  pVersionInfo->revision = FRFXLL_REVISION;
  pVersionInfo->build = FRFXLL_BUILD;
  return FRFXLL_OK;
}
