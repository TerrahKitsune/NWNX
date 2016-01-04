#include "EventHooks.h"
#include "NWNXEvents.h"

#include "../NWNXdll/madCHook.h"
#include "../NWNXdll/IniFile.h"
#pragma comment(lib, "madChook.lib")

extern CNWNXEvents events;

int (__fastcall *CNWSCreature__AddCastSpellActionsNext)( CNWSCreature * cre, void *, unsigned long nSpell, int arg2, int arg3, int metamagic, int arg5, Vector vTarget ,nwn_objid_t arg6, int arg7, int arg8, int arg9, unsigned char arg10, int arg11, int arg12, int arg13, unsigned char arg14 );
int (__fastcall *CNWSCreature__AddItemCastSpellActionsNext)( CNWSCreature * cre, void *, nwn_objid_t oItem, int arg1, int arg2, Vector vTarget, nwn_objid_t oTarget, int arg3 );
int (__fastcall *CNWSCreature__UseFeatNext)( CNWSCreature * cre, void *, unsigned short arg1, unsigned short arg2, nwn_objid_t target, nwn_objid_t target2, Vector * vTarget );
int (__fastcall *CNWSCreature__UseSkillNext)( CNWSCreature * cre, void *, unsigned char skill, unsigned char arg2, nwn_objid_t target, Vector vTarget, nwn_objid_t target2, nwn_objid_t target3, int arg6 );
int (__fastcall * CNWSCreature__UnPolymorphNext)( CNWSCreature * cre, void*, CGameEffect * eff );
int (__fastcall * CNWSCreature__PolymorphNext)( CNWSCreature * cre, void*, int nPoly, CGameEffect * eff, int nLocked );
int (__fastcall *CNWSCreature__AddAttackActionsNext)(CNWSCreature * cre, void*, unsigned long target, int nArg1, int nArg2, int nArg3 );
int (__fastcall *CNWSCreature__ToggleModeNext)( CNWSCreature * cre, void *, unsigned char arg1 );
int (__fastcall *CNWSCreatureStats__GetEpicWeaponDevastatingCriticalNext)( CNWSCreatureStats * cre, void*, CNWSItem * itm );
void (__fastcall *CNWSCreature__SetStealthModeNext)(CNWSCreature * cre, void*, unsigned char bToggle);
void (__fastcall *CNWSCreature__SetDetectModeNext)(CNWSCreature * cre, void*, unsigned char bToggle);
int(__fastcall *CNWSPlayer__PackCreatureIntoMessageNext)(CNWSPlayer * cre, void*);
int(__fastcall *CNWSMessage__SendServerToPlayerExamineGui_ItemDataNext)(CNWSMessage *pMess, void*, CNWSPlayer * ply, nwn_objid_t trg);
int(__fastcall *CNWSMessage__SendServerToPlayerExamineGui_CreatureDataNext)(CNWSMessage *pMess, void*, CNWSPlayer * ply, nwn_objid_t trg);
int(__fastcall *CNWSMessage__SendServerToPlayerExamineGui_PlaceableDataNext)(CNWSMessage *pMess, void*, CNWSPlayer * ply, nwn_objid_t trg);
int(__fastcall *CNWSMessage__SendServerToPlayerExamineGui_TrapDataNext)(CNWSMessage *pMess, void*, CNWSPlayer * ply, nwn_objid_t trg, CNWSCreature * cre, int arg);
int(__fastcall *CNWSMessage__SendServerToPlayerExamineGui_DoorDataNext)(CNWSMessage *pMess, void*, CNWSPlayer * ply, nwn_objid_t trg);

int __fastcall CNWSMessage__SendServerToPlayerExamineGui_DoorData(CNWSMessage *pMess, void*, CNWSPlayer * ply, nwn_objid_t trg){

	if (!ply)
		return CNWSMessage__SendServerToPlayerExamineGui_DoorDataNext(pMess, NULL, ply, trg);

	events.Log("o CNWSMessage__SendServerToPlayerExamineGui_DoorData: %08lx\nCre: %08lx\n", ply->obj_id, trg);

	events.EP->EVENT = EVENT_EXAMINE;
	events.EP->oTarget[0] = trg;

	int nReturn = events.FireEvent(ply->obj_id);

	if (nReturn != 0x80000000){
		return 0;
	}

	return CNWSMessage__SendServerToPlayerExamineGui_DoorDataNext(pMess, NULL, ply, trg);
}

int __fastcall CNWSMessage__SendServerToPlayerExamineGui_TrapData(CNWSMessage *pMess, void*, CNWSPlayer * ply, nwn_objid_t trg, CNWSCreature * cre, int arg){

	if (!ply)
		return CNWSMessage__SendServerToPlayerExamineGui_TrapDataNext(pMess, NULL, ply, trg,cre,arg);

	events.Log("o CNWSMessage__SendServerToPlayerExamineGui_TrapData: %08lx\nCre: %08lx\n", ply->obj_id, trg);

	events.EP->EVENT = EVENT_EXAMINE;
	events.EP->oTarget[0] = trg;
	if (cre)
		events.EP->oTarget[1] = cre->obj.obj_generic.obj_id;
	events.EP->nData[0] = arg;

	int nReturn = events.FireEvent(ply->obj_id);

	if (nReturn != 0x80000000){
		return 0;
	}

	return CNWSMessage__SendServerToPlayerExamineGui_TrapDataNext(pMess, NULL, ply, trg, cre, arg);
}

int __fastcall CNWSMessage__SendServerToPlayerExamineGui_PlaceableData(CNWSMessage *pMess, void*, CNWSPlayer * ply, nwn_objid_t trg){

	if (!ply)
		return CNWSMessage__SendServerToPlayerExamineGui_PlaceableDataNext(pMess, NULL, ply, trg);

	events.Log("o CNWSMessage__SendServerToPlayerExamineGui_PlaceableData: %08lx\nCre: %08lx\n", ply->obj_id, trg);

	events.EP->EVENT = EVENT_EXAMINE;
	events.EP->oTarget[0] = trg;


	int nReturn = events.FireEvent(ply->obj_id);

	if (nReturn != 0x80000000){
		return 0;
	}

	return CNWSMessage__SendServerToPlayerExamineGui_PlaceableDataNext(pMess, NULL, ply, trg);
}

int __fastcall CNWSMessage__SendServerToPlayerExamineGui_CreatureData(CNWSMessage *pMess, void*, CNWSPlayer * ply, nwn_objid_t trg){

	if (!ply)
		return CNWSMessage__SendServerToPlayerExamineGui_CreatureDataNext(pMess, NULL, ply, trg);

	events.Log("o SendServerToPlayerExamineGui_CreatureData: %08lx\nCre: %08lx\n", ply->obj_id, trg);

	events.EP->EVENT = EVENT_EXAMINE;
	events.EP->oTarget[0] = trg;


	int nReturn = events.FireEvent(ply->obj_id);

	if (nReturn != 0x80000000){
		return 0;
	}

	return CNWSMessage__SendServerToPlayerExamineGui_CreatureDataNext(pMess, NULL, ply, trg);
}

int __fastcall CNWSMessage__SendServerToPlayerExamineGui_ItemData(CNWSMessage *pMess, void*, CNWSPlayer * ply, nwn_objid_t trg){

	if (!ply)
		return CNWSMessage__SendServerToPlayerExamineGui_ItemDataNext(pMess, NULL, ply, trg);

	events.Log("o SendServerToPlayerExamineGui_ItemData: %08lx\nItem: %08lx\n", ply->obj_id, trg);

	events.EP->EVENT = EVENT_EXAMINE;
	events.EP->oTarget[0] = trg;


	int nReturn = events.FireEvent(ply->obj_id);

	if (nReturn != 0x80000000){
		return 0;
	}

	return CNWSMessage__SendServerToPlayerExamineGui_ItemDataNext(pMess, NULL, ply, trg);
}

int __fastcall CNWSPlayer__PackCreatureIntoMessage(CNWSPlayer * cre, void*){


	events.Log("o PackCreatureIntoMessage: %08lx\n", cre->obj_id);

	events.EP->EVENT = EVENT_SAVEPC;
	
	int nReturn = events.FireEvent(cre->obj_id);

	if (nReturn != 0x80000000){
		return 1;
	}

	return CNWSPlayer__PackCreatureIntoMessageNext(cre,NULL);
}

void __fastcall CNWSCreature__SetDetectMode(CNWSCreature * cre, void*, unsigned char bToggle){

	if (!cre)
		CNWSCreature__SetDetectModeNext(cre, NULL, bToggle);

	events.Log("o SetDetectMode: %08lx\nMode: %i\nFlag: %u\n", cre->obj.obj_generic.obj_id, 0, bToggle);

	events.EP->EVENT = EVENT_TOGGLEMODE;
	events.EP->nData[0] = 0;
	events.EP->nData[1] = bToggle;

	int nReturn = events.FireEvent(cre->obj.obj_generic.obj_id);

	if (nReturn != 0x80000000){
		return;
	}

	CNWSCreature__SetDetectModeNext(cre, NULL, bToggle);
}

void __fastcall CNWSCreature__SetStealthMode(CNWSCreature * cre, void*, unsigned char bToggle){

	if (!cre)
		CNWSCreature__SetStealthModeNext(cre, NULL, bToggle);

	events.Log("o SetStealthMode: %08lx\nMode: %i\nFlag: %u\n", cre->obj.obj_generic.obj_id, 1, bToggle);

	events.EP->EVENT = EVENT_TOGGLEMODE;
	events.EP->nData[0] = 1;
	events.EP->nData[1] = bToggle;

	int nReturn = events.FireEvent(cre->obj.obj_generic.obj_id);

	if (nReturn != 0x80000000){
		return;
	}

	CNWSCreature__SetStealthModeNext(cre, NULL, bToggle);
}

int __fastcall CNWSCreatureStats__GetEpicWeaponDevastatingCritical( CNWSCreatureStats * cre, void*, CNWSItem * itm ){

	if (!cre)
		return CNWSCreatureStats__GetEpicWeaponDevastatingCriticalNext(cre, NULL, itm);

	events.EP->EVENT=EVENT_DEVCRIT;

	if( itm ){
		events.EP->nData[0] = itm->it_baseitemtype;
		events.EP->oTarget[0] = itm->obj.obj_generic.obj_id;
	}
	else
		events.EP->nData[0] = 256; //Unarmed, invalid

	events.EP->oTarget[1] = cre->cs_original->cre_attack_target;

	events.Log( "o GetEpicWeaponDevastatingCritical: %08lx\nitm: %08lx\ntarget: %08lx\nbaseitem: %i\n", cre->cs_original->obj.obj_generic.obj_id, events.EP->oTarget[0], events.EP->oTarget[1], events.EP->nData[0] );

	int nReturn = events.FireEvent( cre->cs_original->obj.obj_generic.obj_id ); 
	
	if( nReturn != 0x80000000 ){
		return nReturn;
	}

	return CNWSCreatureStats__GetEpicWeaponDevastatingCriticalNext( cre, NULL, itm );
}

int __fastcall CNWSCreature__ToggleMode( CNWSCreature * cre, void *, unsigned char arg1 ){
	

	if (!cre || arg1 == 1 || arg1 == 0)
		return CNWSCreature__ToggleModeNext(cre, NULL, arg1);

	events.Log( "o ToggleMode: %08lx\nMode: %i\n", cre->obj.obj_generic.obj_id , arg1 );

	events.EP->EVENT = EVENT_TOGGLEMODE;
	events.EP->nData[0] = arg1;

	int nReturn = events.FireEvent( cre->obj.obj_generic.obj_id ); 
	
	if( nReturn != 0x80000000 ){
		return nReturn;
	}

	return CNWSCreature__ToggleModeNext( cre, NULL, arg1 );
}

int __fastcall CNWSCreature__AddAttackActions(CNWSCreature * cre, void*, unsigned long target, int nArg1, int nArg2, int nPriority ){
	
	if (!cre)
		return CNWSCreature__AddAttackActionsNext(cre, NULL, target, nArg1, nArg2, nPriority);

	events.EP->EVENT=EVENT_ATTACK;
	events.EP->nData[0]=nArg1;
	events.EP->nData[1]=nArg2;
	events.EP->nData[2]=nPriority;
	events.EP->oTarget[0]=target;

	events.Log( "o AddAttackActions: %08lx\nTarget: %08lx\nArg1: %i\nArg2: %i\nPriority: %i\n", cre->obj.obj_generic.obj_id, target, nArg1, nArg2, nPriority );

	int nReturn = events.FireEvent( cre->obj.obj_generic.obj_id ); 
	
	if( nReturn != 0x80000000 ){
		return nReturn;
	}

	return CNWSCreature__AddAttackActionsNext( cre, NULL, target, nArg1, nArg2, nPriority );
}

int __fastcall CNWSCreature__Polymorph( CNWSCreature * cre, void*, int nPoly, CGameEffect * eff, int nLocked ){

	if( eff && cre ){
	
		events.Log( "o Polymorph: %08X %i\n", cre->obj.obj_generic.obj_id, eff->eff_integers[0] );

		events.EP->oTarget[0] = eff->eff_creator;
		events.EP->nData[0] = eff->eff_integers[0];
		events.EP->nData[1] = eff->eff_casterlevel;
		events.EP->nData[2] = eff->eff_dursubtype;
		events.EP->EVENT = EVENT_POLYMORPH;

		int nReturn = events.FireEvent( cre->obj.obj_generic.obj_id ); 
	
		if( nReturn != 0x80000000 ){
			return nReturn;
		}
	}
	return CNWSCreature__PolymorphNext( cre, NULL, nPoly, eff, nLocked );
}

int __fastcall CNWSCreature__UnPolymorph( CNWSCreature * cre, void*, CGameEffect * eff ){

	if( eff && cre ){

		events.Log( "o UnPolymorph: %08X %i\n", cre->obj.obj_generic.obj_id, eff->eff_integers[0] );

		events.EP->oTarget[0] = eff->eff_creator;
		events.EP->nData[0] = eff->eff_integers[0];
		events.EP->nData[1] = eff->eff_casterlevel;
		events.EP->nData[2] = eff->eff_dursubtype;
		events.EP->EVENT = EVENT_UNPOLYMORPH;

		int nReturn = events.FireEvent( cre->obj.obj_generic.obj_id ); 
	
		if( nReturn != 0x80000000 ){
			return nReturn;
		}
	}
	return CNWSCreature__UnPolymorphNext( cre, NULL, eff );
}

int __fastcall CNWSCreature__UseSkill( CNWSCreature * cre, void *, unsigned char skill, unsigned char arg2, nwn_objid_t target, Vector vTarget, nwn_objid_t target2, nwn_objid_t target3, int arg6 ){

	if (!cre)
		return CNWSCreature__UseSkillNext(cre, NULL, skill, arg2, target, vTarget, target2, target3, arg6);

	//Set the parameters	
	events.EP->lTarget[0] = vTarget;
	events.EP->nData[0] = skill;
	events.EP->nData[1] = arg2;
	events.EP->nData[2] = arg6;
	events.EP->oTarget[0] = target;
	events.EP->oTarget[1] = target2;

	events.EP->EVENT=EVENT_SKILLUSE;

	events.Log( "o UseSkill: %08lx\nskill: %i\narg2: %i\narg3: %08lx\nvTarget X: %f Y: %f Z: %f\ntarget2: %08lx\ntarget3: %08lx\narg6: %i\n", cre->obj.obj_generic.obj_id, skill, arg2, target, vTarget.x, vTarget.y, vTarget.z, target2, target3, arg6 );

	int nReturn = events.FireEvent( cre->obj.obj_generic.obj_id ); 
	
	if( nReturn != 0x80000000 ){
		return nReturn;
	}

	return CNWSCreature__UseSkillNext( cre, NULL, skill, arg2, target, vTarget, target2, target3, arg6 );
}

int __fastcall CNWSCreature__UseFeat( CNWSCreature * cre, void *, unsigned short feat, unsigned short arg2, nwn_objid_t target, nwn_objid_t target2, Vector * vTFarget ){

	if (!cre)
		return CNWSCreature__UseFeatNext(cre, NULL, feat, arg2, target, target2, vTFarget);

	//Set the parameters
	if( vTFarget <= NULL ){
		events.EP->lTarget[0].x = 0.0;
		events.EP->lTarget[0].y = 0.0;
		events.EP->lTarget[0].z = 0.0;
	}
	else{
		events.EP->lTarget[0] = *vTFarget;	
	}

	events.EP->EVENT = EVENT_FEATUSE;
	events.EP->nData[0] = feat;
	events.EP->nData[1] = arg2;
	events.EP->oTarget[0] = target;
	events.EP->oTarget[1] = target2;

	events.Log( "o UseFeat: %08lx\nfeat: %i\narg2: %i\ntarget: %08lx\ntarget2: %08lx\nvTarget X: %f Y: %f Z: %f\n", cre->obj.obj_generic.obj_id, feat, arg2, target, target2, events.EP->lTarget[0].x, events.EP->lTarget[0].y, events.EP->lTarget[0].z ); 

	int nReturn = events.FireEvent( cre->obj.obj_generic.obj_id ); 
	
	if( nReturn != 0x80000000 ){
		return nReturn;
	}

	return CNWSCreature__UseFeatNext( cre, NULL, feat,arg2,target,target2,vTFarget);
}

int __fastcall CNWSCreature__AddItemCastSpellActions( CNWSCreature * cre, void *, nwn_objid_t oItem, int arg1, int arg2, Vector vTarget, nwn_objid_t oTarget, int arg3 ){


	if (!cre)
		return CNWSCreature__AddItemCastSpellActionsNext(cre, NULL, oItem, arg1, arg2, vTarget, oTarget, arg3);

	events.Log( "o AddCastItemSpellActions: %08lx\nItem: %08lx\narg1: %i\narg1: %i\nvTarget X: %f Y: %f Z: %f\nTarget: %08lx\narg3: %i\n", cre->obj.obj_generic.obj_id , oItem, arg1, arg2, vTarget.x, vTarget.y, vTarget.z, oTarget, arg3 ); 

	events.EP->oTarget[0]=oItem;
	events.EP->oTarget[1]=oTarget;
	events.EP->lTarget[0]=vTarget;
	events.EP->nData[0]=arg1;
	events.EP->nData[1]=arg2;
	events.EP->nData[2]=arg3;
	events.EP->EVENT=EVENT_ITEMCAST;

	int nReturn = events.FireEvent( cre->obj.obj_generic.obj_id ); 
	
	if( nReturn != 0x80000000 ){
		return nReturn;
	}

	return CNWSCreature__AddItemCastSpellActionsNext( cre, NULL, oItem, arg1, arg2, vTarget, oTarget, arg3 );
}

int __fastcall CNWSCreature__AddCastSpellActions( CNWSCreature * cre, void *, unsigned long nSpell, int arg2, int arg3, int metamagic, int arg5, Vector vTarget, nwn_objid_t oTarget, int arg7, int arg8, int arg9, unsigned char arg10, int arg11, int arg12, int arg13, unsigned char arg14 ){

	if (!cre)
		return CNWSCreature__AddCastSpellActionsNext(cre, NULL, nSpell, arg2, arg3, metamagic, arg5, vTarget, oTarget, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14);

	events.Log( "o AddCastSpellActions: %08lx\nSpell: %i\nClass Index: %i\narg3: %i\nmetamagic: %i\narg5: %i\nvTarget X: %f Y: %f Z: %f\nTarget: %08lx\narg 7: %i\narg8: %i\narg9: %i\narg10: %i\narg11: %i\narg12: %i\narg13: %i\narg14: %i\n", cre->obj.obj_generic.obj_id , nSpell, arg2, arg3, metamagic, arg5, vTarget.x, vTarget.y, vTarget.z, oTarget, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14 ); 
	
	events.EP->nData[0]=nSpell;
	events.EP->nData[1]=metamagic;
	events.EP->nData[2]=arg2+1; //Class Index
	events.EP->lTarget[0]=vTarget;
	events.EP->EVENT=EVENT_SPELLCAST;

	int nReturn = events.FireEvent( cre->obj.obj_generic.obj_id ); 
	
	if( nReturn != 0x80000000 ){
		return nReturn;
	}

	return CNWSCreature__AddCastSpellActionsNext( cre, NULL, nSpell, arg2, arg3, metamagic, arg5, vTarget, oTarget, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14 );
}

void HookEvents( int nID ){

	DWORD Addr;
	DWORD CallBack;
	DWORD NextHook;
	const char * name;
	char script[30]={0};
	 
	switch( nID ){

		case EVENT_SPELLCAST:
			Addr=0x00491610;
			CallBack=(DWORD)CNWSCreature__AddCastSpellActions;
			NextHook=(DWORD)&CNWSCreature__AddCastSpellActionsNext;
			name="Spellcast";
			break;
		case EVENT_ITEMCAST:
			Addr=0x00491020;
			CallBack=(DWORD)CNWSCreature__AddItemCastSpellActions;
			NextHook=(DWORD)&CNWSCreature__AddItemCastSpellActionsNext;
			name="Itemuse";
			break;
		case EVENT_FEATUSE:
			Addr=0x004A3700;
			CallBack=(DWORD)CNWSCreature__UseFeat;
			NextHook=(DWORD)&CNWSCreature__UseFeatNext;
			name="Featuse";
			break;
		case EVENT_SKILLUSE:
			Addr=0x004A4530;
			CallBack=(DWORD)CNWSCreature__UseSkill;
			NextHook=(DWORD)&CNWSCreature__UseSkillNext;
			name="Skilluse";
			break;
		case EVENT_POLYMORPH:
			Addr=0x004AD960;
			CallBack=(DWORD)CNWSCreature__Polymorph;
			NextHook=(DWORD)&CNWSCreature__PolymorphNext;
			name="Polymorph";
			break;
		case EVENT_UNPOLYMORPH:
			Addr=0x004AE6C0;
			CallBack=(DWORD)CNWSCreature__UnPolymorph;
			NextHook=(DWORD)&CNWSCreature__UnPolymorphNext;
			name="Unpolymorph";
			break;
		case EVENT_ATTACK:
			Addr=0x00493810;
			CallBack=(DWORD)CNWSCreature__AddAttackActions;
			NextHook=(DWORD)&CNWSCreature__AddAttackActionsNext;
			name="Attack";
			break;
		case EVENT_TOGGLEMODE:
			Addr=0x004A5080;
			CallBack=(DWORD)CNWSCreature__ToggleMode;
			NextHook=(DWORD)&CNWSCreature__ToggleModeNext;
			name="Togglemode";
			break;
		case EVENT_DEVCRIT:
			Addr=0x00482280;
			CallBack=(DWORD)CNWSCreatureStats__GetEpicWeaponDevastatingCritical;
			NextHook=(DWORD)&CNWSCreatureStats__GetEpicWeaponDevastatingCriticalNext;
			name="Devcrit";
			break;
		case EVENT_SAVEPC:
			Addr = 0x00435D50;
			CallBack = (DWORD)CNWSPlayer__PackCreatureIntoMessage;
			NextHook = (DWORD)&CNWSPlayer__PackCreatureIntoMessageNext;
			name = "Downloadpc";
			break;
		case EVENT_EXAMINE:
			Addr = 0x00446F60;
			CallBack = (DWORD)CNWSMessage__SendServerToPlayerExamineGui_ItemData;
			NextHook = (DWORD)&CNWSMessage__SendServerToPlayerExamineGui_ItemDataNext;
			name = "Examine";
			break;
		default:
			return;
	}

	CIniFile INI( "NWNX.ini" );

	INI.ReadString( "EVENTS", name, script, 30, "" );

	if( script[0]=='\0' ){

		events.Log( "o %s disabled\n", name );
	}
	else if( HookCode( (PVOID)Addr, (PVOID)CallBack, (PVOID*)NextHook ) ){

		events.Script[nID].CExoStringCpy( script );

		if (nID == EVENT_TOGGLEMODE){
			HookCode((PVOID)0x004BB3F0, (PVOID)CNWSCreature__SetStealthMode, (PVOID*)&CNWSCreature__SetStealthModeNext);
			HookCode((PVOID)0x004BB3D0, (PVOID)CNWSCreature__SetDetectMode, (PVOID*)&CNWSCreature__SetDetectModeNext);
		}
		else if (nID == EVENT_EXAMINE){
			HookCode((PVOID)0x00446B00, (PVOID)CNWSMessage__SendServerToPlayerExamineGui_CreatureData, (PVOID*)&CNWSMessage__SendServerToPlayerExamineGui_CreatureDataNext);
			HookCode((PVOID)0x004474B0, (PVOID)CNWSMessage__SendServerToPlayerExamineGui_PlaceableData, (PVOID*)&CNWSMessage__SendServerToPlayerExamineGui_PlaceableDataNext);
			HookCode((PVOID)0x004475E0, (PVOID)CNWSMessage__SendServerToPlayerExamineGui_TrapData, (PVOID*)&CNWSMessage__SendServerToPlayerExamineGui_TrapDataNext);
			HookCode((PVOID)0x00447970, (PVOID)CNWSMessage__SendServerToPlayerExamineGui_DoorData, (PVOID*)&CNWSMessage__SendServerToPlayerExamineGui_DoorDataNext);
		}

		events.Log( "o Hooked %s -> %s\n", name, events.Script[nID].CStr() );
	}
	else{

		events.Log( "! Failed to hook %s -> %s\n", name, script );
	}	
}