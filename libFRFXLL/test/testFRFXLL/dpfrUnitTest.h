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
      BINARY: testFRFXLL - Unit Tests for Fingerprint Feature Extractor - LowLevel API      
      
      ALGORITHM:      Alexander Ivanisov
                      Yi Chen
                      Salil Prabhakar
      IMPLEMENTATION: Alexander Ivanisov
                      Jacob Kaminsky
                      Lixin Wei
      DATE:           11/08/2011
*/

// dpfrUnitTest.h

#ifndef __FRFXLLUNITTEST_H__
#define __FRFXLLUNITTEST_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if !defined(PBE)
# if defined(__CC_ARM)
//#   define USH
# elif defined(__GNUC__)
#   define SIM
# elif defined(_MSC_VER)
#   include <memory.h>
#   ifndef SIM
#     define SIM
#   endif
# endif
#endif

#if defined(SIM)

#elif defined(PBE)

#include "BAPP.h"
#include "sdebug.h"

extern THeaderPtr BAPPHdr;
extern uint8 DB;

#else // real USH

#ifdef __cplusplus
extern "C" {
#endif

// the following definition is a workaround to bug in <ushx_api.h> in case of C++
#define bool bool 
#include <ushx_api.h>

#ifdef __cplusplus
}
#endif

#endif

#include "FRFXLL.h"
#include "TestAnsiImage.h"

FRFXLL_HANDLE CreateContext(bool withTimer = false);
unsigned int CalculateCRC(const unsigned char data[], size_t size);

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

//#define _CXXTEST_HAVE_STD

#ifdef WINCE
#ifdef x86
#if (_WIN32_WCE == 0x400)
float     __cdecl ceilf(float); // this is missing from stdlib.h in wce400\STANDARDSDK\Include\X86
#endif
#if (_WIN32_WCE == 0x420 || _WIN32_WCE == 0x400)
#ifndef _DEBUG
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
long _ftol(double);
extern inline long _ftol2(double x) { return _ftol(x); }
#ifdef __cplusplus
}
#endif //__cplusplus
#endif
#endif
#endif
#endif

#include <cxxtest/TestSuite.h>
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include <ValueTraitsEx.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
extern void* m_malloc(size_t size, void* _);
extern void m_free(void* p, void* _);
extern int get_block_count();
#ifdef __cplusplus
}
#endif //__cplusplus

struct Handle {
  FRFXLL_HANDLE handle;
  Handle() : handle(NULL) {}
  ~Handle() {
    if (handle != NULL) {
      TS_ASSERT_OK(FRFXLLCloseHandle(handle));
    }
  }
  operator FRFXLL_HANDLE() const {
    return handle;
  }
  operator FRFXLL_HANDLE& () {
    return handle;
  }
  FRFXLL_HANDLE * operator & () {
    return &handle;
  }
};

struct Context : public Handle {
  Context() {
    handle = CreateContext();
          TS_ASSERT(handle != NULL);
  }
};

struct MemoryChecker {
  int initialBlockCount;
  MemoryChecker() {
    Invalidate();
  }
  void Invalidate() {
    initialBlockCount = 0x7fffffff;
  }
  void setUp() {
    initialBlockCount = get_block_count();
  }
  void tearDown() {
    Invalidate();
  }
};

extern size_t m_used, limit;
struct MemoryLimit {
  size_t oldLimit;
  MemoryLimit(size_t remaining)
    : oldLimit(limit)
  {
    limit = m_used + remaining;
  }
  ~MemoryLimit() {
    limit = oldLimit;
  }
};

extern size_t alloc_count, alloc_limit;
struct FailMemoryAllocation {
  size_t oldLimit;
  FailMemoryAllocation(size_t remaining)
    : oldLimit(alloc_limit)
  {
    alloc_limit = alloc_count + remaining;
  }
  ~FailMemoryAllocation() {
    alloc_limit = oldLimit;
  }
};

#define TS_ASSERT_MEMORY_RELEASED(memoryChecker)  TS_ASSERT_EQUALS(memoryChecker.initialBlockCount, get_block_count())

class FixtureWithContext {
public:
  FixtureWithContext() : hCtx(NULL) {
  }
protected:
  FRFXLL_HANDLE hCtx;
  void _setUp() {
    hCtx = CreateContext();
    TS_ASSERT(hCtx != NULL);
  }
  void _tearDown() {
    TS_ASSERT_OK(FRFXLLCloseHandle(hCtx));
  }
};

class FixtureWithExportedData {
protected:
  unsigned char data[3000];
  size_t size;
  void _setUp(FRFXLL_HANDLE hObject, FRFXLL_DATA_TYPE dataType) {
    size = sizeof(data);
    TS_ASSERT_OK(FRFXLLExport(hObject, dataType, NULL, data, &size));
  }
  void _tearDown() {
  }
};

class ImageBuffer {
public:
  static const size_t max_buffer_size = 410000;
private:
  static unsigned char imageBuffer[];
  size_t imageSize;
  size_t shift;

public:
  ImageBuffer() : imageSize(0), shift(0) {}
  ImageBuffer(const unsigned char image[], size_t size, bool bSetSize = true) {
    if (size <= max_buffer_size) {
      memcpy(imageBuffer, image, size);
      if (bSetSize)
        imageSize = size, shift = 0;
    }
  }
  ImageBuffer(const unsigned char c, size_t size, bool bSetSize = true) {
    if (size <= max_buffer_size) {
      memset(imageBuffer, c, size);
      if (bSetSize)
        imageSize = size, shift = 0;
    }
  }

  bool operator ()(const unsigned char image[], size_t size, bool bSetSize = true) {
    if (size > max_buffer_size) return false;
    memcpy(imageBuffer, image, size);
    if (bSetSize)
      imageSize = size, shift = 0;
    return true;
  }
  bool operator ()(const unsigned char c, size_t size, bool bSetSize = true) {
    if (size > max_buffer_size) return false;
    memset(imageBuffer, c, size);
    if (bSetSize)
      imageSize = size, shift = 0;
    return true;
  }
  void Shift(size_t _shift) {
    memmove(imageBuffer + _shift, imageBuffer, imageSize);
    shift = _shift;
  }
  unsigned char * buf ()      { return imageBuffer + shift; }
  operator unsigned char *()  { return imageBuffer + shift; }
  size_t size()               { return imageSize; }
  operator size_t ()          { return imageSize; }
};
class ImageBufferWrapper {
  const unsigned char * imageBuffer;
  const size_t imageSize;

public:
  ImageBufferWrapper(const unsigned char image[], size_t size) : imageBuffer(image), imageSize (size) { }
  operator const unsigned char *()  { return imageBuffer; }
  size_t size()               { return imageSize;   }
  operator size_t ()          { return imageSize;   }
};

class FixtureWithExtractedFeatureSet {
protected:
  FRFXLL_HANDLE hCtx;
  FRFXLL_HANDLE hFtrSet;
  void _setUp(const unsigned char fpData[], size_t size, FRFXLL_DATA_TYPE dataType, unsigned int reserved, bool bInPlace = true) {
    if (hCtx == NULL) {
      hCtx = CreateContext();
      TS_ASSERT(hCtx != NULL);
      hFtrSet = NULL;
      ImageBuffer imageBuffer;
      TS_ASSERT(imageBuffer(fpData, size));
      if (bInPlace) {
        TS_ASSERT_OK(FRFXLLCreateFeatureSetInPlace(hCtx, imageBuffer, size, dataType, reserved, &hFtrSet));
      } else {
        TS_ASSERT_OK(FRFXLLCreateFeatureSet(hCtx, imageBuffer, size, dataType, reserved, &hFtrSet));
      }
      TS_ASSERT(hFtrSet != NULL);
    }
  }
  void _tearDown() {
  }
public:
  FixtureWithExtractedFeatureSet() : hCtx(NULL), hFtrSet(NULL) {}
  ~FixtureWithExtractedFeatureSet() {
    FRFXLLCloseHandle(hFtrSet);
    FRFXLLCloseHandle(hCtx);
  }
};

#endif // __FRFXLLUNITTEST_H__
