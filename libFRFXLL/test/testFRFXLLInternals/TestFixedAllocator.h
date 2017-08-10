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
      BINARY: testFRFXLLInternals - Unit Tests for Fingerprint Feature Extractor Internals
      
      ALGORITHM:      Alexander Ivanisov
                      Yi Chen
                      Salil Prabhakar
      IMPLEMENTATION: Alexander Ivanisov
                      Jacob Kaminsky
                      Lixin Wei
      DATE:           11/08/2011
*/

#ifndef __TestFixedAllocator_h
#define __TestFixedAllocator_h

#include "fixed_allocator.h"

namespace {
  using namespace FingerJetFxOSE;
  class TestFixedAllocator : public CxxTest::TestSuite {
  public:
    void testVector() {
      vector<int, 3> va;
      va.push_back(5);
      va.push_back(3);
      va.push_back(2);
      TS_ASSERT_EQUALS(5, va[0]);
      TS_ASSERT_EQUALS(3, va[1]);
      TS_ASSERT_EQUALS(2, va[2]);
    }
  };
}

#endif
