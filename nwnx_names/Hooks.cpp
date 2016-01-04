#include "NWNXNames.h"

extern CNWNXNames Names;

CExoString * (__fastcall *CNWSCreature__GetDisplayNameNext)( CNWSCreature * pThis, void*, CExoString * );
CExoString * __fastcall CNWSCreature__GetDisplayName( CNWSCreature * pThis, void*, CExoString * cName ){

	CNWSCreature__GetDisplayNameNext(pThis, NULL, cName );

	if( Names.LastID == 0 )
		return cName;

	DynamicNames * DisplayName = Names.Get( Names.LastID );
	
	if( !DisplayName )
		return cName;

	Name * Dynname = DisplayName->Get( pThis->obj.obj_generic.obj_id, false );

	if( !Dynname ){

		DisplayName = Names.Get( 0 );
		if( !DisplayName )
			return cName; 

		Dynname = DisplayName->Get( pThis->obj.obj_generic.obj_id, false );
		if( !Dynname )		
			return cName;
	}

	if( cName->text ){
		Names.mem.nwnx_free( cName->text );		
	}

	cName->len = Dynname->len;
	cName->text = (char*)Names.mem.nwnx_calloc( cName->len+1, 1 );
	strncpy( cName->text, Dynname->text, cName->len );

	Names.Log( "o GetDisplayName: %08X[%08X] -> %s\n", Names.LastID, pThis->obj.obj_generic.obj_id, cName->text );

	return cName;
}

void * (__fastcall * CNWSMessage__WriteGameObjUpdate_UpdateObjectNext)( CNWSMessage * , void*, CNWSPlayer * , CNWSObject * , CLastUpdateObject * , DWORD , DWORD  );
void * __fastcall CNWSMessage__WriteGameObjUpdate_UpdateObject( CNWSMessage * pThis, void*, CNWSPlayer * ply, CNWSObject * obj, CLastUpdateObject * luo, DWORD arg1, DWORD arg2 ){

	Names.LastID = ply->obj_id;
	void * ret = CNWSMessage__WriteGameObjUpdate_UpdateObjectNext( pThis, NULL, ply, obj, luo, arg1, arg2 );
	Names.LastID = 0;

	return ret;
}

int(__fastcall *CNWSMessage__SendServerToPlayerExamineGui_CreatureDataNext)(CNWSMessage *, void *, CNWSPlayer *, DWORD);
int __fastcall CNWSMessage__SendServerToPlayerExamineGui_CreatureData( CNWSMessage * pThis, void *, CNWSPlayer * ply, DWORD arg1 ){

	Names.LastID = ply->obj_id;
	int ret = CNWSMessage__SendServerToPlayerExamineGui_CreatureDataNext( pThis, NULL, ply, arg1 );
	Names.LastID = 0;

	return ret;
}

int(__fastcall *CNWSMessage__SendServerToPlayerPlayerList_AllNext)(CNWSMessage *, void *, CNWSPlayer *ply);
int __fastcall CNWSMessage__SendServerToPlayerPlayerList_All(CNWSMessage *, void *, CNWSPlayer *ply){

	CNWMessage * nwMess = (CNWMessage *)(*NWN_AppManager)->app_server->GetNWSMessage();
	nwMess->CreateWriteMessage(512, ply->pl_id, 1);
	
	CNWSPlayer * ply=NULL;

	CExoLinkedList * plylist = (CExoLinkedList *)(*NWN_AppManager)->app_server->GetPlayerList();	
	CExoLinkedListElement * node = plylist->ListHeader->FirstElement;	
	
	DWORD n = (*(int(**)(void))(**(DWORD **)(0x0066C050 + 4) + 84))();
	DWORD flag = *(DWORD *)(n + 0x100);

	nwMess->WriteBYTE(plylist->ListHeader->Count, 8);
	nwMess->WriteBool(flag);

	while (node){
	
		ply = (CNWSPlayer*)node->Data;
		if (ply){
			nwMess->WriteDWORD(ply->pl_id, 32);
			
		}
		
		node = node->NextElement;
	}

}

void ApplyHooks( ){

	Names.OverwriteFunction( 0x00447480, CNWSCreature__GetDisplayName, (PVOID*)&CNWSCreature__GetDisplayNameNext );
	Names.OverwriteFunction( 0x00445160, CNWSMessage__WriteGameObjUpdate_UpdateObject, (PVOID*)&CNWSMessage__WriteGameObjUpdate_UpdateObjectNext );
	Names.OverwriteFunction( 0x00446B00, CNWSMessage__SendServerToPlayerExamineGui_CreatureData, (PVOID*)&CNWSMessage__SendServerToPlayerExamineGui_CreatureDataNext );
}