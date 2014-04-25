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

#include "NWNXMessages.h"
#include "../NWNXdll/madCHook.h"
#include "../NWNXdll/IniFile.h"

#pragma comment(lib, "madChook.lib")

extern CNWNXMessages cMessages;

int (__thiscall * CNWSMessage__SendServerToPlayerChat_Party)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043D790;
int (__thiscall * CNWSMessage__SendServerToPlayerChat_Talk)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043D9A0;
int (__thiscall * CNWSMessage__SendServerToPlayerChat_Tell)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043DC90;
int (__thiscall * CNWSMessage__SendServerToPlayerChat_Shout)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043DF80;
int (__thiscall * CNWSMessage__SendServerToPlayerChat_DM_Silent_Shout)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043E5C0;
int (__thiscall * CNWSMessage__SendServerToPlayerChat_Whisper)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043E4E0;
//int (__thiscall * CNWSMessage__SendServerToPlayerCCMessage)( CNWSMessage * mess, DWORD PlyID, unsigned char channel, CNWCCMessageData * ccmess, CNWSCombatAttackData * CAD ) = (int (__thiscall * )( CNWSMessage * , DWORD , unsigned char, CNWCCMessageData * , CNWSCombatAttackData * ))0x0044B630;


int (__fastcall *CNWSMessage__SendServerToPlayerCharListNext)( CNWSMessage * obj, void *, CNWSPlayer * ply );
int __fastcall CNWSMessage__SendServerToPlayerCharList( CNWSMessage * obj, void *, CNWSPlayer * ply ){
	
	CExoString str;
	ply->GetPlayerName( &str );

	if( cMessages.SendhakList( ply ) ){	

		cMessages.Log( "o Successfully Sent HAK list to %s\n", str.CStr() );
	}
	else{

		cMessages.Log( "! Failed to send HAK list to %s\n", str.CStr() );
	}

	return CNWSMessage__SendServerToPlayerCharListNext( obj, NULL, ply );
}




CNWNXMessages::~CNWNXMessages(){OnRelease();}

BOOL CNWNXMessages::OnRelease( ){

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

	if( LASTSTR->text )mem.nwnx_free( LASTSTR->text );
	mem.nwnx_free( LASTSTR );

	delete []SS;

	return CNWNXBase::OnRelease();
}

BOOL CNWNXMessages::OnCreate(const char* LogDir){

	char log[MAX_PATH];
	char logrotate[MAX_PATH];

	sprintf( log, "%s\\nwnx_messages.txt", LogDir);
	sprintf( logrotate, "%s\\1\\nwnx_messages.txt", LogDir);

	MoveFile( log, logrotate );

	// call the base class function
	if( !CNWNXBase::OnCreate( log ) )
		return false;

	WriteLogHeader( );
	CIniFile ini( "NWNX.ini" );

	SS=NULL;
	SSLen=0;

	char lpath[MAX_PATH];

	ini.ReadString( "MESSAGES", "Languages", lpath, MAX_PATH-25, "%" );

	if( lpath[0] != '%' ){
		int loaded = LoadLanguages( lpath );	
		if( loaded <= 0 ){
			SS = new ScrambleSet[1];
			SSLen=1;
		}
		else
			Log( "Loaded: %i languages\n\n", loaded-1 );
	}
	else{

		SS = new ScrambleSet[1];
		SSLen=1;
		Log( "No languages where loaded!\n\n" );
	}

	char buffer[128];
	char number[12];

	CNWNXMemory mem;
	TLK = (CExoString *)mem.nwnx_malloc(sizeof(CExoString));
	LASTSTR=(CExoString *)mem.nwnx_malloc(sizeof(CExoString));
	
	TLK->text=NULL;
	LASTSTR->text=NULL;

	LOGLEVEL=ini.ReadInteger( "MESSAGES", "log", 0 );
	ini.ReadString( "MESSAGES", "TLK", buffer, 128, "" );
	int n=0;

	if( buffer[0]=='\0' ){

		Log( "No TLK file was registered in nwnx.ini! Sending Haklist disabled!\n" );
	}
	else{

		TLK->CExoStringCpy( buffer );
		Log( "TLK: %s\n", TLK->CStr() );

		ini.ReadString( "MESSAGES", _itoa( ++n, number, 10 ), buffer, 128, "" ); 
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
			HAK[HAKLen].text=NULL;
			HAK[HAKLen].CExoStringCpy( buffer );
			Log( "HAK %i: %s\n", HAKLen+1, HAK[HAKLen].CStr() );
			HAKLen++;

			ini.ReadString( "MESSAGES", _itoa( ++n, number, 10 ), buffer, 128, "" ); 
		}
		Log( "\n" );
		if( HookCode( (PVOID)0x0043a810, CNWSMessage__SendServerToPlayerCharList, (PVOID*)&CNWSMessage__SendServerToPlayerCharListNext ) )
			Log( "o Hooked SendServerToPlayerCharList!\n" );
		else
			Log( "! Failed to hook SendServerToPlayerCharList!\n" );
		Log( "\n" );
	}
	
	return true;
}

void CNWNXMessages::WriteLogHeader( ){

	fprintf( m_fFile, "NWNXMessages v0.62 created by Terra_777\n\n" );
	fflush( m_fFile );	
}

void CNWNXMessages::Log( const char * formatting, ... ){

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

unsigned long CNWNXMessages::OnRequestObject (char *gameObject, char* Request){

	Log( "o OnRequestObject: 0x%08X: %s( )", gameObject, Request );

	return OBJECT_INVALID;
}

char* CNWNXMessages::OnRequest(char *gameObject, char* Request, char* Parameters){

	switch(atoi(Request)){
		
		case 1: sprintf( Parameters, "%i", SendMessageToPlayer( (CNWSObject *)gameObject, Parameters, 1 ) ); break;
		case 2: sprintf( Parameters, "%i", SendMessageToPlayer( (CNWSObject *)gameObject, Parameters, 2 ) ); break;
		case 3: sprintf( Parameters, "%i", SendMessageToPlayer( (CNWSObject *)gameObject, Parameters, 3 ) ); break;
		case 4: sprintf( Parameters, "%i", SendMessageToPlayer( (CNWSObject *)gameObject, Parameters, 4 ) ); break;
		case 5: sprintf( Parameters, "%i", SendMessageToPlayer( (CNWSObject *)gameObject, Parameters, 5 ) ); break;
		case 6: sprintf( Parameters, "%i", SendMessageToPlayer( (CNWSObject *)gameObject, Parameters, 6 ) ); break;
		case 7: ScrambleMessage( Parameters ); break;
		case 8: GetLanguageID( Parameters ); break;
		case 9: GetLanguageLevel( Parameters ); break;
		case 10: GetLanguageName( Parameters ); break;
		case 11: GetLanguageIDInIndex( Parameters ); break;
		case 12: Parameters[0]='\"'; Parameters[1]='\0'; break;
		default:break;
	}

	return NULL;
}

/*----------------
--Patch functions
----------------*/

int CNWNXMessages::GetLanguageIDInIndex( char * text ){

	int n = atoi( text )+1;

	if( (unsigned)n >= SSLen || n < 1 ){

		strcpy( text, "-1" ); 
		return 0;
	}
	else
		sprintf( text, "%i", SS[n].ID ); 
	
	return 1;
}

int CNWNXMessages::LoadLanguages( const char * path ){

	char fullpath[MAX_PATH];
	unsigned int nCnt = 1;

	strcpy( fullpath, path );
	strcat( fullpath, "*.txt" );

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile( fullpath, &FindFileData );

	if(hFind == INVALID_HANDLE_VALUE ){
		
		Log( "LoadLanguages: Folder is empty or doesnt exist: %s\n", path );
		return 0;
	}

	do{
		if( strstr( FindFileData.cFileName, ".txt" ) )
			nCnt++;
	}while( FindNextFile( hFind, &FindFileData ) != 0 );

	//Do for real

	SS = new ScrambleSet[nCnt];
	if( !SS ){

		Log( "LoadLanguages: Failed to allocate enough memory!\n" );
		return 0;
	}

	strcpy( fullpath, path );
	strcat( fullpath, "*.txt" );
	int nFind = 1;
	hFind = FindFirstFile( fullpath, &FindFileData );

	if(hFind == INVALID_HANDLE_VALUE ){
		
		Log( "LoadLanguages: Folder is empty or doesnt exist: %s\n", path );
		return 0;
	}

	do{
	
		sprintf( fullpath, "%s%s", path, FindFileData.cFileName );
		if( SS[nFind].OpenFromFile( fullpath, 1 ) ){
			Log( "LoadLanguages: Loaded ID: %i Level: %i Language %s: %s\n", SS[nFind].ID, SS[nFind].LEVEL, SS[nFind].Language, fullpath );
			nFind++;
		}
		else{
			Log( "LoadLanguages: Unable to load %s\n", fullpath );
		}


	}while( FindNextFile( hFind, &FindFileData ) != 0 );

	this->SSLen = nCnt;
	return nCnt;
}

int CNWNXMessages::SendhakList( CNWSPlayer * ply ){

	if( !ply )
		return 0;

	CNWSMessage * msg = (*NWN_AppManager)->app_server->GetNWSMessage();
	CNWNXMemory mem;

	CNWMessage * mess = NWNX_CreateMessage( sizeof(int) );

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

int CNWNXMessages::SendMessageToPlayer( CNWSObject * ply, char * text, int nType ){

	CNWSMessage * mess = (*NWN_AppManager)->app_server->GetNWSMessage();
	CNWSPlayer * cli = (*NWN_AppManager)->app_server->GetClientObjectByObjectId( ply->obj_generic.obj_id );
	
	if( !cli )
		return 0;

	DWORD nLen = strlen( text );
	DWORD ID=OBJECT_INVALID;
	
	for( DWORD n=0;n<nLen; n++ ){
		
		if( text[n]==' '){
			
			text[n]='\0';

			LASTSTR->CExoStringCpy( &text[n+1] );

			sscanf( text, "%x", &ID );
			break;
		}
	}

	if( ID == OBJECT_INVALID ){
	
		return 0;
	}	

	int nRet;

	if( nType == 1 ){
		nRet = CNWSMessage__SendServerToPlayerChat_Talk( mess, cli->pl_id, ID, *LASTSTR );
	}
	else if( nType == 2 ){
		nRet = CNWSMessage__SendServerToPlayerChat_Whisper( mess, cli->pl_id, ID, *LASTSTR ); 
	}
	else if( nType == 3 ){
		nRet = CNWSMessage__SendServerToPlayerChat_Party( mess, cli->pl_id, ID, *LASTSTR ); 
	}
	else if( nType == 4 ){
		nRet = CNWSMessage__SendServerToPlayerChat_Tell( mess, cli->pl_id, ID, *LASTSTR ); 
	}
	else if( nType == 5 ){
		nRet = CNWSMessage__SendServerToPlayerChat_Shout( mess, cli->pl_id, ID, *LASTSTR ); 
	}
	else if( nType == 6 ){
		nRet = CNWSMessage__SendServerToPlayerChat_DM_Silent_Shout( mess, cli->pl_id, ID, *LASTSTR ); 
	}
	else{
		nRet=0;
	}

	if( nRet > 0 && LOGLEVEL>0 )
		Log( "o SendMessage (%i): %08X -> %08X: (%u) %s\n", nType, ID, ply->obj_generic.obj_id, LASTSTR->len, LASTSTR->text );

	return nRet;
}

int CNWNXMessages::SendCCMessage( CNWSObject * ply, char * text ){

	CNWSMessage * mess = (*NWN_AppManager)->app_server->GetNWSMessage();
	CNWSPlayer * cli = (*NWN_AppManager)->app_server->GetClientObjectByObjectId( ply->obj_generic.obj_id );

	if( !cli )
		return 0;

	CNWCCMessageData * ccmess = (CNWCCMessageData *)mem.nwnx_malloc(sizeof(CNWCCMessageData));
	if( !ccmess )
		return 0;
	memset( ccmess, 0, sizeof(CNWCCMessageData) ); 

	DWORD chan;
	char * temp = strstr( text, "|" );
	*temp = '\0';
	temp++;

	sscanf( text, "%u", &chan );
	LASTSTR->CExoStringCpy( temp );
	ccmess->SetString( 0, *LASTSTR ); 
	//int nRet = CNWSMessage__SendServerToPlayerCCMessage( mess, cli->pl_id, (unsigned char)chan, ccmess, NULL );
	int nRet = mess->SendServerToPlayerCCMessage( cli->pl_id, (unsigned char)chan, ccmess, NULL );

	Log( "o SendCCMessage (%u): %08X: (%u) %s\n", chan, ply->obj_generic.obj_id, LASTSTR->len, LASTSTR->text );
	mem.nwnx_free( ccmess );
	return nRet;
}

int CNWNXMessages::ScrambleMessage( char * text ){

	DWORD max = strlen( text );
	char * temp;
	char * p1;
	char * p2;
	char * p3;
	char * p4;
	char * p5;
	char * p6;

	temp = strstr( text, "|" );
	
	p1 = text;
	*temp = '\0';
	p2 = temp+1;

	temp = strstr( temp+1, "|" );

	p3 = temp+1;
	*temp = '\0';

	temp = strstr( temp+1, "|" );

	p4 = temp+1;
	*temp = '\0';

	temp = strstr( temp+1, "|" );

	p5 = temp+1;
	*temp = '\0';

	temp = strstr( temp+1, "|" );

	p6 = temp+1;
	*temp = '\0';

	temp = strstr( temp+1, "|" );
	char * ender=NULL;
	while( temp ){

		ender = temp;
		temp = strstr( ender+1, "|" );
	}

	if( ender )
		*ender = '\0';

	ScrambleSet * Set = NULL;

	if( p3[0] != '\0' ){

		int findID = atoi( p3 );

		for( unsigned int n=0;n<this->SSLen;n++ ){

			if( SS[n].ID == findID ){

				Set = &SS[n];
				break;
			}
		}

		if( !Set )
			Set = &SS[0];
	}
	else
		Set = &SS[0];
	

	int nReverse = atoi( p1 );
	char esc[3]={0};
	char * message = new char[strlen(p6)+1];
	char head[10]={0};
	char tail[10]={0};

	strncpy( head, p4, 9 );
	strncpy( tail, p5, 9 );
	strncpy( esc, p2, 2 );
	strcpy( message, p6 );
	strcpy( text, message );

	delete message;

	if( esc[0] == '\'' )
		esc[0] = '\"';

	//Log( "o Scramble: %s %i %s %s %s\n", esc, nReverse > 0, head, tail, text );

	Set->ScrambleText( text, max, esc, nReverse > 0, head, tail );

	return 1;
}

int CNWNXMessages::GetLanguageID( char * text ){

	DWORD len = strlen( text );
	for( DWORD n=0;n<len;n++ ){
		if( isupper( text[n] ) )
			text[n]=tolower(text[n]);
	}

	for( DWORD n=0;n<SSLen;n++ ){

		if( SS[n].Language && strcmp( SS[n].Language, text ) == 0 ){

			sprintf( text, "%i", SS[n].ID );
			return 1;
		}
	}

	sprintf( text, "%i", -1 );
	return 0;
}

int CNWNXMessages::GetLanguageLevel( char * text ){

	int nID = atoi( text );

	for( DWORD n=0;n<SSLen;n++ ){

		if( SS[n].ID == nID ){

			sprintf( text, "%i", SS[n].LEVEL );
			return 1;
		}
	}

	sprintf( text, "%i", -1 );
	return 0;
}

int CNWNXMessages::GetLanguageName( char * text ){

	int nID = atoi( text );

	for( DWORD n=0;n<SSLen;n++ ){

		if( SS[n].ID == nID ){

			if( SS[n].Language ){

				SS[n].Language[0]=toupper(SS[n].Language[0]);
				sprintf( text, "%s", SS[n].Language );
				SS[n].Language[0]=tolower(SS[n].Language[0]);
			}
			else
				sprintf( text, "%s", "Unknown" );

			return 1;
		}
	}

	text[0]='\0';
	return 0;
}