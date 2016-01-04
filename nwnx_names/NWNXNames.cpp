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

#include "NWNXNames.h"
#include "../NWNXdll/madCHook.h"
#include "../NWNXdll/iniFile.h"

#pragma comment(lib, "madChook.lib")

BOOL CNWNXNames::OnRelease( ){
	
	return CNWNXBase::OnRelease();
}

BOOL CNWNXNames::OnCreate(const char* LogDir){

	char log[MAX_PATH];
	
	sprintf( log, "%s\\nwnx_names.txt", LogDir);

	// call the base class function
	if( !CNWNXBase::OnCreate( log ) )
		return false;

	Array=NULL;
	Len=0;
	Alloc=0;
	LastID = 0;

	TempFirstName=0;
	TempLastName=0;

	WriteLogHeader( );
	ApplyHooks( );
	Log( "\n" );

	return true;
}

void CNWNXNames::WriteLogHeader( ){

	fprintf( m_fFile, "NWNXNames v0.93 created by Terra_777\n\n" );
	fflush( m_fFile );	
}

void CNWNXNames::Log( const char * formatting, ... ){

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

bool CNWNXNames::OverwriteFunction( DWORD pCode, PVOID pCallbackFunc, PVOID *pNextHook ){

	if( HookCode( (PVOID)pCode, pCallbackFunc, pNextHook ) ){

		Log( "o Hook: 0x%08X -> 0x%08X\n", (DWORD)pCode, (DWORD)pCallbackFunc );
		return true;
	}
	else{

		Log( "! Hook nwserver: 0x%08X -> 0x%08X\n", (DWORD)pCode, (DWORD)pCallbackFunc );
	}
	return false;
}

/*----------------
--Script interface
----------------*/

unsigned long CNWNXNames::OnRequestObject (char *gameObject, char* Request){

	return OBJECT_INVALID;
}

char* CNWNXNames::OnRequest(char *gameObject, char* Request, char* Parameters){

	for( int n=0;n<strlen( Parameters );n++ ){

		if( Parameters[n]=='~' ){
			Parameters[n]='\0';
			break;
		}
	}

	if( ( ((CNWSObject*)gameObject)->obj_generic.obj_type2 != CGameObject__OBJECT_TYPE_CREATURE || !((CNWSCreature*)gameObject)->cre_is_pc ) && ((CNWSCreature*)gameObject)->obj.obj_generic.obj_id != 0 ){
		Parameters[0]='\0';
		Log( "! %s( %08X: %s ) Object has to be a playerobject!\n", Request, ((CNWSCreature*)gameObject)->obj.obj_generic.obj_id, Parameters );
		return NULL;
	}

	if( strcmp( Request, "SetName" ) == 0 ){

		Log( "o %s( %08X: %s )\n", Request, ((CNWSCreature*)gameObject)->obj.obj_generic.obj_id, Parameters );
		AddName( ((CNWSObject*)gameObject)->obj_generic.obj_id, Parameters );	
	}
	else if( strcmp( Request, "GetName" ) == 0 ){

		Log( "o %s( %08X: %s )\n", Request, ((CNWSCreature*)gameObject)->obj.obj_generic.obj_id, Parameters );
		GetName( ((CNWSObject*)gameObject)->obj_generic.obj_id, Parameters );
	}
	else if( strcmp( Request, "RemoveName" ) == 0 ){

		Log( "o %s( %08X: %s )\n", Request, ((CNWSCreature*)gameObject)->obj.obj_generic.obj_id, Parameters );
		RemoveName( ((CNWSObject*)gameObject)->obj_generic.obj_id, Parameters );
	}
	else if( strcmp( Request, "SetPermanentName" ) == 0 ){

		Log( "o %s( %08X: %s )\n", Request, ((CNWSCreature*)gameObject)->obj.obj_generic.obj_id, Parameters );
		SetPermanentName( ((CNWSCreature*)gameObject), Parameters );
	}
	else if( strcmp( Request, "GetPermanentName" ) == 0 ){

		Log( "o %s( %08X: %s )\n", Request, ((CNWSCreature*)gameObject)->obj.obj_generic.obj_id, Parameters );
		GetPermanentName( ((CNWSCreature*)gameObject), Parameters );
	}
	else if( strcmp( Request, "UpdateObject" ) == 0 ){

		DWORD objID;
		sscanf( Parameters, "%u", &objID );

		Log( "o %s( %08X: %s )\n", Request, objID, Parameters );
		UpdateObjectForAllPlayers( (CNWSObject*)(*NWN_AppManager)->app_server->GetGameObject( objID ) );
	}
	else if (strcmp(Request, "UpdatePCList") == 0){

		CNWSPlayer * ply = (*NWN_AppManager)->app_server->GetClientObjectByObjectId(((CNWSCreature*)gameObject)->obj.obj_generic.obj_id );
		if (ply){
			Log("o %s( %08X: %s )\n", Request, ply->obj_id, Parameters);
			(*NWN_AppManager)->app_server->GetNWSMessage()->SendServerToPlayerPlayerList_All(ply);
		}
	}
	else if( strcmp( Request, "GetFamilarCompanionName" ) == 0 ){

		DWORD objID;
		sscanf( Parameters, "%u", &objID );

		Log( "o %s( %08X: %s )\n", Request, objID, Parameters );
		GetSetFamilarCompanionName( Parameters, 1 );

	}
	else if( strcmp( Request, "SetFamilarCompanionName" ) == 0 ){

		DWORD objID;
		sscanf( Parameters, "%u", &objID );

		Log( "o %s( %08X: %s )\n", Request, objID, Parameters );
		GetSetFamilarCompanionName( Parameters, 0 );

	}
	else{
		Parameters[0]='\0';
		Log( "! %s( %08X: %s ) No such function!\n", Request, ((CNWSCreature*)gameObject)->obj.obj_generic.obj_id, Parameters );
		return NULL;
	}

	return NULL;
}

void CNWNXNames::GetSetFamilarCompanionName( char * input, int nGet ){

	DWORD objID;
	int nType;

	sscanf( input, "%u %i", &objID, &nType );

	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( objID );

	if( !obj || obj->obj_generic.obj_type2 != CGameObject__OBJECT_TYPE_CREATURE ){

		if( nGet )
			input[0]='\0';
		return;
	}

	CExoString * name;

	if( nType == 1 )
		name = &((CNWSCreature*)obj)->cre_stats->cs_acomp_name;
	else
		name = &((CNWSCreature*)obj)->cre_stats->cs_famil_name;

	if( nGet > 0 ){

		if( name->text )
			strcpy( input, name->text );
		else
			input[0]='\0';
	}
	else{

		int nDatastart = 0;
		int nFind = 0;
		for( int n=0;n<strlen( input );n++ ){

			if( input[n]==' ' && nFind == 1 ){
				nDatastart = n+1;
				break;
			}
			else if( input[n]==' ' ){
				nFind++;
			}
		}

		if( name->text )
			mem.nwnx_free( name->text );
		
		name->len = strlen( &input[nDatastart] );
		name->text = (char*)mem.nwnx_calloc( name->len+1, 1 );
		strncpy( name->text, &input[nDatastart], name->len );
	}
}

void CNWNXNames::SendNameUpdate( DWORD SendToID, DWORD ObjectToUpdateID ){

	CNWSPlayer * pc = (*NWN_AppManager)->app_server->GetClientObjectByObjectId( SendToID );
	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( ObjectToUpdateID );

	if( !pc || !obj || obj->obj_generic.obj_type2 != CGameObject__OBJECT_TYPE_CREATURE )
		return;

	CLastUpdateObject * luo = (CLastUpdateObject *)mem.nwnx_calloc( 1, sizeof( CLastUpdateObject ) );

	char * pData;
	DWORD length;

	CNWSMessage * exoMess = (*NWN_AppManager)->app_server->GetNWSMessage( );
	CNWMessage * nwMess = (CNWMessage *)exoMess;

	SetTemporaryName( (CNWSCreature*)obj, SendToID, false );

	nwMess->CreateWriteMessage( 0x400, pc->pl_id, 1 );
	exoMess->WriteGameObjUpdate_UpdateObject( pc, obj, luo, 0, 0x400 );
	nwMess->GetWriteMessage( &pData, &length ); 

	if( length )
		exoMess->SendServerToPlayerMessage( pc->pl_id, 5, 1, pData, length );

	SetTemporaryName( (CNWSCreature*)obj, SendToID, true );

	mem.nwnx_free( luo );
}

void CNWNXNames::SetTemporaryName( CNWSCreature * cre, nwn_objid_t acmIndex, bool restore ){

	CExoString * FirstName = cre->cre_stats->cs_firstname.GetFirstEntry();
	CExoString * LastName = cre->cre_stats->cs_firstname.GetFirstEntry();

	if( restore ){

		if( FirstName->text )
			mem.nwnx_free( FirstName->text );

		if( TempFirstName ){
			FirstName->len = TempFirstNameLen;
			FirstName->text = TempFirstName;
		}

		if( TempLastName ){
			LastName->text = TempLastName;
			LastName->len = TempLastNameLen;
		}

		TempFirstName = 0;
		TempFirstNameLen = 0;
		TempLastName = 0;
		TempLastNameLen = 0;
	}
	else{

		DynamicNames * DisplayName = Get( acmIndex );
		if( !DisplayName )return; 

		Name * Dynname = DisplayName->Get( cre->obj.obj_generic.obj_id, false );
		if( !Dynname )return;

		TempFirstName = FirstName->text;
		TempFirstNameLen = FirstName->len;

		TempLastName = LastName->text;
		TempLastNameLen = LastName->len;

		FirstName->text = (char*)mem.nwnx_calloc( Dynname->len+1, 1 ); 
		FirstName->len = Dynname->len;
		strncpy( FirstName->text, Dynname->text, Dynname->len );
	
		LastName->len = 0;
		LastName->text = NULL;

	}
}

void CNWNXNames::AddName( DWORD objID, char * input ){
	
	DynamicNames * DynName = Add( objID );
	DWORD ID;

	if( DynName == NULL ){
		strcpy( input, "0" );
		return;
	}

	int nDatastart=0;

	sscanf( input, "%x", &ID );

	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( ID );

	if( !obj || obj->obj_generic.obj_type2 != CGameObject__OBJECT_TYPE_CREATURE ){
		strcpy( input, "0" );
		return;
	}
	
	for( int n=0;n<strlen( input );n++ ){

		if( input[n]==' ' ){
			nDatastart = n+1;
			break;
		}
	}

	if( DynName->Add( ID, &input[nDatastart] ) ){
		strcpy( input, "1" );
	}
	else{
		strcpy( input, "0" );
		return;
	}

	if( objID == 0 ){

		nwn_objid_t oPC = (*NWN_AppManager)->app_server->GetFirstPCObject();
		while( oPC != OBJECT_INVALID ){

			SendNameUpdate( oPC, ID );

			oPC = (*NWN_AppManager)->app_server->GetNextPCObject();
		}
	}
	else{

		SendNameUpdate( objID, ID );	
	}
}

void CNWNXNames::GetName( DWORD objID, char * input ){

	DynamicNames * DynName = Get( objID );
	DWORD ID;

	if( DynName == NULL ){
		strcpy( input, "" );
		return;
	}

	sscanf( input, "%x", &ID );


	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( ID );

	if( !obj || obj->obj_generic.obj_type2 != CGameObject__OBJECT_TYPE_CREATURE ){
		strcpy( input, "" );
		return;
	}

	Name * TheName = DynName->Get( ID, false );

	if( TheName == NULL ){
		strcpy( input, "" );
		return;
	}

	strcpy( input, TheName->text );
}

void CNWNXNames::RemoveName( DWORD objID, char * input ){

	DynamicNames * DynName = Get( objID );
	DWORD ID;

	if( DynName == NULL ){
		strcpy( input, "0" );
		return;
	}

	sscanf( input, "%x", &ID );

	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( ID );

	if( !obj || obj->obj_generic.obj_type2 != CGameObject__OBJECT_TYPE_CREATURE ){
		strcpy( input, "0" );
		return;
	}
	
	Name * TheName = DynName->Get( ID, false );

	if( TheName && DynName->Remove( ID ) ){

		strcpy( input, "1" );

		if( objID == 0 ){

			nwn_objid_t oPC = (*NWN_AppManager)->app_server->GetFirstPCObject();
			while( oPC != OBJECT_INVALID ){

				SendNameUpdate( oPC, ID );
				oPC = (*NWN_AppManager)->app_server->GetNextPCObject();
			}
		}
		else{

			SendNameUpdate( objID, ID );	
		}
	}
	else{
		strcpy( input, "0" );
	}
}

void CNWNXNames::SetPermanentName( CNWSCreature * cre, char * input ){

	if( cre->obj.obj_generic.obj_type2 != CGameObject__OBJECT_TYPE_CREATURE ){

		strcpy( input, "0" );
		return;
	}

	DWORD Type;
	int nDatastart=0;

	sscanf( input, "%u", &Type );

	for( int n=0;n<strlen( input );n++ ){

		if( input[n]==' ' ){
			nDatastart = n+1;
			break;
		}
	}

	CExoString * name;

	if( Type == 1 ){

		name = cre->cre_stats->cs_lastname.GetFirstEntry( );
	}
	else{

		name = cre->cre_stats->cs_firstname.GetFirstEntry( );
	}

	if( !name ){

		strcpy( input, "0" );
		return;
	}

	if( name->text )
		mem.nwnx_free( name->text );

	name->len = strlen( &input[nDatastart] );
	name->text = (char*)mem.nwnx_calloc( name->len+1, 1 );

	strncpy( name->text, &input[nDatastart], name->len );

	UpdateObjectForAllPlayers( &cre->obj );

	strcpy( input, "1" );
}

void CNWNXNames::GetPermanentName( CNWSCreature * cre, char * input ){
	
	if( cre->obj.obj_generic.obj_type2 != CGameObject__OBJECT_TYPE_CREATURE ){

		input[0]='\0';
		return;
	}

	DWORD type;

	sscanf( input, "%u", &type );

	CExoString * name;

	if( type == 1 )
		name = cre->cre_stats->cs_lastname.GetFirstEntry( );
	else
		name = cre->cre_stats->cs_firstname.GetFirstEntry( );

	if( !name || !name->text )
		input[0]='\0';
	else
		strcpy( input, name->text );

}

void CNWNXNames::UpdateObjectForAllPlayers( CNWSObject * obj ){

	if( !obj || obj->obj_generic.obj_type2 != CGameObject__OBJECT_TYPE_CREATURE )return;

	nwn_objid_t oPC = (*NWN_AppManager)->app_server->GetFirstPCObject( );
	while( oPC != OBJECT_INVALID ){

		SendNameUpdate( oPC, obj->obj_generic.obj_id );
		
		oPC = (*NWN_AppManager)->app_server->GetNextPCObject( ); 
	}
}

//NamesInterface

DynamicNames * CNWNXNames::Resize( unsigned int newSize ){

	CNWNXMemory mem;

	if( Alloc == 0 ){

		Array = (DynamicNames*)mem.nwnx_calloc( newSize, sizeof( DynamicNames ) );
		if( Array != NULL )
			Alloc=newSize;
		Len = 0;
		return (DynamicNames*)Array;
	}
	else if( newSize == 0 ){

		for( int n=0;n<Len;n++ ){

			Array[n].Resize( 0 );				
		}

		Alloc = 0;
		Len = 0;
		mem.nwnx_free( Array );
		Array = NULL;

		return NULL;
	}
	else if( newSize <= Alloc )
		return Array;

	DynamicNames * Temp = (DynamicNames*)mem.nwnx_calloc( newSize, sizeof( DynamicNames ) );

	if( Temp == NULL )
		return NULL;
	
	if( Len > 0 )
		memcpy( Temp, Array, Len*sizeof( DynamicNames ) );
	
	mem.nwnx_free( Array );

	Array = Temp;
	Alloc = newSize;

	return Array;
}

DynamicNames * CNWNXNames::Add( DWORD ObjID ){

	CNWNXMemory mem;

	DynamicNames * test = Get( ObjID );

	if( test ){

		
		test->Resize( 0 );
		test->ObjID = ObjID;
		return test;
	}

	if( Len == Alloc ){
		if( Resize( Alloc+1 ) == NULL ){
			return NULL;
		}
	}
		
	Array[Len].Construct();
	Array[Len].ObjID = ObjID;
	Len++;

	return &Array[Len-1];
}

bool CNWNXNames::Remove( DWORD ObjID ){

	int nIndex = -1;

	for( int n=0;n<Len;n++ ){

		if( Array[n].ObjID == ObjID ){

			nIndex = n;
			break;
		}
	}

	if( nIndex == -1 )
		return false;

	CNWNXMemory mem;

	Array[nIndex].Resize( 0 );

	if( nIndex == (Len-1) ){
		memset( &Array[nIndex], 0, sizeof( DynamicNames ) );
	}
	else{
		memcpy( &Array[nIndex], &Array[nIndex+1], ((Len-1)-nIndex)*sizeof( DynamicNames ) );
		memset( &Array[Len-1], 0, sizeof( DynamicNames ) );
	}

	Len--;

	return true;
}

DynamicNames * CNWNXNames::Get( DWORD ObjID ){

	int nIndex = -1;

	for( int n=0;n<Len;n++ ){

		if( Array[n].ObjID == ObjID ){

			nIndex = n;
			break;
		}
	}
	
	if( nIndex == -1 )
		return NULL;

	return &Array[nIndex];
}

