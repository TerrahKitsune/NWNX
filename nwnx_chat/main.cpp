#define _CRT_SECURE_NO_WARNINGS
#include "NWNXChat.h"

CNWNXChat chat;

//Export the object
extern "C" __declspec(dllexport) CNWNXChat* GetClassObject(){
	return &chat;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
	return TRUE;
}

