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

#ifndef __DELAY_H
#define __DELAY_H

#include <stdlib.h>
#include <string.h>

namespace FingerJetFxOSE {
  namespace FpRecEngineImpl {
    namespace Embedded {

      template <class T, size_t size> struct delay {
        T buffer[size];
        T * ptr;
        T * end;
        delay(size_t dt, const T & init = T(0)) : ptr(buffer), end(buffer + dt) {
          while(ptr < end) {
            *ptr++ = init;
          }
          ptr = buffer;
        }
        T operator () (T in) {
          T out = *ptr;
          *ptr = in;
          if (++ptr >= end) {
            ptr = buffer;
          }
          return out;
        }
        T operator () () const {
          return *ptr;
        }
      };

      template <class T> struct delay<T,1> {
        T prev;
        T * ptr;
        delay(const T & init = T(0)) : prev(init) {
        }
        T operator () (T in) {
          T out = prev;
          prev = in;
          return out;
        }
        operator T () const {
          return prev;
        }
      };

      template <size_t size> struct delay<bool, size> {
        typedef unsigned char byte;
        static const size_t bytesize = (size + 7) / 8;
        byte buffer[bytesize];
        byte * ptr;
        byte * end;
        byte mask;
        const byte initmask;

        delay(size_t dt, bool init_val = false) 
          : ptr(buffer)
          , end(buffer + (dt + 7) / 8)
          , initmask(1 << (-dt & 7))
        {
          mask = initmask;
          while(ptr < end) {
            *ptr++ = init_val ? 0xff : 0;
          }
          ptr = buffer;
        }
        bool operator () (bool in) {
          bool out = (*ptr & mask) != 0;
          if (in) {
            *ptr |= mask;
          } else {
            *ptr &= ~mask;
          }
          if ((mask <<= 1) == 0) {
            if (++ptr >= end) {
              ptr = buffer;
              mask = initmask;
            } else {
              mask = 1;
            }
          }
          return out;
        }
        bool operator () () const {
          return (*ptr & mask) != 0;
        }
      };

    }
  }
}
#endif 
