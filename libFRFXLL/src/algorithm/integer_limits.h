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

#ifndef __integer_limits_h
#define __integer_limits_h

#include "static_assert.h"
#include <limits>
#include "dpTypes.h"

namespace FingerJetFxOSE {
  template <class T, bool singed_>
  class integer_limits_base : public std::numeric_limits<T> {
  protected:
    static const T _min = 0;
  };
  template <class T>
  class integer_limits_base<T, true> : public std::numeric_limits<T> {
  protected:
    static const T _min = T(typename integer<sizeof(T), false>::type(1) << std::numeric_limits<T>::digits);
  };
  template <class T>
  class integer_limits : public integer_limits_base<T, std::numeric_limits<T>::is_signed> {
    typedef integer_limits_base<T, std::numeric_limits<T>::is_signed> base_t;
    STATIC_ASSERT(std::numeric_limits<T>::is_integer);
  public:
    typedef typename integer<sizeof(T), false>::type unsigned_t;
    typedef typename integer<sizeof(T), true>::type  signed_t;

    static const T min = base_t::_min;
    static const T max = T(unsigned_t(min) - 1);
  };
} // namespace

#endif // __integer_limits_h
