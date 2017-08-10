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

#ifndef __block_fft_h
#define __block_fft_h

#include <dpTypes.h>
#include <intmath.h>
#include <complex.h>
#include <bitset>

namespace FingerJetFxOSE {
namespace FpRecEngineImpl {
namespace Embedded {
namespace FeatureExtractionImpl {
namespace FFT {

  static const uint8 sin_bits = 5;
  inline int16 sin(int32 a) {
    const static int16 sinTable[] = {0, 799, 1567, 2276, 2896, 3406, 3784, 4017, 4096, 4017, 3784, 3406, 2896, 2276, 1567, 799};
    STATIC_ASSERT(countof(sinTable) == (1 << (sin_bits - 1)));
    return ((a & countof(sinTable)) ? -1 : 1) * sinTable[a & (countof(sinTable) - 1)];
  }

  inline int16 cos(int32 a) {
    return sin(a + 8);
  }

  using namespace FingerJetFxOSE;
  using namespace FingerJetFxOSE::FpRecEngineImpl::Embedded;

  template <uint8 bits, uint8 shift> struct bitreverser {
    static int32 reverse(int32 x) { 
      std::bitset<bits> s(x >> shift);
      for (uint8 i = 0; i < bits/2; i++) {
        bool t = s[i];
        s[i] = s[bits - i - 1];
        s[bits - i - 1] = t;
      }
      return int32(s.to_ulong() << shift);
    }
  };
  
  template <int32 a, uint8 s>
  inline int32 swap2bitsets(int32 x) {
    return ((x & a) << s) | ((x & (a << s)) >> s) | (x & ~((a << s) | a));
  }
  // bit reverse any 4 bits (4, 5, 9 are the examples)
  template <int32 a1, uint8 s1, int32 a2, uint8 s2>
  inline int32 bitreverse4(int32 x) {
    x = swap2bitsets<a1, s1>(x);
    x = swap2bitsets<a2, s2>(x);
    return x;
  }

  // reverse bits 0..8
  template <uint8 shift> struct bitreverser<9, shift> {
    static int32 reverse(int32 x) {
      return bitreverse4<0x49 << shift, 2, 0x7 << shift, 6>(x);
    }
  };
  // reverse bits 0..4
  template <uint8 shift> struct bitreverser<5, shift> {
    static int32 reverse(int32 x) {
      return bitreverse4<0x9 << shift, 1, 0x3 << shift, 3>(x);
    }
  };
  // reverse bits 0..3
  template <uint8 shift> struct bitreverser<4, shift> {
    static int32 reverse(int32 x) {
      return bitreverse4<0x5 << shift, 1, 0x3 << shift, 2>(x);
    }
  };

  template <uint8 bits, uint8 shift> static int32 bitreverse(int32 x) {
    return bitreverser<bits, shift>::reverse(x);
  }

  template <uint8 size_bits, uint8 stride_bits>
  inline void shuffle(int32 * data) {
    static const uint32 size   = 1 << size_bits;
    static const uint32 stride = 1 << stride_bits;
    for (int i = stride; i < size - stride; i += stride) {
      const int32 j = bitreverser<size_bits - stride_bits, stride_bits>::reverse(i);
      if (i > j) {
        std::swap(data[i], data[j]);
        std::swap(data[i+1], data[j+1]);
      }
    }
  }

  typedef complex<int32> ci32;

  template <bool inverse, uint8 size_bits, uint8 stride_bits>
  inline void fft(int32 * data) {
    static const int32 n = 1 << size_bits;
    static const uint32 stride = 1 << stride_bits;
    shuffle<size_bits, stride_bits>(data);
    int32 dt = (inverse ? 1 : -1) * (1 << sin_bits);
    for (uint8 ll = stride_bits; ll < size_bits; ll++) {
      const int32 mmax = 1 << ll;
      const int32 istep = mmax << 1;
      dt >>= 1;
      for (int32 m = 0, t = 0; m < mmax; m += stride, t += dt) {
        int32 wr = cos(t);
        int32 wi = sin(t);
        for (uint32 i = m; i < n; i += istep) {
          int32 * pi = data + i;
          int32 * pj = pi + mmax;
          int32 tr = reduce(wr * pj[0] - wi * pj[1], 12);
          int32 ti = reduce(wr * pj[1] + wi * pj[0], 12);
          pj[0] = pi[0] - tr;
          pj[1] = pi[1] - ti;
          pi[0] += tr;
          pi[1] += ti;
        }
      }
    }
  }

  template <bool real, bool inverse, uint8 size_bits, uint8 stride_bits>
  inline void fft1(int32 * data) {
    if (!inverse) {
      fft<inverse, size_bits, stride_bits>(data);
    }
    if (real) {
      static const int32 stride = 1 << stride_bits;
      static const int32 size = 1 << size_bits;
      int32 dt = (inverse ? 1: -1) * (1 << (sin_bits - (size_bits - stride_bits + 1)));
      int32 t = dt + (inverse ? (1 << (sin_bits - 1)) : 0);
      for (int32 i = stride; i <= size/2; i += stride, t += dt) {
        ci32 w(cos(t), sin(t));
        int32 * p1 = data + i;
        int32 * p2 = data + size - i;
        ci32 h1 = ci32(p2[0] + p1[0], p1[1] - p2[1]) << 12;
        ci32 h2 = w * ci32(p1[1] + p2[1], p2[0] - p1[0]);
        ci32 f1 = reduce(h1 + h2, 12 + (inverse ? 0 : 1));
        ci32 f2 = reduce(conj(h1 - h2), 12 + (inverse ? 0 : 1));
        p1[0] = f1.real(); p1[1] = f1.imag(); 
        p2[0] = f2.real(); p2[1] = f2.imag();
      }
      int32 tr = data[0], ti = data[1];
      data[0] = tr + ti; data[1] =  inverse ? (tr - ti) : 0; //0; // suppress highest freaquency
    }
    if (inverse) {
      fft<inverse, size_bits, stride_bits>(data);
    }
  }

  template <bool real, bool inverse, uint8 dim_bits>
  inline void fft2(int32 * data) {
    const int32 size1 = 1 << dim_bits;
    const int32 size2 = size1 << dim_bits;
    if (!inverse) {
      for (int32 y = 0; y < size2; y += size1) {
        fft1<real, inverse, dim_bits, 1>(data + y);
      }
    }
    for (int32 x = 0; x < size1; x += 2) {
      fft<inverse, dim_bits*2, dim_bits>(data + x);
    }
    if (inverse) {
      for (int32 y = 0; y < size2; y += size1) {
        fft1<real, inverse, dim_bits, 1>(data + y);
      }
    }
  }

  template <uint8 size_bits, uint8 shift>
  inline void reduce_array(int32 * data) {
    for (int32 * end = data + (1 << size_bits); data < end; data++) {
      *data = reduce(*data, shift);
    }
  }

  template <uint8 dim_bits, class F>
  inline void enhance_array(int32 * data, F f) {
    int32 size = 1 << dim_bits;
    int32 halfsize = 1 << (dim_bits - 1);
    for (int32 y = 0; y < size; y++) {
      for (int32 x = 0; x < halfsize; x++, data += 2) {
        ci32 v(data[0], data[1]);
        v = f(v, x, y - ((y < halfsize) ? 0 : size));
        data[0] = v.real(); data[1] = v.imag();
      }      
    }
  }

  inline ci32 enhance(ci32 v, int32 x, int32 y) {
   /* const static uint8 mask[16][16] = {
         0,     0,     0,   199,   236,   236,   199,   169,   143,   121,   102,    87,    73,    62,     0,     0,
         0,     0,     0,   205,   240,   232,   197,   167,   141,   120,   102,    86,    73,    62,     0,     0,
         0,     0,   194,   221,   255,   221,   189,   161,   137,   117,    99,    84,    71,    61,     0,     0,
       199,   205,   221,   245,   236,   205,   177,   152,   130,   112,    95,    81,    69,    59,     0,     0,
       236,   240,   255,   236,   211,   186,   163,   141,   122,   105,    90,    77,    66,    56,     0,     0,
       236,   232,   221,   205,   186,   167,   147,   129,   112,    97,    84,    72,    62,    53,     0,     0,
       199,   197,   189,   177,   163,   147,   132,   117,   102,    89,    78,    67,    58,     0,     0,     0,
       169,   167,   161,   152,   141,   129,   117,   104,    92,    81,    71,    62,    54,     0,     0,     0,
       143,   141,   137,   130,   122,   112,   102,    92,    82,    73,    64,    56,     0,     0,     0,     0,
       121,   120,   117,   112,   105,    97,    89,    81,    73,    65,    58,     0,     0,     0,     0,     0,
       102,   102,    99,    95,    90,    84,    78,    71,    64,    58,     0,     0,     0,     0,     0,     0,
        87,    86,    84,    81,    77,    72,    67,    62,    56,     0,     0,     0,     0,     0,     0,     0,
        73,    73,    71,    69,    66,    62,    58,    54,     0,     0,     0,     0,     0,     0,     0,     0,
        62,    62,    61,    59,    56,    53,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    }; //fex_newweight3 */

   /* const static uint8 mask[16][16] = {
       0,     0,     0,   178,   230,   236,   209,   174,   152,   142,   136,   129,   122,   117,     0,     0,
       0,     0,     0,   202,   244,   243,   214,   178,   156,   144,   136,   127,   118,   110,     0,     0,
       0,     0,   190,   226,   253,   240,   207,   172,   152,   141,   133,   124,   115,   107,     0,     0,
     178,   202,   226,   255,   253,   228,   193,   163,   147,   138,   129,   120,   113,   106,     0,     0,
     230,   244,   253,   253,   236,   206,   175,   154,   143,   135,   125,   116,   109,   104,     0,     0,
     236,   243,   240,   228,   206,   179,   158,   147,   139,   130,   120,   111,   104,    99,     0,     0,
     209,   214,   207,   193,   175,   158,   148,   141,   134,   125,   114,   105,    98,     0,     0,     0,
     174,   178,   172,   163,   154,   147,   141,   135,   127,   117,   108,    99,    92,     0,     0,     0,
     152,   156,   152,   147,   143,   139,   134,   127,   118,   109,   100,    93,     0,     0,     0,     0,
     142,   144,   141,   138,   135,   130,   125,   117,   109,   101,    93,     0,     0,     0,     0,     0,
     136,   136,   133,   129,   125,   120,   114,   108,   100,    93,     0,     0,     0,     0,     0,     0,
     129,   127,   124,   120,   116,   111,   105,    99,    93,     0,     0,     0,     0,     0,     0,     0,
     122,   118,   115,   113,   109,   104,    98,    92,     0,     0,     0,     0,     0,     0,     0,     0,
     117,   110,   107,   106,   104,    99,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    }; //fex_newweight3d2 */

    /*const static uint8 mask[16][16] = {
       0,     0,     0,     0,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,
       0,     0,     0,     0,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,
       0,     0,     0,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,
       0,     0,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,     0,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,     0,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,     0,     0,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,     0,     0,     0,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,     0,     0,     0,     0,
     255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,     0,     0,     0,     0,     0,
     255,   255,   255,   255,   255,   255,   255,   255,     0,     0,     0,     0,     0,     0,     0,     0,
     255,   255,   255,   255,   255,   255,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
  };*/ //fex_newweight3_tmp 

   /* x = abs(x); y = abs(y);
    v = reduce(v * int32(mask[x][y]), 8);*/

    int32 r2 = sqr(x) + sqr(y);
    if (r2 <= 6 || r2 >= 169) {
      return 0;
    }

    int32 a = reduce(oct_abs(v), 5);
    ci32 v1 = reduce(v * a, 7); //7
    v = reduce(v + v1, 3);
    return v;
  }

  template <size_t size, size_t spacing>
  class envelope {
    const static size_t _mirr = size + 1 - spacing;
    bool left, right;
  public:
    envelope(bool left_, bool right_) : left(left_), right(right_) {
      STATIC_ASSERT(spacing <= size);
    }
    int32 operator [](int32 i) const { 
      const static size_t _floor = _mirr < spacing ? _mirr : spacing;
      int32 t = (int32) std::min(std::min(left ? _floor : i + 1, right ? _floor : size - i), _floor); 
      return t;
    }
    int32 norm() const { return _mirr; }
  };

  template <uint8 dim_bits, int32 spacing>
  inline void normalize(int32 * data, const envelope<1 << dim_bits, spacing> & xenv, const envelope<1 << dim_bits, spacing> & yenv) {
    int32 * end = data + (1 << (dim_bits * 2));
    int32 mn = *std::min_element(data, end);
    int32 mx = *std::max_element(data, end);
    int32 range = mx - mn;
    int32 div = range;
    int32 trsh = 16;
    if (range < trsh) {
      div = trsh;
      mn -= (trsh - range) / 2;
    }
    div *= xenv.norm() * yenv.norm();
    const static int a = 1 << dim_bits;
    for (int y = 0; y < a; ++y) {
      int32 ye = yenv[y];
      for (int x = 0; x < a; ++x, ++data) {
        *data = divide((*data - mn) * 251 * xenv[x] * ye, div);
      }
    }
  }

  template <uint8 dim_bits, int32 spacing>
  inline void enhance_block(int32 * data, const envelope<1 << dim_bits, spacing> & xenv, const envelope<1 << dim_bits, spacing> & yenv) {
    fft2<true, false, dim_bits>(data);
    enhance_array<dim_bits>(data, enhance);
    fft2<true, true, dim_bits>(data);
    reduce_array<dim_bits*2, dim_bits*2>(data);
    normalize<dim_bits, spacing>(data, xenv, yenv);
  }

}
}
}
}
}

#endif

