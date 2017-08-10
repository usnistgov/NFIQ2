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

#ifndef __DPTYPES_H
#define __DPTYPES_H

#include "IncludeFirst.h"

#ifndef NULL
#define NULL (0)
#endif

#include <stddef.h>

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;
typedef signed char    int8;
typedef signed short   int16;
typedef signed int     int32;

typedef long long               int64;
typedef unsigned long long      uint64;

#ifdef __cplusplus
namespace FingerJetFxOSE {

  template <uint8 bytes, bool is_signed> struct integer;
  template <> struct integer<1,  true> { typedef int8  type; };
  template <> struct integer<2, true> { typedef int16 type; };
  template <> struct integer<4, true> { typedef int32 type; };
  template <> struct integer<8, true> { typedef int64 type; };
  template <> struct integer<1,  false> { typedef uint8  type; };
  template <> struct integer<2, false> { typedef uint16 type; };
  template <> struct integer<4, false> { typedef uint32 type; };
  template <> struct integer<8, false> { typedef uint64 type; };

  template <bool is_signed> struct integer<6, is_signed> { typedef typename integer<8, is_signed>::type type; };

  template <uint8 bytes, bool is_signed, bool is_big_endian> 
  struct integer_with_endian {
  private:
    typedef typename integer<bytes, is_signed>::type value_t;
    unsigned char value[bytes];

    static uint8 shift(uint8 i) {
      return (is_big_endian ? (bytes - i - 1) : i) * 8;
    }
    void assign(value_t val) {
      for (uint8 i = 0; i < bytes; i++) {
        value[i] = (val >> shift(i)) & 0xff;
      }
    }

  public:
    integer_with_endian(value_t val) {
      assign(val);
    }
    operator value_t () {
      value_t val = 0;
      for (uint8 i = 0; i < bytes; i++) {
        val |= (value_t(value[i]) << shift(i));
      }
      return val;
    }
  };

}

#endif


#endif // __DPTYPES_H

