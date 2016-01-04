#include "CNWNXDynRes.h"
#include "../NWNXdll/madCHook.h"
#include "../NWNXdll/IniFile.h"
#include <stdio.h>
#include <math.h>

#pragma comment(lib, "madChook.lib")

extern CNWNXDynRes DynRes;

int (__fastcall *CExoResMan__GetKeyEntryNext)( void * pResMan, void*, char * ResRef, unsigned short uType, void * CExoKeyTable, void * CKeyTableEntry );
void * (__fastcall *CExoResMan__DemandNext)( void * pResMan, void*, CRes * nwRes );
int (__fastcall *CExoResMan__ExistsNext)( void * pResMan, void*, char * ResRef, unsigned short a1, unsigned long * a2 );

int __fastcall CExoResMan__Exists( void * pResMan, void*, char * ResRef, unsigned short a1, unsigned long * a2 ){

	int ret = CExoResMan__ExistsNext(pResMan, NULL, ResRef, a1, a2);

	char file[255];
	DynRes.StringCopyToLower(file, ResRef, 16);
	strcat(file, ".");
	strcat(file, DynRes.GetExtensionFromTypeID(a1));
	FileEntry * node = DynRes.GetExternalFileToLoad(DynRes.LastFile);

	if (node)
		return 1;

	return ret;
}

int __fastcall CExoResMan__GetKeyEntry( void * pResMan, void*, char * ResRef, unsigned short uType, void * CExoKeyTable, void * CKeyTableEntry ){

	DynRes.HasFile=false;

	if( ResRef ){

		char resref[17];
		resref[16]=0;
		DynRes.StringCopyToLower( resref, ResRef, 16 );

		sprintf( DynRes.LastFile, "%s.%s", resref, DynRes.GetExtensionFromTypeID( uType ) );	

		if( uType == 2015 ){

			if( DynRes.logz >= 1 )DynRes.Log( "o Forcing Interal load: %s\n", DynRes.LastFile );		
		}
		else
			DynRes.HasFile=true;
	}

	return CExoResMan__GetKeyEntryNext( pResMan, NULL, ResRef, uType, CExoKeyTable, CKeyTableEntry );
}

void * __fastcall CExoResMan__Demand( void * pResMan, void*, CRes * nwRes ){

	if (!nwRes || nwRes->bHasContents){
		DynRes.HasFile=false;
		return CExoResMan__DemandNext( pResMan, NULL, nwRes );
	}

	if( DynRes.HasFile ){

		FileEntry * file = DynRes.GetExternalFileToLoad( DynRes.LastFile );

		if( file ){

			unsigned long len = 0;
			void * data = DynRes.LoadFile( file, &len );
			if( !data ){
				DynRes.Log( "! File not found: %s -> %s\n", file->file, file->fullpath );
				file->LoadTimes = 0xFFFFFFFF;
			}
			else{
				if( DynRes.logz >= 1 )DynRes.Log( "o External: %s -> %s\n", file->file, file->fullpath );
				file->LoadTimes++;
				return DynRes.SetResEngienData( nwRes, data, len );
			}
		}
		else{
			
			if( DynRes.logz >= 2 )DynRes.Log( "o Internal: %s\n", DynRes.LastFile );
		}
	}
	
	DynRes.totaldemand++;
	return CExoResMan__DemandNext( pResMan, NULL, nwRes );
}

void Hook(){

	BOOL bOk = HookCode( (PVOID)0x00416A30, CExoResMan__Demand, (PVOID*)&CExoResMan__DemandNext );
	DynRes.Log( "o Hooked CExoResMan::Demand: %s\n", bOk == -1 ? "Success" : "Failure" );

	bOk = HookCode( (PVOID)0x00416E30, CExoResMan__GetKeyEntry, (PVOID*)&CExoResMan__GetKeyEntryNext );
	DynRes.Log( "o Hooked CExoResMan::GetKeyEntry: %s\n", bOk == -1 ? "Success" : "Failure" );
	
	bOk = HookCode( (PVOID)0x00416BE0, CExoResMan__Exists, (PVOID*)&CExoResMan__ExistsNext );
	DynRes.Log( "o Hooked CExoResMan::Exists: %s\n", bOk == -1 ? "Success" : "Failure" );

}