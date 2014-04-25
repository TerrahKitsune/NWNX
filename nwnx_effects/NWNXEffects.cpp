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

#include "NWNXEffects.h"
#include "../NWNXdll/madCHook.h"
#include "../NWNXdll/IniFile.h"

#pragma comment(lib, "madChook.lib")

extern CNWNXEffects effects;

int (__fastcall * CNWVirtualMachineCommands__ExecuteCommandSetSubTypeNext)( void * pThis, void*, int a1, int a2 );
int __fastcall CNWVirtualMachineCommands__ExecuteCommandSetSubType( void * pThis, void*, int a1, int a2 ){

	DWORD Order = effects.Order;
	effects.Order=0;

	if( Order > 0 ){

		CGameEffect * eEffect = (CGameEffect *)pThis;
		CGameEffect * eTemp;
		if( (*NWN_VirtualMachine)->StackPopEngineStructure( 0, (void**)&eEffect ) ){

			//Stuff here

			if( Order == 1 ){ //Set spell ID
		
				eEffect->eff_spellid = *(int*)effects.data;
				effects.Log( "o SetEffectSpellID: %i\n", eEffect->eff_spellid );
			}
			else if( Order == 2 ){ //Set effect script

				effects.PrimeScript( eEffect );
			}
			else if( Order == 3 ){ //Set casterlevel
		
				eEffect->eff_casterlevel = *(int*)effects.data;
				effects.Log( "o SetEffectCasterLevel: %i\n", eEffect->eff_casterlevel );
			}
			else if( Order == 4 ){ //Set creator
		
				eEffect->eff_creator = *(nwn_objid_t*)effects.data;
				effects.Log( "o SetEffectCreator: %0X\n", eEffect->eff_creator );
			}
			else if( Order == 5 ){ //Custom effect

				eEffect->SetInteger( 0, 5 );
				eEffect->SetInteger( 1, effects.param );
				effects.Log( "o GetCustomEffect: %i\n", eEffect->eff_id );			
			}
			else if( Order == 6 ){ //Custom effect

				eEffect->SetInteger( 0, effects.param );
				eEffect->SetInteger( 1, 8 );
				effects.Log( "o GetCustomEffect: %i\n", eEffect->eff_id );			
			}

			//Homebrewn unconditional update
			effects.NormalizeLink( eEffect );

			//Endstuff here		

			if( *((WORD*)eEffect+0x4) == 0x28 ){
	
				eEffect->UpdateLinked();	
				eTemp = eEffect;
			}
			else
				eTemp = eEffect;

			if( (*NWN_VirtualMachine)->StackPushEngineStructure( 0, eTemp ) ){
				
				eEffect->dtor();			
				effects.mem.nwnx_free( eEffect );
				return 0;
			}
		}
		
		return -638;
	}

	return CNWVirtualMachineCommands__ExecuteCommandSetSubTypeNext( pThis, NULL, a1, a2 );
}

int (__fastcall *CNWSEffectListHandler__OnEffectRemovedNext)( void * pThis, void*, CNWSObject * obj, CGameEffect * eff );
int __fastcall CNWSEffectListHandler__OnEffectRemoved( void * pThis, void*, CNWSObject * obj, CGameEffect * eff ){

	effects.ExecuteScript( eff, obj->obj_generic.obj_id );

	return CNWSEffectListHandler__OnEffectRemovedNext( pThis, NULL, obj, eff );
}

int (__fastcall * CNWVirtualMachineCommands__ExecuteCommandGetEffectDurationTypNext)( void * pThis, void*, int a1, int a2 );
int __fastcall CNWVirtualMachineCommands__ExecuteCommandGetEffectDurationTyp( void * pThis, void*, int a1, int a2 ){

	DWORD Order = effects.Order;
	effects.Order=0;

	if( Order > 0 ){

		CGameEffect * eEffect = 0;
		if( (*NWN_VirtualMachine)->StackPopEngineStructure( 0, (void**)&eEffect ) ){

			int nData = 0;

			if( Order == 1 )//id 1
				nData = eEffect->eff_id;
			else if( Order == 2 )//id 2
				nData = eEffect->eff_id_02;
			else if( Order == 3 )//Casterlevel
				nData = eEffect->eff_casterlevel;
			else if( Order == 4 )//lastid
				nData = effects.last;
			else if( Order == 5 )
				nData = eEffect->GetInteger( effects.param );

			if( (*NWN_VirtualMachine)->StackPushInteger( nData ) ){

				CGameEffect * eTemp = eEffect;
				if( eEffect ){
					eEffect->dtor( );
					effects.mem.nwnx_free( eEffect );
				}
				return 0;
			}
			else 
				return -638;
		}
		else 
			return -638;
	}

	return CNWVirtualMachineCommands__ExecuteCommandGetEffectDurationTypNext( pThis, NULL, a1, a2 );
}

int (__fastcall * CNWSCreature__GetArmorClassNext)( CNWSCreature * cre, void*);
int __fastcall CNWSCreature__GetArmorClass( CNWSCreature * cre, void*){

	int nAC = CNWSCreature__GetArmorClassNext( cre, NULL );
	
	nAC += effects.CalculateUnconditionalAC( cre );

	return nAC;
}

int (__fastcall * CNWSCreatureStats__GetArmorClassVersusNext)( CNWSCreatureStats * cre, void*, CNWSCreature * target, int a1 );
int __fastcall CNWSCreatureStats__GetArmorClassVersus( CNWSCreatureStats * cre, void*, CNWSCreature * target, int a1){

	int nAC = CNWSCreatureStats__GetArmorClassVersusNext( cre, NULL, target, a1 );
	
	nAC += effects.CalculateUnconditionalAC( cre->cs_original );

	return nAC;
}

int (__fastcall * CNWSCreatureStats__GetAttackModifierVersusNext)( CNWSCreatureStats * cre, void*, CNWSCreature * target );
int __fastcall CNWSCreatureStats__GetAttackModifierVersus( CNWSCreatureStats * cre, void*, CNWSCreature * target ){

	int nAB = CNWSCreatureStats__GetAttackModifierVersusNext( cre, NULL, target );
	
	nAB += effects.CalculateUnconditionalAB( cre->cs_original );

	return nAB;
}

int (__fastcall * CNWSCreatureStats__GetMeleeAttackBonusNext)( CNWSCreatureStats * cre, void*, int a2, int a3, int a4 );
int __fastcall CNWSCreatureStats__GetMeleeAttackBonus( CNWSCreatureStats * cre, void*, int a2, int a3, int a4 ){

	int nAB = CNWSCreatureStats__GetMeleeAttackBonusNext( cre,NULL,a2,a3,a4 );

	nAB += effects.CalculateUnconditionalAB( cre->cs_original );

	return nAB;
}

int (__fastcall * CNWSCreatureStats__GetRangedDamageBonusNext)( CNWSCreatureStats * cre, void* );
int __fastcall CNWSCreatureStats__GetRangedDamageBonus( CNWSCreatureStats * cre, void* ){

	int nAB = CNWSCreatureStats__GetRangedDamageBonusNext( cre, NULL );

	nAB += effects.CalculateUnconditionalAB( cre->cs_original );

	return nAB;
}

CNWNXEffects::~CNWNXEffects(){OnRelease( );}

BOOL CNWNXEffects::OnRelease( ){

	return CNWNXBase::OnRelease();
}

BOOL CNWNXEffects::OnCreate(const char* LogDir){

	char log[MAX_PATH];
	
	sprintf( log, "%s\\nwnx_effects.txt", LogDir);

	// call the base class function
	if( !CNWNXBase::OnCreate( log ) )
		return false;

	bLog=1;

	data = malloc(sizeof(int));
	ESArray=NULL;
	ESArrayLen=0;
	ACHook=false;
	ABHook=false;
	WriteLogHeader( );

	if( HookCode( (PVOID)0x00571160, CNWVirtualMachineCommands__ExecuteCommandSetSubType, (PVOID*)&CNWVirtualMachineCommands__ExecuteCommandSetSubTypeNext ) )
		Log( "o Hooked int CNWVirtualMachineCommands::ExecuteCommandSetSubType\n" );
	else
		Log( "! Failed to hook int CNWVirtualMachineCommands::ExecuteCommandSetSubType\n" );

	if( HookCode( (PVOID)0x004EB5A0, CNWSEffectListHandler__OnEffectRemoved, (PVOID*)&CNWSEffectListHandler__OnEffectRemovedNext ) )
		Log( "o Hooked int CNWSEffectListHandler::OnEffectRemoved\n" );
	else
		Log( "! Failed to hook int CNWSEffectListHandler::OnEffectRemoved\n" );

	if( HookCode( (PVOID)0x00570030, CNWVirtualMachineCommands__ExecuteCommandGetEffectDurationTyp, (PVOID*)&CNWVirtualMachineCommands__ExecuteCommandGetEffectDurationTypNext ) )
		Log( "o Hooked int CNWSEffectListHandler::ExecuteCommandGetEffectDurationTyp\n" );
	else
		Log( "! Failed to hook int CNWSEffectListHandler::ExecuteCommandGetEffectDurationType\n" );

	CIniFile ini( "nwnx.ini" );

	bLog = ini.ReadInteger( "EFFECTS", "LOG", bLog );

	return true;
}

void CNWNXEffects::WriteLogHeader( ){

	fprintf( m_fFile, "NWNXEffects v1.2 created by Terra_777\n\n" );
	fflush( m_fFile );
}

void CNWNXEffects::Log( const char * formatting, ... ){

	if( bLog <= 0 )
		return;

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

unsigned long CNWNXEffects::OnRequestObject (char *gameObject, char* Request){

	return OBJECT_INVALID;
}

char* CNWNXEffects::OnRequest(char *gameObject, char* Request, char* Parameters){

	//Log( "o Request: %s %s\n", Request, Parameters );

	if( strcmp( Request, "SetEffectSpellID" ) == 0 ){

		sscanf( Parameters, "%i", data );
		Order = 1;
	}
	else if( strcmp( Request, "SetEffectScript" ) == 0 ){

		AddScript( Parameters );
		Order = 2;
	}
	else if( strcmp( Request, "SetEffectCasterLevel" ) == 0 ){

		sscanf( Parameters, "%i", data );
		Order = 3;
	}
	else if( strcmp( Request, "GetEffectID" ) == 0 ){

		sscanf( Parameters, "%i", data );
		Order = 1;
		if( *(int*)data==1 )
			Order++;
	}
	else if( strcmp( Request, "SetEffectCreator" ) == 0 ){

		sscanf( Parameters, "%x", data );
		Order = 4;
	}
	else if( strcmp( Request, "GetEffectCasterLevel" ) == 0 ){
		Order=3;
	}
	else if( strcmp( Request, "GetLastEffect" ) == 0 ){
		Order=4;
	}
	else if( strcmp( Request, "UnconditionalAC" ) == 0 ){

		if( !ACHook ){

			if( HookCode( (PVOID)0x004A6E00, CNWSCreature__GetArmorClass, (PVOID*)&CNWSCreature__GetArmorClassNext ) )
				Log( "o Hooked int CNWSCreature::GetArmorClass\n" );
			else
				Log( "! Failed to hook int CNWSCreature::GetArmorClass\n" );
	
			if( HookCode( (PVOID)0x0046DC00, CNWSCreatureStats__GetArmorClassVersus, (PVOID*)&CNWSCreatureStats__GetArmorClassVersusNext ) )
				Log( "o Hooked int CNWSCreatureStats::GetArmorClassVersus\n" );
			else
				Log( "! Failed to hook int CNWSCreatureStats::GetArmorClassVersus\n" );

			ACHook=true;
		}

		param = atoi( Parameters );		
		Order=5;
	}
	else if( strcmp( Request, "GetInt" ) == 0 ){

		param = atoi( Parameters );		
		Order=5;
	}
	else if( strcmp( Request, "GetUAC" ) == 0 ){

		sprintf( Parameters, "%i", CalculateUnconditionalAC( (CNWSCreature *) gameObject ) );
	}
	else if( strcmp( Request, "UnconditionalAB" ) == 0 ){

		if( !ABHook ){

			if( HookCode( (PVOID)0x00470F00, CNWSCreatureStats__GetAttackModifierVersus, (PVOID*)&CNWSCreatureStats__GetAttackModifierVersusNext ) )
				Log( "o Hooked int CNWSCreature::GetAttackModifierVersus\n" );
			else
				Log( "! Failed to hook int CNWSCreature::GetAttackModifierVersus\n" );
	
			if( HookCode( (PVOID)0x0046FB40, CNWSCreatureStats__GetMeleeAttackBonus, (PVOID*)&CNWSCreatureStats__GetMeleeAttackBonusNext ) )
				Log( "o Hooked int CNWSCreatureStats::GetMeleeAttackBonus\n" );
			else
				Log( "! Failed to hook int CNWSCreatureStats::GetMeleeAttackBonus\n" );
		
			if( HookCode( (PVOID)0x00470120, CNWSCreatureStats__GetRangedDamageBonus, (PVOID*)&CNWSCreatureStats__GetRangedDamageBonusNext ) )
				Log( "o Hooked int CNWSCreatureStats::GetRangedDamageBonus\n" );
			else
				Log( "! Failed to hook int CNWSCreatureStats::GetRangedDamageBonus\n" );

			ABHook=true;
		}

		param = atoi( Parameters );		
		Order=6;	
	}
	else if( strcmp( Request, "GetUAB" ) == 0 ){

		sprintf( Parameters, "%i", CalculateUnconditionalAB( (CNWSCreature *) gameObject ) );
	}
	
	return NULL;
}

/*----------------
--Patch functions
----------------*/

void CNWNXEffects::NormalizeLink( CGameEffect * effect ){

	CGameEffect * node = effect->eff_link_id1; 
	while( node ){

		node->eff_casterlevel = effect->eff_casterlevel;
		node->eff_creator = effect->eff_creator;
		node->eff_spellid = effect->eff_spellid;

		node = node->eff_link_id1; 
	}

	node = effect->eff_link_id2; 
	while( node ){

		node->eff_casterlevel = effect->eff_casterlevel;
		node->eff_creator = effect->eff_creator;
		node->eff_spellid = effect->eff_spellid;

		node = node->eff_link_id2; 
	}
}

UINT64 CNWNXEffects::GetEffectID( CGameEffect * effect ){

	UINT64 retrn;
	memcpy( &retrn, effect, sizeof( UINT64 ) );
	return retrn;
}

void CNWNXEffects::AddScript( const char * script ){

	EffectScript * target=NULL;
	for( DWORD n=0;n<ESArrayLen;n++ ){

		if( ESArray[n].script[17] == 0 ){
			target=&ESArray[n];
			break;
		}
	}

	if( target == NULL ){

		void * temp = realloc( ESArray, (ESArrayLen+1)*sizeof( EffectScript ) );

		if( temp == NULL ){

			Log( "! AddScript: realloc failed to resize script array!\n" );
			return;
		}

		ESArray = (EffectScript*)temp;
		target = &ESArray[ESArrayLen++];
		memset( target, 0, sizeof( EffectScript ) );
	}

	strncpy( target->script, script, 16 );
	target->script[16]='\0';
	target->script[17]=1;
}

void CNWNXEffects::PrimeScript( CGameEffect * effect ){

	EffectScript * target=NULL;
	for( DWORD n=0;n<ESArrayLen;n++ ){

		if( ESArray[n].script[17] == 1 ){
			target=&ESArray[n];
			break;
		}
	}

	if( target==NULL )
		return;	

	target->effectID = effect->eff_id;
	target->script[17] = 2;
	Log( "o PrimeScript: %u\n", target->effectID );
}

void CNWNXEffects::ExecuteScript( CGameEffect * effect, nwn_objid_t object_Self ){

	DWORD id = effect->eff_id;
	EffectScript * target=NULL;
	for( DWORD n=0;n<ESArrayLen;n++ ){

		if( ESArray[n].effectID == id && ESArray[n].script[17] == 2 ){
			
			Log( "o ExecuteScript: %s %08X %u\n", ESArray[n].script, object_Self, id );
			last=id;
			ESArray[n].script[17]=0;
			(*NWN_VirtualMachine)->Runscript( &CExoString( ESArray[n].script ), object_Self );			
		}
	}
}

CGameEffect * CNWNXEffects::CreateNewEffect( ){

	CGameEffect * ret = (CGameEffect*)mem.nwnx_malloc( 0x90 );
	if( !ret )
		return NULL;
	return ret->ctor(1);
}

int CNWNXEffects::CalculateUnconditionalAC( CNWSCreature * cre ){

	CGameEffect ** eff = cre->obj.obj_effects;
	DWORD len = cre->obj.obj_effects_len;
	int nType;
	int nTotal = 0;

	if( !eff || len <= 0 )
		return 0;

	for( DWORD n=0;n<len;n++ ){

		if( !eff[n] )
			continue;

		nType = eff[n]->GetScriptEffectType();

		//EFFECT_TYPE_AC_INCREASE
		if( nType == 46 && eff[n]->eff_integers[0]==5 ){
			nTotal += eff[n]->eff_integers[1];
			//Log( "+ %i\n", eff[n]->eff_integers[1] );
		}
		//EFFECT_TYPE_AC_DECREASE
		else if( nType == 47 && eff[n]->eff_integers[0]==5 ){
			nTotal -= eff[n]->eff_integers[1];
			//Log( "- %i\n", eff[n]->eff_integers[1] );
		}

	}

	return nTotal;
}

int CNWNXEffects::CalculateUnconditionalAB( CNWSCreature * cre ){

	CGameEffect ** eff = cre->obj.obj_effects;
	DWORD len = cre->obj.obj_effects_len;
	int nType;
	int nTotal = 0;

	if( !eff || len <= 0 )
		return 0;

	for( DWORD n=0;n<len;n++ ){

		if( !eff[n] )
			continue;

		nType = eff[n]->GetScriptEffectType();

		//EFFECT_TYPE_AB_INCREASE
		if( nType == 40 && eff[n]->eff_integers[1]==8 ){
			nTotal += eff[n]->eff_integers[0];
			//Log( "+ %i\n", eff[n]->eff_integers[1] );
		}
		//EFFECT_TYPE_AB_DECREASE
		else if( nType == 41 && eff[n]->eff_integers[1]==8 ){
			nTotal -= eff[n]->eff_integers[0];
			//Log( "- %i\n", eff[n]->eff_integers[1] );
		}

	}

	return nTotal;
}