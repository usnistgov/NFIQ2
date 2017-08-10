#ifndef OS_H
#define OS_H

#if defined(_WIN32) || defined(ming)
    #define WINDOWS
#else
    #define POSIX
#endif

#endif