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

#ifndef __member_comp_h
#define __member_comp_h

namespace FingerJetFxOSE {

  template <class Elem, class T> struct mem_comp_t {
    typedef bool (T::*comp_t)(Elem e1, Elem e2) const;
    const T & t;
    comp_t comp;

    mem_comp_t(const T & t_, comp_t comp_) 
      : t(t_)
      , comp(comp_)
    {
    }
    bool operator () (Elem e1, Elem e2) const {
      return (t.*comp)(e1, e2);
    }
  };

  template <class Elem, class T>
  inline mem_comp_t<Elem, T> mem_comp(
    const T & t,
    //typename member_comp<Elem, T>::comp_t comp
    bool (T::*comp)(Elem e1, Elem e2) const
  ) {
    return mem_comp_t<Elem, T>(t, comp);
  }

}

#endif
