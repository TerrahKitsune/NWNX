#define _CRT_SECURE_NO_WARNINGS
#include "NWNXWeapons.h"

CNWNXWeapons Weapons;
extern "C" __declspec(dllexport) CNWNXBase* GetClassObject()
{
	return &Weapons;
}

BOOL APIENTRY DllMain (HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}