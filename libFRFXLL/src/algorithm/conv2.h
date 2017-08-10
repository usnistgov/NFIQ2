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

#ifndef __CONV2_H
#define __CONV2_H

#include <stdlib.h>
#include <string.h>

#include "dpTypes.h"

namespace FingerJetFxOSE {
  namespace FpRecEngineImpl {
    namespace Embedded {

      struct symmetric {};
      struct autodetect {};

      template <class symmetry, int32 t0> struct addsub {
        static int32 apply(int32 x, int32 y) {
          return x + y;
        }
      };
      template <class symmetry> struct addsub<symmetry, 0> {
        static int32 apply(uint32 x, uint32 y) {
         return x - y;
        }
      };
      template <> struct addsub<symmetric, 0> {
        static int32 apply(uint32 x, uint32 y) {
         return x + y;
        }
      };

      // 9x9
      template <int32 t0, int32 t1, int32 t2, int32 t3, int32 t4, class symmetry = autodetect>
      class conv9 {
        static const size_t length = 8;
        int32 * p;
        int32 buffer[length*2];
      public:
        conv9() : p(buffer) {
          memset(buffer, 0, sizeof(buffer));
        }
        void add(int32 v) {
          p[0] = p[length] = v;
          if (++p >= buffer + length) {
            p = buffer;
          }
        }
        int32 operator () (int32 v) {
          int32 v4 = addsub<symmetry,t0>::apply(v, p[0]);
          int32 v3 = addsub<symmetry,t0>::apply(p[length-1], p[1]);
          int32 v2 = addsub<symmetry,t0>::apply(p[length-2], p[2]);
          int32 v1 = addsub<symmetry,t0>::apply(p[length-3], p[3]);
          int32 out = p[4] * t0 + v1 * t1 + v2 * t2 + v3 * t3 + v4 * t4;
          add(v);
          return out; //(out + 128) >> 8;
        }
        static int32 vert(size_t width, const uint8 * p) {
          int32 w = int32(width);
          int32 v1 = addsub<symmetry,t0>::apply(p[w],   p[-w]);
          int32 v2 = addsub<symmetry,t0>::apply(p[w*2], p[-w*2]);
          int32 v3 = addsub<symmetry,t0>::apply(p[w*3], p[-w*3]);
          int32 v4 = addsub<symmetry,t0>::apply(p[w*4], p[-w*4]);
          int32 out = p[0] * t0 + v1 * t1 + v2 * t2 + v3 * t3 + v4 * t4;
          return out; // (out + 128) >> 8;
        }
      };

      //13x13
      // symmetry,t0 != 0 : means symmetric, symmetry,t0 == 0 : odd means anti-symmetric
      template <int32 t0, int32 t1, int32 t2, int32 t3, int32 t4, int32 t5, int32 t6, class symmetry = autodetect>
      class conv13 {
        static const size_t length = 12;
        int32 * p;
        int32 buffer[length*2];
      public:
        conv13() : p(buffer) {
          memset(buffer, 0, sizeof(buffer));
        }
        void add(int32 v) {
          p[0] = p[length] = v;
          if (++p >= buffer + length) {
            p = buffer;
          }
        }
        int32 operator () (int32 v) {
          int32 v6 = addsub<symmetry,t0>::apply(v, p[0]);
          int32 v5 = addsub<symmetry,t0>::apply(p[length-1], p[1]);
          int32 v4 = addsub<symmetry,t0>::apply(p[length-2], p[2]);
          int32 v3 = addsub<symmetry,t0>::apply(p[length-3], p[3]);
          int32 v2 = addsub<symmetry,t0>::apply(p[length-4], p[4]);
          int32 v1 = addsub<symmetry,t0>::apply(p[length-5], p[5]);
          int32 out = p[4] * t0 + v1 * t1 + v2 * t2 + v3 * t3 + v4 * t4 + v5 * t5 + v6 * t6;
          add(v);
          return out;
        }
        static int32 vert(size_t width, const uint8 * p) {
          int32 w = int32(width);
          int32 v1 = addsub<symmetry,t0>::apply(p[w],   p[-w]);
          int32 v2 = addsub<symmetry,t0>::apply(p[w*2], p[-w*2]);
          int32 v3 = addsub<symmetry,t0>::apply(p[w*3], p[-w*3]);
          int32 v4 = addsub<symmetry,t0>::apply(p[w*4], p[-w*4]);
          int32 v5 = addsub<symmetry,t0>::apply(p[w*5], p[-w*5]);
          int32 v6 = addsub<symmetry,t0>::apply(p[w*6], p[-w*6]);
          int32 out = p[0] * t0 + v1 * t1 + v2 * t2 + v3 * t3 + v4 * t4 + v5 * t5 + v6 * t6;
          return out;
        }
      };

      //17x17
      template <int32 t0, int32 t1, int32 t2, int32 t3, int32 t4, int32 t5, int32 t6, int32 t7, int32 t8, class symmetry = autodetect>
      class conv17 {
        static const size_t length = 16;
        int32 * p;
        int32 buffer[length*2];
      public:
        conv17() : p(buffer) {
          memset(buffer, 0, sizeof(buffer));
        }
        void add(int32 v) {
          p[0] = p[length] = v;
          if (++p >= buffer + length) {
            p = buffer;
          }
        }
        int32 operator () (int32 v) {
          int32 v8 = addsub<symmetry,t0>::apply(v, p[0]);
          int32 v7 = addsub<symmetry,t0>::apply(p[length-1], p[1]);
          int32 v6 = addsub<symmetry,t0>::apply(p[length-2], p[2]);
          int32 v5 = addsub<symmetry,t0>::apply(p[length-3], p[3]);
          int32 v4 = addsub<symmetry,t0>::apply(p[length-4], p[4]);
          int32 v3 = addsub<symmetry,t0>::apply(p[length-5], p[5]);
          int32 v2 = addsub<symmetry,t0>::apply(p[length-6], p[6]);
          int32 v1 = addsub<symmetry,t0>::apply(p[length-7], p[7]);
          int32 out = p[8] * t0 + v1 * t1 + v2 * t2 + v3 * t3 + v4 * t4 + v5 * t5 + v6 * t6 + v7 * t7 + v8 * t8;
          add(v);
          return out;
        }
        static int32 vert(size_t width, const uint8 * p) {
          int32 w = int32(width);
          int32 v1 = addsub<symmetry,t0>::apply(p[w],   p[-w]);
          int32 v2 = addsub<symmetry,t0>::apply(p[w*2], p[-w*2]);
          int32 v3 = addsub<symmetry,t0>::apply(p[w*3], p[-w*3]);
          int32 v4 = addsub<symmetry,t0>::apply(p[w*4], p[-w*4]);
          int32 v5 = addsub<symmetry,t0>::apply(p[w*5], p[-w*5]);
          int32 v6 = addsub<symmetry,t0>::apply(p[w*6], p[-w*6]);
          int32 v7 = addsub<symmetry,t0>::apply(p[w*7], p[-w*7]);
          int32 v8 = addsub<symmetry,t0>::apply(p[w*8], p[-w*8]);
          int32 out = p[0] * t0 + v1 * t1 + v2 * t2 + v3 * t3 + v4 * t4 + v5 * t5 + v6 * t6 + v7 * t7 + v8 * t8;
          return out;
        }
      };

    }
  }
}

#endif // __CONV2_H
