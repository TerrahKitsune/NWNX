/***************************************************************************
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ***************************************************************************/
#define _CRT_SECURE_NO_WARNINGS

#include "NWNXResources.h"
#include <stdio.h>
#include <stdlib.h>

BOOL CNWNXResources::OnCreate(const char* LogDir){

	//Max path
	char log[MAX_PATH];
	
	//Set logfile
	sprintf (log, "%s\\nwnx_resources.txt", LogDir);

	//Call the base class to make the log
	if (!CNWNXBase::OnCreate(log))
		return false;

	Log( 0, "NWNX Resources v0.1 (Written by Terra_777)\n\n" );

	idLookup=NULL;
	extLookup=NULL;
	ResTable=NULL;
	ResTLen=NULL;

	WIN32_FIND_DATA fileData;
	HANDLE hSearch = FindFirstFile( "nwnx_resman.dll", &fileData );
	if (hSearch != INVALID_HANDLE_VALUE){

		Log( 0, "This plugin isnt compatible with nwnx_resman.dll!\n" );
		FindClose( hSearch );
		return false;
	}
	FindClose( hSearch );

	//open the inifile
	CIniFile iniFile ("nwnx.ini");
	
	iniFile.ReadString( "RESOURCEMANAGER", "SourcePath", ResFolder, 260, "\0" );
	priority = iniFile.ReadInteger( "RESOURCEMANAGER", "LogLevel", 1 );
	BaseRes = iniFile.ReadBool( "RESOURCEMANAGER", "BaseOnly", true );
	Behavior = iniFile.ReadInteger( "RESOURCEMANAGER", "Behavior", 0 );

	//Set our nwnx memory funcs
	nwnx_malloc = (void *(__cdecl *)(size_t))0x0040D550;
	//nwns free routine
	nwnx_free = (void (__cdecl *)(void *))0x0040D560;

	extSubFldr=NULL;
	ExtLen=0;

	//Call the function to hook what we want
	HookCNWRES( );

	SetPathRepository( &iniFile );

	return true;
}

char* CNWNXResources::OnRequest(char *gameObject, char* Request, char* Parameters){

	int nBehavior;
	char ResRef[20];
	int nDataLen;
	void * vData = ExtractData( &nBehavior, ResRef, &nDataLen, (unsigned char*)Parameters );

	NWRes * resE = AddToResTable( ResRef, nBehavior ); 

	if( resE == NULL ){

		strcpy( Parameters, "0 " );
		return NULL;
	}

	switch( atoi( Request ) ){

		case 0: break;
		case 1: break;
		case 2: break;
		case 3: break;
		case 4: resE->vPtr = nwnx_malloc( nDataLen+1 ); 
				strcpy( (char*)resE->vPtr, (char*)vData );
				resE->Data = nDataLen;
				break;
		default: break;
	}

	return NULL;
}

unsigned long CNWNXResources::OnRequestObject (char *gameObject, char* Request){

	return OBJECT_INVALID;
}
	
BOOL CNWNXResources::OnRelease(){

	for( int n=0; n<ExtLen; n++ ){

		if( extSubFldr[n].cStr != NULL )
			nwnx_free( extSubFldr[n].cStr );
	}

	nwnx_free( extSubFldr );

	ReallocResTable( 0 );

	//Log and call the base class
	Log( 0,"o Shutting down\n" );
	
	return CNWNXBase::OnRelease();
}

void CNWNXResources::Log( int nDebugLevel, const char *pcMsg, ... ){

	va_list argList;
	char acBuffer[2048];

	if (m_fFile && priority>=debuglevel)
	{  
		// build up the string
		va_start(argList, pcMsg);
		_vsnprintf(acBuffer, 2047, pcMsg, argList);
		acBuffer[2047] = 0;
		va_end(argList);

		// log string in file
		fprintf (m_fFile, "%s", acBuffer);
		fflush (m_fFile);
	}
}

const char * CNWNXResources::GetPathFromType( unsigned short uType ){

	int n=0;
	const char * czReturn = NULL;

	while( extLookup[n].nLen > 0 ){

		if( idLookup[n] == uType ){

			czReturn = extSubFldr[n].cStr;
			if( czReturn != NULL && strcmp( czReturn, "false" ) == 0 ){
				return NULL;
			}
			break;
		}
			
		n++;
	}

	if( czReturn == NULL )
		return ResFolder;

	return czReturn;
}

void CNWNXResources::SetPathRepository( CIniFile * ini ){

	int n=0;

	while( extLookup[n].nLen > 0 ){
		n++;
	}	

	ExtLen=n;

	extSubFldr = (CExoStr*)nwnx_malloc( sizeof( CExoStr )*(ExtLen+1) );

	if( extSubFldr == NULL )
		return;

	memset( extSubFldr, 0, sizeof( CExoStr )*(ExtLen+1) );

	char Get[100];
	char AbsPath[260];
	bool DefaultExt = ini->ReadBool( "RESOURCEMANAGER", "DefaultSubFolders", false ); 

	for( n=0;n<ExtLen;n++ ){

		ini->ReadString( "RESOURCEMANAGER", extLookup[n].cStr, Get, 100, "\0" );
		//Log( 0, "%s: 0x%04X\n", extLookup[n].cStr, idLookup[n] );
		
		if( Get[0] == '\0' && DefaultExt ){

			if( strcmp( Get, "false" ) == 0 ){

				extSubFldr[n].cStr = (char*)nwnx_malloc( strlen(Get)+1 );
				extSubFldr[n].nLen = strlen(Get)+1;
				strcpy( extSubFldr[n].cStr, Get );
			}
			else{

				strcpy( AbsPath, ResFolder );
				strcat( AbsPath, extLookup[n].cStr );
				strcat( AbsPath, "/" );

				extSubFldr[n].cStr = (char*)nwnx_malloc( strlen(AbsPath)+1 );
				extSubFldr[n].nLen = strlen(AbsPath)+1;

				strcpy( extSubFldr[n].cStr, AbsPath );
			}
		}
		else if( Get[0] != '\0' ){

			strcpy( AbsPath, ResFolder );
			strcat( AbsPath, Get );
			strcat( AbsPath, "/" );

			extSubFldr[n].cStr = (char*)nwnx_malloc( strlen(AbsPath)+1 );
			extSubFldr[n].nLen = strlen(AbsPath)+1;

			strcpy( extSubFldr[n].cStr, AbsPath );
		}	
	}
}

bool CNWNXResources::GetTypeAllowed( unsigned short uType ){

	if( BaseRes ){

		switch( uType ){

			case 0x07DA: return true; //ncs
			case 0x07E9: return true; //uti
			case 0x07EB: return true; //utc
			case 0x07F0: return true; //utt
			case 0x07F3: return true; //uts
			case 0x07F8: return true; //ute
			case 0x07FA: return true; //utd
			case 0x07FC: return true; //utp
			case 0x080A: return true; //utw
			default: return false;
		}
	}

	return true;
}

char * CNWNXResources::GetExtensionFromTypeID( unsigned short uType ){

	int n=0;

	while( extLookup[n].nLen > 0 ){

		if( idLookup[n] == uType )
			return extLookup[n].cStr;

		n++;
	}

	return NULL;
}

NWRes * CNWNXResources::AddToResTable( const char * ResRef, unsigned char Behavior ){

	NWRes * ResE = GetBehaviorForRes( ResRef );

	if( ResE != NULL ){

		ResE->Behavior = Behavior;
		ResE->Data = 0;

		if( ResE->vPtr != NULL )
			nwnx_free( ResE->vPtr );

		ResE->vPtr = NULL;

		return ResE;
	}

	int nSlot = ResTLen;
	ReallocResTable( nSlot+1 );

	if( ResTable==NULL ){
		return NULL;
	}

	ResTable[nSlot].Behavior = Behavior;
	ResTable[nSlot].Data = 0;
	ResTable[nSlot].cResRef = (char*)nwnx_malloc( strlen( ResRef )+1 );
	strcpy( ResTable[nSlot].cResRef, ResRef );

	ResE = &ResTable[nSlot];

	Log( 0, "o Added behavior: %s (%u)\n", ResE->cResRef, ResE->Behavior );

	return ResE;
}

void CNWNXResources::ReallocResTable( int nNewSize ){

	if( nNewSize <= 0 ){

		if( ResTable != NULL ){

			for( int n=0; n<ResTLen;n++ ){

				if( ResTable[n].cResRef != NULL )
					nwnx_free( ResTable[n].cResRef );
				if( ResTable[n].vPtr != NULL )
					nwnx_free( ResTable[n].vPtr );
			}
			nwnx_free( ResTable );
		}

		ResTable = NULL;
		ResTLen = 0;
		return;
	}
	else if( nNewSize <= ResTLen ){
		return;
	}

	void * temp = nwnx_malloc( sizeof( NWRes )*nNewSize );

	if( temp == NULL )
		return;

	memset( temp, 0, sizeof( NWRes )*nNewSize );
	memcpy( temp, ResTable, sizeof( NWRes )*ResTLen );
	nwnx_free( ResTable );

	ResTable = (NWRes*)temp;
	ResTLen = nNewSize;
}

NWRes * CNWNXResources::GetBehaviorForRes( const char * ResRef ){

	if( ResTable == NULL )
		return NULL;

	NWRes * ResE = NULL;

	for( int n=0; n<ResTLen;n++ ){

		if( ResTable[n].cResRef != NULL && strcmp( ResTable[n].cResRef, ResRef ) == 0 ){

			ResE = &ResTable[n];
			break;
		}
	}

	if( ResE == NULL )
		return NULL;

	return ResE;
}

void * CNWNXResources::ExtractData( int * Behavior, char * ResRef, int * DataLen, unsigned char * Raw ){

	char number[10]={0};

	char track = 0;
	char cursor = 0;
	int n=0;
	int nSub=0;

	while( true ){

		cursor = Raw[n];

		if( track == 0 ){

			if( cursor == '|' ){
				nSub=0;
				track++;
				*Behavior = atoi( number );
				memset( number, 0, 10 );
			}
			else{

				number[nSub]=cursor;
				nSub++;
			}
		}
		else if( track==1 ){

			if( cursor == '|' ){
				ResRef[nSub]=0;
				nSub=0;
				track++;
			}
			else{

				ResRef[nSub]=cursor;
				nSub++;
			}	
		}
		else if( track==2 ){

			if( cursor == '|' ){
				nSub=0;
				track++;
				*DataLen = atoi( number );
			}
			else{

				number[nSub]=cursor;
				nSub++;
			}	
		}
		else if( track==3 )
			return &Raw[n];

		n++;
	}
}