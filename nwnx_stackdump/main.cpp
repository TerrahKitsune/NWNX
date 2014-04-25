#define _CRT_SECURE_NO_WARNINGS
#include "NWNXStack.h"

CNWNXStack stack;

//Export the object
extern "C" __declspec(dllexport) CNWNXBase* GetClassObject(){
	return &stack;
}

BOOL APIENTRY DllMain (HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
    return TRUE;
}