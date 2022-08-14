#include "Dll.h"

#define FEATURE_CODE 0x00000001L + 1

DLL_API const char *__cdecl get_version_text()
{
	return VERSION_TEXT;
}

DLL_API const char *__cdecl get_suport_version_text()
{
	return SUPPORT_VERSION_TEXT;
}

DLL_API long __cdecl get_version()
{
	return VERSION;
}

DLL_API long __cdecl get_suport_version()
{
	return SUPPORT_VERSION;
}

static double Calculate(double x, double oper, double y)
{
	switch ((char)oper)
	{
		case '+':
			return x + y;
		case '-':
			return x - y;
		case '*':
			return x * y;
		case '/':
			return x / y;
		default:
			return 0.0;
	}
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

	double x = ((double *) Args)[0], y = ((double *) Args)[2];

	// If we have some new code and it's marked as Feature
	if (VERSION == FEATURE_CODE)
	{
		// We just increase X and Y are that came from Args
		x++; y++;
	}

	auto ret = Calculate(x, ((double *) Args)[1], y);
	return new double(ret);
}
