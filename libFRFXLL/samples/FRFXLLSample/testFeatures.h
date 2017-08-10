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
      BINARY: FRFXLLSample - Sample Code for Fingerprint Feature Extractor - LowLevel API
      
      ALGORITHM:      Alexander Ivanisov
                      Yi Chen
                      Salil Prabhakar
      IMPLEMENTATION: Alexander Ivanisov
                      Jacob Kaminsky
                      Lixin Wei
      DATE:           11/08/2011
*/

#ifndef __TESTFEATURES_H
#define __TESTFEATURES_H

#ifdef __cplusplus
extern "C" {
#endif

#define REG_FEATURES_LENGTH    (1632)
#define PREREG_FEATURES_LENGTH (318)
#define VER_FEATURES_LENGTH    (318)
#define NUM_PREREG_TEMPLATES   (4)

extern const unsigned char *ftrs_E_01_08_01[NUM_PREREG_TEMPLATES];
extern const unsigned char templ_R_01_08_01[REG_FEATURES_LENGTH];
extern const unsigned char templ_R_02_08_01[REG_FEATURES_LENGTH];
extern const unsigned char templ_R_03_08_01[REG_FEATURES_LENGTH];
extern const unsigned char ftr_R_01_08_11[VER_FEATURES_LENGTH];
extern const unsigned char ftr_R_02_08_11[VER_FEATURES_LENGTH];
extern const unsigned char ftr_R_03_08_11[VER_FEATURES_LENGTH];

extern const unsigned char *ftrs_E_04_08_01[4]; // consintent set of feature sets
extern const unsigned char *ftrsFromDiffFingers0[4]; // inconsintent set of feature sets

#ifdef __cplusplus
}
#endif

#endif // __TESTFEATURES_H
