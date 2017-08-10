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

#ifndef __fft_enhace_h
#define __fft_enhace_h

#include "block_fft.h"

namespace FingerJetFxOSE {
namespace FpRecEngineImpl {
namespace Embedded {
namespace FeatureExtractionImpl {
  namespace FFT {

    struct image {
      uint8 * start;
      int32 width;
      int32 size;
      uint8 dummy;
      static const uint8 filler = 0;

      uint8 & operator ()(int32 x, int32 yw) {
        return (0 <= x && x < width && 0 <= yw && yw < size) ? start[x + yw] : (dummy=0,dummy);// SmallerTypeCheck : dummy can overflow 
      }
      uint8 operator ()(int32 x, int32 yw) const {
        return (0 <= x && x < width && 0 <= yw && yw < size) ? ~start[x + yw] : filler;
      }
      image(uint8 * start_, int32 width_, int32 size_) 
        : start(start_), width(width_), size(size_) {}
    };

    inline void init(image & img, int32 x0, int32 w, int32 yw0, int32 hw) {
      for (int32 yw = yw0; yw < yw0 + hw; yw += img.width) {
        for (int32 x = x0; x < x0 + w; ++x) {
          img(x, yw) = 0;
        }
      }
    }

    template <int32 side>
    inline void copy(const image & img, int32 x0, int32 yw0, int32 * block) {
      int yw1 = yw0 + side * img.width;
      for (int32 yw = yw0; yw < yw1; yw += img.width) {
        for (int32 x = x0; x < x0 + side; ++x) {
          *block++ = img(x, yw);
        }
      }
    }

    template <int32 side>
    inline void add(image & img, int32 x0, int32 yw0, int32 * block) {
      for (int32 yw = yw0; yw < yw0 + side * img.width; yw += img.width) {
        for (int32 x = x0; x < x0 + side; ++x) {
          img(x, yw) += (uint8)(*block++);
        }
      }
    }

    inline void inverse(image & img) {
      for (uint8 * p = img.start; p < img.start + img.size; ++p) {
        *p = ~*p;
      }
    }
  }
  // does not work completely in-place, in has to be larger than out at least by block_size * width
  template <uint8 block_bits, int32 spacing>
  inline bool fft_enhance(uint8 * img, size_t width_, size_t size_, size_t buffer_size) {
    int32 width = (int32) width_;
    int32 size  = (int32) size_;

    using namespace FFT;
    using FFT::image;
    const static int32 block_dim = 1 << block_bits;
    STATIC_ASSERT(0 < spacing && spacing <= block_dim);

    const int32 bw = block_dim * width;
    if (size_t(size + bw) > buffer_size) return false;
    memmove(img + bw, img, size);
    image in_img(img + bw, width, size);
    image out_img(img, width, size);

    const static size_t block_size = 1 << block_bits * 2;
    int32 block[block_size];
    const int32 ywmax = size;
    const int32 xmax = width;
    const int32 yspacing = width * spacing;
    init(out_img, 0, width, 0, bw);
    for (int32 yw = yspacing - bw; yw < ywmax; yw += yspacing) {
      //FFT::envelope<block_dim, spacing> yenv(yw < yspacing, yw >= ywmax - yspacing);
      FFT::envelope<block_dim, spacing> yenv(false, false);
      for (int32 x = spacing - block_dim; x < xmax; x += spacing) {
        copy<block_dim>(in_img, x, yw, block);
        //FFT::envelope<block_dim, spacing> xenv(x < spacing, x >= xmax - spacing);
        FFT::envelope<block_dim, spacing> xenv(false, false);
        FFT::enhance_block<block_bits, spacing>(block, xenv, yenv);
        add<block_dim>(out_img, x, yw, block);
      }
      init(out_img, 0, width, yw + bw, yspacing);
    }
    inverse(out_img);
    return true;
  }
}
}
}
}

#endif
