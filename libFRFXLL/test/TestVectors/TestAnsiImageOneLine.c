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
      LIBRARY: libFRFXLLTestVectors.a - Test Vectors and Some Common Functions for the 
                                      Unit Tests for Fingerprint Feature Extractor
      
      ALGORITHM:      Alexander Ivanisov
                      Yi Chen
                      Salil Prabhakar
      IMPLEMENTATION: Alexander Ivanisov
                      Jacob Kaminsky
                      Lixin Wei
      DATE:           11/08/2011
*/

#include "TestAnsiImage.h"

const unsigned char TestAnsiImageBad_OneLine[TEST_ONE_LINE_IMAGE_SIZE] = {
  70, 73, 82, 0, 48, 49, 48, 0, 0, 0, 0, 0, 0, 188, 0, 51, 254, 0, 0, 0, 0, 0, \
  1, 1, 1, 244, 1, 244, 1, 244, 1, 244, 8, 0, 0, 0, 0, 0, 0, 152, 1, 1, 1, 254, \
  0, 0, 138, 0, 1, 0, 230, 227, 225, 225, 228, 225, 225, 224, 224, 225, 225, \
  227, 227, 228, 227, 229, 226, 227, 226, 227, 227, 229, 225, 228, 226, 228, \
  227, 229, 226, 227, 225, 227, 226, 227, 227, 224, 225, 227, 228, 228, 228, \
  230, 231, 230, 231, 226, 229, 229, 229, 230, 233, 230, 230, 227, 226, 225, \
  225, 225, 226, 227, 225, 226, 229, 224, 227, 226, 226, 228, 230, 230, 231, \
  233, 228, 225, 225, 225, 226, 227, 227, 231, 227, 228, 228, 229, 231, 230, \
  227, 226, 222, 226, 225, 224, 225, 227, 225, 225, 223, 226, 226, 228, 228, \
  233, 227, 228, 226, 228, 229, 229, 225, 226, 227, 229, 230, 228, 230, 231, \
  231, 231, 229, 231, 231, 230, 230, 231, 231, 228, 227, 227, 229, 226, 227, \
  227, 229, 228, 229, 227, 227, 228
};
