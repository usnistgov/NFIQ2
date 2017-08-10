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

#include <ValueTraitsEx.h>
#include "deserializeFpData.h"
#include "serializeFpData.h"
#include "FeatureExtraction.h"
#include "TestAnsiImage.h"

using namespace FingerJetFxOSE::FpRecEngineImpl::FIR;
using namespace FingerJetFxOSE::FpRecEngineImpl::Embedded;

namespace {
  struct ImageBuffer {
    static const size_t max_buffer_size = 410000;
    static unsigned char imageBuffer[];
    size_t Size;
    size_t Shift;

  public:
    ImageBuffer() : Size(0), Shift(0) {}
    ImageBuffer(const unsigned char data[], size_t size, size_t shift = 0, bool bSetSize = true) {
      if (size <= max_buffer_size) {
        memcpy(imageBuffer + shift, data, size);
        if (bSetSize)
          Size = size, Shift = shift;
      }
    }
    bool operator ()(const unsigned char data[], size_t size, size_t shift = 0, bool bSetSize = true) {
      if (size > max_buffer_size) return false;
      memcpy(imageBuffer + shift, data, size);
      if (bSetSize)
        Size = size, Shift = shift;
      return true;
    }
    void shift(size_t _shift) {
      memmove(imageBuffer + _shift, imageBuffer, Size);
      Shift = _shift;
    }
    unsigned char * buf ()      { return imageBuffer + Shift; }
    operator unsigned char *()  { return imageBuffer + Shift; }
    size_t size()               { return Size; }
    operator size_t ()          { return Size; }
  };
  unsigned char ImageBuffer::imageBuffer[max_buffer_size];

  void MakeCheckBoard(uint8* p0, size_t width, size_t height, size_t step, uint8 val) {
      std::fill_n(p0, step, val);
      std::fill_n(p0 + step, step, 0);

      size_t step_2 = step * 2;
      size_t step_w = step * width;
      size_t size   = width * height;
      uint8* p = p0 + step_2;

      size_t len;
      for (len = step_2; size_t(p - p0) < width; p += len, len *= 2, len = min(len, width - len)) {
        std::copy_n(p0, len, p);
      }
      for (len = width; size_t(p - p0) < step_w; p += len, len *= 2, len = min(len, step_w - len)) {
        std::copy_n(p0, len, p);
      }

      uint8* p1 = p0 + step_w;
      std::copy_n(p0 + step, step_2, p1);

      p = p1 + step_2;
      for (len = step_2; size_t(p - p1) < width; p += len, len *= 2, len = min(len, width - len)) {
        std::copy_n(p1, len, p);
      }
      for (len = width; size_t(p - p1) < step_w; p += len, len *= 2, len = min(len, step_w - len)) {
        std::copy_n(p1, len, p);
      }

      for (len = width*step_2; size_t(p - p0) < size; p += len, len *= 2, len = min(len, size - len)) {
        std::copy_n(p0, len, p);
      }
  }

  class TestResizeFirBase {
  public:
    FeatureExtractionImpl::Parameters fexImplParam;
  };

  class TestResizeFIRInPlace : public CxxTest::TestSuite, public TestResizeFirBase, public FeatureExtractionInPlace {
  public:
    TestResizeFIRInPlace() : FeatureExtractionInPlace(fexImplParam) {
    }
    void setUp() {
    }
    void tearDown() {
    }

    void testResize_376_211_508() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      //imageResolution = 508;
      uint8* buf = TempAnsiImage;
      size_t Size = TempAnsiImage;
      MakeCheckBoard(const_cast<uint8*>(imgIn), width, height, 3, 255);
      size_t width_ = width / 6 * 4, height_ = height / 6 * 4;
      size_t size_ = width_ * height_;
      std::auto_ptr<uint8> ExpectedImage(new uint8[size_]);
      uint8* pExpImage = ExpectedImage.get();
      MakeCheckBoard(pExpImage, width_, height_, 2, 251);
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size));
      TS_ASSERT_SAME_DATA(buf, pExpImage, size_);
    }
    void testResize_376_211_332() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      imageResolution = 332;
      uint8* buf = TempAnsiImage;
      size_t Size = TempAnsiImage;
      MakeCheckBoard(const_cast<uint8*>(imgIn), width, height, 3, 255);
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_BUFFER);
    }
    void testResize_376_211_332_x() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      imageResolution = 332;
      uint8* buf = TempAnsiImage;
      size_t Size = TempAnsiImage.max_buffer_size;
      MakeCheckBoard(const_cast<uint8*>(imgIn), width, height, 3, 255);
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_BUFFER);
    }
    void testResize_376_211_333() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      imageResolution = 333;
      uint8* buf = TempAnsiImage;
      size_t Size = TempAnsiImage;
      MakeCheckBoard(const_cast<uint8*>(imgIn), width, height, 3, 255);
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_BUFFER);
    }
    void testResize_376_211_333_x() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      imageResolution = 333;
      uint8* buf = TempAnsiImage;
      size_t Size = TempAnsiImage.max_buffer_size;
      MakeCheckBoard(const_cast<uint8*>(imgIn), width, height, 3, 255);
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_BUFFER);
    }
    void testResize_376_211_444() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      imageResolution = 444;
      uint8* buf = TempAnsiImage;
      size_t Size = TempAnsiImage;
      MakeCheckBoard(const_cast<uint8*>(imgIn), width, height, 4, 255);
      size_t width_ = width / 4 * 3 & ~3, height_ = height / 4 * 3 & ~3;
      size_t size_ = width_ * height_;
      std::auto_ptr<uint8> ExpectedImage(new uint8[size_]);
      uint8* pExpImage = ExpectedImage.get();
      MakeCheckBoard(pExpImage, width_, height_, 3, 255);
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size));
      //TS_ASSERT_SAME_DATA(buf, pExpImage, size_);
    }
    void testResize_376_211_445() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      imageResolution = 445;
      uint8* buf = TempAnsiImage;
      size_t Size = TempAnsiImage;
      MakeCheckBoard(const_cast<uint8*>(imgIn), width, height, 4, 255);
      size_t width_ = width / 4 * 3 & ~3, height_ = height / 4 * 3 & ~3;
      size_t size_ = width_ * height_;
      std::auto_ptr<uint8> ExpectedImage(new uint8[size_]);
      uint8* pExpImage = ExpectedImage.get();
      MakeCheckBoard(pExpImage, width_, height_, 3, 255);
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size));
      //TS_ASSERT_SAME_DATA(buf, pExpImage, size_);
    }
    void testResize_376_211_446() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      imageResolution = 446;
      uint8* buf = TempAnsiImage;
      size_t Size = TempAnsiImage;
      MakeCheckBoard(const_cast<uint8*>(imgIn), width, height, 4, 255);
      size_t width_ = width / 4 * 3 & ~3, height_ = height / 4 * 3 & ~3;
      size_t size_ = width_ * height_;
      std::auto_ptr<uint8> ExpectedImage(new uint8[size_]);
      uint8* pExpImage = ExpectedImage.get();
      MakeCheckBoard(pExpImage, width_, height_, 3, 255);
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size));
      //TS_ASSERT_SAME_DATA(buf, pExpImage, size_);
    }
    void testResize_31_31_508() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      height = 31; width  = 31;
      //imageResolution = 508;
      uint8* buf = TempAnsiImage;
      size_t Size = height * width + ANSIRecordHeader::size + ANSIImageHeader::size;
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_IMAGE);
    }
    void testResize_376_211_555() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      imageResolution = 555;
      uint8* buf = TempAnsiImage;
      size_t Size = TempAnsiImage;
      MakeCheckBoard(const_cast<uint8*>(imgIn), width, height, 5, 255);
      size_t width_ = width / 5 * 3 & ~3, height_ = height / 5 * 3 & ~3;
      size_t size_ = width_ * height_;
      std::auto_ptr<uint8> ExpectedImage(new uint8[size_]);
      uint8* pExpImage = ExpectedImage.get();
      MakeCheckBoard(pExpImage, width_, height_, 3, 255);
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size));
      //TS_ASSERT_SAME_DATA(buf, pExpImage, size_);
    }
    void testResize_376_211_556() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      imageResolution = 556;
      uint8* buf = TempAnsiImage;
      size_t Size = TempAnsiImage;
      MakeCheckBoard(const_cast<uint8*>(imgIn), width, height, 5, 255);
      size_t width_ = width / 5 * 3 & ~3, height_ = height / 5 * 3 & ~3;
      size_t size_ = width_ * height_;
      std::auto_ptr<uint8> ExpectedImage(new uint8[size_]);
      uint8* pExpImage = ExpectedImage.get();
      MakeCheckBoard(pExpImage, width_, height_, 3, 255);
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size));
      TS_ASSERT_SAME_DATA(buf, pExpImage, size_);
    }
    void testResize_376_211_557() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      imageResolution = 557;
      uint8* buf = TempAnsiImage;
      size_t Size = TempAnsiImage;
      MakeCheckBoard(const_cast<uint8*>(imgIn), width, height, 5, 255);
      size_t width_ = width / 5 * 3 & ~3, height_ = height / 5 * 3 & ~3;
      size_t size_ = width_ * height_;
      std::auto_ptr<uint8> ExpectedImage(new uint8[size_]);
      uint8* pExpImage = ExpectedImage.get();
      MakeCheckBoard(pExpImage, width_, height_, 3, 255);
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size));
      //TS_ASSERT_SAME_DATA(buf, pExpImage, size_);
    }
    void testResize_376_211_900() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      imageResolution = 900;
      uint8* buf = TempAnsiImage;
      size_t Size = TempAnsiImage;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size));
    }
    void testResize_376_211_600() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      imageResolution = 600;
      uint8* buf = TempAnsiImage;
      size_t Size = TempAnsiImage;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size));
    }
    void testResize_376_211_500() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      imageResolution = 500;
      uint8* buf = TempAnsiImage;
      size_t Size = TempAnsiImage;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size));
    }
    void testResize_376_211_370() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      imageResolution = 370;
      uint8* buf = TempAnsiImage;
      size_t Size = TempAnsiImage;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size));
    }
    void testResize_376_211_360() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      imageResolution = 360;
      uint8* buf = TempAnsiImage;
      size_t Size = TempAnsiImage;
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_BUFFER);
    }
    void testResize_376_211_359() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      imageResolution = 359;
      uint8* buf = TempAnsiImage;
      size_t Size = TempAnsiImage;
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_BUFFER);
    }
    void testResize_376_211_358() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      imageResolution = 358;
      uint8* buf = TempAnsiImage;
      size_t Size = TempAnsiImage;
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_BUFFER);
    }
    void testResize_376_211_222() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      imageResolution = 222;
      uint8* buf = TempAnsiImage;
      size_t Size = TempAnsiImage;
      MakeCheckBoard(const_cast<uint8*>(imgIn), width, height, 2, 255);
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_BUFFER);
    }
    void testResize_376_211_222_x() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      imageResolution = 222;
      uint8* buf = TempAnsiImage;
      size_t Size = TempAnsiImage.max_buffer_size;
      MakeCheckBoard(const_cast<uint8*>(imgIn), width, height, 2, 255);
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_BUFFER);
    }
    void testResize_303_203_360() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      height = 303; width  = 203;
      imageResolution = 360;
      uint8* buf = TempAnsiImage;
      size_t Size = height * width + ANSIRecordHeader::size + ANSIImageHeader::size;
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_BUFFER);
    }
    void testResize_303_203_359() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      height = 303; width  = 203;
      imageResolution = 359;
      uint8* buf = TempAnsiImage;
      size_t Size = height * width + ANSIRecordHeader::size + ANSIImageHeader::size;
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_BUFFER);
    }

    void testResize_300_200_362() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      height = 300; width  = 200;
      imageResolution = 362;
      uint8* buf = TempAnsiImage;
      size_t Size = height * width + ANSIRecordHeader::size + ANSIImageHeader::size;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size));
    }
    void testResize_300_200_361() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      height = 300; width  = 200;
      imageResolution = 361;
      uint8* buf = TempAnsiImage;
      size_t Size = height * width + ANSIRecordHeader::size + ANSIImageHeader::size;
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_BUFFER);
    }

    void testResize_200_200_362() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      height = 200; width  = 200;
      imageResolution = 362;
      uint8* buf = TempAnsiImage;
      size_t Size = height * width + ANSIRecordHeader::size + ANSIImageHeader::size;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size));
    }
    void testResize_200_200_361() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      height = 200; width  = 200;
      imageResolution = 361;
      uint8* buf = TempAnsiImage;
      size_t Size = height * width + ANSIRecordHeader::size + ANSIImageHeader::size;
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_BUFFER);
    }

    void testResize_100_100_362() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      height = 100; width  = 100;
      imageResolution = 362;
      uint8* buf = TempAnsiImage;
      size_t Size = height * width + ANSIRecordHeader::size + ANSIImageHeader::size;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size));
    }
    void testResize_100_100_361() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      height = 100; width  = 100;
      imageResolution = 361;
      uint8* buf = TempAnsiImage;
      size_t Size = height * width + ANSIRecordHeader::size + ANSIImageHeader::size;
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_BUFFER);
    }

    void testResize_80_80_351() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      height = 80; width  = 80;
      imageResolution = 351;
      uint8* buf = TempAnsiImage;
      size_t Size = height * width + ANSIRecordHeader::size + ANSIImageHeader::size;
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_BUFFER);
    }
    void testResize_80_80_350() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      height = 80; width  = 80;
      imageResolution = 350;
      uint8* buf = TempAnsiImage;
      size_t Size = height * width + ANSIRecordHeader::size + ANSIImageHeader::size;
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_BUFFER);
    }

    void testResize_40_40_334() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      height = 40; width  = 40;
      imageResolution = 334;
      uint8* buf = TempAnsiImage;
      size_t Size = height * width + ANSIRecordHeader::size + ANSIImageHeader::size;
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_BUFFER);
    }
    void testResize_40_40_333() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      height = 40; width  = 40;
      imageResolution = 333;
      uint8* buf = TempAnsiImage;
      size_t Size = height * width + ANSIRecordHeader::size + ANSIImageHeader::size;
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_BUFFER);
    }

    void testResize_32_32_508() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      height = 32; width  = 32;
      //imageResolution = 508;
      uint8* buf = TempAnsiImage;
      size_t Size = height * width + ANSIRecordHeader::size + ANSIImageHeader::size;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size));
    }
    void testResize_32_32_334() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      height = 32; width  = 32;
      imageResolution = 334;
      uint8* buf = TempAnsiImage;
      size_t Size = height * width + ANSIRecordHeader::size + ANSIImageHeader::size;
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_BUFFER);
    }
    void testResize_32_32_333() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      height = 32; width  = 32;
      imageResolution = 333;
      uint8* buf = TempAnsiImage;
      size_t Size = height * width + ANSIRecordHeader::size + ANSIImageHeader::size;
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buf, Size), FRFXLL_ERR_INVALID_BUFFER);
    }
  };
  class TestResizeFIR : public CxxTest::TestSuite, public TestResizeFirBase, public FeatureExtraction {
  public:
    TestResizeFIR() : FeatureExtraction(fexImplParam) {
    }
    void setUp() {
    }
    void tearDown() {
    }

    void testResize_376_211_508() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TestAnsiImage, sizeof(TestAnsiImage)));
      //imageResolution = 508;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buffer, maxsize));
      TS_ASSERT_SAME_DATA(TempAnsiImage, TestAnsiImage, sizeof(TestAnsiImage));
    }
    void testResize_31_31_508() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TestAnsiImage, sizeof(TestAnsiImage)));
      height = 31; width  = 31;
      //imageResolution = 508;
      TS_ASSERT_RC(Resize_AnyTo333InPlaceOrBuffer(imgIn, buffer, maxsize), FRFXLL_ERR_INVALID_IMAGE);
      TS_ASSERT_SAME_DATA(TempAnsiImage, TestAnsiImage, sizeof(TestAnsiImage));
    }
    void testResize_376_211_900() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TestAnsiImage, sizeof(TestAnsiImage)));
      imageResolution = 900;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buffer, maxsize));
      TS_ASSERT_SAME_DATA(TempAnsiImage, TestAnsiImage, sizeof(TestAnsiImage));
    }
    void testResize_376_211_600() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TestAnsiImage, sizeof(TestAnsiImage)));
      imageResolution = 600;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buffer, maxsize));
      TS_ASSERT_SAME_DATA(TempAnsiImage, TestAnsiImage, sizeof(TestAnsiImage));
    }
    void testResize_376_211_500() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TestAnsiImage, sizeof(TestAnsiImage)));
      imageResolution = 500;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buffer, maxsize));
      TS_ASSERT_SAME_DATA(TempAnsiImage, TestAnsiImage, sizeof(TestAnsiImage));
    }
    void testResize_376_211_370() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TestAnsiImage, sizeof(TestAnsiImage)));
      imageResolution = 370;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buffer, maxsize));
      TS_ASSERT_SAME_DATA(TempAnsiImage, TestAnsiImage, sizeof(TestAnsiImage));
    }
    void testResize_376_211_369() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TestAnsiImage, sizeof(TestAnsiImage)));
      imageResolution = 369;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buffer, maxsize));
      TS_ASSERT_SAME_DATA(TempAnsiImage, TestAnsiImage, sizeof(TestAnsiImage));
    }
    void testResize_376_211_333() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TestAnsiImage, sizeof(TestAnsiImage)));
      imageResolution = 333;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buffer, maxsize));
      TS_ASSERT_SAME_DATA(TempAnsiImage, TestAnsiImage, sizeof(TestAnsiImage));
    }
    void testResize_376_211_332() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TestAnsiImage, sizeof(TestAnsiImage)));
      imageResolution = 332;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buffer, maxsize));
      TS_ASSERT_SAME_DATA(TempAnsiImage, TestAnsiImage, sizeof(TestAnsiImage));
    }
    void testResize_376_211_300() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TestAnsiImage, sizeof(TestAnsiImage)));
      imageResolution = 300;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buffer, maxsize));
      TS_ASSERT_SAME_DATA(TempAnsiImage, TestAnsiImage, sizeof(TestAnsiImage));
    }
    void testResize_376_211_222() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TempAnsiImage, TempAnsiImage));
      imageResolution = 222;
      //uint8* buf = TempAnsiImage;
      //size_t Size = TempAnsiImage;
      MakeCheckBoard(const_cast<uint8*>(imgIn), width, height, 2, 255);
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buffer, maxsize));
    }
    void testResize_376_211_200() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TestAnsiImage, sizeof(TestAnsiImage)));
      imageResolution = 200;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buffer, maxsize));
      TS_ASSERT_SAME_DATA(TempAnsiImage, TestAnsiImage, sizeof(TestAnsiImage));
    }
    void testResize_376_211_100() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TestAnsiImage, sizeof(TestAnsiImage)));
      imageResolution = 100;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buffer, maxsize));
      TS_ASSERT_SAME_DATA(TempAnsiImage, TestAnsiImage, sizeof(TestAnsiImage));
    }
    void testResize_32_32_508() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TestAnsiImage, sizeof(TestAnsiImage)));
      height = 32; width  = 32;
      //imageResolution = 508;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buffer, maxsize));
      TS_ASSERT_SAME_DATA(TempAnsiImage, TestAnsiImage, sizeof(TestAnsiImage));
    }
    void testResize_32_32_500() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TestAnsiImage, sizeof(TestAnsiImage)));
      height = 32; width  = 32;
      imageResolution = 500;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buffer, maxsize));
      TS_ASSERT_SAME_DATA(TempAnsiImage, TestAnsiImage, sizeof(TestAnsiImage));
    }
    void testResize_32_32_370() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TestAnsiImage, sizeof(TestAnsiImage)));
      height = 32; width  = 32;
      imageResolution = 370;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buffer, maxsize));
      TS_ASSERT_SAME_DATA(TempAnsiImage, TestAnsiImage, sizeof(TestAnsiImage));
    }
    void testResize_32_32_333() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TestAnsiImage, sizeof(TestAnsiImage)));
      height = 32; width  = 32;
      imageResolution = 333;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buffer, maxsize));
      TS_ASSERT_SAME_DATA(TempAnsiImage, TestAnsiImage, sizeof(TestAnsiImage));
    }
    void testResize_32_32_300() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      TS_ASSERT_OK(ReadFIRHeader<ANSIImageRecord>(TestAnsiImage, sizeof(TestAnsiImage)));
      height = 32; width  = 32;
      imageResolution = 300;
      TS_ASSERT_OK(Resize_AnyTo333InPlaceOrBuffer(imgIn, buffer, maxsize));
      TS_ASSERT_SAME_DATA(TempAnsiImage, TestAnsiImage, sizeof(TestAnsiImage));
    }
  };

  class TestFex : public CxxTest::TestSuite {
    FeatureExtractionImpl::Parameters parm;
    FeatureExtraction fex;
  public:
    TestFex() : parm(), fex(parm) {
    }
    void setUp() {
    }
    void tearDown() {
    }

    void testFexAnsi() {
      MatchData md;
      TS_ASSERT_OK(fex.FromFIRSample<ANSIImageRecord>(TestAnsiImage, sizeof(TestAnsiImage), 0, md));
    }
    void testFexAnsiUnalligned() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      uint8 bufMd0[sizeof(MatchData)]={0};
      MatchData* pMd0 = new(bufMd0) MatchData;
      TS_ASSERT_OK(fex.FromFIRSample<ANSIImageRecord>(TempAnsiImage, TempAnsiImage, 0, *pMd0));
      TempAnsiImage.shift(1);
      uint8 bufMd1[sizeof(MatchData)]={0};
      MatchData* pMd1 = new(bufMd1) MatchData;
      TS_ASSERT_OK(fex.FromFIRSample<ANSIImageRecord>(TempAnsiImage, TempAnsiImage, 0, *pMd1));
      TS_ASSERT_SAME_DATA(pMd0, pMd1, sizeof(MatchData));
    }
    void testFexIso() {
      MatchData md;
      TS_ASSERT_OK(fex.FromFIRSample<ISOImageRecord>(TestIsoImage, sizeof(TestIsoImage), 0, md));
    }

  };

  class TestFexInPlace : public CxxTest::TestSuite {
    FeatureExtractionImpl::Parameters parm;
    FeatureExtractionInPlace fex;
  public:
    TestFexInPlace() : parm(), fex(parm) {
    }
    void setUp() {
    }
    void tearDown() {
    }

    void testFexAnsi() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      MatchData md;
      TS_ASSERT_OK(fex.FromFIRSample<ANSIImageRecord>(TempAnsiImage, TempAnsiImage, 0, md));
    }
    void testFexAnsiUnalligned() {
      ImageBuffer TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage));
      uint8 bufMd0[sizeof(MatchData)]={0};
      MatchData* pMd0 = new(bufMd0) MatchData;
      TS_ASSERT_OK(fex.FromFIRSample<ANSIImageRecord>(TempAnsiImage, TempAnsiImage, 0, *pMd0));
      TempAnsiImage(TestAnsiImage, sizeof(TestAnsiImage), 1);
      uint8 bufMd1[sizeof(MatchData)]={0};
      MatchData* pMd1 = new(bufMd1) MatchData;
      TS_ASSERT_OK(fex.FromFIRSample<ANSIImageRecord>(TempAnsiImage, TempAnsiImage, 0, *pMd1));
      TS_ASSERT_SAME_DATA(pMd0, pMd1, sizeof(MatchData));
    }
    void testFexIso() {
      ImageBuffer TempIsoImage(TestIsoImage, sizeof(TestIsoImage));
      MatchData md;
      TS_ASSERT_OK(fex.FromFIRSample<ISOImageRecord>(TempIsoImage, TempIsoImage, 0, md));
    }

  };
} // namespace
