#define _CRT_SECURE_NO_WARNINGS
#include "CNWNXPoly.h"

CNWNXPoly poly;

//Export the object
extern "C" __declspec(dllexport) CNWNXBase* GetClassObject(){
	return &poly;
}

BOOL APIENTRY DllMain (HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
    return TRUE;
}

