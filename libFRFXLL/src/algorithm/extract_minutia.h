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

#ifndef __extract_minutia_H
#define __extract_minutia_H

#include "complex.h"
#include "biffilt.h"
#include "delay.h"
#include "diagnostics.h"
#include "top_n.h"
#include <matchData.h>
#include <bitset>

namespace FingerJetFxOSE {
namespace FpRecEngineImpl {
namespace Embedded {
namespace FeatureExtractionImpl {

  template <size_t stride, int32 t0, int32 t1, uint8 norm_bits>
  struct conv2d3 {
    static const size_t xoffs = 1;
    static const size_t yoffs = 1;

    delay<int32, stride> dv1, dv2;
    delay<int32, 1> dh1, dh2;

    conv2d3(size_t width) 
      : dv1(width)
      , dv2(width) 
    {
    }
    int32 operator () (int32 v0) {
      int32 v1 = dv1(v0);
      int32 v2 = dv2(v1);
      int32 h0 = v1*t0 + (v2 + v0)*t1;
      int32 h1 = dh1(h0);
      int32 h2 = dh2(h1);
      int32 o = h1*t0 + (h2 + h0)*t1;
      return (o + (1 << (norm_bits - 1))) >> norm_bits;
    }
  };

  //template <class T, size_t stride>
  //struct max2d5 {
  //  delay<T, stride*2+1> d1;
  //  delay<T, stride-2> d2;
  //  delay<T, stride> d3;
  //  delay<T, 1> d4;
  //  delay<T, stride*2> dc;
  //
  //  bool operator () (T v) {
  //    T v1 = max(d1(v), v);
  //    T v2 = max(d2(v1), v1);
  //    T v3 = max(d3(v2), v2);
  //    T v4 = max(d4(v3), v3);
  //    return dc(v) >= v4;
  //  }
  //};
  template <class T, size_t stride>
  struct max2d5 {
    static const size_t xoffs = 2;
    static const size_t yoffs = 2;

    T buffer[5][stride+4];
    max2d5() {
      memset(buffer, -1, sizeof(buffer));
    }

    /// v < 0 means outside of the footprint
    bool operator () (T v, size_t x, size_t y) {
      buffer[(y+4)%5][x+4] = v;
      T center = buffer[(y+2)%5][x+2];
      if (center > 0) {
        for (size_t y1 = 0; y1 < 5; y1++) {
          for (size_t x1 = x; x1 < x + 5; x1++) {
            T cur = buffer[y1][x1];
            if (cur < 0 || center < cur) {
              return false;
            }
          }
        }
        return true;
      }
      return false;
    }
  };

  template <class T, size_t stride>
  struct max2d5fast {
    static const size_t xoffs = 4;
    static const size_t yoffs = 4;

    T buffer[7][stride];
    std::bitset<stride+xoffs> mxset[7];
    max2d5fast() {
      memset(buffer, -1, sizeof(buffer));
    }

    /// v < 0 means outside of the footprint
    bool operator () (T v, size_t x, size_t y) {
      int y_=y%7;
      buffer[y_][x] = v;
      if (x == 0) {
        if (mxset[(y-1)%7].any()) {
          mxset[(y-1)%7].reset();
        }
      }
      if ((y >= 6 && y%3 == 0) && (x >= 6 && x%3 == 0)) {
        size_t i = y - 4;
        size_t j = x - 4;

        FindMaxInBlock(i, j);
      }
      if (mxset[y_].test(x)) {
        mxset[y_].reset(x);
        return true;
      }

      return false;
    }
    void FindMaxInBlock(size_t i, size_t j) {
      size_t mi = i;
      size_t mj = j;
      T mValue = buffer[mi%7][mj];
      for (size_t i2 = i; i2 <= i+2; i2++) {
        for (size_t j2 = j; j2 <= j+2; j2++) {
          if (buffer[i2%7][j2] < 0) {
            return;
          }
          if (buffer[i2%7][j2] > mValue) {
            mi = i2;
            mj = j2;
            mValue = buffer[mi%7][mj];
          }
        }
      }
      if (mValue <= 0) {
        return;
      }

      for (size_t i2 = mi-2; i2 <= mi+2; i2++) {
        for (size_t j2 = mj-2; j2 <= mj+2; j2++) {
          if (i2 < i || i2 > i+2 || j2 < j || j2 > j+2) {
            if (buffer[i2%7][j2] < 0 || buffer[i2%7][j2] > mValue) {
              return;
            }
          }
        }
      }
      mxset[(mi+yoffs)%7].set(mj+xoffs);
    }
  };

  struct SmmeParameters {
    struct PixelG {
      int32 gxx, gxy, gyy;
      PixelG(int32 gxx_, int32 gxy_, int32 gyy_) : gxx(gxx_), gxy(gxy_), gyy(gyy_) {}
      PixelG() : gxx(0), gxy(0), gyy(0) {}
    };
    int32 Tb;
    Diagnostics::PixelsSubscriber<PixelG>::ptr_t diagG;
    Diagnostics::PixelsSubscriber<bool>::ptr_t   diagMC;
    Diagnostics::PixelsSubscriber<uint8>::ptr_t  diagOri;
    SmmeParameters() : Tb(1328) {} // 0.1 * 13280
  };
  struct UserFeedbackParameters {
    uint32              minimum_number_of_minutia;
    uint32              minimum_footprint_area;
    UserFeedbackParameters()
    /* secunet change: remove limits
      : minimum_number_of_minutia(4)
      , minimum_footprint_area(11000) */
      : minimum_number_of_minutia(0)
      , minimum_footprint_area(0)
    {}
  };
  struct Parameters {
    biffilt::parameters    biffilt_;
    SmmeParameters         smme;
    UserFeedbackParameters user_feedback;
  };

  template <uint8 neibourhood>
  bool is_in_footprint(size_t xp, size_t yp, size_t width, size_t size, const uint8 * phasemap) {
    xp += neibourhood;
    yp += neibourhood;
    size_t offs = xp + yp * width;
    return xp >= neibourhood * 2 && yp >= neibourhood * 2 && xp < width && offs < size 
      && phasemap[offs - 2 * neibourhood] != phasemap_filler
      && phasemap[offs] != phasemap_filler
      && phasemap[offs - (width * 2 + 2) * neibourhood] != phasemap_filler
      && phasemap[offs - width * 2 * neibourhood] != phasemap_filler;
  }

  struct a_point : public Point {
    uint8 a, v;
    a_point(uint8 v_ = 0) : v(v_) {}
    a_point(int16 x, int16 y, uint8 a_, const uint8 * p, size_t width) 
      : Point(x, y), a(a_), v(p[x + y * width]) {
    }

    a_point next(const uint8 * p, size_t width, size_t size, bool mn, bool relative) const {
      const static int8 offs[8] = {0, 1, 1, 1, 0, -1, -1, -1};
      a_point out = mn ? 255 : 0;
      for (int8 i = -1; i <= 1; ++i) {
        a_point cur(x + offs[(a + i) & 7], y + offs[(a + i - 2) & 7], relative ? a + i & 0xff : a, p, width);
        if (mn ? cur.v <= out.v : cur.v >= out.v) {
          out = cur;
        }
      }
      return out;
    }
  };

  inline bool adjust_angle(uint8 &a, size_t xp, size_t yp, const uint8 * p, size_t width, size_t size, bool relative) {
    a_point start(int16(xp), int16(yp), a >> 5, p, width);
    a_point cur = start.next(p, width, size, false, relative);
    a_point p2 = start.next(p, width, size, true, relative);
    bool mn = (255 - p2.v) > cur.v;
    if (mn) cur = p2;
    uint32 d0 = 0;
    int i;
    for (i = 0; i < 20; ++i) {
      cur = cur.next(p, width, size, mn, relative);
      if (mn == (cur.v >= 128)) break;
      if (!is_in_footprint<1>(cur.x, cur.y, width, size, p)) break;
      uint32 d = Distance2(cur - start);
      if (d <= d0) break;
      d0 = d;
      if (d > 400) break;
      if (i == 0) p2 = cur;
      // const_cast<uint8 &>(p[cur.x + width * cur.y]) = mn ? 255 : 0;
    }
    if (d0 >= 14 * 14) {
      Point base = cur - p2;
      a = FingerJetFxOSE::atan2(-base.y, base.x);
    } else {
      return false;
    }
    return true;
  }

  using std::complex;
  template <size_t stride, size_t ori_scale, class Minutia>
  void extract_minutia(const uint8 * phasemap, size_t width, size_t size, const uint8 * footprint, top_n<Minutia>& top_minutia, const Parameters & param) {
    static const uint8 invalid = 255;
    static const int32 invalidB = -1;
    static const size_t orifilt_size = 13;
    static const size_t yoffs = 3;
    const uint8 * p = phasemap + width * yoffs;
    const uint8 * end = phasemap + size - width;
    const size_t height = size / width;
    FRFXLL_DIAG(param.smme.diagG, SetSize, width, height);
    FRFXLL_DIAG(param.smme.diagMC, SetSize, width, height);
    FRFXLL_DIAG(param.smme.diagOri, SetSize, width, height);
    conv2d3<stride,2,1,5> cxx(width), cxy(width), cyy(width);
    typedef max2d5fast<int32, stride> max5_t;
    max5_t max5;
    delay<bool, stride * (orifilt_size - max5_t::yoffs) - max5_t::xoffs> delay_mc(width * (orifilt_size - max5.yoffs) - max5.xoffs);
    biffilt bf(width, size, phasemap, param.biffilt_);

    delay<complex<int8>, orifilt_size*stride/2> delay_ori_y(orifilt_size * width / 2);
    complex<int16> ori_s1[stride/2] = {0};
    uint8 direction[stride/2] = {0};
    for (size_t y = 0; y < height + orifilt_size; y++) {
      for (size_t x = 0; x < width; p++, x++) {
        bool outside = p >= end;
        int32 gx = 0;
        int32 gy = 0;
        if (!outside) {
          outside = (p[1] == invalid) || (p[-3] == invalid) || (p[width] == invalid) || (p[-3*width] == invalid);
          gx = int32(p[1]) - p[-1];
          gy = int32(p[width]) - p[-int32(width)];
        }
        int32 gxx = gx*gx, gxy = gx*gy, gyy = gy*gy;
        gxx = cxx(gxx); gxy = cxy(gxy); gyy = cyy(gyy);
        FRFXLL_DIAG(param.smme.diagG, SetPixel, x, y, SmmeParameters::PixelG(gxx, gxy, gyy));
        int32 Tb = param.smme.Tb;
        bool e1b = gxx + gyy > 2 * Tb;
        int32 b2 = (Tb - gxx)*(Tb - gyy) - gxy*gxy;
        bool e2b = b2 > 0;
        bool e = (e1b && e2b);
        // if (p[0] == 255) continue; // outside of footprint
        if ((x & 1)==0 && (y & 1)==0) {
          complex<int8> ori = oct_sign(complex<int32>(gxx-gyy, 2*gxy));
          complex<int16> & s1 = ori_s1[x/2];
          s1 += ori; 
          s1 -= delay_ori_y(ori);
        }
        bool mc = delay_mc(max5(outside ? invalidB : e ? b2 : 0, x, y));
        size_t xp = x - cxx.xoffs;
        size_t yp = y + yoffs - cxx.yoffs - orifilt_size;
        mc = mc && is_in_footprint<1>(xp, yp, width, size, phasemap);
        FRFXLL_DIAG(param.smme.diagMC, SetPixel, x, y, mc);
        FRFXLL_DIAG(param.smme.diagOri, SetPixel, x, y, direction[x/2]);
        if (mc) {
          // Minutia candidate
          int confidence = 0;
          bool confirmed = false;
          bool type = false;
          uint8 a;
          for (int i = -2; i <= 2; i += 4) {
            uint8 a0 = direction[x/2] + i;
            int8 c = cos(a0), s = sin(a0);
            if (bf(xp, yp, c, s)) {
              confirmed = true;
              if (bf.confidence > confidence) {
                confidence = bf.confidence;
                a = a0;
                type = bf.type;
              }            
            }
          }
          if (confirmed) {
            // Minutia
            Minutia m;
            m.position.x = int16(xp); // + FingerJetFxOSE::reduce<int16>(bf.xoffs, 7);
            m.position.y = int16(yp); // + FingerJetFxOSE::reduce<int16>(bf.yoffs, 7);
            if (bf.rotate180) {
              a = a + 128 & 0xff;
            }
            if (!adjust_angle(a, xp, yp, phasemap, width, size, false)) {
              adjust_angle(a, xp, yp, phasemap, width, size, true);
            }
            m.theta = a;
            m.conf = confidence; //bf.confidence;
            m.type = confidence > param.biffilt_.type_thresold ? type ? Minutia::type_ridge_ending : Minutia::type_bifurcation : Minutia::type_other; //bf.confidence 
            top_minutia.add(m);
          }
        }
      }
      if ((y & 1)==0) {
        complex<int32> s2(0);
        for (size_t x = 0; x < width/2 + orifilt_size/2; x++) {
          s2 += x < width/2 ? ori_s1[x] : 0;
          s2 -= x >= orifilt_size ? ori_s1[x - orifilt_size] : 0;
          if (x >= orifilt_size/2) {
            uint8 a = FingerJetFxOSE::atan2(s2.real(), s2.imag()) / 2;
            direction[x - orifilt_size/2] = a;
          }
        }
      }
    }
  }

} // namespace
}
}
}

#endif //  __extract_minutia_H
