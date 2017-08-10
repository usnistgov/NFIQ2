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

#ifndef __imresize_h
#define __imresize_h

#include <dpTypes.h>
#include "intmath.h"

namespace FingerJetFxOSE {
namespace FpRecEngineImpl {
namespace Embedded {
namespace FeatureExtractionImpl {

  inline void imresize23_block(uint8* po, size_t o_w, const uint8* pi, size_t i_w) {
    static const uint16 div = 256;        // power of 2
    static const uint16 mul = div / 9;    //28
    for (uint8 * endline = po + o_w; po < endline; pi += 3, po += 2) {
      uint16 i00 = pi[    0], i01 = pi[      1], i02 = pi[      2];
      uint16 i10 = pi[i_w  ], i11 = pi[i_w  +1], i12 = pi[i_w  +2];
      uint16 i20 = pi[i_w*2], i21 = pi[i_w*2+1], i22 = pi[i_w*2+2];
      uint16 o00 = i00*4 + i01*2 + i10*2 + i11, o01 = i02*4 + i01*2 + i12*2 + i11;
      uint16 o10 = i20*4 + i21*2 + i10*2 + i11, o11 = i22*4 + i21*2 + i12*2 + i11;
      po[0] = o00 * mul / div;              
      po[1] = o01 * mul / div;
      po[o_w] = o10 * mul / div;
      po[o_w + 1] = o11 * mul / div;
    }
  }

  // Can work in-place
  template <size_t out_stride> 
  inline void imresize23(uint8 * out_img, size_t out_width, size_t out_size, const uint8 * in_img, size_t in_width) {
    uint8 buf[2 * out_stride] = {0};
    imresize23_block(buf, out_width, in_img, in_width);
    memcpy(out_img, buf, out_width * 2);

    const uint8 * pi = in_img + in_width * 3;
    uint8 * po = out_img + out_width * 2;
    uint8 * end = out_img + out_size;
    for (; po < end; pi += in_width * 3, po += out_width * 2) {
      imresize23_block(po, out_width, pi, in_width);
    }
  }

  // Can work in-place; AI: remains here for compatibility with MEX only
  template <size_t out_stride> 
  inline void imresize23(uint8 * out_img, size_t & out_width, size_t & out_size, const uint8 * in_img, size_t in_width, size_t in_size) {
    out_width = in_width / 6 * 4; // width should be multiple of 4
    if (out_width > out_stride) {
      // center the image
      in_img += (out_width - out_stride) * 3 / 4;
      out_width = out_stride;
    }
    out_size = in_size / in_width / 6 * 4 * out_width; // height should be multiple of 4
    imresize23<out_stride>(out_img, out_width, out_size, in_img, in_width);
  }

  inline void imresize(uint8 * out_img, size_t o_w, size_t o_size, const uint8 * in_img, size_t i_w, size_t scale256) {
    const uint8 * piv = in_img;
    size_t dy = 0;
    uint8 * end = out_img + o_size;
    for (uint8 * po = out_img; po < end; ) {
      const uint8 * pi = piv;
      size_t dx = 0;
      for (uint8 * endline = po + o_w; po < endline; po++) {
        *po = uint8(((pi[  0] * (256 - dx) + pi[    1] * dx) * (256 - dy)
                   + (pi[i_w] * (256 - dx) + pi[i_w+1] * dx) * dy        ) >> 16);
        dx += scale256;
        pi += dx >> 8;
        dx &= 0xff;
      }
      dy += scale256;
      piv += (dy >> 8) * i_w;
      dy &= 0xff;
    }
  }

} // namespace
}
}
}

#endif // __imresize_h

