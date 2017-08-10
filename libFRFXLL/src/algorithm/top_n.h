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

#ifndef __top_n_h
#define __top_n_h

#include <algorithm>
#include <functional> // greater

namespace FingerJetFxOSE {

template <class T> 
class top_n {
  T * const _begin;  // buffer
  T * const _endbuf; // end of buffer
  T * _end;          // current end
  T _threshold;

  bool is_full() const { return end() == endbuf(); }
public:
  top_n(T * begin_, T * end_, const T & threshold_ = T()) 
    : _begin(begin_)
    , _endbuf(end_)
    , _end(begin_)
    , _threshold(threshold_) 
  {}
  T * begin() const { return _begin; }
  T * end() const { return _end; }
  T * endbuf() const { return _endbuf; }
  size_t size() const { return end() - begin(); }
  size_t capacity() const { return endbuf() - begin(); }
  void add(const T & v) {
    if (is_full()) {
      if (v > *begin()) {
        std::pop_heap(begin(), end(), std::greater<T>());
        end()[-1] = v;
        std::push_heap(begin(), end(), std::greater<T>());
      }
    } else {
      if (v > _threshold) {
        *_end++ = v;
        std::push_heap(begin(), end(), std::greater<T>());
      }
    }
  }
  // Don't call add after sort
  void sort() {
    std::sort_heap(begin(), end(), std::greater<T>());
  }
};

} // namespace FingerJetFxOSE

#endif // __top_n_h
