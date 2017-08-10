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

#define VER_STRINGIZE_(x) #x
#define VER_STRINGIZE(x) VER_STRINGIZE_(x)

#define VERSION VER_STRINGIZE(FRFXLL_MAJOR) "." VER_STRINGIZE(FRFXLL_MINOR)"." \
                VER_STRINGIZE(FRFXLL_REVISION) "." VER_STRINGIZE(FRFXLL_BUILD)

FRFXLL_RESULT FRFXLLCreateContext(
  FRFXLL_CONTEXT_INIT * contextInit,  ///< [in] pointer to filled context initialization structure
  FRFXLL_HANDLE_PT phContext          ///< [out] pointer to where to put an open handle to created context
) {
  FRFXLL_CONTEXT_INIT ctxi = {0};
  if (contextInit == NULL) return CheckResult(FRFXLL_ERR_INVALID_PARAM);
  if (phContext == NULL) return CheckResult(FRFXLL_ERR_INVALID_PARAM);
  size_t length = contextInit->length;
  switch (length) {
    case offsetof(FRFXLL_CONTEXT_INIT, interlocked_increment):
    case sizeof(FRFXLL_CONTEXT_INIT): break;
    default: return CheckResult(FRFXLL_ERR_INVALID_PARAM);
  }
  memcpy(&ctxi, contextInit, length);
  const Context * ctx = new(&ctxi) Context(ctxi);
  if (ctx == NULL) return CheckResult(FRFXLL_ERR_NO_MEMORY);
  Ptr<const Context> pctx = ctx;
  // TODO: update settings here as needed
  return ctx->GetHandle(phContext);
}

FRFXLL_RESULT FRFXLLCloseHandle(
  FRFXLL_HANDLE handle            ///< [in] Handle to close. Object is freed when the reference count is zero
) {
  if (handle == NULL) return FRFXLL_OK; // closing NULL handle is not an error (like in case of operator delete)
  Handle * ph = reinterpret_cast<Handle *>(handle);
  if (ph->Check()) {
    delete ph;
    return FRFXLL_OK;
  }
  ConstHandle * pch = reinterpret_cast<ConstHandle *>(handle);
  if (pch->Check()) {
    delete pch;
    return FRFXLL_OK;
  }
  return CheckResult(FRFXLL_ERR_INVALID_HANDLE);
}
