#define _CRT_SECURE_NO_WARNINGS
#include "NWNXTest.h"

CNWNXTest test;

//Export the object
extern "C" __declspec(dllexport) CNWNXBase* GetClassObject(){
	return &test;
}

BOOL APIENTRY DllMain (HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
    return TRUE;
}

