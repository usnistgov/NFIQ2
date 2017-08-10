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

#ifndef __FPDATAOBJ_H
#define __FPDATAOBJ_H

#include "Object.h"
#include <string.h>
#include "serializeFpData.h"

namespace FingerJetFxOSE {
  namespace FpRecEngineImpl {

    struct FpFtrSetObj : public Signature<0x74465046, 0x74655372>, public Object {
      MatchData fpFtrSet;

      FpFtrSetObj(const Context * ctx_)  : Object(ctx_) {}

      FRFXLL_RESULT Import(
        const unsigned char data[],  ///< [in] (fingerprint) data to import
        size_t size,                 ///< [in] size of the (fingerprint) data
        FRFXLL_DATA_TYPE dataType,   ///< [in] type and format of the (fingerprint) data
        const void * parameters      ///< [in] parameters structure, specific to the data type
      ) {
        switch (dataType) {
          case FRFXLL_DT_ISO_FEATURE_SET:
            return ReadIsoFeatures(data, size, fpFtrSet, parameters);
          case FRFXLL_DT_ANSI_FEATURE_SET:
            return ReadAnsiFeatures(data, size, fpFtrSet, parameters);
          default:
            return CheckResult(FRFXLL_ERR_INVALID_PARAM);
        }
      }

      FRFXLL_RESULT Export(
        FRFXLL_DATA_TYPE dataType,   ///< [in] type and format of the (fingerprint) data
        const void * parameters,     ///< [in] parameters structure, specific to the data type
        unsigned char pData[],       ///< [in] (fingerprint) data to import
        size_t *pSize                ///< [in] size of the (fingerprint) data
      ) const {
        FRFXLL_RESULT rc = FRFXLL_OK;
        unsigned char * data = pData;
        size_t size = data ? *pSize : size_t(-1);
        switch (dataType) {
          case FRFXLL_DT_ISO_FEATURE_SET:
            rc = WriteIsoFeatures(data, size, fpFtrSet, parameters, false);
            break;
          case FRFXLL_DT_ANSI_FEATURE_SET:
            rc = WriteAnsiFeatures(data, size, fpFtrSet, parameters, false);
            break;
          default:
            return CheckResult(FRFXLL_ERR_INVALID_PARAM);
        }
        if (rc >= FRFXLL_OK || rc == FRFXLL_ERR_MORE_DATA) {
          *pSize = size;
        }
        return rc;
      }
    };

    template <class T> inline FRFXLL_RESULT Invoke(
      FRFXLL_RESULT (T::*import_f) (
        const unsigned char data[],  ///< [in] (fingerprint) data to import
        size_t size,                 ///< [in] size of the (fingerprint) data
        FRFXLL_DATA_TYPE dataType,     ///< [in] type and format of the (fingerprint) data
        const void * parameters      ///< [in] parameters structure, specific to the data type
      ),
      const Context * ctx,         ///< [in] Context
      const unsigned char data[],  ///< [in] (fingerprint) data to import
      size_t size,                 ///< [in] size of the (fingerprint) data
      FRFXLL_DATA_TYPE dataType,     ///< [in] type and format of the (fingerprint) data
      const void * parameters,     ///< [in] parameters structure, specific to the data type
      FRFXLL_HANDLE_PT pHandle       ///< [out] pointer to where to put an open handle to the fingerprint data
    ) {
      Ptr<T> pT(new(ctx) T(ctx));
      if (!pT) return CheckResult(FRFXLL_ERR_NO_MEMORY); 
      FRFXLL_RESULT rc = (pT->*import_f)(data, size, dataType, parameters);
      if (!FRFXLL_SUCCESS(rc)) return rc;
      const T * cpT = pT;
      return cpT->GetHandle(pHandle, rc);
    }

    template <class T> FRFXLL_RESULT Invoke(
      FRFXLL_RESULT (T::*export_f) (
        FRFXLL_DATA_TYPE dataType,     ///< [in] type and format of the (fingerprint) data
        const void * parameters,     ///< [in] parameters structure, specific to the data type
        unsigned char pData[],       ///< [in] (fingerprint) data to import
        size_t *pSize                ///< [in] size of the (fingerprint) data
      ) const,
      FRFXLL_HANDLE handle,          ///< [in] Handle to data object to export
      FRFXLL_DATA_TYPE dataType,     ///< [in] Type and format of data to export
      const void * parameters,     ///< [in] parameters structure, specific to the data type
      unsigned char pbData[],      ///< [out] Buffer where to export the data, optional
      size_t * pcbData             ///< [in/out] Pointer where to store the length of exported data, optional
    ) {
      Ptr<const T> ptr(handle);
      if (!ptr) return CheckResult(FRFXLL_ERR_INVALID_HANDLE);
      size_t size = *pcbData;
      FRFXLL_RESULT rc = ((*ptr).*export_f)(dataType, parameters, pbData, pcbData); // AI: don't user ->* because of bug in ARM compiler
      if (rc < FRFXLL_OK && pbData != NULL) {
        memset(pbData, 0, size);
      }
      return rc;
    }
  }
}
#endif // __FPDATAOBJ_H
