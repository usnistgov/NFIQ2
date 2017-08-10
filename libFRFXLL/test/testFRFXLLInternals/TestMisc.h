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

#ifndef __TESTMISC_H
#define __TESTMISC_H

#ifdef WINCE
#ifdef NDEBUG
#ifdef x86
#if (_WIN32_WCE == 0x420 || _WIN32_WCE == 0x400)
extern "C" long _ftol(double);
extern "C" long _ftol2(double x) { return _ftol(x); }
#endif
#endif
#endif
#endif

#include <algorithm> // #include "dpQSort.h"
#include "lfsr.h"

using namespace FingerJetFxOSE::FpRecEngineImpl::Embedded;

class TestSort : public CxxTest::TestSuite {
  static const size_t N = 1520;
  uint32 data[N];
  static bool Comp1(const uint32 & x, const uint32 & y) {
    return x < y;
  }
public:
  void setUp() {
    LFSR lfsr = 0xBADF00D;
    for (size_t i = 0; i < N; i++) {
      data[i] = lfsr.Next<uint32>();
    };
  }
  void testRandom() {
    std::sort(data, data+N, Comp1);
    for (size_t i = 1; i < N; i++) {
      TS_ASSERT(data[i] >= data[i-1]);
      if (data[i] < data[i-1]) break;
    }
  }
};
class TestMinutia : public CxxTest::TestSuite {
public:
  void testAngles() {
    uint8 a = 0;
    do {
      Angle angle(a);
      uint8 a_ = angle;
      TS_ASSERT(a == a_);
    } while(++a);
  }
};

class TestScaleDown : public CxxTest::TestSuite {
public:
  void testScaleDown2() {
    TS_ASSERT_EQUALS(0, ScaleDown(0, 1));
    TS_ASSERT_EQUALS(1, ScaleDown(2, 1));
    TS_ASSERT_EQUALS(2, ScaleDown(4, 1));
    for (int i = -20; i <= 20; i++) {
      TS_ASSERT_EQUALS(ScaleDown(i, 1) + 1, ScaleDown(i + 2, 1));
    }
  }
  void testScaleDown4() {
    TS_ASSERT_EQUALS(-1, ScaleDown(-3, 2));
    TS_ASSERT_EQUALS(0, ScaleDown(-1, 2));
    TS_ASSERT_EQUALS(0, ScaleDown(0, 2));
    TS_ASSERT_EQUALS(0, ScaleDown(1, 2));
    TS_ASSERT_EQUALS(1, ScaleDown(3, 2));
    TS_ASSERT_EQUALS(1, ScaleDown(4, 2));
    TS_ASSERT_EQUALS(1, ScaleDown(5, 2));
    for (int i = -20; i <= 20; i++) {
      TS_ASSERT_EQUALS(ScaleDown(i, 2) + 1, ScaleDown(i + 4, 2));
    }
  }
};

class TEST_SUITE(TestRotateAndScale) {
  Point p0;
  Point p1;
  Angle a0;
  Angle a90;
  Angle a45;
public:
  void setUp() { 
    p0 = Point(100, 0);
    p1 = Point(0, 100);
    a0.Assign(0);
    a90.Assign(64);
    a45.Assign(32);
  }
  TEST(testRotate0) {
    Point p = Rotate(p0, a0);
    TS_ASSERT_EQUALS(100, p.x);
    TS_ASSERT_EQUALS(0, p.y);
  }
  TEST(testRotate90) {
    Point p = Rotate(p0, a90);
    TS_ASSERT_EQUALS(0, p.x);
    TS_ASSERT_EQUALS(100, p.y);
  }
  TEST(testRotate0Scale075) {
    Point p = RotateAndScale(p0, a0, 3, 2);
    TS_ASSERT_EQUALS(75, p.x);
    TS_ASSERT_EQUALS(0, p.y);
  }
  TEST(testP0Rotate90Scale075) {
    Point p = RotateAndScale(p0, a90, 3, 2);
    TS_ASSERT_EQUALS(0, p.x);
    TS_ASSERT_EQUALS(75, p.y);
  }
  TEST(testP1Rotate90Scale075) {
    Point p = RotateAndScale(p1, a90, 3, 2);
    TS_ASSERT_EQUALS(-75, p.x);
    TS_ASSERT_EQUALS(0, p.y);
  }
  TEST(testP0Rotate45Scale075) {
    Point p = RotateAndScale(p0, a45, 3, 2);
    TS_ASSERT_EQUALS(53, p.x);
    TS_ASSERT_EQUALS(53, p.y);
  }
};

#endif // __TESTMISC_H
