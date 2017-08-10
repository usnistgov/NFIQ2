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

#ifndef __OBJECT_H
#define __OBJECT_H

# ifdef PHX_FW_UPGRADE
#   ifndef __IncludeFirst_h
#     error --preinclude=IncludeFirst.h is missing in the command line
#   endif
# endif // PHX_FW_UPGRADE

#include "dpTypes.h"
#include "dpError.h"
#include "FRFXLL.h"
#include "settings.h"
#ifdef USE_DPCOMMON
#include "FullComparison.h"
namespace Engine = FingerJetFxOSE::FpRecEngineImpl::Full;
#else
namespace Engine = FingerJetFxOSE::FpRecEngineImpl::Embedded;
#endif


namespace FingerJetFxOSE {
  namespace FpRecEngineImpl {
    using namespace Embedded;

    template <uint32 v0, uint32 v1> class Signature {
      volatile uint32 data[2];
      void Clear() { data[0] = data[1] = 0; }
    public:
      Signature() {
        data[0] = v0;
        data[1] = v1;
      };
      bool Check() const {
        if (this == NULL) return false;
        return v0 == data[0] && v1 == data[1];
      }
      ~Signature() { Clear(); }
    };

    class Context;
    class ObjectBase {
    public:
      struct FreeDelegate {
        void      (*free)     (void * block, void * heapContext);     // Function to free block of memory on the heap
        void * heapContext;   ///< Context passed into heap functions. Heap should be ready to use prior to calling FRFXLLCreateContext
      };
      static void operator delete (void * p) throw() {
        if (p == NULL) return;
        FreeDelegate * fd = reinterpret_cast<FreeDelegate*>(p) - 1;
        fd->free(fd, fd->heapContext);
      }
      // We are not planning to use exceptions, but just in case
      static void operator delete (void * p, const FRFXLL_CONTEXT_INIT * ctxi)  throw() {
        if (p == NULL) return;
        ctxi->free(p, ctxi->heapContext);
      }
      static void * operator new (size_t size, const FRFXLL_CONTEXT_INIT * ctxi)  throw() {
        if (!ctxi) return NULL;
        FreeDelegate * fd = reinterpret_cast<FreeDelegate*>(ctxi->malloc(size + sizeof(FreeDelegate), ctxi->heapContext));
        if (fd == NULL) return NULL;
        fd->heapContext = ctxi->heapContext;
        fd->free = ctxi->free;
        return fd + 1;
      }
      // We need placement new as well
      static void * operator new (size_t size, void * p)  throw() {
        return p;
      }
      // We are not planning to use exceptions, but just in case
      static void operator delete (void * p, void * ctxi)  throw() {
      }
    };
    class Object : public ObjectBase {
      friend class Context;
      template <class T> friend struct Ptr;
      // Signature<0x656a624f, 0x96007463> objSignature;
    public:
      mutable volatile long refCount;
      const Context * ctx;

      Object(const Context * ctx_);
      virtual ~Object();
      FRFXLL_RESULT AddRef() const;
      FRFXLL_RESULT Release() const;

      FRFXLL_RESULT GetHandle(FRFXLL_HANDLE * pHandle, FRFXLL_RESULT rc = FRFXLL_OK) const;
      FRFXLL_RESULT GetHandle(FRFXLL_HANDLE * pHandle, FRFXLL_RESULT rc = FRFXLL_OK);
    };

    class Context : public Signature<0x747e7f63, 0x747865>, public Object, public FRFXLL_CONTEXT_INIT {
    public:
      Engine::Settings settings;
    private:
      static long _increment(volatile long * value) {
        return ++(*value);
      }
      static long _decrement(volatile long * value) {
        return --(*value);
      }
      static long _exchange(volatile long * dest, long value) {
        long tmp = *dest;
        *dest = value;
        return tmp;
      }
      static long _compare_exchange(volatile long * dest, long value, long comperand) {
        long tmp = *dest;
        if (*dest == comperand) *dest = value;
        return tmp;
      }
    public:
      Context(FRFXLL_CONTEXT_INIT &ctxi_) : Object(NULL), FRFXLL_CONTEXT_INIT(ctxi_) {
        ctx = this;
        if (interlocked_decrement == NULL) interlocked_decrement = &_decrement;
        if (interlocked_increment == NULL) interlocked_increment = &_increment;
        if (interlocked_exchange  == NULL) interlocked_exchange  = &_exchange;
        if (interlocked_compare_exchange == NULL) interlocked_compare_exchange = &_compare_exchange;
      }
      virtual ~Context() {
        ctx = NULL;  // To prevent form releasing itself one extra time in ~Object()
      }
      long InterlockedIncrement(volatile long & value) const {
        return interlocked_increment(&value);
      }
      long InterlockedDecrement(volatile long & value) const {
        return interlocked_decrement(&value);
      }
      long InterlockedExchange(volatile long & target, long value) const {
        return interlocked_exchange(&target, value);
      }
      long InterlockedCompareExchange(volatile long & target, long value, long comperand) const {
        return interlocked_compare_exchange(&target, value, comperand);
      }
    };

    inline Object::Object(const Context * ctx_) : refCount(0), ctx(ctx_) { 
      /*when constructing a context ctx == NULL */
      if (ctx) ctx->AddRef(); // Object created on a context should increase refcount of a context
    }
    inline Object::~Object() {
      if (ctx) ctx->Release(); // When object is destroyed it should release context
    }
    inline FRFXLL_RESULT Object::AddRef() const  { 
      if (!ctx) return CheckResult(FRFXLL_ERR_INVALID_PARAM);
      ctx->InterlockedIncrement(refCount);
      return FRFXLL_OK;
    }

    inline FRFXLL_RESULT Object::Release() const {
      if (!ctx) return CheckResult(FRFXLL_ERR_INVALID_PARAM);
      if (ctx->InterlockedDecrement(refCount) == 0) {
        delete this;
      }
      return FRFXLL_OK;
    }

    template <class T> T * FromHandle(FRFXLL_HANDLE h);

    template <class T> struct Ptr : public ObjectBase {
      T * ptr;
      Ptr() : ptr(NULL) {}
      Ptr(FRFXLL_HANDLE h);

      template <class T1> Ptr(T1 * p) : ptr(p) {
        if (ptr) ptr->AddRef();
      }
      template <class T1> Ptr(const Ptr<T1> & p) : ptr(p.ptr) {
        if (ptr) ptr->AddRef();
      }
      // Important: template c-tor does not override implicit copy c-tor!!!
      Ptr(const Ptr<T> & p) : ptr(p.ptr) {
        if (ptr) ptr->AddRef();
      }
      ~Ptr() {
        if (ptr) ptr->Release();
      }
      // operator bool () const { return ptr != NULL; }
      operator T * () { return ptr; }
      T * operator -> () { return ptr; }
      const T * operator -> () const { return ptr; }
      T & operator * () { return *ptr; }
      const T & operator * () const { return *ptr; }
      Ptr & operator = (Ptr p) {    // AI: cannot use const Ptr & as a parameter because p may be a member of *ptr,
        if (ptr != p.ptr) {         //     in which case it may be destroyed too early
          if (p) p->AddRef();       // Must go first!!!
          if (ptr) ptr->Release();
          ptr = p.ptr;
        }
        return *this;
      }
    };

    struct Handle : public Signature<0x646e6168, 0x0000656c>, public ObjectBase {
      Ptr<Object> ptr;
      Handle(Object * pObj)
        : ptr(pObj)
      {}
    };
    struct ConstHandle : public Signature<0x646e6148, 0x0100656c>, public ObjectBase {
      Ptr<const Object> ptr;
      ConstHandle(const Object * pObj)
        : ptr(pObj)
      {}
    };
    inline FRFXLL_RESULT Object::GetHandle(FRFXLL_HANDLE * pHandle, FRFXLL_RESULT rc) const {
      if (rc < FRFXLL_OK) return rc;
      FRFXLL_HANDLE handle = new(ctx) ConstHandle(this);
      if (handle == NULL) return FRFXLL_ERR_NO_MEMORY;
      *pHandle = handle;
      return rc;
    }
    inline FRFXLL_RESULT Object::GetHandle(FRFXLL_HANDLE * pHandle, FRFXLL_RESULT rc) {
      if (rc < FRFXLL_OK) return rc;
      FRFXLL_HANDLE handle = new(ctx) Handle(this);
      if (handle == NULL) return FRFXLL_ERR_NO_MEMORY;
      *pHandle = handle;
      return rc;
    }

    template <class T> const T * FromHandle(FRFXLL_HANDLE h, const Ptr<const T> &) {
      if (h == NULL) return NULL;
      ConstHandle * constHandle = reinterpret_cast<ConstHandle*>(h);
      if (constHandle->Check()) {
        const T * ptr = static_cast<const T*>(constHandle->ptr.ptr);
        if (ptr->Check()) {
          ptr->AddRef();
        } else {
          ptr = NULL;
        }
        return ptr;
      }
      return FromHandle(h, Ptr<T>());
    }
    template <class T> T * FromHandle(FRFXLL_HANDLE h, const Ptr<T> &) {
      if (h == NULL) return NULL;
      Handle * handle = reinterpret_cast<Handle*>(h);
      if (handle->Check()) {
        T * ptr = static_cast<T*>(handle->ptr.ptr);
        if (ptr->Check()) {
          ptr->AddRef();
        } else {
          ptr = NULL;
        }
        return ptr;
      }
      return NULL;
    }

    template <class T> Ptr<T>::Ptr(FRFXLL_HANDLE h)
      : ptr(NULL) 
    {
      ptr = FromHandle(h, *this);
    }
  }
}
using namespace FingerJetFxOSE::FpRecEngineImpl;

#endif
