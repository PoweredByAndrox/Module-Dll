#include "Dll.h"

BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			//MessageBoxA(0, "DLL_PROCESS_ATTACH Successfully!", "Caption!", MB_OK);
			break;
		}
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
		{
			//MessageBoxA(0, "DLL_PROCESS_DETACH Successfully!", "Caption!", MB_OK);
			break;
		}
	}
	return TRUE;
}

