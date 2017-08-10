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

#ifndef __SCALEFACTORS_H
#define __SCALEFACTORS_H

namespace FingerJetFxOSE {
  namespace FpRecEngineImpl {
    namespace Embedded {
      const int32 logMultiplier = 256; // changing this requires new log table
      const uint32 bitsScaleFactor = 12;

      const int32 ScaleFactor = 1 << bitsScaleFactor;
      const int32 ScaleFactor2 = ScaleFactor * ScaleFactor;
      const int32  lgScaleFactor = logMultiplier * bitsScaleFactor;

      const uint8 bitsAngleScale = 8; // changing this requires new sin/cos tables
      const int32 angleScale = 1 << bitsAngleScale; 
      const int32 angleScale2 = angleScale * angleScale;

      const uint32 bitsPmpScale = 8;
      const int32 pmpScale = 1 << bitsPmpScale;
      const int32  lgPmpScale = logMultiplier * bitsPmpScale;

      const uint8 minutiaWeightScale = 64;
    }
  }
}

#endif //  __SCALEFACTORS_H
