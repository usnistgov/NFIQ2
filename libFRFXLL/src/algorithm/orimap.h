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

#ifndef __orimap_h
#define __orimap_h

#include <stdlib.h>
#include <string.h>

#ifndef __CROSSWORKS_ARM
  #include <stdio.h>
#else
  #include <__debug_stdio.h>
#endif

#include "dpTypes.h"
#include "intmath.h"
#include "complex.h"
#include "delay.h"

namespace FingerJetFxOSE {
namespace FpRecEngineImpl {
namespace Embedded {
namespace FeatureExtractionImpl {

  typedef complex<int8> ori_t;

  template <size_t stride, size_t ori_scale>
  inline void raw_orimap(size_t width, size_t size, const uint8 * inImage, bool compute_footprint, ori_t * ori, uint8 * footprint) {
    static const size_t ori_scale2 = ori_scale * ori_scale;
    static const size_t ori_stride = stride / ori_scale;
    const size_t ori_width = width / ori_scale;
    const size_t ori_size = size / ori_scale2;
    const int16 filler = 255; // typical background value: TODO: compute from the image

    delay<int16, stride + 1> x100(width + 1, filler * 2), x102(width + 1, filler * 5), x10d(width + 1), x11d(width + 1);
    delay<int16, stride - 1> x103(width - 1, filler * 10);
    inImage += (width + 1) * 2 + 4;  // offset to put in sync with orientation map
    const uint8 * p0 = inImage;
    const uint8 * p1 = inImage + width - 1;

    delay<int16, stride> x10c(width, filler * 5), x101(width, filler * 25), x201(width), x221(width), x301(width), x321(width);
    delay<int16, 1> x0, x10(filler * 50), x11, x20, x21, x22, x30, x31, x32, x33;
    delay<complex<int32>, 1> dom;

    for (size_t y = 0; y < ori_size - ori_width; y += ori_width) {
      complex<int32> ori_mag[ori_stride] = {0};
      for (size_t i = ori_scale; i; --i) {
        for (size_t x = 0; x < ori_width; ++x) {
          complex<int32> z(0);
          for (size_t j = ori_scale; j; ++p0, ++p1, --j) {
            int16 cur = int16(*p0) + *p1;
            int16 v1 = x100(cur) + cur + x0(*p0);
            int16 v2 = x102(v1) + v1;
            v1 = x103(v2) + v2 + x10c(v1);
            int16 v1x = x101(v1);
            int16 v10 = v1x + v1;
            v10 = x10(v10) - v10;
            int16 v11 = v1x - v1;
            v11 = x11(v11) + v11;
            complex<int32> Z1(v10, v11);
            Z1 = Z1 * Z1;

            int16 v10x = x201(v10);
            int16 v20 = v10x + v10;
            v20 = x20(v20) - v20;
            int16 v21 = v10x - v10;
            v21 = x21(v21) + v21;
            int16 v22 = x221(v11) - v11;
            v22 = x22(v22) + v22;

            int32 G20 = v20 + v22;
            int32 G21 = v20 - v22;
            int32 G22 = 2 * v21;
            complex<int32> Z2(G20 * G21, G20 * G22);

            int16 v20x = x301(v20);
            int16 v30 = v20x + v20;
            int32 X30 = x30(v30) - v30;
            int16 v31 = v20x - v20;
            int32 X31 = x31(v31) + v31;
            int16 v22x = x321(v22);
            int16 v32 = v22x + v22;
            int32 X32 = x32(v32) - v32;
            int16 v33 = v22x - v22;
            int32 X33 = x33(v33) + v33;

            complex<int32> Z3(5*(X30*X30 - X33*X33) + 3*(X31*X31 - X32*X32) + 2*(X30*X32 - X31*X33),
                              10*(X32*X33 + X30*X31) + 18*X31*X32 + 2*X30*X33);

            const int32 k1 = 0; //1; // 20;
            const int32 k2 = 1; // 24;
            const int32 k3 = 0; // 1;
            complex<int32> Z = k1 * Z1 + k2 * Z2 + k3 * Z3;
            z += Z;
          }
          ori_mag[x] += z;
        }
      }
      for (size_t x = 0; x < ori_width; ++x) {
        ori_t o = oct_sign(dom(ori_mag[x]), 50000); // 100000
        if (compute_footprint) footprint[x + y] = (o != ori_t(0)) ? 1 : 0;
        if (ori)               ori[x + y] = o;
      }
    }
    for (size_t x = ori_size - ori_width; x < ori_size; ++x) {
      if (compute_footprint) footprint[x] = 0;
      if (ori)               ori[x] = ori_t(0);
    }
  }

  template <size_t stride, size_t n, class F>
  inline void smooth_orimap(size_t width, size_t size, ori_t * ori, const uint8 * footprint, const F & postproc) {
    const static size_t n1 = n - 1;
    complex<int16> o1[stride] = {0}, o2[stride + n] = {0};
    delay<complex<int16>, n*stride> od1(n*width), od2(n*width);
    for (size_t y = 0; y < size + n1*width; y += width) {
      ori_t * pori = ori + y;
      delay<complex<int32>, n> od3(n), od4(n);
      complex<int32> o3(0), o4(0);
      for (size_t x = 0; x < width + n1; ++x) {
        complex<int32> o2t(0);
        if (x < width) {
          o1[x] += (y < size) ? pori[x] : 0;
          o2[x] += o1[x] - od1(o1[x]);
          o2t = o2[x] - od2(o2[x]);
        }
        if (y >= n1 * width) {
          o3 += o2t;
          o4 += o3 - od3(o3);
          if (x < n1) {
            od4(o4);
          } else {
            o2t = o4 - od4(o4);
            size_t pos = y + x - (width+1)*n1;
            ori[pos] = footprint[pos] ? postproc(o2t) : ori_t(0);
          }
        }
      }
    }
  }

  template <size_t stride, size_t boxsize, class T, class F>
  inline void boxfilt(size_t width, size_t size, T * inout, const F & f) {
    static const size_t n = boxsize;
    static const size_t n2 = boxsize / 2;
    T s1[stride] = {0};
    delay<T, n*stride> d1(n*width);
    for (size_t y = 0; y < size + n2*width; y += width) {
      T * p = inout + y;
      delay<T, n> d2(n);
      T s2(0);
      for (size_t x = 0; x < width + n2; ++x) {
        T t(0);
        if (x < width) {
			T v = (y < size) ? p[x] : 0;
			s1[x] += v;
			t = s1[x] -= d1(v);
        }
        if (y >= n2*width) {
          s2 += t; // SmallerTypeCheck
          if (x < n2) {
            d2(t);
          } else {
            t = s2 -= d2(t);
            t = f(t);
            inout[y - (width+1)*n2 + x] = t;
          }
        }
      }
    }
  }

  template <class T> inline T self(const T & x) { return x; }
  template <size_t stride, size_t boxsize, class T>
  inline void boxfilt(size_t width, size_t size, T * inout) {
    return boxfilt<stride, boxsize>(width, size, inout, self<T>);
  }

  template <uint8 N> inline uint8 g(uint8 x) { return x > N ? 1 : 0; }

  template <size_t stride, size_t size>
  inline void sqrt_orimap(ori_t * ori, uint8 * footprint) {
    for (size_t i = 0; i < size; i ++) {
      ori_t * pori = ori + i;
      if (pori[0] != ori_t(0)) {
        uint8 a = FingerJetFxOSE::atan2(pori[0].real(), pori[0].imag()) / 2;
        pori[0] = footprint[i] ? complex<int16>(cos(a), sin(a)) : 0;
      }
    }
  }

  // horizontal: fill_holes(1, width, width, size, footprint)
  // vertical:   fill_holes(width, size, 1, width, footprint)
  inline void fill_holes(size_t stride_x, size_t size_x, size_t stride_y, size_t size_y, uint8 * footprint) {
    for (size_t y = 0; y < size_y; y += stride_y) {
      uint8 * p = footprint + y;
      size_t x1, x2;
      for (x1 = 0; x1 < size_x; x1 += stride_x) {
        if (p[x1]) break;
      }
      for (x2 = size_x - stride_x; x2 > x1; x2 -= stride_x) {
        if (p[x2]) break;
      }
      for (size_t x = x1 + stride_x; x < x2; x += stride_x) {
        p[x] = 1;
      }
    }
  }

  template <int32 threshold> inline ori_t oct_sign_tr(const complex<int32> & x) {
    return oct_sign(x, threshold);
  }

  inline ori_t div2(const complex<int32> & x) {
    uint8 a = FingerJetFxOSE::atan2(x.real(), x.imag()) / 2;
    return ori_t(cos(a), sin(a));
  }

  template <size_t stride, size_t ori_scale>
  inline void orientation_map_and_footprint(
    size_t width, 
    size_t size, 
    const uint8 * inImage, 
    bool compute_footprint, 
    ori_t * ori, 
    uint8 * footprint
  ) {
    #define ori_stride (stride / ori_scale)  // Workaround for ARM compiler: static const does not work
    const size_t ori_width = width / ori_scale;
    const size_t ori_size = size / ori_scale / ori_scale;

    raw_orimap<stride, ori_scale>(width, size, inImage, compute_footprint, ori, footprint);
    if (compute_footprint) {
      boxfilt<ori_stride, 3>(ori_width, ori_size, footprint, g<3>);
      boxfilt<ori_stride, 5>(ori_width, ori_size, footprint, g<11>);
      boxfilt<ori_stride, 5>(ori_width, ori_size, footprint, g<11>);
      //boxfilt<ori_stride, 5>(ori_width, ori_size, footprint, g<11>);
      fill_holes(1, ori_width, ori_width, ori_size, footprint);
      fill_holes(ori_width, ori_size, 1, ori_width, footprint);
      //boxfilt<ori_stride, 5>(ori_width, ori_size, footprint, g<14>);
      boxfilt<ori_stride, 5>(ori_width, ori_size, footprint, g<14>);
      boxfilt<ori_stride, 5>(ori_width, ori_size, footprint, g<14>);
    }
    if (ori) {
      smooth_orimap<ori_stride, 5>(ori_width, ori_size, ori, footprint, oct_sign_tr<128>);
      smooth_orimap<ori_stride, 5>(ori_width, ori_size, ori, footprint, div2);
    }
    #undef ori_stride
  }

}
}
}
}

#endif // __orimap_h
