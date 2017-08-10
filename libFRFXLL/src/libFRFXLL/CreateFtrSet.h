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

#ifndef __CREATEFTRSET_H
#define __CREATEFTRSET_H

#include "FpDataObj.h"
#include "FeatureExtraction.h"

namespace FingerJetFxOSE {
  namespace FpRecEngineImpl {
    using namespace FIR;
    template <class FeatureExtraction>
    struct FeatureExtractionObj  : public Signature<0x65787446, 0x00727478>, public Object, public HResult {
      FeatureExtraction fex;

      FeatureExtractionObj(const Context * ctx_) 
        : Object(ctx_)
        , fex(ctx_->settings.fex)
      {
      }
      template <class T>
      FRFXLL_RESULT CreateFeatureSet(
        T data[],                      ///< [in] sample
        size_t size,                   ///< [in] size of the sample buffer
        FRFXLL_DATA_TYPE dataType,       ///< [in] type of the sample, for instance image format
        uint32       flags,
        FRFXLL_HANDLE * phFtrSet         ///< [out] handle to feature set
      ) {
        Ptr<FpFtrSetObj> ftrSet(new(ctx) FpFtrSetObj(ctx));
        if (!ftrSet) {
          rc = CheckResult(FRFXLL_ERR_NO_MEMORY);
          return rc;
        }
        switch (dataType) {
          case FRFXLL_DT_ANSI_381_SAMPLE:
            rc = fex.template FromFIRSample<ANSIImageRecord>(data, size, flags, ftrSet->fpFtrSet);
            break;
          case FRFXLL_DT_ISO_19794_4_SAMPLE:
            rc = fex.template FromFIRSample<ISOImageRecord>(data, size, flags, ftrSet->fpFtrSet);
            break;
          case FRFXLL_DT_RAW_SAMPLE:
            rc = fex.template FromFIRSample<RawImageRecord>(data, size, flags, ftrSet->fpFtrSet);
            break;
          default:
            rc = FRFXLL_ERR_INVALID_PARAM;
            break;
        }
        return ftrSet->GetHandle(phFtrSet, GetResult());
      }

      template <class T>
      FRFXLL_RESULT CreateFeatureSet(
        T data[],                      ///< [in] sample
        size_t size,                   ///< [in] size of the sample buffer
        uint32 width,                  ///< [in] width of the image
        uint32 height,                 ///< [in] heidht of the image
        uint32 dpi,                    ///< [in] image resolution [DPI]
        uint32 flags,                  ///< [in] select which features of the algorithm to use
        FRFXLL_HANDLE * phFtrSet       ///< [out] handle to feature set
      ) {
        if (width > 2000 || height > 2000)                         return FRFXLL_ERR_INVALID_IMAGE;
        if (dpi < 300 || dpi > 1024)                               return FRFXLL_ERR_INVALID_IMAGE;
        if (width * 500 < 150 * dpi  || width * 500 > 812 * dpi)   return FRFXLL_ERR_INVALID_IMAGE; // in range 0.3..1.62 in
        if (height * 500 < 150 * dpi || height * 500 > 1000 * dpi) return FRFXLL_ERR_INVALID_IMAGE; // in range 0.3..2.0 in
        
        Ptr<FpFtrSetObj> ftrSet(new(ctx) FpFtrSetObj(ctx));
        if (!ftrSet) {
          rc = CheckResult(FRFXLL_ERR_NO_MEMORY);
          return rc;
        }
        rc = fex.FromRawSample(data, size, width, height, dpi, flags, ftrSet->fpFtrSet);
        return ftrSet->GetHandle(phFtrSet, GetResult());
      }
    };
  }
}
#endif // __CREATEFTRSET_H
