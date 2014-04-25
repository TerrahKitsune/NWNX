#define _CRT_SECURE_NO_WARNINGS
#include "CNWNXRand.h"

CNWNXRand crand;

//Export the object
extern "C" __declspec(dllexport) CNWNXBase* GetClassObject(){
	return &crand;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
	return TRUE;
}

