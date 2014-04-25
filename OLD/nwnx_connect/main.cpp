#define _CRT_SECURE_NO_WARNINGS
#include "NWNXConnect.h"

CNWNXConnect nconnect;

//Export the object
extern "C" __declspec(dllexport) CNWNXBase* GetClassObject(){
	return &nconnect;
}

BOOL APIENTRY DllMain (HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
    return TRUE;
}

