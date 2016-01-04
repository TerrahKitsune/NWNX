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

#include "NWNXMinipatch.h"
#include "../NWNXdll/madCHook.h"
#include "../NWNXdll/IniFile.h"
#include <math.h>
#include <time.h>

#pragma comment(lib, "madChook.lib")

extern CNWNXMinipatch minipatch;

void(__fastcall *CNWSCreature__BroadcastAttackOfOpportunityNext)(CNWSObject * obj, void *, nwn_objid_t target, int arg);
void __fastcall CNWSCreature__BroadcastAttackOfOpportunity(CNWSObject * obj, void *, nwn_objid_t target, int arg){

	if (target==OBJECT_INVALID)
		return CNWSCreature__BroadcastAttackOfOpportunityNext(obj, NULL, target, arg);

	//minipatch.Log("o BroadcastAttackOfOpportunity: %08X -> %08X\n", obj->obj_generic.obj_id, target);

	obj->obj_vartable.SetObject(*minipatch.AoOLocOjb, target);
	minipatch.inScript = true;
	(*NWN_VirtualMachine)->Runscript(minipatch.AoOEvent, obj->obj_generic.obj_id);
	minipatch.inScript = false;

	if (obj->obj_vartable.GetInt(*minipatch.AoOLocInt) > 0)
		return;

	return CNWSCreature__BroadcastAttackOfOpportunityNext(obj, NULL, target, arg);
}

void (__fastcall *CNWSObject__ClearAllPartyInvalidActionsNext)( CNWSObject * obj, void *);
void __fastcall CNWSObject__ClearAllPartyInvalidActions( CNWSObject * obj, void *){
	
	if( obj != NULL && obj->obj_generic.obj_type2 == CGameObject__OBJECT_TYPE_CREATURE )
		return;

	CNWSObject__ClearAllPartyInvalidActionsNext( obj, NULL );
}

int (__fastcall * CNWSItem__MergeItemNext)( CNWSItem * pThis, void*, CNWSItem * itm );
int __fastcall CNWSItem__MergeItem( CNWSItem * pThis, void*, CNWSItem * itm ){

	DWORD (__thiscall *CNWBaseItemArray__GetBaseItem)(void *pTHIS, DWORD arg1) = (DWORD (__thiscall*)(void *pTHIS, DWORD arg1))0x00512AF0;

	DWORD * g_pRules = (DWORD*)0x0066C060;

	DWORD baseItem = CNWBaseItemArray__GetBaseItem( (void*)(*(DWORD *)(*g_pRules + 0x18)), pThis->it_baseitemtype );

	WORD StackSizeMax = *(WORD *)(baseItem + 36);

	if( StackSizeMax <= 1 )
		return 0;
	else
		StackSizeMax = 0xFFFF;

	if( !pThis->CompareItem( itm ) ){
		return 0;
	}

	DWORD PrimaryStack = pThis->it_stacksize;
	DWORD NewSize = itm->it_stacksize + PrimaryStack;

	if( NewSize > StackSizeMax ){

		pThis->it_stacksize = StackSizeMax;
		itm->it_stacksize += PrimaryStack - StackSizeMax;
		return 0;
	}

	pThis->it_stacksize = NewSize;
	return 1;
}

char(__fastcall * CNWSCreatureStats__GetSkillRankNext)(CNWSCreatureStats * pThis, void*, unsigned char skill, CNWSObject * obj, int nBase);
char __fastcall CNWSCreatureStats__GetSkillRank(CNWSCreatureStats * pThis, void*, unsigned char skill, CNWSObject * obj, int nBase){

	char ret = CNWSCreatureStats__GetSkillRankNext(pThis, NULL, skill, obj, nBase);

	if (nBase)
		return ret;

	if (skill == 17){

		//Creature has TS effect
		if (pThis && pThis->cs_original && pThis->cs_original->cre_vision_type & 4){
			
			int nLevel = pThis->GetLevel(true);		
			int fECL = 0;
				
			if (minipatch.ECLVar)
				fECL = (int)pThis->cs_original->obj.obj_vartable.GetFloat( *minipatch.ECLVar );

			int nTotal = (nLevel + fECL) / 2;
			
			if (ret + nTotal > 127)
				ret = 127;
			else
				ret = ret + nTotal;

			//minipatch.Log("Spot: %d\n", ret);
		}
	}

	return ret;
}

int lock=0;

int(__fastcall * CNetLayerWindow__UnpacketizeFullMessagesNext)(CNWSItem * pThis, void*, int n);
int __fastcall CNetLayerWindow__UnpacketizeFullMessages(CNWSItem * pThis, void*, int n){

	int result = CNetLayerWindow__UnpacketizeFullMessagesNext(pThis,NULL,n);
	if (n==1 && result == 1){

		lock++;

		if (lock > 1000000){
			minipatch.Log("! CNetLayerWindow::UnpacketizeFullMessages(%d): FORCE UNHANG!\n", n);
			lock = 0;
			return 0;
		}
	}
	else
		lock = 0;

	return result;
}

int(__fastcall *CNWSModule__AIUpdateNext)(CNWSModule * mod);
int runLock;

int __fastcall CNWSModule__AIUpdate(CNWSModule * mod){

	int ret = CNWSModule__AIUpdateNext(mod);
	if (ret == 1 && !runLock) {
		runLock = 1;
		int ret2 = CNWSModule__AIUpdate(mod);
		runLock = 0;
		//cmem.Log("! AIUpdate lock: %d\n", ret2);
		return ret2;
	}
	return ret;
}

int test=0;

bool(__fastcall *CNWSMessage__TestObjectVisibleNext)(CNWSMessage* pThis, void*, CNWSCreature * creTrg, CNWSCreature * creOri);
bool __fastcall CNWSMessage__TestObjectVisible(CNWSMessage* pThis, void*, CNWSCreature * creTrg, CNWSCreature * creOri){

	bool nRet = CNWSMessage__TestObjectVisibleNext(pThis, NULL, creTrg, creOri);
	if (nRet)
		return true;
	//No DM vision, no self vision, no non creatures, no magic invis
	if (creTrg == creOri || 
		creOri->obj.obj_generic.obj_type2 != 5 || 
		creTrg->obj.obj_generic.obj_type2 != 5 ||
		creTrg->GetInvisible(&creOri->obj, 0) || 
		creTrg->IsDM() || 
		creOri->IsDM() )
		return nRet;

	//minipatch.Log("test: %d\n", test++);

	float fRange = 0.0;

	if (minipatch.ECLVar)
		fRange = fRange = creOri->obj.obj_vartable.GetFloat(*minipatch.RangeVar);

	if ( fRange > 1.0/*(creOri->cre_vision_type & 4)*/){

		CScriptLocation lOrigo;
		CScriptLocation lTarget;

		creOri->obj.GetScriptLocation(&lOrigo);
		creTrg->obj.GetScriptLocation(&lTarget);

		Vector vDist;
		vDist.x = lOrigo.loc_position.x - lTarget.loc_position.x;
		vDist.y = lOrigo.loc_position.y - lTarget.loc_position.y;
		vDist.z = lOrigo.loc_position.z - lTarget.loc_position.z;

		vDist.x = vDist.x * vDist.x;
		vDist.y = vDist.y * vDist.y;
		vDist.z = vDist.z * vDist.z;

		double dist = sqrt(vDist.x + vDist.y + vDist.z);

		return dist <= fRange;
	}

	return false;
}

void *(__fastcall *CNWSModule__GetPlayerTURDFromListNext)(CNWSModule* pThis, void*, CNWSPlayer * ply);
void * __fastcall CNWSModule__GetPlayerTURDFromList(CNWSModule* pThis, void*, CNWSPlayer * ply){

	minipatch.lastturd = time(NULL);
	minipatch.Log("o GetPlayerTURDFromList: %08X - %08X\n",pThis,ply);

	return CNWSModule__GetPlayerTURDFromListNext(pThis,NULL,ply);
}

BOOL CNWNXMinipatch::OnRelease( ){

	if (ECLVar){

		if (ECLVar->text)
			mem.nwnx_free(ECLVar->text);
		mem.nwnx_free(ECLVar);
	}

	if (RangeVar){

		if (RangeVar->text)
			mem.nwnx_free(RangeVar->text);
		mem.nwnx_free(RangeVar);
	}

	Log("o Shutdown! %ld\n", time(NULL)-lastturd);

	return CNWNXBase::OnRelease();
}

BOOL CNWNXMinipatch::OnCreate(const char* LogDir){

	lastturd = time(NULL);
	inScript = false;

	char log[MAX_PATH];
	char logmove[MAX_PATH];
	sprintf(log, "%s\\nwnx_minipatch.txt", LogDir);
	sprintf(logmove, "%s\\1\\nwnx_minipatch.txt", LogDir);

	MoveFile(log, logmove);
	// call the base class function
	if( !CNWNXBase::OnCreate( log ) )
		return false;

	WriteLogHeader( );

	CIniFile ini("nwnx.ini");

	if (ini.ReadBool("MINIPATCH","WeaponColor",true))
		RemoveCopyItemAndModifyColorCap( );
	
	if (ini.ReadBool("MINIPATCH", "PartyActionQueue", true)){
		if (HookCode((PVOID)0x004DFA60, CNWSObject__ClearAllPartyInvalidActions, (PVOID*)&CNWSObject__ClearAllPartyInvalidActionsNext))
			Log("o Hooked ClearPartyInvalidActions!\n");
		else
			Log("! Failed to hook ClearPartyInvalidActions!\n");
	}

	if (ini.ReadBool("MINIPATCH", "AOO", false)){

		minipatch.AoOEvent = (CExoString*)minipatch.mem.nwnx_malloc(sizeof(CExoString));
		ZeroMemory(minipatch.AoOEvent, sizeof(CExoString));
		minipatch.AoOEvent->CExoStringCpy("nwnx_aoo");

		minipatch.AoOLocInt = (CExoString*)minipatch.mem.nwnx_malloc(sizeof(CExoString));
		ZeroMemory(minipatch.AoOLocInt, sizeof(CExoString));
		minipatch.AoOLocInt->CExoStringCpy("AOO_FLAG");

		minipatch.AoOLocOjb = (CExoString*)minipatch.mem.nwnx_malloc(sizeof(CExoString));
		ZeroMemory(minipatch.AoOLocOjb, sizeof(CExoString));
		minipatch.AoOLocOjb->CExoStringCpy("AOO_TARGET");


		if (HookCode((PVOID)0x004A27C0, CNWSCreature__BroadcastAttackOfOpportunity, (PVOID*)&CNWSCreature__BroadcastAttackOfOpportunityNext))
			Log("o Hooked BroadcastAttackOfOpportunity!\n");
		else
			Log("! Failed to hook BroadcastAttackOfOpportunity!\n");
	}

	if (ini.ReadBool("MINIPATCH", "UnlimitedMerge", false)){
		if (HookCode((PVOID)0x004FEC40, CNWSItem__MergeItem, (PVOID*)&CNWSItem__MergeItemNext)){
			Log("o Hooked MergeItem!\n");
			RemoveSetStacksizeCap();
		}
		else
			Log("! Failed to hook MergeItem!\n");
	}

	if (ini.ReadBool("MINIPATCH", "AllChat", false)){
		RemoveChatListenRestrictions();
	}

	if (ini.ReadBool("MINIPATCH", "TSNoStealth", false)){
		TrueSightNoStealth();

		if (ini.ReadBool("MINIPATCH", "SpotBonusForTS", false)){
			SpotBonusForTS();
		}
		if (ini.ReadBool("MINIPATCH", "BlindSight", false)){

			RangeVar = (CExoString*)minipatch.mem.nwnx_malloc(sizeof(CExoString));
			ZeroMemory(RangeVar, sizeof(CExoString));
			RangeVar->CExoStringCpy("TS_RANGE");

			if (HookCode((PVOID)0x004409F0, CNWSMessage__TestObjectVisible, (PVOID*)&CNWSMessage__TestObjectVisibleNext))
				Log("o Hooked TestObjectVisible!\n");
			else
				Log("! Failed to hook TestObjectVisible!\n");
		}
	}
	
	if (ini.ReadBool("MINIPATCH", "AIUpdateHangCheck", true)){

		if (HookCode((PVOID)0x004D1D00, CNWSModule__AIUpdate, (PVOID*)&CNWSModule__AIUpdateNext))
			Log("o Hooked CNWSModule::AIUpdate!\n");
		else
			Log("! Failed to hook CNWSModule::AIUpdate!\n");
	}

	if (ini.ReadBool("MINIPATCH", "MessagesHangCheck", true)){

		if(HookCode((PVOID)0x004208C0, CNetLayerWindow__UnpacketizeFullMessages, (PVOID*)&CNetLayerWindow__UnpacketizeFullMessagesNext))
			Log("o Hooked CNetLayerWindow::UnpacketizeFullMessages!\n");
		else
			Log("! Failed to CNetLayerWindow::UnpacketizeFullMessages!\n");
	}

	if (ini.ReadBool("MINIPATCH", "TurdFix", true)){

		if (HookCode((PVOID)0x004D7330, CNWSModule__GetPlayerTURDFromList, (PVOID*)&CNWSModule__GetPlayerTURDFromListNext))
			Log("o Hooked CNWSModule::GetPlayerTURDFromList!\n");
		else
			Log("! Failed to CNWSModule::GetPlayerTURDFromList!\n");
	}

	return true;
}

void CNWNXMinipatch::WriteLogHeader( ){

	fprintf( m_fFile, "NWNXMinipatch v0.53 created by Terra_777\n\n" );
	fflush( m_fFile );	
}

void CNWNXMinipatch::Log( const char * formatting, ... ){

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

void CNWNXMinipatch::SpotBonusForTS(){

	ECLVar =  (CExoString*)minipatch.mem.nwnx_malloc(sizeof(CExoString));
	ZeroMemory(ECLVar, sizeof(CExoString));
	ECLVar->CExoStringCpy("CS_ECL");

	if (HookCode((PVOID)0x0047E2C0, CNWSCreatureStats__GetSkillRank, (PVOID*)&CNWSCreatureStats__GetSkillRankNext))
		Log("o Hooked GetSkillRank!\n");
	else
		Log("! Failed to hook GetSkillRank!\n");
}

void CNWNXMinipatch::RemoveCopyItemAndModifyColorCap( void ){

	//Absorlute address
	DWORD * CopyItemAndModifyCap = (DWORD*)0x0058A872;
	DWORD DefaultPrivs;
	unsigned char * func = (unsigned char*)CopyItemAndModifyCap;

	//Make the memoryspace writeable
	VirtualProtect( CopyItemAndModifyCap, 1, PAGE_EXECUTE_READWRITE, &DefaultPrivs );
	
	//0x0058A871: BA [04] 00 00 00: mov edx 0x04
	func[0]=0xFF; //<- set cap to 255

	//Restore the privs back to default
	VirtualProtect( CopyItemAndModifyCap, 1, DefaultPrivs, NULL );

	Log( "o Removed CopyItemAndModify weapon color cap!\n" );
}

void CNWNXMinipatch::RemoveSetStacksizeCap( void ){

	DWORD privs;
	unsigned char * func = (unsigned char*)0x0058E1CA;

	VirtualProtect( func, 4, PAGE_EXECUTE_READWRITE, &privs ); 
	//0058E1C9      66:8B48 24    MOV CX,WORD PTR DS:[EAX+24]
	//0058E1C9      66:B9 FFFF    MOV CX,7FFF
	func[0]=0xB9;
	func[1]=0xFF;
	func[2]=0xFF;

	VirtualProtect( func, 4, privs, NULL );

	Log( "o Removed SetStackSize cap!\n" );
}

void CNWNXMinipatch::TrueSightNoStealth(){

	DWORD privs;
	unsigned char * func = (unsigned char*)0x004A02BD;

	VirtualProtect(func, 3, PAGE_EXECUTE_READWRITE, &privs);

	func[0] = 0x90;
	func[1] = 0x90;
	func[2] = 0x90;

	VirtualProtect(func, 3, privs, NULL);

	func = (unsigned char*)0x004A0379;

	VirtualProtect(func, 8, PAGE_EXECUTE_READWRITE, &privs);

	func[0] = 0x90;
	func[1] = 0x90;
	func[2] = 0x90;
	func[3] = 0x90;
	func[4] = 0x90;
	func[5] = 0x90;
	func[6] = 0x90;
	func[7] = 0x90;

	VirtualProtect(func, 8, privs, NULL);

	Log("o TrueSightNoStealth: Trueseeing no longer detects stealth!\n");
}

void CNWNXMinipatch::RemoveChatListenRestrictions( ){

	DWORD privs;
	unsigned char * func = (unsigned char*)0x0043CD01;

	VirtualProtect( func, 7, PAGE_EXECUTE_READWRITE, &privs ); 

	func[0]=0xEB;
	func[1]=0x2A;
	func[2]=0x90;
	func[3]=0x90;
	func[4]=0x90;
	func[5]=0x90;
	func[6]=0x90;

	VirtualProtect( func, 7, privs, NULL );

	func = (unsigned char*)0x0043CED1;

	VirtualProtect( func, 7, PAGE_EXECUTE_READWRITE, &privs ); 

	func[0]=0xEB;
	func[1]=0x2A;
	func[2]=0x90;
	func[3]=0x90;
	func[4]=0x90;
	func[5]=0x90;
	func[6]=0x90;

	VirtualProtect( func, 7, privs, NULL );

	Log( "o RemoveChatListenRestrictions: visible checks removed from chat!\n" );
}
/*----------------
--Script interface
----------------*/

unsigned long CNWNXMinipatch::OnRequestObject (char *gameObject, char* Request){

	Log( "o OnRequestObject: 0x08X: %s( )", gameObject, Request );

	return OBJECT_INVALID;
}

char* CNWNXMinipatch::OnRequest(char *gameObject, char* Request, char* Parameters){


	Log( "o OnRequest: 0x%08X: %s( %s )", gameObject, Request, Parameters );
	
	return NULL;
}

/*----------------
--Patch functions
----------------*/