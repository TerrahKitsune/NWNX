#include "NWNXData.h"
#include "../NWNXdll/madCHook.h"
#include "../NWNXdll/IniFile.h"
#include <stdio.h>
#include <math.h>


#pragma comment(lib, "madChook.lib")

extern CNWNXData nwnxdata;

int(__fastcall *CCodeBaseInternal__AddBinaryDataNext)(void * pThis, void*, CExoString & DB, CExoString & Column, CExoString & Ver, char Flag, void * ptr, DWORD Size);
void * (*__fastcall CCodeBaseInternal__GetBinaryDataNext)(void * pThis, void*, CExoString & DB, CExoString & Column, CExoString & Ver, char &flag, DWORD &Size);

void * __fastcall CCodeBaseInternal__GetBinaryData(void * pThis, void*, CExoString & DB, CExoString & Column, CExoString & Ver, char &flag, DWORD &Size){

	if (strcmp(DB.text, "DATA") != 0){
		return CCodeBaseInternal__GetBinaryDataNext(pThis, NULL, DB, Column, Ver, flag, Size);
	}

	GFF * gffFile = new GFF();

	if (gffFile){

		if (gffFile->LoadGFFFromFile(Column.text)){

			if (gffFile->Type[0] == 'B' &&
				gffFile->Type[1] == 'I' &&
				gffFile->Type[2] == 'C' &&
				gffFile->TopStruct->FieldCount > 1){

				for (int n = 0; n < gffFile->TopStruct->FieldCount; n++){

					if (strcmp(gffFile->TopStruct->Fields[n].Label, "IsPC") == 0 ||
						strcmp(gffFile->TopStruct->Fields[n].Label, "IsDM") == 0){

						//nwnxdata.Log("o Corrected: %s\n", gffFile->TopStruct->Fields[n].Label);

						gffFile->TopStruct->Fields[n].Data = 0;
					}
				}
			}

		}
		else
			delete gffFile;
	}
	else{

		nwnxdata.Log("! Failed to create memory: %s\n", Column.text);
		return NULL;
	}
		
	void * bdata = NULL;
	GFFPack * gPack = new GFFPack(gffFile);

	if (gPack){

		if (gPack->RAW){

			bdata = nwnxdata.mem.nwnx_malloc(gPack->TotalSize);
			if (bdata){
				memcpy(bdata, gPack->RAW, gPack->TotalSize);
				Size = gPack->TotalSize;
				delete gPack;
			}
		}
		else{

			delete gPack;
		}
	}

	delete gffFile;
	
	if (bdata)
		nwnxdata.Log("o Fetched: %s\n", Column.text);
	else
		nwnxdata.Log("! Failed to retrive file: %s\n", Column.text);

	return bdata;
}

int __fastcall CCodeBaseInternal__AddBinaryData(void * pThis, void*, CExoString & DB, CExoString & Column, CExoString & Ver, char Flag, void * ptr, DWORD Size){

	//nwnxdata.Log("o AddBinaryData: %s %s %s\n", DB.text, Column.text, Ver.text);

	if (strcmp(DB.text, "DATA") != 0){
		return CCodeBaseInternal__AddBinaryDataNext(pThis, NULL, DB, Column, Ver, Flag, ptr, Size);
	}	

	if (!ptr || Size <= 0){

		nwnxdata.Log("! NWScript: WriteToFile nwserver passed NULL pointer or size was 0!\n");
		return 0;
	}

	char Path[MAX_PATH];

	char Ext[5] = { 0 };
	Ext[0] = '.';

	Ext[1] = (char)tolower(*((char*)ptr));
	Ext[2] = (char)tolower(*((char*)ptr + 1));
	Ext[3] = (char)tolower(*((char*)ptr + 2));
	

	int nDotIndex = -1;
	bool pOk = false;

	for (int n = 0; n < MAX_PATH; n++){

		if (Column.text[n] == 0){

			Path[n] = 0;
			pOk = true;
			break;
		}
		else if (Column.text[n] == '\\' || Column.text[n] == '/'){

			Path[n] = '/';
			nDotIndex = -1;
		}
		else if (Column.text[n] == '.'){

			Path[n] = '.';
			nDotIndex = n;
		}
		else{

			Path[n] = Column.text[n];
		}
	}

	if (!pOk){
		return 0;
	}
	else if (nDotIndex != -1){

		Path[nDotIndex] = 0;
	}

	strcat(Path, Ext);

	FILE * fNew = fopen(Path, "wb");
	
	if (!fNew)
		return 0;

	fwrite(ptr, 1, Size, fNew);

	fclose(fNew);

	nwnxdata.Log("o Saved: %s\n", Path);

	return 1;
}

void HookFuncs(){

	BOOL bOk = HookCode((PVOID)0x005D60C0, CCodeBaseInternal__AddBinaryData, (PVOID*)&CCodeBaseInternal__AddBinaryDataNext);
	nwnxdata.Log("o Hooked CCodeBaseInternal::AddBinaryData: %s\n", bOk == -1 ? "Success" : "Failure");

	bOk = HookCode((PVOID)0x005D5D70, CCodeBaseInternal__GetBinaryData, (PVOID*)&CCodeBaseInternal__GetBinaryDataNext);
	nwnxdata.Log("o Hooked CCodeBaseInternal::GetBinaryData: %s\n\n", bOk == -1 ? "Success" : "Failure");

}