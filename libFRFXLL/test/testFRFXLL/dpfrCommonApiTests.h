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

//
// The test cases defined here are intended to run on all platforms, including the
// ones with constrained system resources.
//

#ifndef __FRFXLLCOMMONAPITESTS_H__
#define __FRFXLLCOMMONAPITESTS_H__

#include "dpfrUnitTest.h"

extern unsigned char heap_ptr[5852];
extern FRFXLL_RESULT InitializeHeap();
extern unsigned int m_get_current_tick_in_ms();

class LibraryInformation : public CxxTest::TestSuite {
private:
  FRFXLL_VERSION ver;
  MemoryChecker memoryChecker;
public:
  void setUp() {
    memoryChecker.setUp();
  }
  void tearDown() {
    TS_ASSERT_MEMORY_RELEASED(memoryChecker);
    memoryChecker.tearDown();
  }
  void testGetLibraryVersionWithValidParameters() {
    TS_ASSERT_OK(FRFXLLGetLibraryVersion(&ver));
  }
  void testGetLibraryVersionWithNullVersionPtr() {
    TS_ASSERT_EQUALS(FRFXLLGetLibraryVersion(NULL), FRFXLL_ERR_INVALID_PARAM);
  }
};

class ContextCreation : public CxxTest::TestSuite {
private:
  FRFXLL_CONTEXT_INIT ctx_init;
  FRFXLL_HANDLE hCtx;
  MemoryChecker memoryChecker;
public:
  void setUp() {
    memoryChecker.setUp();
    TS_ASSERT_OK(InitializeHeap());

    ctx_init.length = sizeof(FRFXLL_CONTEXT_INIT);
    ctx_init.heapContext = heap_ptr;
    ctx_init.malloc = &m_malloc;
    ctx_init.free = &m_free;
    ctx_init.get_current_tick_in_ms = &m_get_current_tick_in_ms;

    hCtx = NULL;
  }
  void tearDown() {
    TS_ASSERT_MEMORY_RELEASED(memoryChecker);
    memoryChecker.tearDown();
  }
  void testCreateContextWithNullContextInitPtr() {
    TS_ASSERT_EQUALS(FRFXLLCreateContext(NULL, &hCtx), FRFXLL_ERR_INVALID_PARAM);
    TS_ASSERT(hCtx == NULL);
  }
  void testCreateContextWithNullContextHandlePtr() {
    TS_ASSERT_EQUALS(FRFXLLCreateContext(&ctx_init, NULL), FRFXLL_ERR_INVALID_PARAM);
  }
  void testCreateContextOutOfMemory() {
    FailMemoryAllocation fma(0);
    TS_ASSERT_EQUALS(FRFXLLCreateContext(&ctx_init, &hCtx), FRFXLL_ERR_NO_MEMORY);
    TS_ASSERT(hCtx == NULL);
  }
  void testCreateContextOutOfMemoryForHandle() {
    FailMemoryAllocation fma(1);
    TS_ASSERT_EQUALS(FRFXLLCreateContext(&ctx_init, &hCtx), FRFXLL_ERR_NO_MEMORY);
    TS_ASSERT(hCtx == NULL);
  }
  void testCreateContextMakesOnlyTwoAllocation() {
    FailMemoryAllocation fma(2);
    TS_ASSERT_OK(FRFXLLCreateContext(&ctx_init, &hCtx));
    TS_ASSERT(hCtx != NULL);
    TS_ASSERT_OK(FRFXLLCloseHandle(hCtx));
  }
  void testCreateContextWithValidParameters() {
    TS_ASSERT_OK(FRFXLLCreateContext(&ctx_init, &hCtx));
    TS_ASSERT(hCtx != NULL);
    TS_ASSERT_OK(FRFXLLCloseHandle(hCtx));
    TS_ASSERT_EQUALS(FRFXLLCloseHandle(hCtx), FRFXLL_ERR_INVALID_HANDLE); // intended test step.
  }
};

class OperationsWithCreatedContext : public CxxTest::TestSuite, private FixtureWithContext {
private:
  FRFXLL_HANDLE hCtx2, hCtxInvalid;
  MemoryChecker memoryChecker;
public:
  void setUp() {
    memoryChecker.setUp();
    FixtureWithContext::_setUp();
    hCtx2 = NULL;
    TS_ASSERT(hCtx != NULL);
  }
  void tearDown() {
    FixtureWithContext::_tearDown();
    TS_ASSERT_MEMORY_RELEASED(memoryChecker);
    memoryChecker.tearDown();
  }
  void testDuplicateContextHandleWithNullInputContextHandle() {
    TS_ASSERT_EQUALS(FRFXLLDuplicateHandle(NULL, &hCtx2), FRFXLL_ERR_INVALID_HANDLE);
    TS_ASSERT(hCtx2 == NULL);
  }
  void testDuplicateContextHandleWithInvalidInputContextHandle() {
    TS_ASSERT_EQUALS(FRFXLLDuplicateHandle(hCtxInvalid, &hCtx2), FRFXLL_ERR_INVALID_HANDLE);
    TS_ASSERT(hCtx2 == NULL);
  }
  // void testDuplicateContextHandleWithInvalidMemoryAddress() {
  //   if (ProtectedMemoryCheck()) {
  //     TS_ASSERT_EQUALS(FRFXLLDuplicateHandle((FRFXLL_HANDLE)0x20000000, &hAlign2), FRFXLL_ERR_INVALID_HANDLE);
  //   }
  // }
  void testDuplicateContextHandleWithNullOutputContextHandlePtr() {
    TS_ASSERT_EQUALS(FRFXLLDuplicateHandle(hCtx, NULL), FRFXLL_ERR_INVALID_PARAM);
  }
  void testDuplicateContextHandleOutOfMemory() {
    FailMemoryAllocation fma(0);
    TS_ASSERT_EQUALS(FRFXLLDuplicateHandle(hCtx, &hCtx2), FRFXLL_ERR_NO_MEMORY);
    TS_ASSERT(hCtx2 == NULL);
  }
  void testDuplicateContextHandleMakesOnlyOneAllocation() {
    FailMemoryAllocation fma(1);
    TS_ASSERT_OK(FRFXLLDuplicateHandle(hCtx, &hCtx2));
    TS_ASSERT(hCtx2 != NULL);
    TS_ASSERT_OK(FRFXLLCloseHandle(hCtx2));
  }
  void testDuplicateContextHandleWithSameContextHandle() {
    hCtx2 = hCtx;
    TS_ASSERT_OK(FRFXLLDuplicateHandle(hCtx, &hCtx2));
    TS_ASSERT(hCtx2 != NULL);
    TS_ASSERT_OK(FRFXLLCloseHandle(hCtx2));
  }
  void testDuplicateContextHandleWithValidParameters() {
    TS_ASSERT_OK(FRFXLLDuplicateHandle(hCtx, &hCtx2));
    TS_ASSERT(hCtx2 != NULL);
    TS_ASSERT_OK(FRFXLLCloseHandle(hCtx2));
  }
};


#endif // __FRFXLLCOMMONAPITESTS_H__
