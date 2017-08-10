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

#ifndef __MATCHDATA_H
#define __MATCHDATA_H

#include "dpError.h"
#include "dpTypes.h"
#include "ScaleFactors.h"
#include "version.h"
#include <algorithm>
#include "fixed_allocator.h"
#include <integer_limits.h>

namespace FingerJetFxOSE {
namespace FpRecEngineImpl {
namespace Embedded {
  
  // Scale down signed integer types with rounding
  // Two properties: 
  // a) rounding is done at 0.5
  // b) for every x, round(x) - round(x - 1) == 1 (operation should not favor any bin)
  // bits: scale down by 2^bits, bits should be >= 1
  template <class T> inline T ScaleDown(T x, uint8 bits) {
    return (x + (1 << (bits-1))) >> bits;
  }
  inline int16 ToShort(int32 x) {
    return int16(ScaleDown(x, bitsScaleFactor));
  };

  using std::min;

  struct Point {
    int16 x;
    int16 y;
    Point() : x(0), y(0) {}
    Point(int16 x_, int16 y_) : x(x_), y(y_) {}
    Point operator += (Point p) {
      x += p.x;
      y += p.y;
      return *this;
    }
    Point operator -= (Point p) {
      x -= p.x;
      y -= p.y;
      return *this;
    }
  };
  inline Point operator + (Point p1, Point p2) {
    return Point(p1.x + p2.x, p1.y + p2.y);
  }
  inline Point operator - (Point p1, Point p2) {
    return Point(p1.x - p2.x, p1.y - p2.y);
  }

  inline uint32 Distance2(Point p) {
    return int32(p.x) * p.x + int32(p.y) * p.y;
  }
  struct Angle {
    int16 c; // cosine * angleScale
    int16 s; // sine * angleScale
    Angle() : c(angleScale), s(0) {}
    Angle(int16 c_, int16 s_) : c(c_), s(s_) {}
    Angle(uint8 a) : c(cos(a)), s(sin(a)) {}
    static int16 sin(uint8 a) {
      const static int16 sinTable[128] = {
        0,   6,   13,  19,   25,  31,  38,  44,   50,  56,  62,  68,   74,  80,  86,  92, 
        98,  104, 109, 115,  121, 126, 132, 137,  142, 147, 152, 157,  162, 167, 172, 177,
        181, 185, 190, 194,  198, 202, 206, 209,  213, 216, 220, 223,  226, 229, 231, 234, 
        237, 239, 241, 243,  245, 247, 248, 250,  251, 252, 253, 254,  255, 255, 256, 256, 

        256, 256, 256, 255,  255, 254, 253, 252,  251, 250, 248, 247,  245, 243, 241, 239, 
        237, 234, 231, 229,  226, 223, 220, 216,  213, 209, 206, 202,  198, 194, 190, 185, 
        181, 177, 172, 167,  162, 157, 152, 147,  142, 137, 132, 126,  121, 115, 109, 104, 
        98,  92,  86,  80,   74,  68,  62,  56,   50,  44,  38,  31,   25,  19,  13,  6, 
      };           
      return ((a & 0x80) ? -1 : 1) * sinTable[a & 0x7f];
    }
    static int16 cos(uint8 a) {
      return sin((a + 64 & 0xff));
    }
    void Assign(uint8 a) {
      *this = Angle(a);
    }
    static void swap(int16 &c, int16 &s) {
      int16 t = c;
      c = s;
      s = t;
    }
    static uint8 atan2(int16 c, int16 s) {
      const uint8 NumAtanEntries = 96;  // To reduce rounding errors: dAtan(x)/dx /. x->0 == 3 Pi / 4 that corresponds to 96
      const static uint8 atan[NumAtanEntries + 1] = { // + 1 Pi/4 point : c == s;
        0, 0, 1, 1, 2, 2, 3, 3, 3, 4, 4, 5, 5, 5, 6, 6, 7, 7, 8, 8, 8, 9, 9, 10, 10, \
        10, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, \
        18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, \
        24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 28, 28, 28, 28, 29, \
        29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32
      };
      bool sn = s < 0;
      if (sn) s = -s;
      bool cn = c < 0;
      if (cn) c = -c;
      bool cls = c < s;
      if (cls) swap(c, s);
      uint8 out = atan[s * NumAtanEntries / c];
      if (cls) out = 0x40 - out;
      if (cn) out = 0x80 - out;
      if (sn) out = - out;
      return out;
    }
    operator uint8 () const {
      return atan2(c, s);
    }
    Angle operator - () const {
      return Angle(c, -s);
    }
  };
  inline Point Rotate(Point p, Angle a) {
    Point r;
    r.x = int16(ScaleDown(int32(p.x) * a.c - int32(p.y) * a.s, bitsAngleScale));
    r.y = int16(ScaleDown(int32(p.x) * a.s + int32(p.y) * a.c, bitsAngleScale));
    return r;
  }
  // scale factor is equal to scale / (2 ^ bitsShift)
  // i.e. scale = 3, bitsShift = 2 : 3.0 / (2.0 ^ 2)  = 0.75
  inline Point RotateAndScale(Point p, Angle a, int16 scale, uint8 bitsShift) {
    uint8 shift = bitsAngleScale + bitsShift;
    Point r;
    r.x = int16(ScaleDown(scale * (int32(p.x) * a.c - int32(p.y) * a.s), shift));
    r.y = int16(ScaleDown(scale * (int32(p.x) * a.s + int32(p.y) * a.c), shift));
    return r;
  }

  struct Minutia {
    Point position;
    uint8 pmp, conf;
    uint8 theta; 
    uint8 type;
    enum {
      type_other = 0,
      type_ridge_ending = 1,
      type_bifurcation = 2,
    };

    Minutia() : pmp(0), conf(0), theta(0), type(type_other) {} // minimum acceptable
  };

  struct Footprint {
    static const int height = 50;
    static const int width = 32;
    uint32 data[height];
    uint32 area;
    Footprint() {
      Init();
    }
    void Init() {
      area = 0;
      for (int i = height; i; ) data[--i] = 0;
    }
    // in pixels
    bool InBounds(int32 x, int32 y) const {
      return (uint32(x) < uint32(width)) && (uint32(y) < uint32(height));
    }
    // get
    bool GetPixel(int32 x, int32 y) const {
      return InBounds(x,y) && (data[y] & (1 << x));
    }
    // set, ignore out of bounds
    struct boolref {
      uint32 & row;
      uint32 mask;
      boolref(uint32 & row_, uint32 mask_) 
        : row(row_), mask(mask_) {}
      bool operator = (bool val) {
        if (val) row |= mask; else row &= ~mask;
        return val;
      }
    };
    boolref Pixel(int32 x, int32 y) {
      return InBounds(x,y) ? boolref(data[y], 1 << x) : boolref(data[0], 0);
    }
    // in world co-ord, read only
    bool operator() (int32 x, int32 y) const {
      return GetPixel(x / 8, y / 8);
    }
    bool operator() (Point p) const {
      return GetPixel(p.x / 8, p.y / 8);
    }
    // Compute footprint area: this is not the same as field area, due to losses in compression/representation
    uint32 ComputeArea() const {
      uint32 area = 0;
      for (int y = 0; y < height; y++) {  // my co-ord
        for (int x = 0; x < width; x++) { // my co-ord
          if (GetPixel(x, y)) {
            area++;
          }
        }
      }
      return area * 8 * 8;
    }
  };

  template <size_t maxNumMin>
  struct MinutiaPoints {
    static const size_t Capacity = maxNumMin;
    Minutia minutia[Capacity];
    size_t numMinutia;
    Point  center;       // precomputed

    MinutiaPoints() : numMinutia(0) {}
    static size_t capacity() {
      return Capacity;
    }
    size_t size() const {
      return numMinutia;
    }
    // rotate and shift so that all points' coordinates are positive
    void RotateAndShift(uint8 rotation) {
      size_t i;
      for (i = 0; i < size(); i++) {
        minutia[i].position = Rotate(minutia[i].position, rotation);
        minutia[i].theta += rotation;
      }
      Point pmin(integer_limits<int16>::max, integer_limits<int16>::max);
      for (i = 0; i < size(); i++) {
        pmin.x = min(pmin.x, minutia[i].position.x);
        pmin.y = min(pmin.y, minutia[i].position.y);
      }
      for (i = 0; i < size(); i++) {
        minutia[i].position -= pmin;
      }
    }
  };

  inline bool CompareMinutiaByConfidence(const Minutia&  mp1, const Minutia& mp2)
  {
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
  inline bool operator > (const Minutia & mp1, const Minutia & mp2) {
    return CompareMinutiaByConfidence(mp1, mp2);
  }

  struct MatchData : public MinutiaPoints</* remove limit: 76*/255> {
    Footprint footprint;
    Point offset;

    void Init() {
      footprint.Init();
    }
    void Precompute() {
    }
    template <size_t n1> MatchData & operator = (const MinutiaPoints<n1> & mp) {
      static_cast<MinutiaPoints<Capacity> &>(*this) = mp;
      return *this;
    }
  };

} // namespace
}
}

#endif // __MATCHDATA_H
