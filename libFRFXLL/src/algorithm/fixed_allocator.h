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

#ifndef __fixed_allocator_h
#define __fixed_allocator_h

#include <stdlib.h>
#include <memory>
#include <vector>

namespace FingerJetFxOSE {
  template <class T, size_t size>
  class fixed_allocator : public std::allocator<T> {
  public:
    typedef T * pointer;
    typedef size_t size_type;
    template<class _Other>
    struct rebind {
      typedef fixed_allocator<_Other, size> other;
    };
    unsigned char buffer[size*sizeof(T)];
    fixed_allocator() {}
    pointer allocate(size_type, const void* = NULL) {
      return reinterpret_cast<pointer>(buffer);
    }
    void deallocate(pointer, size_type) {
    }
    size_type max_size( ) const {
      return size;
    }
    // for compatibility with msvc
    template<class _Other, size_type _Size>
    fixed_allocator(const fixed_allocator<_Other, _Size> & _Right) {}
  };
  
  template <class T, size_t _max_capacity>
  class vector : public std::vector<T, fixed_allocator<T, _max_capacity> > {
    typedef std::vector<T, fixed_allocator<T, _max_capacity> > base_t;
  public:
    static const size_t Capacity = _max_capacity;

    vector() { base_t::reserve(_max_capacity); }
    vector(size_t count_, const T & val_) : base_t(count_, val_) { base_t::reserve(_max_capacity); }
    vector(const vector & v) : base_t(v) {}
    vector(const base_t & v) : base_t(v) {}
    bool full() const { return base_t::size() == base_t::capacity(); }
  };

  template <class T>
  class allocator_in_buffer : public std::allocator<T> {
  public:
    typedef T * pointer;
    typedef size_t size_type;
    template<class _Other>
    struct rebind {
      typedef allocator_in_buffer<_Other> other;
    };
    T * buffer;
    size_t size;
    allocator_in_buffer(T * buffer_, size_t size_) 
      : buffer(buffer_) 
      , size(size_)
    {}
    pointer allocate(size_type size, const void* = NULL) {
      return buffer;
    }
    void deallocate(pointer buffer, size_type) {
    }
    size_type max_size( ) const {
      return size;
    }
    template<class _Other>
    allocator_in_buffer(const allocator_in_buffer<_Other> & _Right) 
      : buffer(_Right.buffer)
      , size(_Right.size * sizeof(T) / sizeof(_Other))
    {}
  };

  template <class T>
  class vector_in_buffer : public std::vector<T, allocator_in_buffer<T> > {
    typedef std::vector<T, allocator_in_buffer<T> > base_t;
  public:
    vector_in_buffer(T * buffer_, size_t size_) 
      : base_t(typename base_t::allocator_type(buffer_, size_)) 
    {
      base_t::reserve(size_); 
    }
    vector_in_buffer(T * buffer_, size_t size_, size_t count_, const T & val_) 
      : base_t(count_, val_, typename base_t::allocator_type(buffer_, size_)) 
    { 
        base_t::reserve(size_); 
    }
    vector_in_buffer(const vector_in_buffer & v) : base_t(v) {}
    vector_in_buffer(const base_t & v) : base_t(v) {}
    bool full() const { return base_t::size() == base_t::capacity(); }
  };
}

#endif // __fixed_allocator_h
