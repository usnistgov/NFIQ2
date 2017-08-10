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

// dpfrUnitTest.cpp

#include "dpfrUnitTest.h"

unsigned char heap_ptr[5852];
int m_block_count = 0;
size_t alloc_count = 0;           // count all memory allocations (don't decrement at deallocation)
size_t alloc_limit = size_t(-1);  // if alloc_count >= alloc_limit we fail allocation for testing
size_t m_used = 0x10000000;
size_t limit = size_t(-1);

int get_block_count() {
  return m_block_count;
}

void * m_malloc(size_t size, void * _)
{
  if (alloc_count >= alloc_limit) {
    return NULL;
  }
  if (m_used + size > limit) {
    return NULL;
  }
  size_t * ptr = NULL;
#ifdef SIM
  ptr = (size_t *)malloc(size + sizeof(size_t));
#elif defined(PBE)
  _printf(("Call malloc here!\n")); // TODO: put appropriate heap function here
  return 0; // TODO: put appropriate heap function here
#else
  ptr = (size_t *) pvPortMalloc_in_heap(size + sizeof(size_t), _);
#endif
  if (!ptr) return NULL;
  m_used += size;
  m_block_count++;
  alloc_count++;
  ptr[0] = size;
  return ptr + 1;
}

void m_free(void * p, void * _)
{
  size_t * ptr = (size_t *)p - 1;
  m_used -= *ptr;
#ifdef SIM
  free(ptr);
#elif defined(PBE)
  _printf(("Call free here!\n")); // TODO: put appropriate heap function here
#else
  vPortFree_in_heap(p, _);
#endif
  --m_block_count;
}

FRFXLL_RESULT InitializeHeap()
{
  FRFXLL_RESULT rc = FRFXLL_OK;

#if defined(USH) && !defined(SIM)
  // overhead for each memory block allocated on the heap
  const size_t memory_block_control_structure_size = 8 + sizeof(size_t);
  const size_t heap_overhead = 36;

  int status = 0;
  size_t total_heap_size;
  FRFXLL_MEMORY_REQUIREMENTS mem_req = {0};
  rc = FRFXLLGetMemoryRequirements(sizeof(mem_req), &mem_req);
  if (!FRFXLL_SUCCESS(rc)) return rc;

  total_heap_size = 5 * mem_req.handle + 1 * mem_req.context + 1 * mem_req.fpFeatureSet
    + 1 * mem_req.fpTemplate + 1 * mem_req.alignmentData + 1 * mem_req.comparisonOperation
    + 10 * memory_block_control_structure_size + heap_overhead;
  if (total_heap_size > sizeof(heap_ptr)) return FRFXLL_ERR_NO_MEMORY;

  status = init_heap(total_heap_size, heap_ptr);
  if (status == FALSE) return FRFXLL_ERR_NO_MEMORY;
#endif

  return rc;
}

#ifdef SIM
#include <time.h> // for clock()
#endif

unsigned int m_get_current_tick_in_ms()
{
#ifdef SIM
#ifdef WINCE
  return 0; // ?
#else
  return clock();
#endif
#elif defined(PBE)
  return 0; // ?
#else
  return 0; // ?
#endif
}

FRFXLL_HANDLE CreateContext(bool timer)
{
  FRFXLL_RESULT rc = FRFXLL_OK;

  rc = InitializeHeap();
  if (!FRFXLL_SUCCESS(rc)) return NULL;

  FRFXLL_CONTEXT_INIT ctx_init = {
    sizeof(FRFXLL_CONTEXT_INIT), // length
    heap_ptr,                  // heapContext
    &m_malloc,     // malloc
    &m_free,        // free
  };
  if ( timer ) {
    ctx_init.get_current_tick_in_ms = &m_get_current_tick_in_ms;
  }

  FRFXLL_HANDLE h_context = NULL;
  rc = FRFXLLCreateContext(&ctx_init, &h_context);
  if (!FRFXLL_SUCCESS(rc)) return NULL;

  return h_context;
}

unsigned int CalculateCRC(const unsigned char data[], size_t size)
{
  unsigned short r = 55665;
  unsigned short c1 = 52845;
  unsigned short c2 = 22719;

  unsigned int sum = 0;
  unsigned char cipher;
  for ( size_t i = 0; i < size; i++ )
  {
    cipher = ( data[i] ^ (r>>8) );
    r = (cipher + r) * c1 + c2;
    sum += cipher;
  }

  return sum;
}

unsigned char ImageBuffer::imageBuffer[max_buffer_size];
