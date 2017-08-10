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
      LIBRARY: libdpfrfxTestVectors - Test Vectors and Some Common Functions for the 
                                      Unit Tests for Fingerprint Feature Extractor
      
      ALGORITHM:      Alexander Ivanisov
                      Yi Chen
                      Salil Prabhakar
      IMPLEMENTATION: Alexander Ivanisov
                      Jacob Kaminsky
                      Lixin Wei
      DATE:           11/08/2011
*/

// -*- C++ -*-
//

#include <stddef.h>

#ifdef _MSC_VER
//#if !(_WIN32_WCE == 0x420 || _WIN32_WCE == 0x400)
#if defined(_STLP_DONT_USE_EXCEPTIONS) && _STLP_DONT_USE_EXCEPTIONS == 0
# define CXXTEST_HAVE_EH
#endif
#endif // _MSC_VER
#include "ConsoleErrorPrinter.h"


int run() {
  CxxTest::initialize();
  return CxxTest::dpfrErrorPrinter(":", "").run();
}

#ifdef SIM
int main() {
  return run();
}
#else
extern "C" {
  void ush_app_main(unsigned char *, unsigned int);
  int ush_lib_app();
}

int ush_lib_app() {
  return 0;
}

int main() {
  ush_app_main(NULL, 1);
  run();
}
#endif // SIM

// The CxxTest "world"
<CxxTest world>

