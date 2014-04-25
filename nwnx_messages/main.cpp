#define _CRT_SECURE_NO_WARNINGS
#include "NWNXMessages.h"

CNWNXMessages cMessages;

//Export the object
extern "C" __declspec(dllexport) CNWNXBase* GetClassObject(){
	return &cMessages;
}

BOOL APIENTRY DllMain (HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
    return TRUE;
}

