#define _CRT_SECURE_NO_WARNINGS
#include "NWNXMagic.h"

CNWNXMagic Magic;
extern "C" __declspec(dllexport) CNWNXBase* GetClassObject()
{
	return &Magic;
}

BOOL APIENTRY DllMain (HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}