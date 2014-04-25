#define _CRT_SECURE_NO_WARNINGS
#include "CNWNXDynRes.h"

CNWNXDynRes DynRes;

//Export the object
extern "C" __declspec(dllexport) CNWNXBase* GetClassObject(){
	return &DynRes;
}

BOOL APIENTRY DllMain (HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
    return TRUE;
}

