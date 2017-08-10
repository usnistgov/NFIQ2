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
      BINARY: testFRFXLLInternals - Unit Tests for Fingerprint Feature Extractor Internals
      
      ALGORITHM:      Alexander Ivanisov
                      Yi Chen
                      Salil Prabhakar
      IMPLEMENTATION: Alexander Ivanisov
                      Jacob Kaminsky
                      Lixin Wei
      DATE:           11/08/2011
*/

#ifndef __LFSR_H
#define __LFSR_H

#include <dpTypes.h>

struct LFSR {
  uint32 state;
  LFSR(uint32 init = 0xBADF00D) : state(init) {} 
  template <class T> T Next() {
    T x = 0;
    for (size_t i = 0; i < sizeof(T) * 8; i++) {
      x = (x << 1) | (NextBit() ? 1 : 0);
    }
    return x;
  }
  bool NextBit() {
    state = (state >> 1) ^ (-(int32)(state & 1u) & 0xd0000001u); /* taps 32 31 29 1 */
    return state & 1;
  }
};

#endif // __LFSR_H
