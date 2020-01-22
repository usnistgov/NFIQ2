#include <string>

std::string g_modulePath;

# ifdef _WIN32
#include <windows.h>

extern "C" int APIENTRY DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved )
{
  if( fdwReason == DLL_PROCESS_ATTACH )
  {
    char buffer[MAX_PATH];
    int n = GetModuleFileNameA( hinstDLL, buffer, sizeof( buffer ) );
    if( n > 0 && n < ( int )sizeof( buffer ) )
    {
      char* c = strrchr( buffer, '\\' );
      if( c != nullptr )
      {
        *c = 0;
        g_modulePath =  buffer;
        g_modulePath += '\\';
      }
    }
  }
  return TRUE;
}

# elif 
#include <cstring>
#include <dlfcn.h>

void __attribute__( ( constructor ) ) my_load( void );

void my_load( void )
{
  Dl_info info;
 if (dladdr(my_load, &info))
 {
      char* c = strrchr( info.dli_fname, '/' );
      if( c != nullptr )
      {
        *c = 0;
        g_modulePath =  info.dli_fname;
        g_modulePath += '/';
      }
 }
# endif
