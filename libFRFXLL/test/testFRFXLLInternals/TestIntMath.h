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

#ifndef __TESTINTMATH_H
#define __TESTINTMATH_H

#include <intmath.h>

using namespace FingerJetFxOSE;

class TestIntMath : public CxxTest::TestSuite {
public:
  template <class T>
  void _testDivideS() {
    TS_ASSERT_EQUALS(2, divide<T>(3, 2));
    TS_ASSERT_EQUALS(1, divide<T>(2, 2));
    TS_ASSERT_EQUALS(1, divide<T>(1, 2));
    TS_ASSERT_EQUALS(0, divide<T>(0, 2));
    TS_ASSERT_EQUALS(-2, divide<T>(-3, 2));
    TS_ASSERT_EQUALS(-1, divide<T>(-2, 2));
    TS_ASSERT_EQUALS(-1, divide<T>(-1, 2));
    TS_ASSERT_EQUALS(2, divide<T>(-3,-2));
    TS_ASSERT_EQUALS(1, divide<T>(-2,-2));
    TS_ASSERT_EQUALS(1, divide<T>(-1,-2));
    TS_ASSERT_EQUALS(-2, divide<T>(3,-2));
    TS_ASSERT_EQUALS(-1, divide<T>(2,-2));
    TS_ASSERT_EQUALS(-1, divide<T>(1,-2));
    TS_ASSERT_EQUALS(0, divide<T>(0,-2));

    TS_ASSERT_EQUALS(1, divide<T>(4, 3));
    TS_ASSERT_EQUALS(2, divide<T>(5, 3));
    TS_ASSERT_EQUALS(1, divide<T>(-4,-3));
    TS_ASSERT_EQUALS(-1, divide<T>(4,-3));
    TS_ASSERT_EQUALS(-1, divide<T>(-4,3));
    TS_ASSERT_EQUALS(2, divide<T>(-5,-3));
    TS_ASSERT_EQUALS(-2, divide<T>(5,-3));
    TS_ASSERT_EQUALS(-2, divide<T>(-5,3));
  }
  template <class T>
  void _testDivideU() {
    TS_ASSERT_EQUALS(2, divide<T>(3, 2));
    TS_ASSERT_EQUALS(1, divide<T>(2, 2));
    TS_ASSERT_EQUALS(1, divide<T>(1, 2));
    TS_ASSERT_EQUALS(0, divide<T>(0, 2));
    TS_ASSERT_EQUALS(0, divide<T>(0, 2));
    TS_ASSERT_EQUALS(1, divide<T>(4, 3));
    TS_ASSERT_EQUALS(2, divide<T>(5, 3));
  }
  void testDivide() {
    _testDivideS<signed>();
    _testDivideU<unsigned>();
    _testDivideS<long long>();
    _testDivideU<unsigned long long>();
  }
  void testMinus10Log10() {
    TS_ASSERT_EQUALS(SHRT_MAX, minus10_log10(0));
    TS_ASSERT_EQUALS(0, minus10_log10(INT_MAX));
    TS_ASSERT_EQUALS(20, minus10_log10(INT_MAX/100));
    TS_ASSERT_EQUALS(40, minus10_log10(INT_MAX/10000));
  }
  void test10Log10() {
    TS_ASSERT_EQUALS(SHRT_MIN, ten_log10(0));
    TS_ASSERT_EQUALS(0, ten_log10(1));
    TS_ASSERT_EQUALS(10, ten_log10(10));
    TS_ASSERT_EQUALS(20, ten_log10(100));
    TS_ASSERT_EQUALS(60, ten_log10(1000000));
  }
  void test_log2_factorial() {
    const static int32 table[] = {
      0, 0, 256, 662, 1174, 1768, 2430, 3149, 3917, 4728, 5579, 6464, 7382, 8329, 
      9304, 10304, 11328, 12374, 13442, 14529, 15636, 16760, 17902, 19060, 20234, 
      21422, 22626, 23843, 25074, 26317, 27574, 28842, 30122, 31413, 32716, 34029,
      35352, 36686, 38029, 39382, 40745, 42116, 43497, 44886, 46283, 47689, 49103,
      50525, 51955, 53392, 54837, 56289, 57749, 59215, 60688, 62168, 63655, 65148,
      66648, 68154, 69666, 71184, 72709, 74239, 75775, 77316, 78864, 80417, 81975,
      83539, 85108, 86682, 88262, 89846, 91436, 93031, 94630, 96234, 97843, 99457,
      101076, 102699, 104326, 105958, 107595, 109235, 110881, 112530, 114184,
      115841, 117503, 119169, 120839, 122513, 124191, 125873, 127559, 129249,
      130942, 132639, 134340, 136044, 137752, 139464, 141180, 142898, 144621,
      146347, 148076, 149808, 151544, 153284, 155027, 156773, 158522, 160274,
      162030, 163789, 165551, 167316, 169084, 170855, 172629, 174407, 176187,
      177970, 179756, 181545, 183337, 185132, 186930, 188731, 190534, 192340,
      194149, 195961, 197775, 199592, 201412, 203234, 205059, 206887, 208718,
      210550, 212386, 214224, 216065, 217908, 219753, 221601, 223452, 225305,
      227161, 229018, 230879, 232741, 234606, 236474, 238344, 240216, 242090,
      243967, 245846, 247727, 249611, 251496, 253384, 255275, 257167, 259062,
      260959, 262858, 264759, 266662, 268567, 270475, 272384, 274296, 276210,
      278126, 280044, 281964, 283886, 285810, 287736, 289664, 291594, 293526,
      295460, 297396, 299334, 301273, 303215, 305159, 307104, 309052, 311001,
      312952, 314906, 316861, 318817, 320776, 322737, 324699, 326663, 328629,
      330597, 332566, 334538, 336511, 338486, 340462, 342440, 344421, 346402,
      348386, 350371, 352358, 354347, 356337, 358329, 360323, 362318, 364315,
      366314, 368314, 370316, 372320, 374325, 376332, 378340, 380350, 382362,
      384375, 386390, 388406, 390424, 392444, 394465, 396488, 398512, 400537,
      402565, 404593, 406624, 408656, 410689, 412724, 414760, 416798, 418837,
      420878, 422920, 424963, 427008, 429055
    };
    for (uint32 i = 0; i < countof(table); i++) {
      TSM_ASSERT_DELTA(i, table[i], log2_factorial_table(i), int32(i >> 1));
      TSM_ASSERT_DELTA(i, table[i], log2_factorial_ramanujan(i), int32(i >> 1) + 1);
      TSM_ASSERT_DELTA(i, table[i], log2_factorial_stirling(i), int32(i >> 1) + 16);
    }
  }
};

#endif // __TESTINTMATH_H

