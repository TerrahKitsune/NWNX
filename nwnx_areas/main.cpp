#define _CRT_SECURE_NO_WARNINGS
#include "NWNXAreas.h"

CNWNXAreas areas;

//Export the object
extern "C" __declspec(dllexport) CNWNXBase* GetClassObject(){
	return &areas;
}

BOOL APIENTRY DllMain (HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
    return TRUE;
}

