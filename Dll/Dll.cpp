#include "Dll.h"

static bool test_function()
{
	OutputDebugStringA("Something new is here!");
	return 1;
}

DLL_API void *__cdecl init_dll(void *Args)
{
	MessageBoxA(0, (std::string("init_dll Func! Args: ") + (const char *)Args).c_str(),
		"From Attached DLL", MB_OK);
	return (void *)1;
}
DLL_API void *__cdecl start_dll(void *Args)
{
	MessageBoxA(0, (std::string("start_dll Func! Args: ") + (const char *)Args).c_str(),
		"From Attached DLL", MB_OK);

	test_function();
	return (void *)1;
}
