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

#ifndef __diagnostics_h
#define __diagnostics_h

namespace FingerJetFxOSE {
  namespace Diagnostics {

    #define FRFXLL_DIAG(pointer, method, ...) if (pointer.Valid()) (pointer).ptr->method(__VA_ARGS__)

    template <class T>
    struct PtrSubscriber {
      T * ptr;
      PtrSubscriber() : ptr(0) {}
      bool Valid() const { return ptr != 0; }
    };

    template <class T>
    struct Subscriber {
      typedef PtrSubscriber< Subscriber<T> > ptr_t;
      virtual void SetData(const T & data) {}
    };

    template <class T>
    struct PixelsSubscriber {
      typedef PtrSubscriber< PixelsSubscriber<T> > ptr_t;

      virtual void SetSize(size_t width, size_t height) {}
      virtual void SetPixel(size_t x, size_t y, const T & pixel) {}
    };

  }
}

#endif //  __diagnostics_h
