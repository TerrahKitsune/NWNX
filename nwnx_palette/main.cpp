#define _CRT_SECURE_NO_WARNINGS
#include "NWNXPalette.h"

CNWNXPalette palette;

//Export the object
extern "C" __declspec(dllexport) CNWNXBase* GetClassObject(){
	return &palette;
}

BOOL APIENTRY DllMain (HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
    return TRUE;
}