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
      BINARY: FRFXLLSample - Sample Code for Fingerprint Feature Extractor - LowLevel API
      
      ALGORITHM:      Alexander Ivanisov
                      Yi Chen
                      Salil Prabhakar
      IMPLEMENTATION: Alexander Ivanisov
                      Jacob Kaminsky
                      Lixin Wei
      DATE:           11/08/2011
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "FRFXLL.h"

#include "testFeatures.h"
#include "TestAnsiImage.h"

#define _printf(x) printf x; fflush(stdout)

int cMem = 0;
#define CheckMem() UT_ASSERT(cMem == 0)

static void * m_malloc(size_t size, void * _) {
  ++cMem;
  return malloc(size);
}
static void m_free(void * p, void * _) {
  free(p);
  --cMem;
}

FRFXLL_RESULT CreateContext(FRFXLL_HANDLE_PT phContext) {
  // overhead for each memory block allocated on the heap
  const size_t memory_block_control_structure_size = 8;
  const size_t heap_overhead = 36;
  FRFXLL_RESULT rc = FRFXLL_OK;
  
  size_t total_heap_size;
  FRFXLL_CONTEXT_INIT ctx_init = {
    sizeof(FRFXLL_CONTEXT_INIT), // length
    NULL,                  // heapContext
    &m_malloc,     // malloc
    &m_free,        // free
  };
  FRFXLL_HANDLE h_context = NULL;

  rc = FRFXLLCreateContext(&ctx_init, &h_context);
  if (FRFXLL_SUCCESS(rc))
    *phContext = h_context;

  return rc;
}
#define CREATE_CONTEXT(phCtx) UT_ASSERT_OK(CreateContext(phCtx));

#ifndef countof
#define countof(a) (sizeof(a)/sizeof(a[0]))
#endif

#define STRINGIZE(x)  STRINGIZE1(x)
#define STRINGIZE1(x) #x

static int g_RC=0;

#define UT_ASSERT(x) if (!(x)) { \
  _printf(("\n%s:%d: error : %s \n %s\n", __FILE__, __LINE__, #x, STRINGIZE(x)));g_RC++; \
}

void __ut_assert_equal(const char * file, int line, unsigned x, unsigned y, const char * s1, const char * s2) {
  if (x != y) {
    _printf(("\n%s:%d: error : 0x%x != 0x%x, %s \n %s\n", file, line, x, y, s1, s2));
    g_RC++;
  }
}

#define UT_ASSERT_EQUALS(x, y) __ut_assert_equal(__FILE__, __LINE__, x, y, #x " != " #y, STRINGIZE(x)  " != " STRINGIZE(y))

#define UT_ASSERT_OK(x) {\
  FRFXLL_RESULT rc = x; \
  if (!FRFXLL_SUCCESS(rc)) { \
    _printf(("\n%s:%d: error 0x%x : %s\n", __FILE__, __LINE__, rc, STRINGIZE(x))); \
    g_RC++;\
  } \
}

void __ut_dump(const unsigned char * data, size_t size) {
  size_t i;
  for (i = 0; i < size; i++) {
    _printf(("0x%02x, ", data[i]));
    if ((i & 0xf) == 0xf) {
      _printf(("\n"));
    }
  }
  _printf(("\n"));
}

void __ut_assert_same_data(const char * file, int line, const unsigned char * data, size_t size, const char * s1, const unsigned char * ref, size_t ref_size, const char * s2) {
  if (size != ref_size || memcmp(data, ref, size)) {
    _printf(("\n%s:%d: error : %s differs from %s", file, line, s1, s2));
    g_RC++;
    if (size != ref_size) {
      _printf(("in size : %d != %d\n", size != ref_size));
    } else {
      size_t i, j = 0;
      _printf((" first 10 differences:\n"));
      for (i = 0; i < size; i++) {
        if (data[i] != ref[i]) {
          _printf(("data[0x%04x] = 0x%02x != 0x%02x\n", i, data[i], ref[i]));
          if (++j >= 10) break;
        }
      }
    }
    __ut_dump(data, size);
  }
}
#define UT_ASSERT_SAME_DATA(x, sx, y, sy) __ut_assert_same_data(__FILE__, __LINE__, (x), (sx), STRINGIZE(x), (y), (sy), STRINGIZE(y))

void TestGetLibraryVersion() {
  FRFXLL_VERSION version = {0};
  UT_ASSERT_OK(FRFXLLGetLibraryVersion(&version));
  _printf(("Version %d.%d.%d.%d\n", version.major, version.minor, version.revision, version.build));
}

void TestCreateContext() {
  FRFXLL_HANDLE hCtx = NULL;
  CREATE_CONTEXT(&hCtx);
  UT_ASSERT(hCtx != NULL);

  UT_ASSERT_OK(FRFXLLCloseHandle(hCtx));
}

static const unsigned char expectedFeatureSet[] = {
  0x46, 0x4d, 0x52, 0x00, 0x20, 0x32, 0x30, 0x00, 0x00, 0x00, 0x01, 0xb6, 0x00, 0x00, 0x00, 0xc0,
  0x01, 0x5e, 0x00, 0xc5, 0x00, 0xc5, 0x01, 0x00, 0x00, 0x00, 0x56, 0x44, 0x80, 0x5c, 0x01, 0x4f,
  0x3d, 0x5e, 0x80, 0x5d, 0x00, 0xcb, 0x89, 0x5d, 0x40, 0x4b, 0x01, 0x44, 0xc0, 0x5c, 0x40, 0x67,
  0x01, 0x37, 0x43, 0x59, 0x40, 0x75, 0x00, 0x56, 0x82, 0x57, 0x80, 0x0b, 0x01, 0x25, 0xaf, 0x54,
  0x80, 0x5d, 0x01, 0x32, 0xc3, 0x52, 0x40, 0x7a, 0x01, 0x29, 0x4d, 0x4f, 0x40, 0x42, 0x01, 0x48,
  0x45, 0x4f, 0x80, 0x1a, 0x01, 0x1f, 0xb1, 0x4d, 0x80, 0x95, 0x00, 0x5c, 0x7d, 0x4b, 0x80, 0x9d,
  0x01, 0x2c, 0x47, 0x49, 0x80, 0x74, 0x00, 0x37, 0xf7, 0x46, 0x80, 0x7d, 0x00, 0xbf, 0x77, 0x46,
  0x40, 0x8f, 0x01, 0x2a, 0xc0, 0x46, 0x40, 0x6f, 0x00, 0x56, 0xff, 0x45, 0x40, 0x9c, 0x00, 0x99,
  0x69, 0x45, 0x80, 0x14, 0x00, 0xf5, 0xa3, 0x44, 0x40, 0x65, 0x01, 0x43, 0xb7, 0x44, 0x40, 0x98,
  0x01, 0x53, 0xb5, 0x44, 0x40, 0x0b, 0x01, 0x3d, 0xb5, 0x44, 0x40, 0x15, 0x01, 0x4e, 0xc1, 0x43,
  0x80, 0xb3, 0x00, 0x43, 0x6a, 0x43, 0x80, 0x2d, 0x00, 0xa2, 0x95, 0x42, 0x40, 0xb0, 0x00, 0x98,
  0x64, 0x41, 0x40, 0x6b, 0x01, 0x00, 0xe8, 0x41, 0x40, 0x6d, 0x00, 0xf2, 0x85, 0x41, 0x80, 0x91,
  0x00, 0xee, 0x65, 0x40, 0x80, 0xa6, 0x01, 0x2d, 0xb5, 0x40, 0x40, 0x3f, 0x01, 0x0c, 0xab, 0x40,
  0x40, 0x33, 0x00, 0xa5, 0x09, 0x3f, 0x40, 0x3f, 0x01, 0x19, 0xb3, 0x3f, 0x80, 0xa3, 0x00, 0x57,
  0xe0, 0x3f, 0x80, 0x67, 0x00, 0x5b, 0x91, 0x3f, 0x80, 0x3f, 0x01, 0x00, 0xa4, 0x3f, 0x80, 0x1f,
  0x00, 0xe8, 0x1b, 0x3e, 0x40, 0x8c, 0x00, 0xf5, 0xd7, 0x3e, 0x40, 0x90, 0x01, 0x14, 0x4d, 0x3e,
  0x40, 0x5a, 0x00, 0x2f, 0x84, 0x3e, 0x40, 0x7f, 0x00, 0xce, 0x79, 0x3e, 0x40, 0x0e, 0x00, 0xf5,
  0xa5, 0x3e, 0x40, 0xb0, 0x00, 0xfb, 0x52, 0x3e, 0x40, 0x62, 0x00, 0x1c, 0x72, 0x3d, 0x80, 0x9d,
  0x00, 0x22, 0xe7, 0x3d, 0x40, 0x11, 0x01, 0x2c, 0x3b, 0x3d, 0x40, 0x7b, 0x01, 0x4e, 0x39, 0x3d,
  0x40, 0x91, 0x01, 0x55, 0x3b, 0x3d, 0x80, 0x68, 0x01, 0x56, 0x3b, 0x3d, 0x80, 0x57, 0x00, 0x21,
  0x79, 0x3c, 0x40, 0x98, 0x00, 0xf8, 0x58, 0x3c, 0x80, 0x7a, 0x01, 0x18, 0x58, 0x3c, 0x80, 0x5a,
  0x01, 0x21, 0xbd, 0x3c, 0x40, 0x13, 0x00, 0xfc, 0xa7, 0x3c, 0x80, 0x48, 0x01, 0x1a, 0xac, 0x3c,
  0x40, 0xb8, 0x01, 0x07, 0xd5, 0x3b, 0x40, 0x6b, 0x00, 0x23, 0x76, 0x3b, 0x80, 0x6f, 0x01, 0x2c,
  0xc5, 0x3b, 0x40, 0x9b, 0x01, 0x39, 0x3d, 0x3b, 0x40, 0x5d, 0x00, 0x37, 0x7d, 0x3a, 0x80, 0xa4,
  0x00, 0x49, 0x5d, 0x3a, 0x80, 0x96, 0x00, 0x91, 0x7d, 0x3a, 0x80, 0x4e, 0x00, 0x63, 0x10, 0x39,
  0x80, 0xa4, 0x00, 0xbd, 0xe9, 0x39, 0x40, 0x26, 0x00, 0xcc, 0x96, 0x39, 0x80, 0x69, 0x00, 0xd4,
  0xfb, 0x39, 0x80, 0x83, 0x00, 0xf4, 0xcd, 0x39, 0x40, 0x50, 0x01, 0x3f, 0xc3, 0x39, 0x40, 0x86,
  0x00, 0xfb, 0x5e, 0x39, 0x00, 0x00,  
};

unsigned char image[TESTIMAGESIZE];

void TestCreateFeatureSet() {
  unsigned char data[500];
  size_t size = sizeof(data);

  FRFXLL_HANDLE hCtx = NULL, hFeatureSet = NULL;
  CREATE_CONTEXT(&hCtx);
  UT_ASSERT(hCtx != NULL);

  memcpy(image, TestAnsiImage, TESTIMAGESIZE);
  UT_ASSERT_OK(FRFXLLCreateFeatureSet(hCtx, image, sizeof(image), FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFeatureSet));
  UT_ASSERT_OK(FRFXLLCloseHandle(hCtx));
  UT_ASSERT(hFeatureSet != NULL);
  UT_ASSERT_OK(FRFXLLExport(hFeatureSet, FRFXLL_DT_ISO_FEATURE_SET, NULL, data, &size));
  FRFXLLCloseHandle(hFeatureSet);
  UT_ASSERT_SAME_DATA(data, size, expectedFeatureSet, sizeof(expectedFeatureSet));
  UT_ASSERT(memcmp(image, TestAnsiImage, TESTIMAGESIZE) == 0);
}

void TestCreateFeatureSetInPlace() {
  unsigned char data[500];
  size_t size = sizeof(data);

  FRFXLL_HANDLE hCtx = NULL, hFeatureSet = NULL;
  CREATE_CONTEXT(&hCtx);
  UT_ASSERT(hCtx != NULL);

  memcpy(image, TestAnsiImage, TESTIMAGESIZE);
  UT_ASSERT_OK(FRFXLLCreateFeatureSetInPlace(hCtx, image, sizeof(image), FRFXLL_DT_ANSI_381_SAMPLE, 0, &hFeatureSet));
  UT_ASSERT_OK(FRFXLLCloseHandle(hCtx));
  UT_ASSERT(hFeatureSet != NULL);
  UT_ASSERT_OK(FRFXLLExport(hFeatureSet, FRFXLL_DT_ISO_FEATURE_SET, NULL, data, &size));
  FRFXLLCloseHandle(hFeatureSet);
  UT_ASSERT_SAME_DATA(data, size, expectedFeatureSet, sizeof(expectedFeatureSet));
}

void TestCreateFeatureSetFromRaw() {
  unsigned char data[500];
  size_t size = sizeof(data);

  FRFXLL_HANDLE hCtx = NULL, hFeatureSet = NULL;
  CREATE_CONTEXT(&hCtx);
  UT_ASSERT(hCtx != NULL);

  memcpy(image, TestAnsiImage, TESTIMAGESIZE);
#define width(image) (image[36 + 14 - 5] << 8) | (image[36 + 14 - 4])
#define height(image) (image[36 + 14 - 3] << 8) | (image[36 + 14 - 2])
  UT_ASSERT_OK(FRFXLLCreateFeatureSetFromRaw(hCtx, image + 36 + 14, sizeof(image) - 36 - 14, 
                                                    width(image), height(image), 508, 0, &hFeatureSet));
#undef height
#undef width
  UT_ASSERT_OK(FRFXLLCloseHandle(hCtx));
  UT_ASSERT(hFeatureSet != NULL);
  UT_ASSERT_OK(FRFXLLExport(hFeatureSet, FRFXLL_DT_ISO_FEATURE_SET, NULL, data, &size));
  FRFXLLCloseHandle(hFeatureSet);
  UT_ASSERT_SAME_DATA(data, size, expectedFeatureSet, sizeof(expectedFeatureSet));
  UT_ASSERT(memcmp(image, TestAnsiImage, TESTIMAGESIZE) == 0);
}

void TestCreateFeatureSetInPlaceFromRaw() {
  unsigned char data[500];
  size_t size = sizeof(data);

  FRFXLL_HANDLE hCtx = NULL, hFeatureSet = NULL;
  CREATE_CONTEXT(&hCtx);
  UT_ASSERT(hCtx != NULL);

  memcpy(image, TestAnsiImage, TESTIMAGESIZE);
#define width(image) (image[36 + 14 - 5] << 8) | (image[36 + 14 - 4])
#define height(image) (image[36 + 14 - 3] << 8) | (image[36 + 14 - 2])
  UT_ASSERT_OK(FRFXLLCreateFeatureSetInPlaceFromRaw(hCtx, image + 36 + 14, sizeof(image) - 36 - 14, 
                                                    width(image), height(image), 508, 0, &hFeatureSet));
#undef height
#undef width
  UT_ASSERT_OK(FRFXLLCloseHandle(hCtx));
  UT_ASSERT(hFeatureSet != NULL);
  UT_ASSERT_OK(FRFXLLExport(hFeatureSet, FRFXLL_DT_ISO_FEATURE_SET, NULL, data, &size));
  FRFXLLCloseHandle(hFeatureSet);
  UT_ASSERT_SAME_DATA(data, size, expectedFeatureSet, sizeof(expectedFeatureSet));
}

int RunTests() {
  _printf(("Starting tests.\n"));
  TestGetLibraryVersion();
  _printf(("."));
  TestCreateContext();
  _printf(("."));
  TestCreateFeatureSet();
  _printf(("."));
  TestCreateFeatureSetInPlace();
  _printf(("."));
  TestCreateFeatureSetFromRaw();
  _printf(("."));
  TestCreateFeatureSetInPlaceFromRaw();
  _printf(("."));

  CheckMem();
  _printf((".\nDone\n"));

  return g_RC;
}

int main() {
  return RunTests();
  //getchar();
  //return 0;
}
