#include "NWNXResources.h"

CNWNXResources Resources;
extern "C" __declspec(dllexport) CNWNXBase* GetClassObject()
{
	return &Resources;
}

BOOL APIENTRY DllMain (HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}