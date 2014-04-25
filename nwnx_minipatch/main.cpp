#define _CRT_SECURE_NO_WARNINGS
#include "NWNXMinipatch.h"

CNWNXMinipatch minipatch;

//Export the object
extern "C" __declspec(dllexport) CNWNXBase* GetClassObject(){
	return &minipatch;
}

BOOL APIENTRY DllMain (HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
    return TRUE;
}

