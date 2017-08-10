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

#include "CreateFtrSet.h"

typedef FeatureExtractionObj<Engine::FeatureExtractionInPlace> FexObj;
FRFXLL_RESULT FRFXLLCreateFeatureSetInPlace(
  FRFXLL_HANDLE hContext,          ///< [in] Handle to a fingerprint recognition context
  unsigned char fpData[],          ///< [in] fingerprint sample, buffer is overridden during feature extraction to save memory
  size_t size,                     ///< [in] size of the sample buffer
  FRFXLL_DATA_TYPE dataType,       ///< [in] type of the sample, for instance image format
  unsigned int flags,              ///< [in] Set to 0 for default or bitwise or of any of the FRFXLL_FEX_xxx flags
  FRFXLL_HANDLE_PT phFeatureSet    ///< [out] pointer to where to put an open handle to the feature set
) {
  if (fpData == NULL)  return CheckResult(FRFXLL_ERR_INVALID_PARAM);
  if (phFeatureSet == NULL) return CheckResult(FRFXLL_ERR_INVALID_PARAM);
  CheckInvalidFlagsCombinationR(flags, FRFXLL_FEX_DISABLE_ENHANCEMENT | FRFXLL_FEX_ENABLE_ENHANCEMENT);
  Ptr<const Context> ctx(hContext);
  if (!ctx) return CheckResult(FRFXLL_ERR_INVALID_HANDLE);
  Ptr<FexObj> fex(new(ctx) FexObj(ctx));
  if (!fex) return CheckResult(FRFXLL_ERR_NO_MEMORY);
  return fex->CreateFeatureSet(fpData, size, dataType, flags, phFeatureSet);
}
