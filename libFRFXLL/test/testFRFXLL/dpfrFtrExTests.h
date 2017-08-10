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
      BINARY: testFRFXLL - Unit Tests for Fingerprint Feature Extractor - LowLevel API      
      
      ALGORITHM:      Alexander Ivanisov
                      Yi Chen
                      Salil Prabhakar
      IMPLEMENTATION: Alexander Ivanisov
                      Jacob Kaminsky
                      Lixin Wei
      DATE:           11/08/2011
*/

// dpfrFtrExTests.h
//
// Extensive test cases for feature extraction.
//

#ifndef __FRFXLLFTREXTESTS_H__
#define __FRFXLLFTREXTESTS_H__

#include "dpfrUnitTest.h"

class TEST_SUITE(CreatingFeatureSetAnsi), private FixtureWithContext {
  typedef CreatingFeatureSetAnsi suite_t;
private:
  static const FRFXLL_DATA_TYPE invalidDataType = static_cast<FRFXLL_DATA_TYPE>(0xFFFFFFFF);
  FRFXLL_HANDLE hFtrSet;
  MemoryChecker memoryChecker;
  unsigned int savedCRC;
  ImageBuffer TempImage;
public:
  void setUp() {
    memoryChecker.setUp();
    FixtureWithContext::_setUp();
    hFtrSet = NULL;
    TS_ASSERT(TempImage(TestAnsiImage, sizeof(TestAnsiImage)));
    savedCRC = CalculateCRC(TempImage, TempImage);
  }
  void tearDown() {
    TS_ASSERT_EQUALS_X(CalculateCRC(TempImage, TempImage), savedCRC);
    FixtureWithContext::_tearDown();
    TS_ASSERT_MEMORY_RELEASED(memoryChecker);
    memoryChecker.tearDown();
  }
  TEST(testCreateFeatureSetWithNullContextHandle) {
    TS_ASSERT_RC(FRFXLLCreateFeatureSet(NULL, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_HANDLE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  TEST(testCreateFeatureSetWithNullFeatureSetHandlePtr) {
    TS_ASSERT_RC(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, NULL), FRFXLL_ERR_INVALID_PARAM);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  TEST(testCreateFeatureSetWithNullImageDataPtr) {
    TS_ASSERT_RC(FRFXLLCreateFeatureSet(hCtx, NULL, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_PARAM);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet))
  }
  TEST(testCreateFeatureSetWithShortImageDataSize) {
    TS_ASSERT_RC(FRFXLLCreateFeatureSet(hCtx, TempImage, 5, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  TEST(testCreateFeatureSetWithOneByteShorterImageDataSize) {
    TS_ASSERT_RC(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage.size() - 1, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  TEST(testCreateFeatureSetWithOneByteLongerImageDataSize) {
    TS_ASSERT_OK(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage.size() + 1, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  TEST(testCreateFeatureSetWithLongImageDataSize) {
    TS_ASSERT_OK(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage.max_buffer_size, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  TEST(testCreateFeatureSetOutOfMemory) {
    FailMemoryAllocation fma(0);
    TS_ASSERT_RC(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_NO_MEMORY);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  TEST(testCreateFeatureSetOutOfMemoryForFeatureSetObject) {
    FailMemoryAllocation fma(1);
    TS_ASSERT_RC(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_NO_MEMORY);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  TEST(testCreateFeatureSetOutOfMemoryForHandle) {
    FailMemoryAllocation fma(2);
    TS_ASSERT_RC(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_NO_MEMORY);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  TEST(testCreateFeatureSetMakesOnlyThreeAllocations) {
    FailMemoryAllocation fma(3);
    TS_ASSERT_OK(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  TEST(testCreateFeatureSetAsInvalidDataType) {
    TS_ASSERT_RC(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, invalidDataType, 0, &hFtrSet), FRFXLL_ERR_INVALID_PARAM);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  TEST(testCreateFeatureSetAsFeatureSetDataType) {
    TS_ASSERT_RC(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_FEATURE_SET, 0, &hFtrSet), FRFXLL_ERR_INVALID_PARAM);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  TEST(testCreateAsFeatureSetAnsiSampleDataType) {
    TS_ASSERT_OK(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
};


#define TRUNCATEDIMAGESIZE 40000

class CreatingFeatureSetWithTestImages : public CxxTest::TestSuite, private FixtureWithContext {
private:
  FRFXLL_HANDLE hFtrSet;
  MemoryChecker memoryChecker;
  unsigned int savedCRC;
public:
  void setUp() {
    memoryChecker.setUp();
    FixtureWithContext::_setUp();
    hFtrSet = NULL;
  }
  void tearDown() {
    FixtureWithContext::_tearDown();
    TS_ASSERT_MEMORY_RELEASED(memoryChecker);
    memoryChecker.tearDown();
  }
  void testCreateFeatureBadImage_ANY_BINARY() {
    ImageBufferWrapper TempImage(TestAnsiImageBad_ANY_BINARY, sizeof(TestAnsiImageBad_ANY_BINARY));
    savedCRC = CalculateCRC(TempImage, TempImage);
    TS_ASSERT_RC(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempImage, TempImage), savedCRC);
  }
  void testCreateFeatureBadImage_OneLine() {
    ImageBufferWrapper TempImage(TestAnsiImageBad_OneLine, sizeof(TestAnsiImageBad_OneLine));
    savedCRC = CalculateCRC(TempImage, TempImage);
    TS_ASSERT_RC(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempImage, TempImage), savedCRC);
  }
  // This test returns FRFXLL_ERR_FB_TOO_SMALL_AREA (0x80048004L), instead of FRFXLL_ERR_FB_IMAGE_TOO_NOISY (0x80048001); it appears that FRFXLL_ERR_FB_IMAGE_TOO_NOISY is not used
  void testCreateFeatureBadImage_CONSTANT_IMAGE() {
    ImageBuffer TempAnsiImage(0x10, sizeof(TestAnsiImage));
    TS_ASSERT(TempAnsiImage(TestAnsiImage, IMAGE_HEADER_SIZE, false));
    savedCRC = CalculateCRC(TempAnsiImage, TempAnsiImage);
    TS_ASSERT_RC(FRFXLLCreateFeatureSet(hCtx, TempAnsiImage, TempAnsiImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_FB_TOO_SMALL_AREA); // 0x80048004L
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempAnsiImage, TempAnsiImage), savedCRC);
  }
  void testCreateFeatureBrcmImage00() {
    ImageBufferWrapper TempImage(TestBrcmImage00, sizeof(TestBrcmImage00));
    savedCRC = CalculateCRC(TempImage, TempImage);
    TS_ASSERT_OK(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempImage, TempImage), savedCRC);
  }
  void testCreateFeatureAuthentecImage01() {
    ImageBufferWrapper TempImage(TestAuthentec01, sizeof(TestAuthentec01));
    savedCRC = CalculateCRC(TempImage, TempImage);
    TS_ASSERT_OK(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempImage, TempImage), savedCRC);
  }
  void testCreateFeatureAuthentecImage01WithOneByteShorter() {
    ImageBufferWrapper TempImage(TestAuthentec01, sizeof(TestAuthentec01) - 1);
    savedCRC = CalculateCRC(TempImage, TempImage);
    TS_ASSERT_RC(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempImage, TempImage), savedCRC);
  }
  void testCreateFeatureAuthentecImage01WithOneByteLonger() {
    ImageBufferWrapper TempImage(TestAuthentec01, sizeof(TestAuthentec01) + 1);
    savedCRC = CalculateCRC(TempImage, TempImage);
    TS_ASSERT_OK(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempImage, TempImage), savedCRC);
  }
  void testCreateFeatureWithTruncatedImage() {
    ImageBufferWrapper TempImage(TestAuthentec01, TRUNCATEDIMAGESIZE);
    savedCRC = CalculateCRC(TempImage, TempImage);
    TS_ASSERT_RC(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempImage, TempImage), savedCRC);
  }
  void testCreateFeatureAuthentecImage02() {
    ImageBufferWrapper TempImage(TestAuthentec02, sizeof(TestAuthentec02));
    savedCRC = CalculateCRC(TempImage, TempImage);
    TS_ASSERT_OK(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempImage, TempImage), savedCRC);
  }
  void testCreateFeatureAuthentecImage02WithOneRowLonger() {
    ImageBuffer TempAnsiImage(TestAuthentec02, sizeof(TestAuthentec02));
    TS_ASSERT(TempAnsiImage(TestAuthentec02_HeaderWithOneRowLonger, IMAGE_HEADER_SIZE, false));
    savedCRC = CalculateCRC(TempAnsiImage, TempAnsiImage);
    TS_ASSERT_OK(FRFXLLCreateFeatureSet(hCtx, TempAnsiImage, TempAnsiImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempAnsiImage, TempAnsiImage), savedCRC);
  }
  void testCreateFeatureTestAnsiImageMartini() {
    ImageBufferWrapper TempImage(TestAnsiImageMartini, sizeof(TestAnsiImageMartini));
    savedCRC = CalculateCRC(TempImage, TempImage);
    TS_ASSERT_OK(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempImage, TempImage), savedCRC);
  }
  void testCreateFeatureTestAnsiImageMartiniCE() {
    ImageBufferWrapper TempImage(TestAnsiImageMartiniCE, sizeof(TestAnsiImageMartiniCE));
    savedCRC = CalculateCRC(TempImage, TempImage);
    TS_ASSERT_OK(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempImage, TempImage), savedCRC);
  }
  void testCreateFeatureTestAnsiImage200() {
    ImageBufferWrapper TempImage(TestAnsiImage200, sizeof(TestAnsiImage200));
    savedCRC = CalculateCRC(TempImage, TempImage);
    TS_ASSERT_RC(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempImage, TempImage), savedCRC);
  }
  void testCreateFeatureTestAnsiImage250() {
    ImageBufferWrapper TempImage(TestAnsiImage250, sizeof(TestAnsiImage250));
    savedCRC = CalculateCRC(TempImage, TempImage);
    TS_ASSERT_RC(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempImage, TempImage), savedCRC);
  }
  void testCreateFeatureTestAnsiImage299() {
    ImageBuffer TempAnsiImage(TestAnsiImage300, sizeof(TestAnsiImage300));
    SetResolutionInANSI381Image(TempAnsiImage, 299);
    savedCRC = CalculateCRC(TempAnsiImage, TempAnsiImage);
    TS_ASSERT_RC(FRFXLLCreateFeatureSet(hCtx, TempAnsiImage, TempAnsiImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempAnsiImage, TempAnsiImage), savedCRC);
  }
  void testCreateFeatureTestAnsiImage300() {
    ImageBufferWrapper TempImage(TestAnsiImage300, sizeof(TestAnsiImage300));
    savedCRC = CalculateCRC(TempImage, TempImage);
    TS_ASSERT_OK(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempImage, TempImage), savedCRC);
  }
  void testCreateFeatureTestAnsiImage1000() {
    ImageBufferWrapper TempImage(TestAnsiImage1000, sizeof(TestAnsiImage1000));
    savedCRC = CalculateCRC(TempImage, TempImage);
    TS_ASSERT_OK(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempImage, TempImage), savedCRC);
  }
  void testCreateFeatureTestAnsiImage1001() {
    ImageBuffer TempAnsiImage(TestAnsiImage1000, sizeof(TestAnsiImage1000));
    SetResolutionInANSI381Image(TempAnsiImage, 1001);
    savedCRC = CalculateCRC(TempAnsiImage, TempAnsiImage);
    TS_ASSERT_RC(FRFXLLCreateFeatureSet(hCtx, TempAnsiImage, TempAnsiImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempAnsiImage, TempAnsiImage), savedCRC);
  }
  void testCreateFeatureTestAnsiImage_100_100() {
    ImageBufferWrapper TempImage(TestAnsiImage_100_100, sizeof(TestAnsiImage_100_100));
    savedCRC = CalculateCRC(TempImage, TempImage);
    TS_ASSERT_RC(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_FB_TOO_SMALL_AREA);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempImage, TempImage), savedCRC);
  }
  void testCreateFeatureIso() {
    ImageBufferWrapper TempImage(TestIsoImage, sizeof(TestIsoImage));
    savedCRC = CalculateCRC(TempImage, TempImage);
    TS_ASSERT_OK(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ISO_19794_4_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempImage, TempImage), savedCRC);
  }
};

class TEST_SUITE(CreatingFeatureSetAnsiUnalligned), private FixtureWithContext {
private:
  static const FRFXLL_DATA_TYPE invalidDataType = static_cast<FRFXLL_DATA_TYPE>(0xFFFFFFFF);
  FRFXLL_HANDLE hFtrSet;
  MemoryChecker memoryChecker;
  unsigned int savedCRC;
  ImageBuffer TempAnsiImage;
public:
  void setUp() {
    memoryChecker.setUp();
    FixtureWithContext::_setUp();
    TS_ASSERT(TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage)));
    hFtrSet = NULL;
    //savedCRC = CalculateCRC(TempAnsiImage, TempAnsiImage);
  }
  void tearDown() {
    //TS_ASSERT_EQUALS_X(CalculateCRC(TempAnsiImage, TempAnsiImage), savedCRC);
    FixtureWithContext::_tearDown();
    TS_ASSERT_MEMORY_RELEASED(memoryChecker);
    memoryChecker.tearDown();
  }
  REPEATED_TEST(testCreateFeatureSetUnalligned, n, 3) {
    TempAnsiImage.Shift(n + 1);
    savedCRC = CalculateCRC(TempAnsiImage, TempAnsiImage);
    TS_ASSERT_OK(FRFXLLCreateFeatureSet(hCtx, TempAnsiImage, TempAnsiImage/*sizeof(TestAnsiImage)*/, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
    TS_ASSERT_EQUALS_X(CalculateCRC(TempAnsiImage, TempAnsiImage), savedCRC);
  }
};
#endif // __FRFXLLFTREXTESTS_H__
