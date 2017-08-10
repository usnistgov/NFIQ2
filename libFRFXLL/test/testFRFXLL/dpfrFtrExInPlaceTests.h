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

// dpfrFtrExInPlaceTests.h

#ifndef __FRFXLLFTREXINPLACETESTS_H__
#define __FRFXLLFTREXINPLACETESTS_H__

#include "dpfrUnitTest.h"

class CreatingFeatureSetInPlace : public CxxTest::TestSuite, private FixtureWithContext {
private:
  static const FRFXLL_DATA_TYPE invalidDataType = static_cast<FRFXLL_DATA_TYPE>(0xFFFFFFFF);
  FRFXLL_HANDLE hFtrSet;
  MemoryChecker memoryChecker;
  ImageBuffer TempImage;
public:
  void setUp() {
    memoryChecker.setUp();
    FixtureWithContext::_setUp();
    hFtrSet = NULL;
    TS_ASSERT(TempImage(TestAnsiImage, sizeof(TestAnsiImage)));
  }
  void tearDown() {
    FixtureWithContext::_tearDown();
    TS_ASSERT_MEMORY_RELEASED(memoryChecker);
    memoryChecker.tearDown();
  }
  void testCreateFeatureSetWithNullContextHandle() {
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(NULL, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_HANDLE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureSetWithNullFeatureSetHandlePtr() {
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, NULL), FRFXLL_ERR_INVALID_PARAM);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureSetWithNullImageDataPtr() {
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, NULL, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_PARAM);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet))
  }
  void testCreateFeatureSetWithShortImageDataSize() {
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, 5, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureSetWithOneByteShorterImageDataSize() {
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage.size() - 1, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureSetWithOneByteLongerImageDataSize() {
    TS_ASSERT_OK(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage.size() + 1, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureSetWithLongImageDataSize() {
    TS_ASSERT_OK(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage.max_buffer_size, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureSetOutOfMemory() {
    FailMemoryAllocation fma(0);
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_NO_MEMORY);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureSetOutOfMemoryForFeatureSetObject() {
    FailMemoryAllocation fma(1);
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_NO_MEMORY);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureSetOutOfMemoryForHandle() {
    FailMemoryAllocation fma(2);
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_NO_MEMORY);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureSetMakesOnlyThreeAllocations() {
    FailMemoryAllocation fma(3);
    TS_ASSERT_OK(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureSetAsInvalidDataType() {
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, invalidDataType, 0, &hFtrSet), FRFXLL_ERR_INVALID_PARAM);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureSetAsFeatureSetDataType() {
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_FEATURE_SET, 0, &hFtrSet), FRFXLL_ERR_INVALID_PARAM);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateAsFeatureSetAnsiSampleDataType() {
    TS_ASSERT_OK(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateWithEnhancementDisabled() {
    TS_ASSERT_OK(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, FRFXLL_FEX_DISABLE_ENHANCEMENT, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateWithEnhancementEnabled() {
    TS_ASSERT_OK(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, FRFXLL_FEX_ENABLE_ENHANCEMENT, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateWithEnhancementDisallowedCombination() {
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, FRFXLL_FEX_ENABLE_ENHANCEMENT | FRFXLL_FEX_DISABLE_ENHANCEMENT, &hFtrSet), FRFXLL_ERR_INVALID_PARAM);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
};

#define TRUNCATEDIMAGESIZE 40000

class CreatingFeatureSetInPlaceWithTestImages : public CxxTest::TestSuite, private FixtureWithContext {
private:
  FRFXLL_HANDLE hFtrSet;
  MemoryChecker memoryChecker;
  //unsigned int savedCRC;
  ImageBuffer TempImage;
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
    TS_ASSERT(TempImage(TestAnsiImageBad_ANY_BINARY, sizeof(TestAnsiImageBad_ANY_BINARY)));
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureBadImage_OneLine() {
    TS_ASSERT(TempImage(TestAnsiImageBad_OneLine, sizeof(TestAnsiImageBad_OneLine)));
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  // This test returns FRFXLL_ERR_FB_TOO_SMALL_AREA (0x80048004L), instead of FRFXLL_ERR_FB_IMAGE_TOO_NOISY (0x80048001); it appears that FRFXLL_ERR_FB_IMAGE_TOO_NOISY is not used
  void testCreateFeatureBadImage_CONSTANT_IMAGE() {
    TS_ASSERT(TempImage(0x10, sizeof(TestAnsiImage)));
    TS_ASSERT(TempImage(TestAnsiImage, IMAGE_HEADER_SIZE, false));
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_FB_TOO_SMALL_AREA); // 0x80048004L
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureBrcmImage00() {
    TS_ASSERT(TempImage(TestBrcmImage00, sizeof(TestBrcmImage00)));
    TS_ASSERT_OK(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureAuthentecImage01() {
    TS_ASSERT(TempImage(TestAuthentec01, sizeof(TestAuthentec01)));
    TS_ASSERT_OK(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureAuthentecImage01WithOneByteShorter() {
    TS_ASSERT(TempImage(TestAuthentec01, sizeof(TestAuthentec01)));
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage.size() - 1, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureAuthentecImage01WithOneByteLonger() {
    TS_ASSERT(TempImage(TestAuthentec01, sizeof(TestAuthentec01)));
    TS_ASSERT_OK(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage.size() + 1, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureWithTruncatedImage() {
    TS_ASSERT(TempImage(TestAuthentec01, sizeof(TestAuthentec01)));
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TRUNCATEDIMAGESIZE, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void _testCreateFeatureAuthentecImage02() {
    TS_ASSERT(TempImage(TestAuthentec02, sizeof(TestAuthentec02)));
    TS_ASSERT_OK(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureAuthentecImage02WithOneRowLonger() {
    TS_ASSERT(TempImage(TestAuthentec02, sizeof(TestAuthentec02)));
    TS_ASSERT(TempImage(TestAuthentec02_HeaderWithOneRowLonger, IMAGE_HEADER_SIZE, false));
    TS_ASSERT_OK(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }

  void testCreateFeatureTestImageMartini() {
    TS_ASSERT(TempImage(TestAnsiImageMartini, sizeof(TestAnsiImageMartini)));
    TS_ASSERT_OK(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureTestImageMartiniCE() {
    TS_ASSERT(TempImage(TestAnsiImageMartiniCE, sizeof(TestAnsiImageMartiniCE)));
    TS_ASSERT_OK(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }

  void testCreateFeatureTestImageValidityWrongRecordLength() {
    TS_ASSERT(TempImage(TestAnsiImageValidityWrongRecordLength, TestAnsiImageValiditySize));
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }

  void testCreateFeatureTestImageValidity() {
    TS_ASSERT(TempImage(TestAnsiImageValidityWrongRecordLength, TestAnsiImageValiditySize + 50));
    TS_ASSERT_OK(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }

  void testCreateFeatureTestImage200() {
    TS_ASSERT(TempImage(TestAnsiImage200, sizeof(TestAnsiImage200)));
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureTestImage250() {
    TS_ASSERT(TempImage(TestAnsiImage250, sizeof(TestAnsiImage250)));
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureTestImage299() {
    TS_ASSERT(TempImage(TestAnsiImage300, sizeof(TestAnsiImage300)));
    SetResolutionInANSI381Image(TempImage, 299);
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureTestImage300() {
    TS_ASSERT(TempImage(TestAnsiImage300, sizeof(TestAnsiImage300)));
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureTestImage332() {
    TS_ASSERT(TempImage(TestAnsiImage300, sizeof(TestAnsiImage300)));
    SetResolutionInANSI381Image(TempImage, 332);
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureTestImage333() {
    TS_ASSERT(TempImage(TestAnsiImage300, sizeof(TestAnsiImage300)));
    SetResolutionInANSI381Image(TempImage, 333);
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureTestImage1000() {
    TS_ASSERT(TempImage(TestAnsiImage1000, sizeof(TestAnsiImage1000)));
    TS_ASSERT_OK(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureTestImage1001() {
    TS_ASSERT(TempImage(TestAnsiImage1000, sizeof(TestAnsiImage1000)));
    SetResolutionInANSI381Image(TempImage, 1001);
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_INVALID_IMAGE);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureTestImage_100_100() {
    TS_ASSERT(TempImage(TestAnsiImage_100_100, sizeof(TestAnsiImage_100_100)));
    TS_ASSERT_RC(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet), FRFXLL_ERR_FB_TOO_SMALL_AREA);
    TS_ASSERT_EQUALS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
  void testCreateFeatureIso() {
    TS_ASSERT(TempImage(TestIsoImage, sizeof(TestIsoImage)));
    TS_ASSERT_OK(FRFXLLCreateFeatureSet(hCtx, TempImage, TempImage, FRFXLL_DT_ISO_19794_4_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
};

class TEST_SUITE(CreatingFeatureSetInPlaceUnallignedAnsi), private FixtureWithContext {
private:
  static const FRFXLL_DATA_TYPE invalidDataType = static_cast<FRFXLL_DATA_TYPE>(0xFFFFFFFF);
  FRFXLL_HANDLE hFtrSet;
  MemoryChecker memoryChecker;
  ImageBuffer TempImage;
public:
  void setUp() {
    memoryChecker.setUp();
    FixtureWithContext::_setUp();
    TS_ASSERT(TempImage(TestAnsiImage, sizeof(TestAnsiImage)));
    hFtrSet = NULL;
  }
  void tearDown() {
    FixtureWithContext::_tearDown();
    TS_ASSERT_MEMORY_RELEASED(memoryChecker);
    memoryChecker.tearDown();
  }
  REPEATED_TEST(testCreateFeatureSetUnalligned, n, 3) {
    TempImage.Shift(n + 1);
    TS_ASSERT_OK(FRFXLLCreateFeatureSetInPlace(hCtx, TempImage, TempImage, FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFtrSet));
    TS_ASSERT_DIFFERS(hFtrSet, 0);
    TS_ASSERT_OK(FRFXLLCloseHandle(hFtrSet));
  }
};

#endif // __FRFXLLFTREXINPLACETESTS_H__
