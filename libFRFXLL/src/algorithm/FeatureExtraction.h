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
      LIBRARY: FRFXLL - Fingerprint Feature Extractor - Low Level API

      ALGORITHM:      Alexander Ivanisov
                      Yi Chen
                      Salil Prabhakar
      IMPLEMENTATION: Alexander Ivanisov
                      Jacob Kaminsky
                      Lixin Wei
      DATE:           11/08/2011
*/

#ifndef __FEATUREEXTRACTION_H
#define __FEATUREEXTRACTION_H

#include "imresize.h"
#include "orimap.h"
#include "freeman.h"
#include "extract_minutia.h"
#include "matchData.h"
#include "FingerImageRecord.h"
#include "top_n.h"
#include "fft_enhance.h"

namespace FingerJetFxOSE {
namespace FpRecEngineImpl {
namespace Embedded {

  using namespace FeatureExtractionImpl;

  inline void UpdateOffset(MatchData & md){
    static const int16 xMax = 255, yMax = 399;
    int16 x0 = std::numeric_limits<int16>::max();
    int16 x1 = std::numeric_limits<int16>::min();
    int16 y0 = std::numeric_limits<int16>::max();
    int16 y1 = std::numeric_limits<int16>::min();
    size_t i;
    for (i = 0; i < md.size(); i++) {
      int16 x = md.minutia[i].position.x;
      int16 y = md.minutia[i].position.y;
      x0 = std::min(x0, x);
      x1 = std::max(x1, x);
      y0 = std::min(y0, y);
      y1 = std::max(y1, y);
    }
    int16 xoffs = 0;
    if (x1 > xMax) {
      xoffs = (x0 + x1 - xMax) >> 1;
      md.offset.x += xoffs;
    }
    int16 yoffs = 0;
    if (y1 > yMax) {
      yoffs = std::min(y0, int16(y1 - yMax));
      md.offset.y += yoffs;
    }
    //size_t j = 0;
    for (i = 0; i < md.size(); i++) {
      int16 x = md.minutia[i].position.x -= xoffs;
      int16 y = md.minutia[i].position.y -= yoffs;
      if (x < 0 || x > xMax || y < 0 || y > yMax) {
        memcpy(md.minutia + i, md.minutia + i + 1, ((md.size() - (i + 1)) * sizeof(*md.minutia)));
        i--;
        md.numMinutia--;
      }
    }
  }

  struct FeatureExtractionBase {
    static const size_t maxwidth = 256;
    static const size_t maxheight = 400;

    static const size_t int_resolution = 333;
    static const size_t int_resolution_min_in_place = 362; // ~= 333 * sqrt((4^2+3)/4^2)
    static const size_t ori_scale = 4;
    static const int32 imageScale = 127 * 5;
    // flags for reserved (flags) parameters
    static const uint32 flag_disable_fft_enhancement = 0x1;
    static const uint32 flag_enable_fft_enhancement  = 0x2;

    static const uint8 enh_block_bits = 5;
    static const int32 enh_spacing = 17;

    const uint8 * imgIn;
    uint8 * img;
    size_t height;
    size_t width;
    size_t size;

    int32 xOffs; // Phasemap offset
    int32 yOffs; // Phasemap offset

    uint8 * footprint;
    ori_t * ori;
    size_t ori_width;
    size_t ori_size;
    uint16 imageResolution; // In DPI
    uint8 readerCode;
    const FeatureExtractionImpl::Parameters & param;

    FeatureExtractionBase(const FeatureExtractionImpl::Parameters & param_)
      : xOffs(1)
      , yOffs(6)
      , param(param_)
    {}
    void SetOriSize() {
      ori_width = int((width - 1) / ori_scale + 1);
      size_t ori_height = int((size/width - 1) / ori_scale + 1);
      ori_size = ori_height * ori_width;
    }

    void WriteFootprint(Footprint & fp, const Point & offset) {
      uint32 area = 0;
      for (int32 y = 0; y < fp.height; y++) {
        for (int32 x = 0; x < fp.width; x++) {
          size_t xi = reduce(((x * 8 + offset.x) * imageResolution + imageResolution / 2) / imageScale, 2);
          size_t yi = reduce(((y * 8 + offset.y) * imageResolution + imageResolution / 2) / imageScale, 2) * ori_width;
          bool b = (xi < ori_width) && (yi < ori_size) && (footprint[xi + yi]);
          if (b) area++;
          fp.Pixel(x,y) = b;
        }
      }
      fp.area = area * 8 * 8;
    }
    void WriteHeader(MatchData & md) {
      md.offset.x = int16(xOffs * imageScale / imageResolution);
      md.offset.y = int16(yOffs * imageScale / imageResolution);
    }

    void RescaleMinutia(MatchData & md) {
      for (size_t i = 0; i < md.size(); i++) {
        md.minutia[i].position.x = int16(md.minutia[i].position.x * imageScale / imageResolution);
        md.minutia[i].position.y = int16(md.minutia[i].position.y * imageScale / imageResolution);
      }
    }

    FRFXLL_RESULT From333DpiImg(
      uint32 flags,
      size_t buffer_size,
      MatchData &md
    ) {
      ori = reinterpret_cast<ori_t *>(img + size);
      if ((flags & flag_enable_fft_enhancement) != 0) {
        size_t enh_buffer_size = size + max(ori_size * sizeof(ori_t), width << enh_block_bits);
        AssertR(enh_buffer_size + ori_size <= buffer_size, FRFXLL_ERR_INVALID_IMAGE); 
        footprint = img + enh_buffer_size;
        orientation_map_and_footprint<maxwidth, ori_scale>(width, size, img, true, NULL, footprint);
        if (! fft_enhance<enh_block_bits, enh_spacing>(img, width, size, enh_buffer_size)) {
          return FRFXLL_ERR_INTERNAL;
        }
        orientation_map_and_footprint<maxwidth, ori_scale>(width, size, img, false, ori, footprint);
      } else {
        footprint = img + size + ori_size * sizeof(ori_t);
        orientation_map_and_footprint<maxwidth, ori_scale>(width, size, img, true, ori, footprint);
      }
      freeman_phasemap<ori_scale>(width, size, img, ori, img);
      /* remove limit:
	  top_n<Minutia> top_minutia(md.minutia, md.minutia + std::min(md.capacity(), size_t(68)));*/
	  top_n<Minutia> top_minutia(md.minutia, md.minutia + md.capacity());
      extract_minutia<maxwidth, ori_scale>(img, width, size, footprint, top_minutia, param);
      md.numMinutia = top_minutia.size();
      top_minutia.sort();
      RescaleMinutia(md);
      WriteHeader(md);                // order is important : this fills offset
      UpdateOffset(md);
      WriteFootprint(md.footprint, md.offset);  // this uses offset
      if (   md.size() < param.user_feedback.minimum_number_of_minutia
          || md.footprint.area < param.user_feedback.minimum_footprint_area) {
        return FRFXLL_ERR_FB_TOO_SMALL_AREA;
      }
      return FRFXLL_OK;
    }

    template <class T_FIR>
    FRFXLL_RESULT ReadFIRHeader(
      const uint8 data[],             ///< [in] sample buffer
      size_t buffer_size              ///< [in] size of the sample buffer
    ) {
      T_FIR record(data, buffer_size);
      if (!record.IsValid()) return record;
      CheckR(Init(record.pData, buffer_size, record.Header.GetWidth(), record.Header.GetHeight(), record.Header.GetResolution()));
      CheckR(record.Header.GetDeviceCode(readerCode));
      return FRFXLL_OK;
    }

    FRFXLL_RESULT Init(
      const uint8 data[],             ///< [in] sample buffer
      size_t size_,                   ///< [in] size of the sample buffer
      uint32 width_,                  ///< [in] width of the image
      uint32 height_,                 ///< [in] heidht of the image
      uint32 imageResolution_         ///< [in] pixel resolution [DPI]
    ) {
      AssertR(size_ >= width_ * height_, FRFXLL_ERR_INVALID_IMAGE);
      imgIn = data;
      width = width_;
      height = height_;
      size = width_ * height_;
      imageResolution = imageResolution_;
      xOffs = 1;
      yOffs = 6;
      return FRFXLL_OK;
    }

    FRFXLL_RESULT Resize_AnyTo333InPlaceOrBuffer(
      const uint8 in_img[],           ///< [in] input sample buffer, not preserved
      uint8 buffer[],                 ///< [] working buffer that is modified, can be the same as image buffer, or start before it
      size_t buffer_size              ///< [in] size of the working buffer
    ) {
      AssertR(width != 0 && size != 0, FRFXLL_ERR_INVALID_IMAGE);

      AssertR(height >= 32 && width >= 32, FRFXLL_ERR_INVALID_IMAGE);

      size_t in_width = width, in_height = height;
      if (imageResolution <= 550 && imageResolution >= 450) {
        // 500/333 = 1500/999 ~= 3/2
        // width  ~= in_width / (3/2) = in_width / 3 * 2 ~= in_width / 6 * 4
        width  = in_width  / 6 * 4; // width should be multiple of 4
        height = in_height / 6 * 4;
      } else {
        width  = (in_width  * int_resolution / imageResolution) & ~(ori_scale - 1);
        height = (in_height * int_resolution / imageResolution) & ~(ori_scale - 1);
      }

      if (height > maxheight) {        // center the image vertically
        size_t maxheight_in = (imageResolution <= 550 && imageResolution >= 450) ? maxheight * 3 / 2 : maxheight * imageResolution / int_resolution;
        size_t diff = in_height - maxheight_in;
        AssertR(diff > 0, FRFXLL_ERR_INTERNAL);
        in_img += (diff / 2) * in_width;
        yOffs += int32((height - maxheight) / 2);
        height = maxheight;
      }

      if (width > maxwidth) {        // center the image horizontally
        size_t maxwidth_in = (imageResolution <= 550 && imageResolution >= 450) ? maxwidth * 3 / 2 : maxwidth * imageResolution / int_resolution;
        size_t diff = in_width - maxwidth_in;
        AssertR(diff > 0, FRFXLL_ERR_INTERNAL);
        in_img += diff / 2;
        xOffs += int32((width - maxwidth) / 2);
        width = maxwidth;
      }

      size = width * height;

      AssertR(width != 0 && size != 0, FRFXLL_ERR_INVALID_IMAGE);

      SetOriSize();
      size_t buffer_size_needed = size + ori_size * (1 + sizeof(ori_t));
      AssertR(imageResolution >= int_resolution_min_in_place || in_img < buffer || in_img >= buffer + size, FRFXLL_ERR_INVALID_BUFFER); // FRFXLL_ERR_INVALID_IMAGE);
      //AssertR(imageResolution >= int_resolution || in_img < buffer || in_img >= buffer + size, FRFXLL_ERR_INVALID_BUFFER);//FRFXLL_ERR_INTERNAL); //FRFXLL_ERR_FB_TOO_SMALL_AREA);//
      AssertR(buffer_size_needed <= buffer_size, FRFXLL_ERR_INVALID_IMAGE); //FRFXLL_ERR_INVALID_BUFFER); // FRFXLL_ERR_FB_TOO_SMALL_AREA);//

      if (imageResolution <= 550 && imageResolution >= 450) {
        imresize23<maxwidth>(buffer, width, size, in_img, in_width);
      } else if (imageResolution <= int_resolution + 33 && imageResolution >= int_resolution - 33 && (in_width % 4) == 0) {
        memmove(buffer, in_img, size);
        imageResolution = muldiv(imageResolution, 5, 3);
      } else {
        size_t scale256 = imageResolution * size_t(256) / int_resolution;
        imresize(buffer, width, size, in_img, in_width, scale256);
        imageResolution = 500;
      }
      return FRFXLL_OK;
    }

    FRFXLL_RESULT ExtractMinutiae (
      const uint8 in_img[],           ///< [in] input sample buffer, not preserved
      uint8 buffer[],                 ///< [] working buffer that is modified, can be the same as image buffer, or start before it
      size_t buffer_size,             ///< [in] size of the working buffer
      uint32 flags,
      MatchData &md
    ) {
      CheckR(Resize_AnyTo333InPlaceOrBuffer(in_img, buffer, buffer_size));
      img = buffer;
      return From333DpiImg(flags, buffer_size, md);
    }
  };

  struct FeatureExtractionInPlace : public FeatureExtractionBase {

    FeatureExtractionInPlace(const FeatureExtractionImpl::Parameters & param_)
      : FeatureExtractionBase (param_)
    {}

    template <class T_FIR>
    FRFXLL_RESULT FromFIRSample(
      uint8 data[],                   ///< [in] sample buffer, modified to save memory
      size_t data_size,               ///< [in] size of the sample buffer
      uint32 flags,
      MatchData &md
    ) {
      CheckR(ReadFIRHeader<T_FIR>(data, data_size));
      AssertR(imageResolution >= int_resolution_min_in_place, FRFXLL_ERR_INVALID_IMAGE);
      return ExtractMinutiae(imgIn, data, data_size, flags, md);
    }

    FRFXLL_RESULT FromRawSample(
      uint8 data[],                   ///< [in] sample buffer
      size_t size_,                   ///< [in] size of the sample buffer
      uint32 width_,                  ///< [in] width of the image
      uint32 height_,                 ///< [in] heidht of the image
      uint32 imageResolution_,        ///< [in] pixel resolution [DPI]
      uint32 flags,
      MatchData &md
    ) {
      CheckR(Init(data, size_, width_, height_, imageResolution_));
      AssertR(imageResolution >= int_resolution_min_in_place, FRFXLL_ERR_INVALID_IMAGE);
      return ExtractMinutiae(imgIn, data, size_, flags, md);
    }

  };

  /// Contains the temporary buffer, so the size is larger, that's why we create a separate class
  struct FeatureExtraction : public FeatureExtractionBase {
    static const size_t maxsize = maxwidth * (maxheight + maxheight / 4);
    uint8 buffer[maxsize];

    FeatureExtraction(const FeatureExtractionImpl::Parameters & param_)
      : FeatureExtractionBase (param_)
    {
    }

    template <class T_FIR>
    FRFXLL_RESULT FromFIRSample(
      const uint8 data[],             ///< [in] input sample buffer, preserved
      size_t data_size,               ///< [in] size of the input sample buffer
      uint32 flags,
      MatchData &md
    ) {
      CheckR(ReadFIRHeader<T_FIR>(data, data_size));
      return ExtractMinutiae(imgIn, buffer, maxsize, flags, md);
    }

    FRFXLL_RESULT FromRawSample(
      const uint8 data[],             ///< [in] sample buffer
      size_t size_,                   ///< [in] size of the sample buffer
      uint32 width_,                  ///< [in] width of the image
      uint32 height_,                 ///< [in] heidht of the image
      uint32 imageResolution_,        ///< [in] pixel resolution [DPI]
      uint32 flags,                   ///< [in] select which features of the algorithm to use
      MatchData &md
    ) {
      CheckR(Init(data, size_, width_, height_, imageResolution_));
      return ExtractMinutiae(imgIn, buffer, maxsize, flags, md);
    }
  };

}
}
}

#endif // __FEATUREEXTRACTION_H

