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
      BASED ON:       William T. Freeman, Edward H. Adelson
                      The Design and Use of Steerable Filters
      IMPLEMENTATION: Alexander Ivanisov
                      Jacob Kaminsky
                      Lixin Wei
      DATE:           11/08/2011
*/

#ifndef __freeman_h
#define __freeman_h

#include <stdlib.h>
#include <string.h>

#include "complex.h"

#ifndef __CROSSWORKS_ARM
  #include <stdio.h>
#else
  #include <__debug_stdio.h>
#endif

#include "dpTypes.h"
#include "intmath.h"
#include "delay.h"
#include "conv2.h"

namespace FingerJetFxOSE {
namespace FpRecEngineImpl {
namespace Embedded {
namespace FeatureExtractionImpl {

// this value should not occure naturally in phasemap within footprint 
#define phasemap_filler ((uint8)127) 
  /* Note: zero taps are already multiplied by 2
    taps for 122 : rms error = 1.44%
    -225    -7    48    14     1
     244    78    20     2     0
       0    71    37     6     0
       0   -92   -12     8     1
     244    78    20     2     0
       0    52    27     4     0
    -179   -23    21     7     1
  */
  // Outside the footprint is encoded with 255 (filler), normal footprint is in range [0, 254]
  template <size_t ori_scale, class Tout, class ori_t> 
  void freeman_phasemap(size_t width, size_t size, const uint8 * inImage, ori_t * ori, Tout * outImage) {
    static const size_t ori_scale2 = ori_scale * ori_scale;
    const size_t ori_width = width / ori_scale;
    static const size_t flt_size2 = 4; // filter half-size

    const uint8 * p = inImage + (width+1) * flt_size2;
    Tout * p2 = outImage;
    // Uncomment this if you want to try IIR instead of FIR when running in-place 
    // #define IIR
  #ifdef IIR
    for (size_t i = width * flt_size2; i; --i) {
      *p2++ = phasemap_filler;
    }
  #endif

    conv9<-112, -7, 48, 14, 1> x20;
    conv9<122, 78, 20, 2, 0> x22, x33;
    conv9<0, 71, 37, 6, 0> x21;
    conv9<0, -92, -12, 8, 1> x30;
    conv9<0, 52, 27, 4, 0> x32;
    conv9<-90, -23, 21, 7, 1> x31;

    for (size_t y = ori_width; y < size / ori_scale2 - ori_width; y += ori_width) {
      ori_t * pori = ori + y;
      for (size_t i = ori_scale; i; --i) {
        for (size_t x = 0; x < ori_width; ++x) {
          int32 a10 = pori[x].real();
          int32 a11 = pori[x].imag();
      
          int32 a20 = sincosnorm(a10 * a10);
          int32 a21 = sincosnorm(2 * a10 * a11);
          int32 a22 = sincosnorm(a11 * a11);
          
          int32 a30 = sincosnorm(a10 * a20);
          int32 a31 = sincosnorm(3 * a20 * a11);
          int32 a32 = sincosnorm(3 * a22 * a10);
          int32 a33 = sincosnorm(a11 * a22);

          for (size_t j = flt_size2; j; ++p, ++p2, --j) {
            int32 v20 = x20(x22.vert(width, p));
            int32 v21 = x21(x21.vert(width, p));
            int32 v22 = x22(x20.vert(width, p));

            int32 v30 = x30(x33.vert(width, p));
            int32 v31 = x31(x32.vert(width, p));
            int32 v32 = x32(x31.vert(width, p));
            int32 v33 = x33(x30.vert(width, p));

            int32 x2 = a20 * v20 + a21 * v21 + a22 * v22;
            int32 x3 = a30 * v30 + a31 * v31 + a32 * v32 + a33 * v33; 
            x2 = (x2 + 1024) >> 11;
            x3 = (x3 + 1024) >> 11;
            *p2 = x2 ? ((127 - oct_sign(complex<int32>(x2, x3)).real()) & 0xf0) : phasemap_filler;     // 27.3M
          }
        }
      }
    }
  #ifdef IIR
    for (size_t i = width * flt_size2; i; --i) {
  #else
    for (size_t i = width * flt_size2 * 2; i; --i) {
  #endif 
      *p2++ = phasemap_filler;
    }
  }

} // namespace
}
}
}

#endif //  __freeman_h

