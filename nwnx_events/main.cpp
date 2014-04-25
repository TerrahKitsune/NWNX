#define _CRT_SECURE_NO_WARNINGS
#include "NWNXEvents.h"

CNWNXEvents events;

//Export the object
extern "C" __declspec(dllexport) CNWNXBase* GetClassObject(){
	return &events;
}

BOOL APIENTRY DllMain (HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
    return TRUE;
}

