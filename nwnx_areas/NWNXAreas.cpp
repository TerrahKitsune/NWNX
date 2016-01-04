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

extern CNWNXAreas areas;

void CNWNXAreas::SendNameUpdate(DWORD SendToID, DWORD ObjectToUpdateID){

	//Meh, doesnt work
	return;

	CNWSPlayer * pc = (*NWN_AppManager)->app_server->GetClientObjectByObjectId(SendToID);
	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject(ObjectToUpdateID);

	if (!pc || !obj || obj->obj_generic.obj_type2 != CGameObject__OBJECT_TYPE_CREATURE)
		return;

	CLastUpdateObject * luo = (CLastUpdateObject *)mem.nwnx_malloc(sizeof(CLastUpdateObject));

	if (!luo)
		return;

	int(__thiscall *CLastUpdateObject__CLastUpdateObject)(CLastUpdateObject *pThis) = (int(__thiscall *)(CLastUpdateObject *))0x0046ACF0;

	CLastUpdateObject__CLastUpdateObject(luo);

	char * pData;
	DWORD length;

	CNWSMessage * exoMess = (*NWN_AppManager)->app_server->GetNWSMessage();
	CNWMessage * nwMess = (CNWMessage *)exoMess;

	nwMess->CreateWriteMessage(0x400, pc->pl_id, 1);
	exoMess->WriteGameObjUpdate_UpdateObject(pc, obj, luo, 0, 0x400);
	nwMess->GetWriteMessage(&pData, &length);

	if (length)
		exoMess->SendServerToPlayerMessage(pc->pl_id, 5, 1, pData, length);

	mem.nwnx_free(luo);
}

void CNWNXAreas::ResendClientArea(nwn_objid_t obj){

	CNWSPlayer * ply = (*NWN_AppManager)->app_server->GetClientObjectByObjectId(obj);
	if (!ply)
		return;

	CNWSCreature * cre = (CNWSCreature *)ply->GetGameObject();
	if (!cre)
		return;

	CNWSArea * are = cre->obj.GetArea();
	if (!are)
		return;

	CNWSMessage * mess = (*NWN_AppManager)->app_server->GetNWSMessage();
	if (!mess)
		return;

	Log("o Resending Area %08X to %08X\n",are->obj_id,obj);

	uint32_t temp = cre->obj.obj_generic.field_0014;
	cre->obj.obj_generic.field_0014 = 0;

	int(__thiscall *CNWSMessage__SendServerToPlayerArea_ClientArea)(CNWSMessage *pThis, CNWSPlayer*ply, CNWSArea*are, float f1, float f2, float f3, Vector * pos, int flag) = (int(__thiscall *)(CNWSMessage *, CNWSPlayer*, CNWSArea*, float, float, float, Vector *, int))0x00439EB0;

	CNWSMessage__SendServerToPlayerArea_ClientArea(mess, ply, are, cre->obj.obj_orientation.x, cre->obj.obj_orientation.y, cre->obj.obj_orientation.z, &cre->obj.obj_position,0);

	cre->obj.obj_generic.field_0014 = temp;
}

bool AreaExists(CNWSModule* mod, nwn_objid_t area){

	for (int n = 0; n < mod->mod_areas_len; n++){
		if (mod->mod_areas[n] == area)
			return true;
	}

	return false;
}

bool CNWNXAreas::FixAllCreatures(CNWSModule* mod){

	if (!backlog)
		return false;

	Log("o Currenct automap data on existing creatures\n");

	if (mod==NULL)
		mod = (*NWN_AppManager)->app_server->srv_internal->GetModule();

	CGameObjectArray *pGameObjArray = ((*NWN_AppManager)->app_server)->GetObjectArray();
	if (!pGameObjArray){
		return false;
	}

	bool ret = false;
	CGameObjectArrayElement **pObjects = pGameObjArray->Objects;
	CGameObjectArrayElement *pElement;
	CGenericObject *pObject = NULL;
	for (int i = 0; i <= 0xFFF; i++) {

		pElement = pObjects[i];
		while (pElement){

			pObject = (CGenericObject *)pElement->Object;

			if (pObject && pObject->obj_type2 == 5){
				if (FixCreature(mod, (CNWSCreature*)pObject, false))
					ret = true;
			}

			pElement = (CGameObjectArrayElement *)pElement->field_8;
		}
	}

	return ret;
}

bool CNWNXAreas::FixCreature(CNWSModule* mod, CNWSCreature* pObject,bool log)
{
	if (pObject->AreaData.AreaCount==0){

		for (size_t i = 0; i < mod->mod_areas_len; ++i) {
			
			AddAreaToCreature(mod, pObject, mod->mod_areas[i]);		
		}

		if (pObject->AreaData.AreaCount == mod->mod_areas_len){
			if (log)Log("o Fixed creature %08X Added all: %u\n", pObject->obj.obj_generic.obj_id, mod->mod_areas_len);
			return true;
		}
	}

	DWORD *as = (DWORD *)pObject->AreaData.Areas;
	size_t i = 0;
	int rem = 0;
	int add = 0;
	while (i < pObject->AreaData.AreaCount) {
		if (CExoArrayList_uint32_contains((CExoArrayList_uint32*)&mod->mod_areas, as[i]) == -1) {
			RemoveAreaFromCreature(pObject, as[i]);
			rem++;
		}
		else {
			++i;
		}
	}

	as = (DWORD *)mod->mod_areas;
	for (size_t i = 0; i < mod->mod_areas_len; ++i) {
		if (CExoArrayList_uint32_contains((CExoArrayList_uint32*)&pObject->AreaData.Areas, as[i]) == -1) {
			AddAreaToCreature(mod, pObject, as[i]);
			add++;
		}
	}

	if (add > 0 || rem>0){
		if (log)Log("o Fixed creature %08X Add: %d Removed %d\n", pObject->obj.obj_generic.obj_id, add, rem);
		return true;
	}
	return false;
}

void * (__fastcall *CNWSCreature__SetAutoMapDataNext)(CNWSCreature *pCre, void*, int32_t num_areas, uint32_t *arealist, uint8_t **minimaps);
void * __fastcall CNWSCreature__SetAutoMapData(CNWSCreature *pCre, void*, int32_t num_areas, uint32_t *arealist, uint8_t **minimaps){

	if (areas.HasUpdated){
		areas.FixAllCreatures(NULL);
		areas.UpdateAreasForDMs();
		areas.HasUpdated = false;
	}

	void * fix = CNWSCreature__SetAutoMapDataNext(pCre, NULL, num_areas, arealist, minimaps);

	CNWSModule * mod = (*NWN_AppManager)->app_server->srv_internal->GetModule();
	if (mod) {
		areas.FixCreature(mod, pCre);
	}

	return fix;
}

void * (__fastcall *CNWSMessage__ComputeNumAutoMapUpdatesRequiredNext)(CNWSMessage * pThis, void*, CNWSCreature * cre, void * PlayerLastUpdateObj, unsigned long * ulongptr);
void * __fastcall CNWSMessage__ComputeNumAutoMapUpdatesRequired(CNWSMessage * pThis, void*, CNWSCreature * cre, void * PlayerLastUpdateObj, unsigned long * ulongptr){

	if (areas.HasUpdated){
		areas.FixAllCreatures(NULL);
		areas.UpdateAreasForDMs();
		areas.HasUpdated = false;
	}

	CNWSModule * mod = (*NWN_AppManager)->app_server->srv_internal->GetModule();
	if (mod) {
		if (areas.FixCreature(mod, cre)){	

			if (PlayerLastUpdateObj)
				*(DWORD *)((DWORD*)PlayerLastUpdateObj + 0x164) = 0;
		}
	}

	return CNWSMessage__ComputeNumAutoMapUpdatesRequiredNext(pThis, NULL, cre, PlayerLastUpdateObj, ulongptr);
}

int (__fastcall *CNWSArea__LoadTileSetInfoNext)(CNWSArea *pThis, void*, CResStruct * res);
int __fastcall CNWSArea__LoadTileSetInfo(CNWSArea *pThis, void*, CResStruct * res){

	areas.IsK = true;

	int n = CNWSArea__LoadTileSetInfoNext(pThis, NULL, res);
	if (!n){
		areas.IsK = false;
		areas.Log("! LoadTileSetInfo failed!\n");
	}

	return n;
}

int(__fastcall *CNWSArea__LoadGITNext)(CNWSArea *pThis, void*, int n);
int __fastcall CNWSArea__LoadGIT(CNWSArea *pThis, void*, int n){

	if (!areas.IsK)
		return 0;

	int ok = CNWSArea__LoadGITNext(pThis, NULL, n);
	if (!ok){
		areas.IsK = false;
		areas.Log("! LoadGIT failed!\n");
	}

	return ok;
}

DWORD (__fastcall *CNWSTrigger__GetTargetAreaNext)(CNWSTrigger *pThis, void*);
DWORD __fastcall CNWSTrigger__GetTargetArea(CNWSTrigger *pThis, void*){

	CExoString * str = (CExoString *)(pThis + 0x1C8);

	if (*(DWORD *)(pThis + 0x24C) && str){

		CNWSModule * mod = (*NWN_AppManager)->app_server->srv_internal->GetModule();
		nwn_objid_t wp = mod->FindObjectByTagTypeOrdinal(str,0x0C,0);
		if (wp == 0 || wp == OBJECT_INVALID)
			return OBJECT_INVALID;

		CNWSWaypoint * objwp = (*NWN_AppManager)->app_server->srv_internal->GetWaypointByGameObjectID(wp);
		if (!objwp)
			return OBJECT_INVALID;

		CNWSObject * obj = (CNWSObject *)(*(int(**)(void))(*(DWORD *)pThis + 0x5C))();
		if (!obj)
			return OBJECT_INVALID;

		CNWSArea * are = obj->GetArea();
		if (!are)
			return OBJECT_INVALID;

		return *(DWORD *)(are + 0xC8);
	}

	return OBJECT_INVALID;
}

CNWNXAreas::~CNWNXAreas(){OnRelease( );};

BOOL CNWNXAreas::OnRelease( ){

	return CNWNXBase::OnRelease();
}

BOOL CNWNXAreas::OnCreate(const char* LogDir){

	backlog = true;
	IsK = true;
	HasUpdated = false;
	char log[MAX_PATH];
	char logmove[MAX_PATH];
	sprintf(log, "%s\\nwnx_areas.txt", LogDir);
	sprintf(logmove, "%s\\1\\nwnx_areas.txt", LogDir);

	MoveFile(log, logmove);

	// call the base class function
	if( !CNWNXBase::OnCreate( log ) )
		return false;

	WriteLogHeader( );	
	test=NULL;

	HookCode((PVOID)0x004B1DD0, (PVOID)CNWSCreature__SetAutoMapData, (PVOID*)&CNWSCreature__SetAutoMapDataNext);
	HookCode((PVOID)0x00439130, (PVOID)CNWSMessage__ComputeNumAutoMapUpdatesRequired, (PVOID*)&CNWSMessage__ComputeNumAutoMapUpdatesRequiredNext);

	//HookCode((PVOID)0x005A68A0, (PVOID)CNWSTrigger__GetTargetArea, (PVOID*)&CNWSTrigger__GetTargetAreaNext);

	HookCode((PVOID)0x0050CD10, (PVOID)CNWSArea__LoadTileSetInfo, (PVOID*)&CNWSArea__LoadTileSetInfoNext);
	HookCode((PVOID)0x0050D1A0, (PVOID)CNWSArea__LoadGIT, (PVOID*)&CNWSArea__LoadGITNext);

	//NOP
	DWORD privs;
	unsigned char * func = (unsigned char*)0x004CD383;

	VirtualProtect(func, 32, PAGE_EXECUTE_READWRITE, &privs);

	for (int n = 0; n < 32;n++)
		func[n] = 0x90;

	VirtualProtect(func, 32, privs, NULL);

	return true;
}

void CNWNXAreas::WriteLogHeader( ){

	fprintf( m_fFile, "NWNXAreas v1.50 created by Terra_777\nBased on previous versions by Virusman and Maxrock\n\n" );
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

	if( strcmp( Request, "GET_AREA" ) == 0 ){
		
		CNWSModule * mod = (*NWN_AppManager)->app_server->srv_internal->GetModule( );
		DWORD id = 0;
		sscanf( Parameters, "%u", &id );
		if( id >= mod->mod_areas_len || id < 0 )
			Last = OBJECT_INVALID;
		else
			Last = mod->mod_areas[id];

		//Log("o GetArea: %u -> %08X\n", id, Last);

	}
	/*else if (strcmp(Request, "RESEND") == 0){
		
		nwn_objid_t oPC;
		sscanf(Parameters,"%x", &oPC);
		ResendClientArea(oPC);
	}*/
	else if (strcmp(Request, "COMMIT") == 0){
		if (areas.HasUpdated){
			areas.FixAllCreatures(NULL);
			areas.UpdateAreasForDMs();
			areas.HasUpdated = false;
		}
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

bool CheckTiles(CNWSArea * area){
	
	if (area->are_height < 1 || area->are_width < 1 || ((DWORD *)(area + 284))==NULL)
		return false;

	return true;
}

bool CNWNXAreas::HasPlayers(CNWSArea * Area){

	nwn_objid_t nTmpObj;
	Area->GetFirstObjectInArea(&nTmpObj);
	do {
		CGenericObject *pObject = (*NWN_AppManager)->app_server->srv_internal->GetGameObject(nTmpObj);
		if (!pObject)
			continue;
		if (pObject->obj_type2 == 0x5) {
			CNWSCreature *pCreature = (CNWSCreature *)pObject;
			if (pCreature->cre_is_pc || pCreature->IsDM())
				return true;
		}
	} while (Area->GetNextObjectInArea(&nTmpObj));

	return false;
}

bool CNWNXAreas::CheckHasPlayersInTrans(){

	nwn_objid_t oPC = (*NWN_AppManager)->app_server->GetFirstPCObject();
	CNWSCreature * cre = NULL;

	while (oPC != OBJECT_INVALID){

		cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(oPC);
		if (cre && !cre->obj.GetArea())
			return true;

		oPC = (*NWN_AppManager)->app_server->GetNextPCObject();
	}

	return false;
}

long double CNWNXAreas::GetHeight( CNWSArea * are, char * str ){

	Vector v;

	sscanf( str, "%f %f %f", &v.x, &v.y, &v.z );

	Log( "o ComputeHeight: %f %f %f\n", v.x,v.y,v.z );

	return are->ComputeHeight( v );
}

void CNWNXAreas::DestoryObject(nwn_objid_t id){

	/*CNWSArea * are = obj->GetArea();

	if (are){
		are->RemoveObjectFromArea(obj->obj_generic.obj_id);		
	}

	obj->SetArea(NULL);*/
	
	CWorldTimer * time = (*NWN_AppManager)->app_server->GetWorldTimer();
	DWORD seconds = time->GetWorldTimeSecond();
	DWORD days = time->GetCalendarDayFromSeconds(seconds);
	CServerAIMaster * bitch = (*NWN_AppManager)->app_server->GetServerAIMaster();
	DWORD event = *(DWORD *)0x00633990; //CServerAIMaster__EVENT_DESTROY_OBJECT
	CNWSModule * mod = (*NWN_AppManager)->app_server->srv_internal->GetModule();

	bitch->AddEventDeltaTime(days, seconds, mod->obj_id, id, event, 0);

	/*void(__fastcall *pDestructor)(CNWSObject *pTHIS, void *pVOID, int flag) = (void(__fastcall*)(CNWSObject *pTHIS, void *pVOID, int flag))(*(DWORD*)((DWORD*)(obj->obj_generic.obj_type)));
	pDestructor(obj, NULL, 1);*/

	//(**(void(__thiscall ***)(CNWSObject*, DWORD))obj)(obj, 1);

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
			if (pPlayer){
				pMessage->SendServerToPlayerDungeonMasterAreaList(pPlayer);
			}
			pElement = pPlayerList->GetNext(pElement);
		}
	}
}

void CNWNXAreas::AddAreaToCreature(CNWSModule *pModule, CNWSCreature *pObject, DWORD nAreaID)
{
	void * temp = mem.nwnx_malloc((pObject->AreaData.AreaCount + 1) * 4);

	if (pObject->AreaData.MiniMap){
		memcpy(temp, pObject->AreaData.MiniMap, pObject->AreaData.AreaCount * 4);
		mem.nwnx_free(pObject->AreaData.MiniMap);
	}

	pObject->AreaData.MiniMap = (CMiniMapData **)temp;

	void *pMinimap = mem.nwnx_malloc(0x80);
	memset(pMinimap, 0, 0x80);
	pObject->AreaData.MiniMap[pObject->AreaData.AreaCount] = (CMiniMapData *)pMinimap;

	CExoArrayList_uint32___Add((CExoArrayList_uint32*)&pObject->AreaData.Areas, NULL, nAreaID);

	pObject->AreaData.AreaCount2 = pObject->AreaData.AreaCount;
	pObject->AreaData.MiniMapCount = pObject->AreaData.AreaCount;

	SendNameUpdate(pObject->obj.obj_generic.obj_id, nAreaID);
}

void CNWNXAreas::AddAreaToCreatures(CNWSModule *pModule, DWORD nAreaID)
{
	//Log("o Adding area %08X to all creatures\n", nAreaID);

	CGameObjectArray *pGameObjArray = ((*NWN_AppManager)->app_server)->GetObjectArray();
	if (!pGameObjArray){
		return;
	}

	if (!backlog){

		CGameObjectArrayElement **pObjects = pGameObjArray->Objects;
		CGameObjectArrayElement *pElement;
		CGenericObject *pObject = NULL;
		for (int i = 0; i <= 0xFFF; i++) {

			pElement = pObjects[i];
			while (pElement){

				pObject = (CGenericObject *)pElement->Object;

				if (pObject && pObject->obj_type2 == 5){
					AddAreaToCreature(pModule, (CNWSCreature *)pObject, nAreaID);
				}

				pElement = (CGameObjectArrayElement *)pElement->field_8;
			}
		}
	}
	/*CGameObjectArray *pGameObjArray = ((*NWN_AppManager)->app_server)->GetObjectArray();

	if (!pGameObjArray) return;
	for (int i = 0; i <= 0xFFF; i++) {
		CGameObjectArrayElement **pObjects = pGameObjArray->Objects;
		CGameObjectArrayElement *pElement = pObjects[i];
		if (!pElement) continue;
		CNWSCreature *pObject = (CNWSCreature *)pElement->Object;
		if (!pObject) continue;
		if (pObject->obj.obj_generic.obj_type2 == 5){
			AddAreaToCreature(pModule, pObject, nAreaID);
		}
	}*/

	//Log("o Adding area %08X to all turds\n", nAreaID);

	CNWSModule *pRealModule = (*NWN_AppManager)->app_server->srv_internal->GetModule();

	if ((pRealModule->mod_PlayerTURDList.ListHeader) && (pRealModule->mod_PlayerTURDList.ListHeader->Count > 0)) {
		CExoLinkedListHeader *Header = pRealModule->mod_PlayerTURDList.ListHeader;
		CExoLinkedListElement *Element = Header->FirstElement;
		CNWSPlayerTURD *TURD = NULL;
		while (Element) {
			CNWSPlayerTURD *pTURD = (CNWSPlayerTURD *)Element->Data;
			if (pTURD) {

				if (pTURD->AreaData.MiniMap && pTURD->AreaData.AreaCount && pTURD->AreaData.Areas) {

					int nIndex = -1;
					for (size_t i = 0; i < pTURD->AreaData.AreaCount; i++) {
						if (pTURD->AreaData.Areas[i] == nAreaID) {
							nIndex = i;
							break;
						}

					}
					if (nIndex != -1) {
						
						void * temp = mem.nwnx_malloc((pTURD->AreaData.AreaCount + 1) * 4);
						memcpy(temp, pTURD->AreaData.MiniMap, pTURD->AreaData.AreaCount * 4);
						mem.nwnx_free(pTURD->AreaData.MiniMap);
						pTURD->AreaData.MiniMap = (CMiniMapData **)temp;

						void *pMinimap = mem.nwnx_malloc(0x80);
						memset(pMinimap, 0, 0x80);
						pTURD->AreaData.MiniMap[pTURD->AreaData.AreaCount] = (CMiniMapData *)pMinimap;

						CExoArrayList_uint32___Add((CExoArrayList_uint32*)&pTURD->AreaData.Areas, NULL, nAreaID);
					}
				}
			}
			Element = Element->NextElement;
		}
	}
}

void CNWNXAreas::UpdateAreasForDMs()
{
	Log("o Sending Arealist to DMs\n");

	CNWSMessage *pMessage = ((*NWN_AppManager)->app_server)->GetNWSMessage();
	CExoLinkedList *pPlayerList = (CExoLinkedList*)((*NWN_AppManager)->app_server)->GetPlayerList();
	CExoLinkedListElement *pElement = pPlayerList->GetHeadPos();
	while (pElement) {
		CNWSPlayer *pPlayer = (CNWSPlayer*)pPlayerList->GetAtPos(pElement);
		if (pPlayer){
			pMessage->SendServerToPlayerDungeonMasterAreaList(pPlayer);
		}
		pElement = pPlayerList->GetNext(pElement);
	}
}

void CNWNXAreas::RemoveAreaFromCreature(CNWSCreature *pObject, DWORD nAreaID)
{
	//Log("o Removing for %08X for %08X\n", nAreaID, pObject->obj.obj_generic.obj_id);

	int nIndex = pObject->AreaData.IndexOf(nAreaID);

	if (nIndex != -1) {
		mem.nwnx_free(pObject->AreaData.MiniMap[nIndex]);
		pObject->AreaData.MiniMap[nIndex] = NULL;
		memcpy(&pObject->AreaData.MiniMap[nIndex], &pObject->AreaData.MiniMap[nIndex + 1], (pObject->AreaData.AreaCount - nIndex - 1) * 4);
		//pObject->AreaData.RemoveArea(nAreaID, nIndex);
		CExoArrayList_uint32___Remove(&pObject->AreaData.Areas, NULL, nAreaID);
		pObject->AreaData.AreaCount2 = pObject->AreaData.AreaCount;
		pObject->AreaData.MiniMapCount = pObject->AreaData.AreaCount;
	}	
}

void CNWNXAreas::GetSafeLocation(CScriptLocation * loc){
	
	CExoString * exo = (CExoString *)mem.nwnx_malloc(sizeof(CExoString));
	const char * name = "nwnx_areas";
	
	exo->text = (char*)mem.nwnx_malloc(strlen(name)+1);
	strcpy(exo->text,name);

	CNWSObject * obj;
	CNWSModule * mod = (*NWN_AppManager)->app_server->srv_internal->GetModule();
	nwn_objid_t target = mod->mod_vartable.GetNWNObject(*exo);

	mem.nwnx_free(exo->text);
	mem.nwnx_free(exo);

	if (target == 0 || target == OBJECT_INVALID){
		obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject(target);

		if (obj){
			obj->GetScriptLocation(loc);
			if (loc->loc_area != 0 && loc->loc_area != OBJECT_INVALID)
				return;
		}
	}

	CNWSArea * are = mod->GetArea(mod->mod_areas[0]);
	nwn_objid_t nTmpObj;
	are->GetFirstObjectInArea(&nTmpObj);
	do {
		obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject(nTmpObj);
		if (obj){
			if (obj){
				obj->GetScriptLocation(loc);
				if (loc->loc_area != 0 && loc->loc_area != OBJECT_INVALID)
					return;
			}
		}

	} while (are->GetNextObjectInArea(&nTmpObj));

	loc->loc_area = mod->mod_areas[0];

	areas.Log("! unable to find safe location!\n");

	return;
}

void CNWNXAreas::RemoveAreaForCreatures(CNWSModule *pModule, DWORD nAreaID, CNWSArea * are)
{

	CNWSModule *pRealModule = (*NWN_AppManager)->app_server->srv_internal->GetModule();

	if ((pRealModule->mod_PlayerTURDList.ListHeader) && (pRealModule->mod_PlayerTURDList.ListHeader->Count > 0)) {
		CExoLinkedListHeader *Header = pRealModule->mod_PlayerTURDList.ListHeader;
		CExoLinkedListElement *Element = Header->FirstElement;
		CNWSPlayerTURD *TURD = NULL;
		while (Element) {
			CNWSPlayerTURD *pTURD = (CNWSPlayerTURD *)Element->Data;
			if (pTURD) {


				if (pTURD->AreaData.MiniMap && pTURD->AreaData.AreaCount && pTURD->AreaData.Areas) {

					int nIndex = -1;
					for (size_t i = 0; i < pTURD->AreaData.AreaCount; i++) {
						if (pTURD->AreaData.Areas[i] == nAreaID) {
							nIndex = i;
							break;
						}

					}
					if (nIndex != -1) {
						//areas.Log(3, "Cleaning up TURD...\n");
						mem.nwnx_free(pTURD->AreaData.MiniMap[nIndex]);
						memcpy(&pTURD->AreaData.MiniMap[nIndex], &pTURD->AreaData.MiniMap[nIndex + 1], (pTURD->AreaData.AreaCount - nIndex - 1) * 4);
						//pTURD->AreaData.MiniMap = (CMiniMapData **)mem.nwnx_realloc(pTURD->AreaData.MiniMap, (pTURD->AreaData.AreaCount - 1) * 4);

						memcpy(&pTURD->AreaData.Areas[nIndex], &pTURD->AreaData.Areas[nIndex + 1], (pTURD->AreaData.AreaCount - nIndex - 1) * 4);
						//pTURD->AreaData.Areas = (nwn_objid_t *)mem.nwnx_realloc(pTURD->AreaData.Areas, (pTURD->AreaData.AreaCount - 1) * 4);

						pTURD->AreaData.AreaCount--;
					}
				}

				if (pTURD->TURD_AreaId == nAreaID){


					CNWSObject * obj = (CNWSObject *)pTURD;
					are->RemoveObjectFromArea(obj->obj_generic.obj_id);
					obj->SetArea(NULL);
					Log("o Reset area on turd %08X %u\n",obj->obj_generic.obj_id,(DWORD)obj->obj_generic.obj_type2);

					/*CScriptLocation * loc = (CScriptLocation *)mem.nwnx_malloc(sizeof(CScriptLocation));
					GetSafeLocation(loc);

					CNWSArea * start = pRealModule->GetArea(loc->loc_area);

					obj->SetArea(start);
					obj->SetOrientation(loc->loc_orientation);
					obj->SetPosition(loc->loc_position,1);

					Log("o Moved TURD %08X -> %08X\n", pTURD->TURD_PlayerID,loc->loc_area);

					mem.nwnx_free(loc);*/

					/*Log("o Removing TURD %08X\n", pTURD->TURD_PlayerID);
					Element->Data = NULL;
					pModule->mod_PlayerTURDList.Remove(Element);
					((*NWN_AppManager)->app_server)->GetObjectArray()->DEL(pTURD->TURD_PlayerID);
					pTURD->ScalarDestructor(1);
					Element = Header->FirstElement;*/
				}
			}
			Element = Element->NextElement;
		}
	}

	//Log("o Removing area %08X from all creatures\n", nAreaID);

	CGameObjectArray *pGameObjArray = ((*NWN_AppManager)->app_server)->GetObjectArray();
	if (!pGameObjArray){
		return;
	}

	CGameObjectArrayElement **pObjects = pGameObjArray->Objects;
	CGameObjectArrayElement *pElement;
	CNWSObject *pObject = NULL;
	for (int i = 0; i <= 0xFFF; i++) {

		pElement = pObjects[i];
		while (pElement){

			pObject = (CNWSObject *)pElement->Object;

			if (pObject){

				if (pObject->obj_area_id == nAreaID && pObject->obj_generic.obj_type2 != 4){

					//Log("o Removing object %08X %u\n", pObject->obj_generic.obj_id, (DWORD)pObject->obj_generic.obj_type2);

					//AI master cannot destroy doors
					if (pObject->obj_generic.obj_type2 == 10 || pObject->obj_generic.obj_type2==16){

						void(__fastcall *pDestructor)(CNWSObject *pTHIS, void *pVOID, int flag) = (void(__fastcall*)(CNWSObject *pTHIS, void *pVOID, int flag))(*(DWORD*)((DWORD*)(pObject->obj_generic.obj_type)));
						pDestructor(pObject, NULL, 1);
					}
					else{
						
						if (pObject->obj_generic.obj_type2 == 5){
							((CNWSCreature *)pObject)->cre_IsImmortal = false;							
						}

						pObject->obj_is_destroyable = true;
						DestoryObject(pObject->obj_generic.obj_id);
					}
				}
				else if (!backlog && pObject->obj_generic.obj_type2 == 5){					
					RemoveAreaFromCreature((CNWSCreature *)pObject, nAreaID);
				}
			}

			pElement = (CGameObjectArrayElement *)pElement->field_8;
		}
	}

	/*CGameObjectArray *pGameObjArray = ((*NWN_AppManager)->app_server)->GetObjectArray();

	if (!pGameObjArray) return;
	for (int i = 0; i <= 0xFFF; i++) {
		CGameObjectArrayElement **pObjects = pGameObjArray->Objects;
		CGameObjectArrayElement *pElement = pObjects[i];
		if (!pElement) continue;
		CNWSCreature *pObject = (CNWSCreature *)pElement->Object;
		if (!pObject) continue;
		if (pObject->obj.obj_generic.obj_type2 == 5) {
			RemoveAreaFromCreature(pObject, nAreaID);
		}
	}*/

	//Log("o Removing area %08X from all turds\n", nAreaID);
}

nwn_objid_t CNWNXAreas::CreateArea( const char * resref ){

	Log("o Creating area %s\n", resref);

	CResRef * ref = (CResRef*)mem.nwnx_malloc(sizeof(CResRef));
	memset(ref->resref, 0, 16);
	strncpy(ref->resref, resref, 16);

	CNWSArea * newArea = (CNWSArea *)mem.nwnx_malloc(0x20C);

	if (!newArea || !newArea->CNWSArea(*ref, 0, OBJECT_INVALID)){
		Log("! Creating area failed: unable to create area %s\n", resref);
		if (newArea)
			newArea->scalar_Destructor(1);
		mem.nwnx_free(ref);
		return OBJECT_INVALID;
	}

	if (!newArea->LoadArea(0) || !areas.IsK || !CheckTiles(newArea)){
		Log("! Creating area failed: unable to load area %s\n", resref);
		newArea->scalar_Destructor(1);
		mem.nwnx_free(ref);
		return OBJECT_INVALID;
	}

	DWORD nAreaID = newArea->obj_id;

	CNWSModule * mod = (*NWN_AppManager)->app_server->srv_internal->GetModule();	
	
	CExoArrayList_uint32___Add(&mod->mod_areas, NULL, nAreaID);

	AddAreaToCreatures(mod, nAreaID);
	HasUpdated = true;
	
	Log("o Loaded area %08X %u\n", nAreaID, mod->mod_areas_len);

	mem.nwnx_free(ref);

	return nAreaID;
}

int CNWNXAreas::DestroyArea( nwn_objid_t nAreaID ){

	Log("o Removing area %08X\n", nAreaID);

	if (!nAreaID || nAreaID == OBJECT_INVALID){
		Log("! Removing area failed %08X area isnt valid\n", nAreaID);
		return 0;
	}

	CNWSModule * mod = (*NWN_AppManager)->app_server->srv_internal->GetModule();

	CNWSArea * Area = (CNWSArea *)mod->GetArea(nAreaID);

	if (!Area){

		Log("! Removing area failed %08X area not found\n", nAreaID);

		RemoveAreaForCreatures(mod, nAreaID, Area);
		UpdateAreasForDMs();

		int index = CExoArrayList_uint32_contains((CExoArrayList_uint32*)&mod->mod_areas, nAreaID);
		if (index != -1){
			memcpy(&mod->mod_areas[index], &mod->mod_areas[index + 1], (mod->mod_areas_len - index - 1) * 4);
			mod->mod_areas_len--;
		}
		
		return 0;
	}
	else if (Area->are_numplayers > 0 || HasPlayers(Area)){
		Log("! Removing area failed %08X area has players\n", nAreaID);
		return 0;
	}
	else if (CheckHasPlayersInTrans()){
		Log("! Removing area failed %08X player is in transition\n", nAreaID);
		return 0;
	}
	else if (Area->obj_id==1){
		Log("! Removing area failed %08X area is the entry\n", nAreaID);
		return 0;
	}

	RemoveAreaForCreatures(mod, nAreaID, Area);

	/*nwn_objid_t nTmpObj;
	Area->GetFirstObjectInArea(&nTmpObj);
	do {
		CGenericObject *pObject = (*NWN_AppManager)->app_server->srv_internal->GetGameObject(nTmpObj);
		if (!pObject)
			continue;
		if (pObject->obj_type2 == 0x5) {
			CNWSCreature *pCreature = (CNWSCreature *)pObject;
			if (pCreature->cre_is_pc)
				continue;
		}
		//areas.Log(2, "Destroying object %08lX\n", nTmpObj);
	
		DestoryObject((CNWSObject*)pObject);

	} while (Area->GetNextObjectInArea(&nTmpObj));*/
	
	HasUpdated = true;
	CExoArrayList_uint32___Remove(&mod->mod_areas, NULL, nAreaID);

	/*int index = CExoArrayList_uint32_contains((CExoArrayList_uint32*)&mod->mod_areas, nAreaID);
	if (index != -1){
		memcpy(&mod->mod_areas[index], &mod->mod_areas[index + 1], (mod->mod_areas_len - index - 1) * 4);
		mod->mod_areas_len--;
	}*/

	Area->scalar_Destructor(1);

	Log("o Removed area %08X\n", nAreaID);

	return 1;
}