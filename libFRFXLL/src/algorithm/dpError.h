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

#ifndef __DPERROR_H
#define __DPERROR_H

#include "../../include/FRFXLL_Results.h"

#ifndef CheckResult
#define CheckResult(rc) DiagCheckResult(rc, __LINE__, __FILE__)
#endif

#if 0
#include <stdio.h>
namespace FingerJetFxOSE {
  namespace FpRecEngineImpl {
    inline FRFXLL_RESULT DiagCheckResult(FRFXLL_RESULT rc, int line, const char * file) {
      if (rc < FRFXLL_OK) {
        printf("%s(%d) : warning %08x : DiagCheckResult rc < FRFXLL_OK", file, line, rc);
        return rc; 
      }
      return rc; 
    }
  }
}
#else
namespace FingerJetFxOSE {
  namespace FpRecEngineImpl {
    inline FRFXLL_RESULT DiagCheckResult(FRFXLL_RESULT rc, int, const char *) {
      if (rc < FRFXLL_OK) {
        return rc; 
      }
      return rc; 
    }
  }
}
#endif

namespace FingerJetFxOSE {
  namespace FpRecEngineImpl {
    template <class T>
    struct HResultT {
      T rc;
      HResultT() : rc(FRFXLL_OK) {}
      HResultT(T rc_) : rc(rc_) {}
      template <class Other>
      HResultT(HResultT<Other> & hr) : rc(hr.rc) {}
      FRFXLL_RESULT GetResult() const { return rc; }
      bool IsBad() const { return rc < FRFXLL_OK; }
      bool IsBad(FRFXLL_RESULT rc_) { rc = rc_; return IsBad(); }
      operator FRFXLL_RESULT() const { return rc; }
    };

    typedef HResultT<FRFXLL_RESULT> HResult;
    typedef HResultT<FRFXLL_RESULT &> HResultRef;

  }
}
# define CheckR(rc_) { FRFXLL_RESULT __local_rc = CheckResult(rc_); if (__local_rc < FRFXLL_OK) return __local_rc;  }
# define CheckV(rc_) { FRFXLL_RESULT __local_rc = CheckResult(rc_); if (__local_rc < FRFXLL_OK) { rc = __local_rc; return; } }
# define AssertR(expr, err) CheckR( (expr) ? FRFXLL_OK : (err) )
# define AssertV(expr, err) CheckV( (expr) ? FRFXLL_OK : (err) )
# define CheckInvalidFlagsCombinationR(flags, value) AssertR(((flags) & (value)) != (value), FRFXLL_ERR_INVALID_PARAM)

#endif // __DPERROR_H

