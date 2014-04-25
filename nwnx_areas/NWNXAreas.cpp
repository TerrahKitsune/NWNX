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

#include "NWNXAreas.h"
#include "../NWNXdll/madCHook.h"
#include <vector>

#pragma comment(lib, "madChook.lib")


CNWNXAreas::~CNWNXAreas(){OnRelease( );};

BOOL CNWNXAreas::OnRelease( ){

	return CNWNXBase::OnRelease();
}

BOOL CNWNXAreas::OnCreate(const char* LogDir){

	char log[MAX_PATH];
	
	sprintf( log, "%s\\nwnx_areas.txt", LogDir);

	// call the base class function
	if( !CNWNXBase::OnCreate( log ) )
		return false;

	WriteLogHeader( );	
	test=NULL;

	return true;
}

void CNWNXAreas::WriteLogHeader( ){

	fprintf( m_fFile, "NWNXAreas v1.0 created by Terra_777\nBased on previous versions by Virusman and Maxrock\n\n" );
	fflush( m_fFile );	
}

void CNWNXAreas::Log( const char * formatting, ... ){

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

unsigned long CNWNXAreas::OnRequestObject (char *gameObject, char* Request){

	if( strcmp( Request, "GET_LAST_AREA_ID" ) == 0 )
		return Last;

	return OBJECT_INVALID;
}

char* CNWNXAreas::OnRequest(char *gameObject, char* Request, char* Parameters){

	//Log( "o Request: %s %s\n", Request, Parameters );
	if( strcmp( Request, "GET_AREA" ) == 0 ){
		
		CNWSModule * mod = (*NWN_AppManager)->app_server->srv_internal->GetModule( );
		DWORD id = 0;
		sscanf( Parameters, "%u", &id );
		if( id >= mod->mod_areas_len )
			Last = OBJECT_INVALID;
		else
			Last = mod->mod_areas[id];

	}
	else if( strcmp( Request, "GET_HEIGHT" ) == 0 ){
		sprintf( Parameters, "%f", GetHeight( ((CNWSObject*)gameObject)->AsNWSArea(), Parameters ) );	
	}
	else if( strcmp( Request, "GET_PLAY" ) == 0 ){

		sprintf( Parameters, "%d", ((CNWSObject*)gameObject)->AsNWSArea()->are_numplayers );
	}
	else if( strcmp( Request, "CREATE_AREA" ) == 0 ){
		Last = CreateArea( Parameters );
	}
	else if (strncmp(Request, "DESTROY_AREA", 12) == 0){

		nwn_objid_t id=0;
		sscanf( Parameters, "%x", &id );

		sprintf( Parameters, "%i", DestroyArea( id ) );
		return NULL;
	}
	else if (strncmp(Request, "SET_AREA_NAME", 13) == 0){
		RenameArea( (CNWSArea*)gameObject, Parameters );
		return NULL;
	}

	return NULL;
}

/*----------------
--Patch functions
----------------*/

long double CNWNXAreas::GetHeight( CNWSArea * are, char * str ){

	Vector v;

	sscanf( str, "%f %f %f", &v.x, &v.y, &v.z );

	Log( "o ComputeHeight: %f %f %f\n", v.x,v.y,v.z );

	return are->ComputeHeight( v );
}

void CNWNXAreas::RenameArea( CNWSArea * are, char * str ){

	if( !are )
		return;

	CExoLinkedList *NameList = are->area_name.List;
	if (NameList) {
		CExoLocStringNode *NameNode = (CExoLocStringNode*)NameList->ListHeader->FirstElement->Data;
			
		if( NameNode->Text.text ){
			Log( "o Renaming area: %s -> %s\n", NameNode->Text.text, str );
			mem.nwnx_free( NameNode->Text.text );
		}
		NameNode->Text.text = (char*)mem.nwnx_malloc( strlen( str )+1 );
		strcpy( NameNode->Text.text, str ); 
	
		CNWSMessage *pMessage = ((*NWN_AppManager)->app_server)->GetNWSMessage();
		CExoLinkedList *pPlayerList = (CExoLinkedList*)((*NWN_AppManager)->app_server)->GetPlayerList();
		CExoLinkedListElement *pElement = pPlayerList->GetHeadPos();
		while (pElement) {
			CNWSPlayer *pPlayer = (CNWSPlayer*)pPlayerList->GetAtPos(pElement);
			if (pPlayer) pMessage->SendServerToPlayerDungeonMasterAreaList(pPlayer);
			pElement = pPlayerList->GetNext(pElement);
		}
	}
}

nwn_objid_t CNWNXAreas::CreateArea( const char * resref ){

	CNWSModule * mod = (*NWN_AppManager)->app_server->srv_internal->GetModule( );

	CResRef ref;

	memset( ref.resref, 0, 16 );
	strncpy( ref.resref, resref, 16 );

	CNWSArea * newArea = (CNWSArea *)mem.nwnx_malloc( 0x20C );

	if( !newArea )
		return OBJECT_INVALID;

	newArea->CNWSArea( ref, 0 , OBJECT_INVALID );
	
	if( !newArea )
		return OBJECT_INVALID;

	if( !newArea->LoadArea( 0 ) ){

		Log( "! Created area: unable to create area %s\n", resref );
		newArea->scalar_Destructor( 1 );
		return OBJECT_INVALID;
	}

	nwn_objid_t AreaID = newArea->obj_id;

	//CExoArrayList_uint32___Add( mod->mod_areas, NULL, AreaID );
	nwn_objid_t * temp = (nwn_objid_t*)mem.nwnx_malloc( (mod->mod_areas_len+1)*sizeof( nwn_objid_t ) );
	if( !temp ){
		newArea->scalar_Destructor( 1 );
		return OBJECT_INVALID;
	}

	memcpy( temp, mod->mod_areas, mod->mod_areas_len*sizeof( nwn_objid_t ) );
	mem.nwnx_free( mod->mod_areas );
	mod->mod_areas=temp;
	mod->mod_areas[mod->mod_areas_len]=AreaID;
	mod->mod_areas_len++;
	mod->mod_areas_alloc = mod->mod_areas_len;

	newArea->UpdatePlayerAutomaps( );

	//Add minimaps
	CGameObjectArray *pGameObjArray = ((*NWN_AppManager)->app_server)->GetObjectArray();

	if( pGameObjArray ){
		for(int i=0; i<=0xFFF; i++)	{
			CGameObjectArrayElement **pObjects = pGameObjArray->Objects;
			CGameObjectArrayElement *pElement = pObjects[i];
			if(!pElement) continue;
			CNWSCreature *pObject = (CNWSCreature *) pElement->Object;
			if(!pObject) continue;
			if(pObject->obj.obj_generic.obj_type2 == 5) {
				if(pObject->AreaData.Areas && pObject->AreaData.Areas) {				
					pObject->AreaData.AddArea( AreaID );
					//Log( "Added minimaps: %08X\n", pObject->obj.obj_generic.obj_id );
				}
			}
		}
	}

	//End minimaps

	//Update TURRRRDS
	if( mod->mod_PlayerTURDList.ListHeader && mod->mod_PlayerTURDList.ListHeader > 0 ){

		CExoLinkedListHeader * Header = mod->mod_PlayerTURDList.ListHeader;
		CExoLinkedListElement *Element = Header->FirstElement;

		CNWSPlayerTURD * TURD;
		while( Element ){
			
			TURD = (CNWSPlayerTURD*)Element->Data;

			if( TURD->AreaData.Areas && TURD->AreaData.MiniMap ){
				TURD->AreaData.AddArea( AreaID );
			}
			Element = Element->NextElement;
		}
	}

	//Send updates to DMs
	CNWSMessage *pMessage = ((*NWN_AppManager)->app_server)->GetNWSMessage();
	CExoLinkedList *pPlayerList = (CExoLinkedList*)((*NWN_AppManager)->app_server)->GetPlayerList();
	CExoLinkedListElement *pElement = pPlayerList->GetHeadPos();
	while (pElement) {
		CNWSPlayer *pPlayer = (CNWSPlayer*)pPlayerList->GetAtPos(pElement);
		if (pPlayer) pMessage->SendServerToPlayerDungeonMasterAreaList(pPlayer);
		pElement = pPlayerList->GetNext(pElement);
	}

	//Debug
	Log( "o Created area: %08X (%u)\n", AreaID, mod->mod_areas_len );

	return AreaID;
}

int CNWNXAreas::DestroyArea( nwn_objid_t nAreaID ){

	if( !nAreaID || nAreaID == OBJECT_INVALID)
		return 0;
	
	CNWSModule * mod = (*NWN_AppManager)->app_server->srv_internal->GetModule( );
	CNWSArea * Area = (CNWSArea *)mod->GetArea( nAreaID );
	if( !Area || Area->obj_id == OBJECT_INVALID || Area->are_numplayers > 0 ){

		Log( "! Destroy area: area invalid or contains players!\n" );
		return 0;
	}

	int nIndex = -1;
	for( unsigned int n=0;n<mod->mod_areas_len;n++ ){

		if( mod->mod_areas[n]==nAreaID ){

			nIndex=n;
			break;
		}
	}

	if( nIndex==-1 )
		return 0;

	CGameObjectArray *objArray = ((*NWN_AppManager)->app_server)->GetObjectArray();
	nwn_objid_t nTmpObj;
	std::vector<uint32_t> Objects;

	int bContinue=0;
	bContinue = Area->GetFirstObjectInArea(&nTmpObj);
	do{
		Objects.push_back(nTmpObj);
	} while (Area->GetNextObjectInArea(&nTmpObj));

	CGenericObject *pObject;

	while (!Objects.empty()) {
		nTmpObj = Objects.back();
		Objects.pop_back();
		pObject = (CGenericObject*)((*NWN_AppManager)->app_server->srv_internal)->GetGameObject(nTmpObj);
		if (!pObject) continue;
	
		if (pObject->obj_type2 == 16){

			if ((mod->mod_PlayerTURDList.ListHeader) && (mod->mod_PlayerTURDList.ListHeader->Count > 0)) {
				CNWSPlayerTURD *ObjTURD= (CNWSPlayerTURD*)pObject;
				CExoLinkedListHeader *Header = mod->mod_PlayerTURDList.ListHeader;
				CExoLinkedListElement *Element = Header->FirstElement;
				CNWSPlayerTURD *TURD = NULL;
				while (Element) {
					TURD = (CNWSPlayerTURD*)Element->Data;
					if (TURD->TURD_id == ObjTURD->TURD_id ) {
						
						if( Element->PrevoiusElement )
							Element->PrevoiusElement->NextElement = Element->NextElement;
						if( Element->NextElement )
							Element->NextElement->PrevoiusElement = Element->PrevoiusElement;

						if( Header->FirstElement == Element ){

							if( Element->NextElement )
								Header->FirstElement = Element->NextElement;
							else
								Header->FirstElement = NULL;
						}

						if( Header->LastElement == Element ){
							
							if( Element->PrevoiusElement )
								Header->LastElement = Element->PrevoiusElement;
							else
								Header->LastElement = NULL;
						}
						
						Element->Data=NULL;
						mem.nwnx_free( Element );
						break;
					}
					Element = Element->NextElement;
				}
				Header->Count--;
			}
		}

		void (__fastcall *pDestructor)(CGenericObject *pTHIS, void *pVOID, int flag) = (void (__fastcall*)(CGenericObject *pTHIS, void *pVOID, int flag))(*(DWORD*)((DWORD*)(pObject->obj_type)));
		pDestructor(pObject, NULL, 1);
		objArray->DEL(nTmpObj); 	
	}


	memcpy( &mod->mod_areas[nIndex], &mod->mod_areas[nIndex+1], sizeof(nwn_objid_t)*(mod->mod_areas_len-nIndex-1) ); 
	mod->mod_areas[mod->mod_areas_len-1]=0;
	mod->mod_areas_len--;

	//Remove minimaps
	CGameObjectArray *pGameObjArray = ((*NWN_AppManager)->app_server)->GetObjectArray();

	if( pGameObjArray ){
		for(int i=0; i<=0xFFF; i++)	{
			CGameObjectArrayElement **pObjects = pGameObjArray->Objects;
			CGameObjectArrayElement *pElement = pObjects[i];
			if(!pElement) continue;
			CNWSCreature *pObject = (CNWSCreature *) pElement->Object;
			if(!pObject) continue;
			if(pObject->obj.obj_generic.obj_type2 == 5) {
				if(pObject->AreaData.Areas && pObject->AreaData.MiniMap ) {				
					pObject->AreaData.RemoveArea( nAreaID );
				}
			}
		}
	}

	CNWSArea * OriginalArea = (CNWSArea *)mod->GetArea( mod->mod_areas[0] );

	//Remove minimaps from turds
	if ((mod->mod_PlayerTURDList.ListHeader) && (mod->mod_PlayerTURDList.ListHeader->Count > 0)) {
		CExoLinkedListHeader *Header = mod->mod_PlayerTURDList.ListHeader;
		CExoLinkedListElement *Element = Header->FirstElement;
		CNWSPlayerTURD *TURD = NULL;
		while (Element) {
			TURD = (CNWSPlayerTURD*)Element->Data;
			if (TURD->AreaData.MiniMap) {
				TURD->AreaData.AddArea(nAreaID);
			}
			Element = Element->NextElement;
		}
	}

	Area->scalar_Destructor( 1 );

	//Send updates to DMs
	CNWSMessage *pMessage = ((*NWN_AppManager)->app_server)->GetNWSMessage();
	CExoLinkedList *pPlayerList = (CExoLinkedList*)((*NWN_AppManager)->app_server)->GetPlayerList();
	CExoLinkedListElement *pElement = pPlayerList->GetHeadPos();
	while (pElement) {
		CNWSPlayer *pPlayer = (CNWSPlayer*)pPlayerList->GetAtPos(pElement);
		if (pPlayer) pMessage->SendServerToPlayerDungeonMasterAreaList(pPlayer);
		pElement = pPlayerList->GetNext(pElement);
	}	

	Log( "o Destroy area: %08X (%u)\n", nAreaID, mod->mod_areas_len );

	return 1;
}