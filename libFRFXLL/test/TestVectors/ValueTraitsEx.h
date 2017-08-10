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
      LIBRARY: libFRFXLLTestVectors.a - Test Vectors and Some Common Functions for the 
                                      Unit Tests for Fingerprint Feature Extractor
      
      ALGORITHM:      Alexander Ivanisov
                      Yi Chen
                      Salil Prabhakar
      IMPLEMENTATION: Alexander Ivanisov
                      Jacob Kaminsky
                      Lixin Wei
      DATE:           11/08/2011
*/

// ValueTraitsEx.h

#ifndef __VALUE_TRAITS_EX_H__
#define __VALUE_TRAITS_EX_H__

#include "../../include/FRFXLL_Results.h"

inline const char* GetSymbolicName(FRFXLL_RESULT hr) {
  #define char const char
  static
  #include "dpResults.dbg"
  #undef char
  FRFXLL_RESULT hr_;
  for (int i = 0; (hr_ = dpResultsSymbolicNames[i].MessageId) != -1; i++) {
    if (hr_ == hr) {
      return dpResultsSymbolicNames[i].SymbolicName;
    }
  }

  return "Unknown";
}

#undef CXXTEST_COMPARISONS
#define CXXTEST_COMPARISONS(CXXTEST_X, CXXTEST_Y, CXXTEST_T) \
    CXXTEST_TEMPLATE_INSTANTIATION inline bool equals( CXXTEST_X x, CXXTEST_Y y ) { return (((CXXTEST_T)x) == ((CXXTEST_T)y)); } \
    CXXTEST_TEMPLATE_INSTANTIATION inline bool equals( CXXTEST_Y y, CXXTEST_X x ) { return (((CXXTEST_T)y) == ((CXXTEST_T)x)); } \
    CXXTEST_TEMPLATE_INSTANTIATION inline bool differs( CXXTEST_X x, CXXTEST_Y y ) { return (((CXXTEST_T)x) != ((CXXTEST_T)y)); } \
    CXXTEST_TEMPLATE_INSTANTIATION inline bool differs( CXXTEST_Y y, CXXTEST_X x ) { return (((CXXTEST_T)y) != ((CXXTEST_T)x)); } \
    CXXTEST_TEMPLATE_INSTANTIATION inline bool lessThan( CXXTEST_X x, CXXTEST_Y y ) { return (((CXXTEST_T)x) < ((CXXTEST_T)y)); } \
    CXXTEST_TEMPLATE_INSTANTIATION inline bool lessThan( CXXTEST_Y y, CXXTEST_X x ) { return (((CXXTEST_T)y) < ((CXXTEST_T)x)); } \
    CXXTEST_TEMPLATE_INSTANTIATION inline bool lessThanEquals( CXXTEST_X x, CXXTEST_Y y ) { return (((CXXTEST_T)x) <= ((CXXTEST_T)y)); } \
    CXXTEST_TEMPLATE_INSTANTIATION inline bool lessThanEquals( CXXTEST_Y y, CXXTEST_X x ) { return (((CXXTEST_T)y) <= ((CXXTEST_T)x)); }

enum x_t {};
enum hr_t {};

namespace CxxTest {
  typedef unsigned int uint32;

  enum x_t {};
  CXXTEST_TEMPLATE_INSTANTIATION
  class ValueTraits<const x_t> {
    char _asString[12];
  public:
      ValueTraits(const x_t & t) {
        numberToString<uint32>(uint32(size_t(t)), copyString( _asString, "0x" ), 16);
      }
      const char *asString( void ) const { return _asString; }
  };
  CXXTEST_COPY_CONST_TRAITS(x_t);


  CXXTEST_TEMPLATE_INSTANTIATION
  class ValueTraits<const hr_t> {
    char _asString[50];
  public:
      ValueTraits(const hr_t & t) {
        copyString(copyString(copyString(numberToString<uint32>(t, copyString(_asString, "0x" ), 16), " ("), GetSymbolicName(t)), ")");
      }
      const char *asString( void ) const { return _asString; }
  };
  CXXTEST_COPY_CONST_TRAITS(hr_t);

    template<class Y>
    void doAssertEquals(const char *file, unsigned line, const char *xExpr, void * const & x, const char *yExpr, const Y & y, const char *message) {
      doAssertEquals(file, line, xExpr, x_t(size_t(x)), yExpr, x_t(size_t(y)), message);
    }
    template<class Y>
    void doAssertEquals(const char *file, unsigned line, const char *xExpr, const FRFXLL_RESULT & x, const char *yExpr, const Y & y, const char *message) {
      doAssertEquals(file, line, xExpr, hr_t(x), yExpr, hr_t(y), message);
    }
    template<class X>
    void doAssertEquals(const char *file, unsigned line, const char *xExpr, const X & x, const char *yExpr, const ::x_t & y, const char *message) {
      doAssertEquals(file, line, xExpr, x_t(size_t(x)), yExpr, x_t(y), message);
    }
    inline
    void doAssertEquals(const char *file, unsigned line, const char *xExpr, void * const & x, const char *yExpr, const ::x_t & y, const char *message) {
      doAssertEquals(file, line, xExpr, x_t(size_t(x)), yExpr, x_t(y), message);
    }
    template<class Y>
    void doAssertDiffers(const char *file, unsigned line, const char *xExpr, void * const & x, const char *yExpr, const Y & y, const char *message) {
      doAssertDiffers(file, line, xExpr, x_t(size_t(x)), yExpr, x_t(size_t(y)), message);
    }
    template<class Y>
    void doAssertDiffers(const char *file, unsigned line, const char *xExpr, const FRFXLL_RESULT & x, const char *yExpr, const Y & y, const char *message) {
      doAssertDiffers(file, line, xExpr, hr_t(x), yExpr, hr_t(y), message);
    }
    template<class X>
    void doAssertDiffers(const char *file, unsigned line, const char *xExpr, const X & x, const char *yExpr, const ::x_t & y, const char *message) {
      doAssertDiffers(file, line, xExpr, x_t(size_t(x)), yExpr, x_t(y), message);
    }
    //template<>
    inline
    void doAssertDiffers(const char *file, unsigned line, const char *xExpr, void * const & x, const char *yExpr, const ::x_t & y, const char *message) {
      doAssertDiffers(file, line, xExpr, x_t(size_t(x)), yExpr, x_t(y), message);
    }
}
// TS_ASSERT_EQUALS_X
#   define ___ETS_ASSERT_EQUALS_X(f,l,x,y,m)    CxxTest::doAssertEquals( (f), (l), #x, x_t(size_t(x)), #y, x_t(size_t(y)), (m) )
#   define ___TS_ASSERT_EQUALS_X(f,l,x,y,m)     { _TS_TRY { ___ETS_ASSERT_EQUALS_X(f,l,x,y,m); } __TS_CATCH(f,l) }

#   define _ETS_ASSERT_EQUALS_X(f,l,x,y)        ___ETS_ASSERT_EQUALS_X(f,l,x,y,0)
#   define _TS_ASSERT_EQUALS_X(f,l,x,y)         ___TS_ASSERT_EQUALS_X(f,l,x,y,0)

#   define ETS_ASSERT_EQUALS_X(x,y)             _ETS_ASSERT_EQUALS_X(__FILE__,__LINE__,x,y)
#   define TS_ASSERT_EQUALS_X(x,y)              _TS_ASSERT_EQUALS_X(__FILE__,__LINE__,x,y)

#   define _ETSM_ASSERT_EQUALS_X(f,l,m,x,y)     ___ETS_ASSERT_EQUALS_X(f,l,x,y,TS_AS_STRING(m))
#   define _TSM_ASSERT_EQUALS_X(f,l,m,x,y)      ___TS_ASSERT_EQUALS_X(f,l,x,y,TS_AS_STRING(m))

#   define ETSM_ASSERT_EQUALS_X(m,x,y)          _ETSM_ASSERT_EQUALS_X(__FILE__,__LINE__,m,x,y)
#   define TSM_ASSERT_EQUALS_X(m,x,y)           _TSM_ASSERT_EQUALS_X(__FILE__,__LINE__,m,x,y)

// TS_ASSERT_DIFFERS_X
#   define ___ETS_ASSERT_DIFFERS_X(f,l,x,y,m)   CxxTest::doAssertDiffers( (f), (l), #x, x_t(size_t(x)), #y, x_t(size_t(y)), (m) )
#   define ___TS_ASSERT_DIFFERS_X(f,l,x,y,m)    { _TS_TRY { ___ETS_ASSERT_DIFFERS_X(f,l,x,y,m); } __TS_CATCH(f,l) }

#   define _ETS_ASSERT_DIFFERS_X(f,l,x,y)       ___ETS_ASSERT_DIFFERS_X(f,l,x,y,0)
#   define _TS_ASSERT_DIFFERS_X(f,l,x,y)        ___TS_ASSERT_DIFFERS_X(f,l,x,y,0)

#   define ETS_ASSERT_DIFFERS_X(x,y)            _ETS_ASSERT_DIFFERS_X(__FILE__,__LINE__,x,y)
#   define TS_ASSERT_DIFFERS_X(x,y)             _TS_ASSERT_DIFFERS_X(__FILE__,__LINE__,x,y)

#   define _ETSM_ASSERT_DIFFERS_X(f,l,m,x,y)    ___ETS_ASSERT_DIFFERS_X(f,l,x,y,TS_AS_STRING(m))
#   define _TSM_ASSERT_DIFFERS_X(f,l,m,x,y)     ___TS_ASSERT_DIFFERS_X(f,l,x,y,TS_AS_STRING(m))

#   define ETSM_ASSERT_DIFFERS_X(m,x,y)         _ETSM_ASSERT_DIFFERS_X(__FILE__,__LINE__,m,x,y)
#   define TSM_ASSERT_DIFFERS_X(m,x,y)          _TSM_ASSERT_DIFFERS_X(__FILE__,__LINE__,m,x,y)

// TS_ASSERT_EQUALS_H
#   define ___ETS_ASSERT_EQUALS_H(f,l,x,y,m)    CxxTest::doAssertEquals( (f), (l), #x, hr_t(x), #y, hr_t(y), (m) )
#   define ___TS_ASSERT_EQUALS_H(f,l,x,y,m)     { _TS_TRY { ___ETS_ASSERT_EQUALS_H(f,l,x,y,m); } __TS_CATCH(f,l) }

#   define _ETS_ASSERT_EQUALS_H(f,l,x,y)        ___ETS_ASSERT_EQUALS_H(f,l,x,y,0)
#   define _TS_ASSERT_EQUALS_H(f,l,x,y)         ___TS_ASSERT_EQUALS_H(f,l,x,y,0)

#   define ETS_ASSERT_EQUALS_H(x,y)             _ETS_ASSERT_EQUALS_H(__FILE__,__LINE__,x,y)
#   define TS_ASSERT_EQUALS_H(x,y)              _TS_ASSERT_EQUALS_H(__FILE__,__LINE__,x,y)

#   define _ETSM_ASSERT_EQUALS_H(f,l,m,x,y)     ___ETS_ASSERT_EQUALS_H(f,l,x,y,TS_AS_STRING(m))
#   define _TSM_ASSERT_EQUALS_H(f,l,m,x,y)      ___TS_ASSERT_EQUALS_H(f,l,x,y,TS_AS_STRING(m))

#   define ETSM_ASSERT_EQUALS_H(m,x,y)          _ETSM_ASSERT_EQUALS_H(__FILE__,__LINE__,m,x,y)
#   define TSM_ASSERT_EQUALS_H(m,x,y)           _TSM_ASSERT_EQUALS_H(__FILE__,__LINE__,m,x,y)

#define TS_ASSERT_RC(e, rc)                     TS_ASSERT_EQUALS(e, rc)
#define TS_ASSERT_OK(e)                         TS_ASSERT_RC(e, FRFXLL_OK)

#endif // __VALUE_TRAITS_EX_H__
