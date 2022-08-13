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

_EXTERN_C
	DLL_API void *__cdecl start_dll(void *Args);
	DLL_API void *__cdecl init_dll(void *Args);
_END_EXTERN_C
