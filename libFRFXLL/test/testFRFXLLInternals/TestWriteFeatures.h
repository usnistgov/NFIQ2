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

#ifndef __TESTWRITEFEATURES_H
#define __TESTWRITEFEATURES_H

#include <string.h>
#include "serializeFpData.h"
#include <algorithm> // sort
#include "TestFingerMinutiaRecords.h"
namespace {

  inline bool CompMinConf1 (const Minutia & mp1, const Minutia & mp2) {
    if (mp1.conf > mp2.conf ) return true;
    if (mp1.conf < mp2.conf ) return false;
    if (mp1.position.y < mp2.position.y) return true;
    if (mp1.position.y > mp2.position.y) return false;
    if (mp1.position.x < mp2.position.x) return true;
    if (mp1.position.x > mp2.position.x) return false;
    if (mp1.theta < mp2.theta) return true;
    if (mp1.theta > mp2.theta) return false;
    return true;
  }

  class TestWriteFeatures : public CxxTest::TestSuite {
    MatchData mdIn;
  public:
    void setUp() {
      memset(&mdIn, 0xaa, sizeof(mdIn));
      TS_ASSERT_EQUALS(FRFXLL_OK, ReadIsoFeatures(iso_fmd, iso_fmd_len, mdIn, NULL));
    }
    void testWriteIsoFeatures() {
      uint8 data[600] = {0};
      size_t size = sizeof(data);
      memset(&data, 0x55, size);
      TS_ASSERT_EQUALS(FRFXLL_OK, WriteIsoFeatures(data, size, mdIn, 0, false));
      Reader r(data, size, true);
      r.cur = r.start + 14;
      uint16 width = 0, height = 0;
      r >> width >> height;
      r.cur = r.start + 27;
      uint8 nMinu = 0;
      r >> nMinu;
      uint16 xMax = 0, yMax = 0;
      for (int i = 0; i < nMinu; i++) {
        uint16 x = 0, y = 0;
        r >> x >> y;
        x &= 0x3fff;
        y &= 0x3fff;
        xMax = max(x, xMax);
        yMax = max(y, yMax);
        r.skip(2);
      }
      TS_ASSERT_LESS_THAN(xMax, width);
      TS_ASSERT_LESS_THAN(yMax, height);
    }
    void testWriteAnsiFeatures() {
      uint8 data[600] = {0};
      size_t size = sizeof(data);
      memset(&data, 0x55, size);
      TS_ASSERT_OK(WriteAnsiFeatures(data, size, mdIn, 0, false));
      Reader r(data, size, true);
      r.cur = r.start + 16;
      uint16 width = 0, height = 0;
      r >> width >> height;
      r.cur = r.start + 29;
      uint8 nMinu = 0;
      r >> nMinu;
      uint16 xMax = 0, yMax = 0;
      for (int i = 0; i < nMinu; i++) {
        uint16 x = 0, y = 0;
        r >> x >> y;
        x &= 0x3fff;
        y &= 0x3fff;
        xMax = max(x, xMax);
        yMax = max(y, yMax);
        r.skip(2);
      }
      TS_ASSERT_LESS_THAN(xMax, width);
      TS_ASSERT_LESS_THAN(yMax, height);
    }
  };

} // namespace

#endif // __TESTWRITEFEATURES_H
