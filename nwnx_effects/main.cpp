#define _CRT_SECURE_NO_WARNINGS
#include "NWNXEffects.h"

CNWNXEffects effects;

//Export the object
extern "C" __declspec(dllexport) CNWNXBase* GetClassObject(){
	return &effects;
}

BOOL APIENTRY DllMain (HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
    return TRUE;
}

