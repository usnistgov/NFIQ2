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

#ifndef __COMPLEX_H
#define __COMPLEX_H

#include "dpTypes.h"
#include "complex"
#include "limits"
#include "intmath.h"

namespace FingerJetFxOSE {
  namespace FpRecEngineImpl {
    namespace Embedded {

      using std::numeric_limits;
      using std::complex;

      template <class T> 
      inline T sign(const T & x) {
        return x > 0 ? 1 : x < 0 ? -1 : 0;
      }

      template <class T> 
      inline T oct_abs(const std::complex<T> & t) {
        T x = FingerJetFxOSE::abs(t.real());
        T y = FingerJetFxOSE::abs(t.imag());
        T n = FingerJetFxOSE::max(x, y);
        return FingerJetFxOSE::max(n, FingerJetFxOSE::reduce((x + y) * 181, 8));
      }

      template <class T> 
      inline complex<int8> oct_sign(const std::complex<T> & t, const T & threshold = 0) {
        T x = FingerJetFxOSE::abs(t.real());
        T y = FingerJetFxOSE::abs(t.imag());
        T n = FingerJetFxOSE::max(x, y) / 127;
        if (n <= threshold) return 0;
        n = FingerJetFxOSE::max(n, (x + y) / 180);
        return complex<int8>(t / n);
      }

      template <class T> 
      inline complex<T> operator >> (const std::complex<T> & t, uint8 n) {
        return complex<T>(t.real() >> n, t.imag() >> n);
      }

      template <class T> 
      inline complex<T> operator << (const std::complex<T> & t, uint8 n) {
        return complex<T>(t.real() << n, t.imag() << n);
      }

      template <class T> 
      inline complex<T> & operator <<= (std::complex<T> & t, uint8 n) {
        t = t << n;
        return t;
      }

      using FingerJetFxOSE::reduce;

      template <class T> 
      inline complex<T> reduce(const std::complex<T> & a, uint8 n){
        const T adder = 1 << (n-1);
        return (a + complex<T>(adder, adder)) >> n;
      }

    }
  }
}

#endif // __COMPLEX_H

