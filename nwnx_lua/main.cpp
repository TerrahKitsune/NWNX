#define _CRT_SECURE_NO_WARNINGS
#include "NWNXLua.h"

CNWNXLua lua;

//Export the object
extern "C" __declspec(dllexport) CNWNXBase* GetClassObject(){
	return &lua;
}

BOOL APIENTRY DllMain (HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
    return TRUE;
}