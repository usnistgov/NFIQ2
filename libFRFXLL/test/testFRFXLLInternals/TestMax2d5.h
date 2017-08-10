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

#ifndef __TestMax2d5_h
#define __TestMax2d5_h

#include "complex.h"
#include "extract_minutia.h"
#include <string>
#include <bitset>

int32 value[10][20]={
  {0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 3, 4, 5, 6, 7, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0},
  {0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
  {0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0},
  {0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0},
};
//value[5][10];// max value; max5 is expected to return true at y = 7, x = 12

int32 value1[11][22]={
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

class TestMax2d5 : public CxxTest::TestSuite {
public:
  void Test_Max2d5() {
    static const size_t stride = 200;
    typedef FeatureExtractionImpl::max2d5<int32, stride> max5_t;
    max5_t max5;
    for (size_t y = 0; y < countof(value); y++) {
      for (size_t x = 0; x < countof(*value); x++) {
        int val = value[y][x];
        bool bIsMax = max5(val, x, y);
        if (bIsMax) {
          TS_ASSERT(y == 5 + max5.yoffs && x == 10 + max5.xoffs);
        } else {
          TS_ASSERT(y != 5 + max5.yoffs || x != 10 + max5.xoffs);
        }
      }
    }
  }

  void Test_Max2d5_fast() {
    static const size_t stride = 200;
    typedef FeatureExtractionImpl::max2d5fast<int32, stride> max5_t;
    max5_t max5;
    for (size_t y = 0; y < countof(value); y++) {
      for (size_t x = 0; x < countof(*value); x++) {
        int val = value[y][x];
        bool bIsMax = max5(val, x, y);
        if (bIsMax) {
          TS_ASSERT(y == 5 + max5.yoffs && x == 10 + max5.xoffs);
        } else {
          TS_ASSERT(y != 5 + max5.yoffs || x != 10 + max5.xoffs);
        }
      }
    }
  }

  void Test_Max2d5_fast1() {
    static const size_t stride = 200;
    typedef FeatureExtractionImpl::max2d5fast<int32, /*stride*/countof(*value1)> max5_t;
    max5_t max5;
    for (size_t y = 0; y < countof(value1); y++) {
      for (size_t x = 0; x < countof(*value1); x++) {
        int val = value1[y][x];
        bool bIsMax = max5(val, x, y);
        if (bIsMax) {
          //TS_ASSERT(y == 5 + max5.yoffs && x == 10 + max5.xoffs);
          //TS_TRACE(TS_AS_STRING(y));
          //TS_TRACE((std::string(TS_AS_STRING(y)) + ", " + std::string(TS_AS_STRING(x)) + "; " + 
          //          std::string(TS_AS_STRING(y-max5.yoffs)) + ", " + std::string(TS_AS_STRING(x-max5.xoffs)) ).c_str());
        } else {
          //TS_ASSERT(y != 5 + max5.yoffs || x != 10 + max5.xoffs);
        }
      }
    }
  }

};

#endif // __TestMax2d5_h
