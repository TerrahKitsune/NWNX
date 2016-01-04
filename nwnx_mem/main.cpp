#define _CRT_SECURE_NO_WARNINGS
#include "NWNXMem.h"

CNWNXMem cmem;

//Export the object
extern "C" __declspec(dllexport) CNWNXBase* GetClassObject(){
	return &cmem;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
	return TRUE;
}