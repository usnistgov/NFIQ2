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

#if !(defined(FRFXLL_MAJOR) && defined(FRFXLL_MINOR) && defined(FRFXLL_REVISION))
// #pragma message("Definitions FRFXLL_MAJOR, FRFXLL_MINOR, FRFXLL_REVISION are missing. Reverting to defaults.")
#define FRFXLL_MAJOR 5
#define FRFXLL_MINOR 2
#define FRFXLL_REVISION 0
#if !(defined(FRFXLL_BUILD))
// #pragma message("Definition BUILD is missing. Reverting to default.")
#define FRFXLL_BUILD 0
#endif
#endif
