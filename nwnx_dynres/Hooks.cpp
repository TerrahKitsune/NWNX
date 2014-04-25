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
int (__fastcall *CCodeBaseInternal__AddBinaryDataNext)( void * pThis, void*, CExoString & DB, CExoString & Column, CExoString & Ver, char Flag, void * ptr, DWORD Size );
void * (*__fastcall CCodeBaseInternal__GetBinaryDataNext)( void * pThis, void*, CExoString & DB, CExoString & Column, CExoString & Ver, char &flag, DWORD &Size);

void * __fastcall CCodeBaseInternal__GetBinaryData( void * pThis, void*, CExoString & DB, CExoString & Column, CExoString & Ver, char &flag, DWORD &Size){

	if( strcmp( DB.text, "DYNRES" ) > 0 ){
		return CCodeBaseInternal__GetBinaryDataNext( pThis, NULL, DB, Column, Ver, flag, Size );
	}

	char Path[MAX_PATH];

	if( strstr( Column.text, ".bic" ) ){

		for( int n=0;n<Column.len;n++ ){
			if( Column.text[n] == '\\' )
				Column.text[n] = '/';
		}

		strcpy( Path, Column.text );
	}
	else{

		strcpy( Path, Column.text );
		strcat( Path, ".bic" );

		BinaryTree::BTEntry * node = DynRes.Find( Path );	
		if( node ){

			strcpy( Path, ((FileEntry*)node->data)->fullpath );
		}
		else{
			return 0;
		}
	}

	FileEntry FE;

	memset( &FE, 0, sizeof( FileEntry ) );
	FE.file = NULL;
	FE.fullpath = Path;

	void * ret = DynRes.LoadFile( &FE, &Size );

	if( (DWORD)ret < 0x10 )
		ret = NULL;

	return ret;
}

int __fastcall CCodeBaseInternal__AddBinaryData( void * pThis, void*, CExoString & DB, CExoString & Column, CExoString & Ver, char Flag, void * ptr, DWORD Size ){

	if( strcmp( DB.text, "DYNRES" ) > 0 ){
		return CCodeBaseInternal__AddBinaryDataNext( pThis, NULL, DB, Column, Ver, Flag, ptr, Size );
	}

	if( !ptr || Size<=0 ){

		DynRes.Log( "! NWScript: WriteToFile nwserver passed NULL pointer or size was 0!\n" );
		return 0;
	}

	char * ResRef=strstr( Column.text, "|" );
	if( ResRef ){
		*ResRef='\0';
		ResRef=NULL;
	}
	DWORD DotIndex=0;

	for( DWORD n=0;n<Column.len;n++ ){

		if( Column.text[n]=='\\' ){
			Column.text[n]='/';
			ResRef=&Column.text[n+1];
		}
		else if( Column.text[n]=='/' ){
			ResRef=&Column.text[n+1];
		}
		else if( Column.text[n]=='.' ){
			DotIndex=n;
		}
	}
		
	if( DotIndex > 0 )
		Column.text[DotIndex]='\0';

	if( !ResRef ){

		DynRes.Log( "! NWScript: WriteToFile no resref (or path) found in: %s\n", Column.text );
		return 0;
	}

	for( DWORD n=0;n<16;n++ ){

		if( ResRef[n]=='\0' )
			break;
		else if( !isalnum( ResRef[n] ) && !isalpha( ResRef[n]) ){
			ResRef[n]='_';
		}
		else
			ResRef[n]=(char)tolower( ResRef[n] );
	}

	ResRef[16]='\0';

	char Ext[5]={0};
	Ext[0]='.';

	if( strncmp( (char*)ptr, "BIC", 3 )==0 ){

		Ext[1]=(char)tolower( *((char*)ptr) );
		Ext[2]=(char)tolower( *((char*)ptr+1) );
		Ext[3]=(char)tolower( *((char*)ptr+2) );
	}
	else{

		Ext[1]=(char)tolower( *((char*)ptr) );
		Ext[2]=(char)tolower( *((char*)ptr+1) );
		Ext[3]=(char)tolower( *((char*)ptr+2) );
	}

	strcat( Column.text, Ext );

	FILE * fNew = fopen( Column.text, "wb" );
	if( !fNew ){

		DynRes.Log( "! NWScript: WriteToFile unable to create file: %s\n", Column.text );
		return 0;
	}

	size_t Retrn = fwrite( ptr, 1, Size, fNew );
	fclose( fNew );

	if( Retrn < Size ){

		DynRes.Log( "! NWScript: WriteToFile unable to write %u bytes\n", Size-Retrn );
		Retrn=0;		
		remove( Column.text );	
		return 0;
	}

	strcpy( DynRes.LastFile, Column.text );
	DynRes.Log( "o NWScript: WriteToFile created file: %s\n", Column.text );
	return Retrn;	
}

int __fastcall CExoResMan__Exists( void * pResMan, void*, char * ResRef, unsigned short a1, unsigned long * a2 ){

	if( a1 != 2015 ){
		char resref[17];
		resref[16]=0;
		DynRes.StringCopyToLower( resref, ResRef, 16 );

		sprintf( DynRes.LastFile, "%s.%s", resref, DynRes.GetExtensionFromTypeID( a1 ) );	

		FileEntry * file = DynRes.GetExternalFileToLoad( DynRes.LastFile );
		if( file ){

			unsigned long len = 0;
			void * data = DynRes.LoadFile( file, &len );
			if( !data ){
				if( DynRes.logz >= 1 )DynRes.Log( "! Exists: File not found: %s -> %s\n", file->file, file->fullpath );
				file->LoadTimes = 0xFFFFFFFF;
			}
			else{

				if( DynRes.LastExists )DynRes.mem.nwnx_free( DynRes.LastExists );
				DynRes.LastExists = data;
				if( DynRes.logz >= 1 )DynRes.Log( "o Exists: External: %s -> %s\n", file->file, file->fullpath );				
				file->LoadTimes++;
				a2=(unsigned long*)data;
				return 1;
			}
		}
		else{
			
			if( DynRes.logz >= 2 )DynRes.Log( "o Exists: Internal: %s\n", DynRes.LastFile );
		}
	}
	DynRes.LastFile[0]=0;
	return CExoResMan__ExistsNext( pResMan, NULL, ResRef, a1, a2 );
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

	if( nwRes->bHasContents ){
		DynRes.HasFile=false;
		return CExoResMan__DemandNext( pResMan, NULL, nwRes );
	}

	if( DynRes.HasFile ){

		FileEntry * file = DynRes.GetExternalFileToLoad( DynRes.LastFile );

		if( file ){

			unsigned long len = 0;
			void * data = DynRes.LoadFile( file, &len );
			if( !data ){
				if( DynRes.logz >= 1 )DynRes.Log( "! File not found: %s -> %s\n", file->file, file->fullpath );
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

	bOk = HookCode( (PVOID)0x005D60C0, CCodeBaseInternal__AddBinaryData, (PVOID*)&CCodeBaseInternal__AddBinaryDataNext );
	DynRes.Log( "o Hooked CCodeBaseInternal::AddBinaryData: %s\n", bOk == -1 ? "Success" : "Failure" );

	bOk = HookCode( (PVOID)0x005D5D70, CCodeBaseInternal__GetBinaryData, (PVOID*)&CCodeBaseInternal__GetBinaryDataNext );
	DynRes.Log( "o Hooked CCodeBaseInternal::GetBinaryData: %s\n\n", bOk == -1 ? "Success" : "Failure" );
}