#define _CRT_SECURE_NO_WARNINGS
#include "ResHooks.h"

#include "../NWNXdll/madCHook.h"
#pragma comment(lib, "madCHook.lib")

void * g_pResMan;
bool InHook;

extern CNWNXResources Resources;

int (__fastcall *CExoResMan__GetKeyEntryNext)( void * pResMan, void*, char * ResRef, unsigned short uType, void * CExoKeyTable, void * CKeyTableEntry );
void * (__fastcall *CExoResMan__DemandNext)( void * pResMan, void*, CRes * nwRes );


int __fastcall CExoResMan__GetKeyEntry( void * pResMan, void*, char * ResRef, unsigned short uType, void * CExoKeyTable, void * CKeyTableEntry ){

	if( ResRef != NULL ){

		g_pResMan=pResMan;
		Resources.LastType=uType;
		strncpy( Resources.ResRef, ResRef, 16 );
	}
	else
		memset( Resources.ResRef, 0, 20 );

	return CExoResMan__GetKeyEntryNext( pResMan, NULL, ResRef, uType, CExoKeyTable, CKeyTableEntry );
}

void * __fastcall CExoResMan__Demand( void * pResMan, void*, CRes * nwRes ){

	if( Resources.ResRef[0] != 0 && Resources.GetTypeAllowed( Resources.LastType ) ){

		g_pResMan=pResMan;
		char fullFile[260];

		sprintf( fullFile, "%s.%s", Resources.ResRef, Resources.GetExtensionFromTypeID( Resources.LastType ) );

		//Load our resources and get the loaded size
		unsigned long nLoadedSize = nwRes->LoadResource( fullFile );

		if( nLoadedSize == 1 ){

			memset( Resources.ResRef, 0, 20 );
			return 0;
		}
		//We loaded something, return it
		else if( nLoadedSize > 0 ){

			if( Resources.priority == 1 && nwRes->fileCLen > 0 )
				Resources.Log( 0, "o Loaded External File: %s: %lu bytes\n", fullFile, nLoadedSize );
			memset( Resources.ResRef, 0, 20 );
			return nwRes->fileContents;
		}
	}

	void * nReturn = CExoResMan__DemandNext( pResMan, NULL, nwRes );

	if( Resources.ResRef[0] != 0 && Resources.priority == 1 ){

		Resources.Log( 0, "o Loaded Internal File: %s.%s: %lu bytes\n", Resources.ResRef, Resources.GetExtensionFromTypeID( Resources.LastType ), nwRes->fileCLen );
	}
	memset( Resources.ResRef, 0, 20 );
	return nReturn;
}

void HookCNWRES( ){

	InHook = false;
	BOOL bOk = 0;

	//Construct the hook
	DWORD dIDs = *(DWORD*)(*(DWORD*)((*(DWORD*)0x0066C040)+0x18)+0x4);
	DWORD dExt = *(DWORD*)(*(DWORD*)((*(DWORD*)0x0066C040)+0x18)+0x8);

	Resources.idLookup = (unsigned short*)dIDs;
	Resources.extLookup = (CExoStr*)dExt;

	bOk = HookCode( (PVOID)0x00416A30, CExoResMan__Demand, (PVOID*)&CExoResMan__DemandNext );
	Resources.Log( 0, "Hooked CExoResMan::Demand: %s\n", bOk == -1 ? "Success" : "Failure" );

	bOk = HookCode( (PVOID)0x00416E30, CExoResMan__GetKeyEntry, (PVOID*)&CExoResMan__GetKeyEntryNext );
	Resources.Log( 0, "Hooked CExoResMan::GetKeyEntry: %s\n", bOk == -1 ? "Success" : "Failure" );

	Resources.Log( 0, "\nCExoBaseInternal Extention ID Array: 0x%0X\n", Resources.idLookup );
	Resources.Log( 0, "CExoBaseInternal Extention CExoString Array: 0x%0X\n\n", Resources.extLookup );
}

CRes * GetResObject( char * ResRef, unsigned short uType ){

	CRes * ( __thiscall * CExoResMan__GetResObject)( void * pResman, char * resref, unsigned short uType ) = (CRes *(__thiscall *)(void *, char *,unsigned short))0x00416FF0;

	InHook = true;
	
	CRes * cRet = CExoResMan__GetResObject( g_pResMan, ResRef, uType );

	InHook = false;

	return cRet;
}

void SetResObject( CRes * nwRes, char * ResRef, unsigned short uType ){

	void ( __thiscall * CExoResMan__SetResObject)( void * pResman, char * resref, unsigned short uType, CRes * res ) = (void (__thiscall *)(void *, char *,unsigned short, CRes *))0x00417050;

	InHook = true;
	
	CExoResMan__SetResObject( g_pResMan, ResRef, uType, nwRes );

	InHook = false;
}