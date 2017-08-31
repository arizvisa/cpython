#ifndef __modulepath_h
#define __modulepath_h

#include <windows.h>

static DWORD GetHInstancePathW(HINSTANCE, wchar_t**);
static DWORD GetHInstanceWorkingDirectoryW(HINSTANCE, wchar_t**);
static DWORD GetHInstanceWorkingDirectory(HINSTANCE, char**);
static DWORD GetHInstanceRelativeDirectory(HINSTANCE, const char*, char**);

#endif
