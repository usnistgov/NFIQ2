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

#ifndef __TESTANSIIMAGE_H
#define __TESTANSIIMAGE_H

#ifdef __cplusplus
inline void SetResolutionInANSI381Image(unsigned char * ANSI381Image, unsigned short resolution) {
  unsigned char resolutionLo = resolution & 0xff;
  unsigned char resolutionHi = resolution >> 8;
  unsigned char * resolutions = ANSI381Image + 24;  // offset to resolutions in the image
  resolutions[0] = resolutions[2] = resolutions[4] = resolutions[6] = resolutionHi;
  resolutions[1] = resolutions[3] = resolutions[5] = resolutions[7] = resolutionLo;
}

extern "C" {
#endif

#define TEST_IMAGE_SIZE                   79386
#define TEST_MAXIMUM_IMAGE_SIZE           275050
#define TEST_BRCM_IMAGE_SIZE              92210
#define TEST_AUTHENTEC_IMAGE_01_SIZE      63358 // 63359
#define TEST_AUTHENTEC_IMAGE_02_SIZE      276050
#define IMAGE_HEADER_SIZE                 50
#define TEST_ONE_LINE_IMAGE_SIZE          188
#define TEST_ANSI_IMAGE_DRAGON_FLY_SIZE   98114 //0x17f42
#define TEST_ANSI_IMAGE_MARTINI_SIZE      275050 //0x4326a
#define TEST_ANSI_IMAGE_MARTINI_CE_SIZE   229874 //0x381f2
#define TEST_ANSI_IMAGE_200_SIZE          16352
#define TEST_ANSI_IMAGE_250_SIZE          25468
#define TEST_ANSI_IMAGE_300_SIZE          36600
#define TEST_ANSI_IMAGE_1000_SIZE         406738
#define TEST_ANSI_IMAGE_100_100           10050
#define TEST_ISO_IMAGE_SIZE               204850

extern const unsigned char TestAnsiImage[TEST_IMAGE_SIZE];
extern const unsigned char TestAnsiImageBad_ANY_BINARY[TEST_MAXIMUM_IMAGE_SIZE];
extern const unsigned char TestAnsiImageBad_NOT_ENOUGH_FTR[TEST_MAXIMUM_IMAGE_SIZE];
extern const unsigned char TestAnsiImageBad_NO_CENTRAL_REGION[TEST_MAXIMUM_IMAGE_SIZE];
extern const unsigned char TestAnsiImageBad_AREA_TOO_SMALL[TEST_MAXIMUM_IMAGE_SIZE];
extern const unsigned char TestAnsiImageBad_OneLine[TEST_ONE_LINE_IMAGE_SIZE];

extern const unsigned char TestBrcmImage00[TEST_BRCM_IMAGE_SIZE];
extern const unsigned char TestBrcmImage01[TEST_BRCM_IMAGE_SIZE];
extern const unsigned char TestBrcmImage02[TEST_BRCM_IMAGE_SIZE];
extern const unsigned char TestBrcmImage03[TEST_BRCM_IMAGE_SIZE];
extern const unsigned char TestBrcmImage04[TEST_BRCM_IMAGE_SIZE];

extern const unsigned char TestAuthentec01[TEST_AUTHENTEC_IMAGE_01_SIZE];
extern const unsigned char TestAuthentec02[TEST_AUTHENTEC_IMAGE_02_SIZE];
extern const unsigned char TestAuthentec02_HeaderWithOneRowLonger[IMAGE_HEADER_SIZE];

extern const unsigned char TestAnsiImageMartini[TEST_ANSI_IMAGE_MARTINI_SIZE];
extern const unsigned char TestAnsiImageMartiniCE[TEST_ANSI_IMAGE_MARTINI_CE_SIZE];

extern const unsigned char TestAnsiImage200 [TEST_ANSI_IMAGE_200_SIZE];
extern const unsigned char TestAnsiImage250 [TEST_ANSI_IMAGE_250_SIZE];
extern const unsigned char TestAnsiImage300 [TEST_ANSI_IMAGE_300_SIZE];
extern const unsigned char TestAnsiImage1000[TEST_ANSI_IMAGE_1000_SIZE];
extern const unsigned char TestAnsiImage_100_100[TEST_ANSI_IMAGE_100_100];

extern const unsigned int TestAnsiImageValiditySize;
extern const unsigned char TestAnsiImageValidityWrongRecordLength[];

extern const unsigned int TestAnsiImageUpekSwipeSize;
extern const unsigned char TestAnsiImageUpekSwipe[];

extern const unsigned int TestAnsiImage10vfSize;
extern const unsigned char TestAnsiImage10vf[];

extern const unsigned char TestIsoImage[TEST_ISO_IMAGE_SIZE];

#ifdef __cplusplus
}
#endif

#endif // __TESTANSIIMAGE_H
