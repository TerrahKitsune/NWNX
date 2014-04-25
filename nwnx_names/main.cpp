#define _CRT_SECURE_NO_WARNINGS
#include "NWNXNames.h"

CNWNXNames Names;
extern "C" __declspec(dllexport) CNWNXBase* GetClassObject()
{
	return &Names;
}

BOOL APIENTRY DllMain (HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}