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

#include "NWNXConnect.h"
#include "../NWNXdll/madCHook.h"
#include "../NWNXdll/IniFile.h"

#pragma comment(lib, "madChook.lib")

extern CNWNXConnect nconnect;

int (__fastcall *CNWSMessage__SendServerToPlayerCharListNext)( CNWSMessage * obj, void *, CNWSPlayer * ply );
int __fastcall CNWSMessage__SendServerToPlayerCharList( CNWSMessage * obj, void *, CNWSPlayer * ply ){
	
	CExoString str;
	ply->GetPlayerName( &str );

	if( nconnect.SendhakList( ply ) ){	

		nconnect.Log( "o Successfully Sent HAK list to %s\n", str.CStr() );
	}
	else{

		nconnect.Log( "! Failed to send HAK list to %s\n", str.CStr() );
	}

	return CNWSMessage__SendServerToPlayerCharListNext( obj, NULL, ply );
}




CNWNXConnect::~CNWNXConnect(){OnRelease();}

BOOL CNWNXConnect::OnRelease( ){

	CNWNXMemory mem;

	if( TLK ){

		if( TLK->text )
			mem.nwnx_free( TLK->text );

		mem.nwnx_free( TLK );
	}

	if( HAK ){

		for( int n=0;n<HAKLen;n++ ){

			if( HAK[n].text ){
				mem.nwnx_free( HAK[n].text );
			}	
		}

		mem.nwnx_free( HAK );
	}
	return CNWNXBase::OnRelease();
}

BOOL CNWNXConnect::OnCreate(const char* LogDir){

	char log[MAX_PATH];
	char logrotate[MAX_PATH];

	sprintf( log, "%s\\nwnx_connect.txt", LogDir);
	sprintf( logrotate, "%s\\1\\nwnx_connect.txt", LogDir);

	MoveFile( log, logrotate );

	// call the base class function
	if( !CNWNXBase::OnCreate( log ) )
		return false;

	WriteLogHeader( );

	char buffer[128];
	char number[12];

	CNWNXMemory mem;
	TLK = (CExoString *)mem.nwnx_malloc(sizeof(CExoString));
	CIniFile ini( "NWNX.ini" );

	ini.ReadString( "CONNECT", "TLK", buffer, 128, "" );
	int n=0;

	if( buffer[0]=='\0' ){

		Log( "No TLK file was registered in nwnx.ini!\n" );
		return true;
	}
	else{

		TLK->CExoStringCpy( buffer );
		Log( "TLK: %s\n", TLK->CStr() );
	}

	ini.ReadString( "CONNECT", _itoa( ++n, number, 10 ), buffer, 128, "" ); 
	HAKLen=0;
	void * temp;
	HAK=NULL;

	while( buffer[0]!='\0' ){

		temp = mem.nwnx_malloc( sizeof(CExoString)*(HAKLen+1) );
		if( HAK ){
			memcpy( temp, HAK, sizeof(CExoString)*HAKLen );
			mem.nwnx_free( HAK );
		}
		
		HAK = (CExoString*)temp;
		HAK[HAKLen].CExoStringCpy( buffer );
		Log( "HAK %i: %s\n", HAKLen+1, HAK[HAKLen].CStr() );
		HAKLen++;

		ini.ReadString( "CONNECT", _itoa( ++n, number, 10 ), buffer, 128, "" ); 
	}
	Log( "\n" );
	if( HookCode( (PVOID)0x0043a810, CNWSMessage__SendServerToPlayerCharList, (PVOID*)&CNWSMessage__SendServerToPlayerCharListNext ) )
		Log( "o Hooked SendServerToPlayerCharList!\n" );
	else
		Log( "! Failed to hook SendServerToPlayerCharList!\n" );
	Log( "\n" );
	return true;
}

void CNWNXConnect::WriteLogHeader( ){

	fprintf( m_fFile, "NWNXConnect v0.2 created by Terra_777\n\n" );
	fflush( m_fFile );	
}

void CNWNXConnect::Log( const char * formatting, ... ){

	va_list argList;
	char *pos;

	if( m_fFile ){  

		if( ftell( m_fFile ) > m_maxLogSizeKB ){

			fclose( m_fFile );
			m_fFile = fopen( m_LogFile, "w" );
			WriteLogHeader( );
			fprintf( m_fFile, "o Logfile hit maximum size limit, starting again.\n" );
		}

		// build up the string
		va_start( argList, formatting );
		vsprintf( acBuffer, formatting, argList );
		va_end( argList );

		// replace any percent signs
		pos = strchr( acBuffer, '%' );
		while( pos ){

			*pos = '~';
			pos = strchr( acBuffer, '%' );
		}


		fprintf( m_fFile, acBuffer );
		fflush( m_fFile );
	}
}

/*----------------
--Script interface
----------------*/

unsigned long CNWNXConnect::OnRequestObject (char *gameObject, char* Request){

	Log( "o OnRequestObject: 0x%08X: %s( )", gameObject, Request );

	return OBJECT_INVALID;
}

char* CNWNXConnect::OnRequest(char *gameObject, char* Request, char* Parameters){

	Log( "o OnRequest: 0x%08X: %s( %s )\n", gameObject, Request, Parameters );

	return NULL;
}

/*----------------
--Patch functions
----------------*/


int CNWNXConnect::SendhakList( CNWSPlayer * ply ){

	if( !ply )
		return 0;

	CNWSMessage * msg = (*NWN_AppManager)->app_server->GetNWSMessage();
	CNWNXMemory mem;

	CNWMessage * mess = NWNX_CreateMessage( );

	if( !mess ){

		return 0;
	}

	mess->WriteINT( HAKLen, 32 );
	for( int n=0;n<HAKLen;n++ ){
		mess->WriteCExoString( HAK[n], 32 );
	}

	mess->WriteCExoString( *TLK, 32 );

	char * data=0;
	DWORD len=0;

	if( mess->GetWriteMessage( &data, &len ) ){

		msg->SendServerToPlayerMessage( ply->pl_id, 100, 0, data, len );
		NWNX_DestroyMessage( mess );
		
		return 1;
	}
	else
		NWNX_DestroyMessage( mess );

	return 0;
}