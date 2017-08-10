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

      BASED ON:       C++0x standard static_assert
      DATE:           11/08/2011
*/

#ifndef __STATIC_ASSERT_H
#define __STATIC_ASSERT_H

#include "dpTypes.h"

#define JOIN1(x, y) x##y
#define JOIN(x, y) JOIN1(x, y)

#define STATIC_ASSERT(expr) \
  enum { JOIN(__static_assert, __LINE__) = sizeof(::FingerJetFxOSE::StaticAssert::Asserter<(bool)(expr)>) }

namespace FingerJetFxOSE {
  namespace StaticAssert {

    template <bool>  class Asserter;
    template <>  class Asserter<true> { enum { x = 1 }; };

  };
};

#endif // __STATIC_ASSERT_H


