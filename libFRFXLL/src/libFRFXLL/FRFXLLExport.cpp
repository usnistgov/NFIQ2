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

#include "FpDataObj.h"

FRFXLL_RESULT FRFXLLExport(
  FRFXLL_HANDLE handle,          ///< [in] Handle to data object to export
  FRFXLL_DATA_TYPE dataType,     ///< [in] Type and format of data to export
  const FRFXLL_OUTPUT_PARAM_ISO_ANSI * parameters,     ///< [in] parameters structure, specific to the data type
  unsigned char pbData[],      ///< [out] Buffer where to export the data, optional
  size_t * pcbData             ///< [in/out] Pointer where to store the length of exported data, optional
) {
  if (pcbData == NULL) {
    return FRFXLL_ERR_INVALID_PARAM;
  }
  switch (dataType) {
    case FRFXLL_DT_ISO_FEATURE_SET:
    case FRFXLL_DT_ANSI_FEATURE_SET:
      return Invoke(&FpFtrSetObj::Export, handle, dataType, parameters, pbData, pcbData);
  }
  return FRFXLL_ERR_INVALID_PARAM;
}
