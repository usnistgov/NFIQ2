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

#ifndef __TestTop_n_h
#define __TestTop_n_h

#include "top_n.h"
#include "lfsr.h"

using namespace FingerJetFxOSE;

typedef std::pair<int, int> int_pair;
struct int_3 {
  int_3(int _i1, int _i2, int _i3) : i1(_i1), i2(_i2), i3(_i3) {};
  int i1;
  int i2;
  int i3;
};

class TestTop_n : public CxxTest::TestSuite {
public:
  void TestTop_N_Add() {
    int buffer[11] = {0};
    for (int i = 0; i < 11; i++) buffer[i]=i;
    int_3 val_size1[] = {
      int_3(12,  1, 12),
      int_3(11,  2, 11),
      int_3( 0,  2, 11),
      int_3(10,  3, 10),
      int_3(10,  4, 10),
      int_3( 9,  5, 9),
      int_3( 8,  6, 8),
      int_3(13,  7, 8),
      int_3(15,  8, 8),
      int_3(18,  9, 8),
      int_3( 7, 10, 7),
      int_3(20, 10, 8),
      int_3( 7, 10, 8),
      int_3( 9, 10, 9),
      int_3(17, 10, 9),
      int_3(12, 10, 10),
      int_3(14, 10, 10),
      int_3(16, 10, 11),
    };
    top_n<int> tp(buffer, buffer+10, 0);

    int * beg = tp.begin();
    int * end = tp.end();
    int * edb = tp.endbuf();
    size_t size = tp.size();
    size_t cap =  tp.capacity();
    TS_ASSERT_EQUALS(tp.size(), 0);
    TS_ASSERT_EQUALS(tp.capacity(), 10);

    for (int i = 0; i < countof(val_size1); i++) {
      tp.add(val_size1[i].i1);
      TS_ASSERT_EQUALS(tp.size(), val_size1[i].i2);
      TS_ASSERT_EQUALS(isHeap(tp.begin(), tp.end()), true);
      TS_ASSERT_EQUALS(isHeapTailNotTouched(tp.begin(), tp.end(), tp.endbuf()), true);
      TS_ASSERT_EQUALS(buffer[0], val_size1[i].i3);
    }

    tp.sort();
    for (int i = 0; i < 9; i++) {
      TS_ASSERT_LESS_THAN_EQUALS(buffer[i+1], buffer[i]);
    }
  }

  void TestTop_N_AddIncreasing() {
    int buffer[11] = {0};
    for (int i = 0; i < 11; i++) buffer[i]=i;
    top_n<int> tp(buffer, buffer+10, 0);

    for (int i = 1; i < 11; i++) {
      tp.add(i);
      TS_ASSERT_EQUALS(tp.size(), i);
      TS_ASSERT_EQUALS(isHeap(tp.begin(), tp.end()), true);
      TS_ASSERT_EQUALS(isHeapTailNotTouched(tp.begin(), tp.end(), tp.endbuf()), true);
      TS_ASSERT_EQUALS(buffer[0], 1);
    }

    for (int i = 11; i < 20; i++) {
      tp.add(i);
      TS_ASSERT_EQUALS(tp.size(), 10);
      TS_ASSERT_EQUALS(isHeap(tp.begin(), tp.end()), true);
      TS_ASSERT_EQUALS(isHeapTailNotTouched(tp.begin(), tp.end(), tp.endbuf()), true);
      TS_ASSERT_EQUALS(buffer[0], i-9);
    }
    tp.sort();
    for (int i = 0; i < 9; i++) {
      TS_ASSERT_LESS_THAN_EQUALS(buffer[i+1], buffer[i]);
    }
  }

  void TestTop_N_AddDecreasing() {
    int buffer[11] = {0};
    for (int i = 0; i < 11; i++) buffer[i]=i;
    top_n<int> tp(buffer, buffer+10, 0);

    for (int i = 20; i > 10; i--) {
      tp.add(i);
      TS_ASSERT_EQUALS(tp.size(), 21 - i);
      TS_ASSERT_EQUALS(isHeap(tp.begin(), tp.end()), true);
      TS_ASSERT_EQUALS(isHeapTailNotTouched(tp.begin(), tp.end(), tp.endbuf()), true);
      TS_ASSERT_EQUALS(buffer[0], i);
    }

    for (int i = 10; i > 0; i--) {
      tp.add(i);
      TS_ASSERT_EQUALS(tp.size(), 10);
      TS_ASSERT_EQUALS(isHeap(tp.begin(), tp.end()), true);
      TS_ASSERT_EQUALS(isHeapTailNotTouched(tp.begin(), tp.end(), tp.endbuf()), true);
      TS_ASSERT_EQUALS(buffer[0], 11);
    }
    tp.sort();
    for (int i = 0; i < 9; i++) {
      TS_ASSERT_LESS_THAN_EQUALS(buffer[i+1], buffer[i]);
    }
  }

  void TestTop_N_AddRandom() {
    int buffer[11] = {0};
    for (int i = 0; i < 11; i++) buffer[i]=i;
    top_n<int> tp(buffer, buffer+10, 0);

    int SortedBuffer[10] = {0};
    int SortedBufferLen = 0;

    LFSR lfsr = 0xBADF00D;

    int iCountPositive = 0;
    for (int i = 0; i < 30; i++) {
      int Rand1 = lfsr.Next<uint16>();
      int Rand2 = Rand1 - (uint16)~(1 << 15);
      if (Rand2 > 0) {
        iCountPositive++;
        if (SortedBufferLen < 10) {
          SortedBuffer[SortedBufferLen++] = Rand2;
          std::sort(SortedBuffer, SortedBuffer + SortedBufferLen);
        } else {
          if (SortedBuffer[0] < Rand2){
            SortedBuffer[0] = Rand2;
            std::sort(SortedBuffer, SortedBuffer + SortedBufferLen);
          }
        }
      }
      tp.add(Rand2);
      TS_ASSERT_EQUALS(tp.size(), min(iCountPositive, 10));
      TS_ASSERT_EQUALS(isHeap(tp.begin(), tp.end()), true);
      TS_ASSERT_EQUALS(isHeapTailNotTouched(tp.begin(), tp.end(), tp.endbuf()), true);
      TS_ASSERT_EQUALS(buffer[0], SortedBuffer[0]);
    }
    tp.sort();
    for (int i = 0; i < 9; i++) {
      TS_ASSERT_LESS_THAN_EQUALS(buffer[i+1], buffer[i]);
    }
  }

  template <class T>
  bool isHeap(T * const begin, T * const end) {
    for (T *p=begin+1; p<end; p++) {
      if (*p < *begin) {
        return false;
      }
    }
    return true;
  }

  template <class T>
  bool isHeapTailNotTouched(T * const begin, T * const end, T * const endbuf) {
    for (T *p=end; p<endbuf+1; p++) {
      if (*p != p - begin) {
        return false;
      }
    }
    return true;
  }

};
#endif // __TestTop_n_h
