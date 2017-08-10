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

#ifndef __dpfrErrorPrinter_h
#define __dpfrErrorPrinter_h

// dpfrErrorPrinter is a simple TestListener that
// just prints "OK" if everything goes well, otherwise
// reports the error in the format of compiler messages.

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <cxxtest/ErrorFormatter.h>

#if !defined(PBE)
# if defined(__CC_ARM)
//
//# elif defined(__GNUC__)
//#   define SIM
# elif defined(__GNUC__) || defined(_MSC_VER)
#   ifndef SIM
#     define SIM
#   endif
# else // __GNUC__ || _MSC_VER
#   define USH
# endif // __GNUC__ || _MSC_VER
#endif

#if defined(SIM)

#ifdef WINCE
#include <tchar.h>
#define main _tmain

#if (_WIN32_WCE == 0x420 || _WIN32_WCE == 0x400)
#define __REQUIRED_RPCNDR_H_VERSION__ 450 // Workaround for compilation under STANDARDSDK 420 and Windows Mobile 2003
#endif

#if (_WIN32_WCE == 0x420 || _WIN32_WCE == 0x400)
#if !defined(WIN32_PLATFORM_PSPC) && !defined(WIN32_PLATFORM_WFSP)
//#define WINCEMACRO
//#define COM_NO_WINDOWS_H
// Workaround for compilation under STANDARDSDK 420 due to failed definition of LPGLOBALINTERFACETABLE
// in C:\Program Files\Windows CE Tools\wce420\STANDARDSDK_420\include\ARMV4I\objidl.h, line 10704
#define __IGlobalInterfaceTable_INTERFACE_DEFINED__
#endif
#endif

#include "CcService.h"
#include "CcTransport.h"

#if (_WIN32_WCE == 0x420 || _WIN32_WCE == 0x400)
#if !defined(WIN32_PLATFORM_PSPC) && !defined(WIN32_PLATFORM_WFSP)
#ifdef _DEBUG
// Workaround for linking under STANDARDSDK 420 (ARMV4I and x86), Debug
#include <stdarg.h>
#include <altcecrt.h>

#ifndef x86
#define DebugBreak() __debugbreak()
#endif

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
//int __cdecl _VCrtDbgReportW(int nRptType, const TCHAR * szFile, int nLine, const TCHAR * szModule, const TCHAR * szFormat, va_list arglist);
//int __cdecl __crtMessageWindowW(int nRptType, const TCHAR * szFile, const TCHAR * szLine, const TCHAR * szModule, const TCHAR * szUserMessage);
int __cdecl _CrtDbgReportW(_In_ int _nReportType, _In_opt_z_ const wchar_t * _szFilename, _In_ int _nLineNumber, _In_opt_z_ const wchar_t * _szModuleName, _In_opt_z_ const wchar_t * _szFormat, ...)
{
    //int retval(0);
    va_list arglist;

    va_start(arglist, _szFormat);

    //retval = _VCrtDbgReportW(_nReportType, _szFilename, _nLineNumber, _szModuleName, _szFormat, arglist);
    //wchar_t szLineMessage[20] = {0};
    //_itow_s(_nLineNumber, szLineMessage, 20, 10);
    //retval = __crtMessageWindowW(_nReportType, _szFilename, (_nLineNumber? szLineMessage : NULL), _szModuleName, _szFormat);
    wchar_t szOutMessage[1024];
    _snwprintf(szOutMessage, 1024, L"%s(%d) : Assertion failed: ", _szFilename, _nLineNumber);
    size_t len = wcslen(szOutMessage);
    _vsnwprintf(szOutMessage+len, 1024-len-2, _szFormat, arglist);
    wcscat(szOutMessage, L"\n");
    OutputDebugStringW(szOutMessage);//.\_test.cpp(108) : Assertion failed: strlen("return CxxTest::dpfrErrorPrinter().run()") == 555

    va_end(arglist);

    exit(-1);
    return 0;
}
#ifdef __cplusplus
}
#endif //__cplusplus

#endif // _DEBUG
#endif // !defined(WIN32_PLATFORM_PSPC) && !defined(WIN32_PLATFORM_WFSP)
#endif // (_WIN32_WCE == 0x420 || _WIN32_WCE == 0x400)

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS        // some CString constructors will be explicit
#include <atlbase.h>

class CcStream
{
public:
  CcStream()
  {
    if (__argc > 1 && *__wargv[1]) {
      uPort = _wtol(__wargv[1]);
    } else {
      uPort = 0;
    }
    if (uPort) {
      Connect();
    }
  };
  ~CcStream()
  {
    if (pIccStream) {
      pIccStream->Disconnect();
    }
    pIccStream = 0;

    if (pIccService) {
      pIccService->Disconnect();
    }
    pIccService = 0;

    pIccTransport = 0;
    CoUninitialize();
  };

  FRFXLL_RESULT Connect()
  {
    if (!uPort) {
      return S_FALSE;
    }
    WCHAR szMsg[255];
    CRegKey key;
    FRFXLL_RESULT hr;
    key.Create(HKEY_CURRENT_USER, _T("Software\\Microsoft\\CoreCon"));
    TCHAR szConnectivityDll[_MAX_PATH];
    ULONG cch = sizeof(szConnectivityDll) / sizeof(szConnectivityDll[0]);
    if ( !key.QueryStringValue(_T("TransportDll"), szConnectivityDll, &cch ) ) {
      HMODULE hLib = LoadLibrary(szConnectivityDll);
      CreateTransportInstanceFunction pfn = (CreateTransportInstanceFunction)GetProcAddress(hLib, _T("CreateTransportInstance"));
      CComPtr<IUnknown>pUnk;
      hr = pfn(&pUnk);
      pUnk.QueryInterface(&pIccTransport);

      VARIANT_BOOL b;
      hr = pIccTransport->get_SupportsNonUniqueStreamId(&b);

      pIccTransport.QueryInterface(&pIccService);
      if ( FAILED( hr = pIccService->Initialize(L"MyDevice") ) ) {
        wsprintf(szMsg, _T("Failed to initialize stream hr=%08x\n"), hr);
        OutputDebugString(szMsg);
      }

      DWORD dwCk;
      hr = pIccService->Connect(10000, VARIANT_FALSE, &dwCk);

      LPOLESTR szID = L"ip=127.0.0.1 port=%d";
      TCHAR szIPAdddress[255];
      wsprintfW(szIPAdddress, szID, uPort);
      TCHAR szIPAdddressMsg[255];
      wsprintfW(szIPAdddressMsg, L"%s\n", szIPAdddress);
      OutputDebugString(szIPAdddressMsg);
      hr = pIccTransport->CreateStream(szIPAdddress, 60000, VARIANT_FALSE, &pIccStream);
      if ( FAILED(hr)) {
        wsprintf(szMsg, _T("Failed to create stream hr=%08x\n"), hr);
        OutputDebugString(szMsg);
        return hr;
      }

      wsprintf(szMsg, _T("Created stream\n"));
      OutputDebugString(szMsg);

      return S_OK;
    }

    return E_FAIL;
  };

  void SendToStream(char *p) {
    if (!pIccStream) {
      printf(p);
      return;
    }
    FRFXLL_RESULT hr = pIccStream->Send(10000, (LPBYTE)p, strlen(p));
    //Sleep(0);
    BYTE buffer[1024];
    DWORD cbBuffer = sizeof(buffer);

    //VARIANT_BOOL bData;
    //hr = pIccStream->IsDataAvailable(&bData);

    hr = pIccStream->Recv(10000, buffer, &cbBuffer);
  }

  UINT uPort;
  CComPtr<ICcTransport> pIccTransport;
  CComPtr<ICcTransportStream> pIccStream;
  CComPtr<ICcService> pIccService;
};

# undef printf
# define printf(...)     {char __tmp_buf[128]; sprintf(__tmp_buf,__VA_ARGS__); CcStream::SendToStream(__tmp_buf);}
# define _printf(...) printf(__VA_ARGS__)

#else // WINCE
# define _printf(...) printf(__VA_ARGS__); fflush(stdout)
#endif // WINCE

#elif defined(PBE)

# define _printf(x) DPRINTF(DBG_BIOS_IFACE, x)

#else // real USH

extern "C" {
# include <stdio.h>
# define bool bool
# include <ushx_api.h>
}

# define REQ_CONSOLE         1
# ifdef REQ_CONSOLE
#   undef printf
#   define printf(...)     {char __tmp_buf[128]; sprintf(&__tmp_buf[0],__VA_ARGS__); uart_print_string((unsigned char *)&__tmp_buf[0]);}
# else
#   define printf(...)     
# endif
# define _printf(x) printf x

#endif

namespace CxxTest {
  // we need this class to initialize adapter first
  class dpfrErrorPrinterBase {
  protected:
    class Adapter : public OutputStream
#ifdef WINCE
    , public CcStream
#endif // WINCE
    {
      unsigned pos;
      static const unsigned width = 79;
    public:
      Adapter() : pos(0) { }
      void flush() { }
      void wraparound(const char * s) {
        char c = 0;
        size_t len = 0;
        for (; *s; s++, len++) {
          c = *s;
          if (*s == '\n') {
            pos = 0;
          } else {
            pos++;
          }
        }
        if (pos >= width && c == '.' && len == 1) { 
          pos = 0;
          _printf("\n");
        }
      }
      OutputStream & operator << (const char *s) { 
        _printf(s);
        wraparound(s);
        return *this; 
      }
      OutputStream & operator << (Manipulator m) { return OutputStream::operator<<(m); }
      OutputStream & operator << (size_t i) {
        char s[1 + 3 * sizeof(i)];
        numberToString( i, s );
        _printf(s);
        return *this;
      }
    };
    Adapter adapter;
    // virtual ~dpfrErrorPrinterBase() { } // we should not need it since there is no dynamic memory allocation
  };

  class dpfrErrorPrinter : public dpfrErrorPrinterBase, public ErrorFormatter {
  public:
    dpfrErrorPrinter( const char *preLine = "(", const char *postLine = ")" )
      : ErrorFormatter( &adapter, preLine, postLine ) { 
    }
    //void enterTest(const TestDescription & td) {
    //  adapter << td.suiteName() << "::" << td.testName() << Adapter::endl;
    //}
  };
}

#endif // __dpfrErrorPrinter_h
