#define _CRT_SECURE_NO_WARNINGS
#include "NWNXData.h"

CNWNXData nwnxdata;

//Export the object
extern "C" __declspec(dllexport) CNWNXBase* GetClassObject(){
	return &nwnxdata;
}

BOOL APIENTRY DllMain (HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
    return TRUE;
}
