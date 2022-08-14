#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <iostream>
#include <functional>
#include <map>
#include <algorithm>
#include <vector>

#include <utility>

#ifdef DLL_EXPORTS
	#define DLL_API __declspec(dllexport)
#else
	#define DLL_API __declspec(dllimport)
#endif

#define VERSION 0x00000001L
#define VERSION_TEXT "0.0.1"
#define SUPPORT_VERSION 0x00000000L
#define SUPPORT_VERSION_TEXT "0.0.0"

_EXTERN_C
	DLL_API void *__cdecl start_dll(void *Args);
	DLL_API void *__cdecl init_dll(void *Args);

	DLL_API long __cdecl get_version();
	DLL_API long __cdecl get_suport_version();

	DLL_API const char *__cdecl get_version_text();
	DLL_API const char *__cdecl get_suport_version_text();
_END_EXTERN_C
