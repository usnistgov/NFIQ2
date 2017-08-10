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

#ifndef __TESTDELAY_H
#define __TESTDELAY_H

#include "delay.h"
#include "lfsr.h"
using namespace FingerJetFxOSE::FpRecEngineImpl::Embedded;

class TestDelay : public CxxTest::TestSuite {
public:
  // Normally skip this test: it is taking too long
  void testDelayUint8() {
    size_t dt = 17;
    delay<uint8, 20> d(dt);
    size_t i = 0;
    for (; i < dt; i++) {
      TS_ASSERT_EQUALS(0, d(uint8(i)));
    }
    for (; i < 100; i++) {
      TS_ASSERT_EQUALS(i - dt, d(uint8(i)));
    }
  }
  template <size_t size, size_t dt>
  void _TestDelayBool() {
    LFSR l1, l2;
    delay<bool, size> d(dt);
    size_t i = 0;
    for (; i < dt; i++) {
      TS_ASSERT_EQUALS(false, d(l1.NextBit()));
    }
    for (; i < 100; i++) {
      TS_ASSERT_EQUALS(l2.NextBit(), d(l1.NextBit()));
    }
  }
  void testDelayBool20_17() {
    _TestDelayBool<20,17>();
  }
  void testDelayBool20_16() {
    _TestDelayBool<20,16>();
  }
  void testDelayBool20_15() {
    _TestDelayBool<20,15>();
  }
  void testDelayBool20_20() {
    _TestDelayBool<20,20>();
  }
  void testDelayBool24_24() {
    _TestDelayBool<24,24>();
  }
  void testDelayBool24_16() {
    _TestDelayBool<24,16>();
  }
  void testDelayBool24_17() {
    _TestDelayBool<24,17>();
  }
  void testDelayBool24_1() {
    _TestDelayBool<24,1>();
  }
  void testDelayBool20_1() {
    _TestDelayBool<20,1>();
  }
  void testDelayBoolUint() {
    LFSR l1, l2;
    size_t dt = 17;
    delay<uint8, 20> d(dt);
    size_t i = 0;
    for (; i < dt; i++) {
      TS_ASSERT_EQUALS(0, d(l1.NextBit()));
    }
    for (; i < 100; i++) {
      TS_ASSERT_EQUALS(uint8(l2.NextBit()), d(l1.NextBit()));
    }
  }
};

#endif // __TESTDELAY_H
