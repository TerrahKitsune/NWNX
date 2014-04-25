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

#include "NWNXWeapons.h"
#include "../NWNXdll/madCHook.h"
#include "AssemblyHelper.h"
#include "../NWNXdll/iniFile.h"

#pragma comment(lib, "madChook.lib")

BOOL CNWNXWeapons::OnRelease( ){
	
	if( WB != NULL )
		delete WB;

	if( DevCritScript != NULL )
		free( DevCritScript );

	if( own )
		free( own );

	return CNWNXBase::OnRelease();
}

BOOL CNWNXWeapons::OnCreate(const char* LogDir){


	char log[MAX_PATH];
	
	sprintf( log, "%s\\nwnx_weapons.txt", LogDir);

	// call the base class function
	if( !CNWNXBase::OnCreate( log ) )
		return false;

	WriteLogHeader( );

	WB = new WeaponBehavior;

	if( WB == NULL ){

		Log( "! UNABLE TO CREATE WEAPON TABLE!\n" );
		return false;
	}
	else{

		Log( "BaseItemType Finessable WFOCUS EWFOCUS WSPEC EWSPEC WCHOICE IMPCRIT OCRIT DCRIT CRITMOD CRITTHREAT\n" );

		int nLoop=0;
		WeaponBehavior::WeaponEntry * data = WB->GetFirstNext( true );
		while( data != NULL ){

			if( data->BASE_TYPE != 0xFFFFFFFF )
				Log( "% 12u % 10u % 6u % 7u % 5u % 6u % 7u % 7u % 5u % 5u % 7u % 10u\n", data->BASE_TYPE, data->FINESSABLE, data->WEAPON_FOCUS_FEAT, data->EPIC_WEAPON_FOCUS_FEAT, data->WEAPON_SPECIALIZATION_FEAT, data->EPIC_WEAPON_SPECIALIZATION_FEAT, data->WEAPON_OF_CHOICE_FEAT, data->IMPROVED_CIRITCAL_FEAT, data->OVERWHELMING_CRITICAL_FEAT, data->DEVASTATING_CRITICAL_FEAT, data->CriticalDamageMod, data->CriticalHitThreat );
			
			data = WB->GetFirstNext( false );		
		}
		Log( "\nLoaded: %i rows of data!\n\n", WB->Len );
	}
	DevCritBehavior=0;

	ownLen=0;
	own=0;

	//Signature recognision isnt working properly, force absorlute addresses
	//CIniFile iniFile ("nwnx.ini");

	ISNWSERVER = true;//iniFile.ReadBool( "WEAPONS", "IsServer", false );

	DevCritScript = (char*)calloc( 16,1 );
	strcpy( DevCritScript, "nwnx_devcrit" );
	RemoveCopyItemAndModifyColorCap( );
	ApplyHooks( );

	return true;
}

void CNWNXWeapons::WriteLogHeader( ){

	fprintf( m_fFile, "NWNXWeapons v1.3 created by Terra_777\n\n" );
	fflush( m_fFile );	
}

void CNWNXWeapons::Log( const char * formatting, ... ){

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

DWORD CNWNXWeapons::FindAddress( DWORD pNWSERVER ){

	if( false ){

		FILE * fSignatures = fopen( "signatures.txt", "a" );
		unsigned char * func = (unsigned char *)pNWSERVER;
		fprintf( fSignatures, "0x%08X:", pNWSERVER );
		fflush( fSignatures );
		for( int n=0;n<50;n++ ){

			fprintf( fSignatures, " %02X", func[n] ); 
		}
		fprintf( fSignatures, "\n" );
		fflush( fSignatures );
	}
	
	if( ISNWSERVER )
		return pNWSERVER;

	char Sig[]="0x0058A872: FF 00 00 00 74 21 83 F8 65 74 1C 83 F8 68 74 17 83 F8 1A 75 0B 85 C9 75 22 BA 03 00 00 00 EB 24 83 F8 41 75 16 EB 0F 83 F9 01 74 0A 83 F9 02 BA 08 00\n\
	0x00481210: 56 57 8B F9 6A 2A E8 D5 DA FF FF 85 C0 74 53 8B 74 24 0C 85 F6 0F 84 A9 00 00 00 8B 46 0C 83 F8 4E 0F 84 9D 00 00 00 83 F8 24 0F 84 94 00 00 00 83 F8\n\
	0x004812E0: 8B 44 24 04 85 C0 0F 84 ED 00 00 00 8B 40 0C 83 F8 4E 0F 84 E1 00 00 00 83 F8 24 0F 84 D8 00 00 00 83 F8 6F 0F 87 E2 00 00 00 33 D2 8A 90 94 14 48 00\n\
	0x00481510: 8B 44 24 04 85 C0 0F 84 87 01 00 00 8B 40 0C 83 F8 4E 0F 84 7B 01 00 00 83 F8 24 0F 84 72 01 00 00 83 F8 6F 0F 87 7F 01 00 00 33 D2 8A 90 64 17 48 00\n\
	0x004817E0: 8B 44 24 04 85 C0 0F 84 84 01 00 00 8B 40 0C 83 F8 4E 0F 84 78 01 00 00 83 F8 24 0F 84 6F 01 00 00 83 F8 6F 0F 87 7C 01 00 00 33 D2 8A 90 30 1A 48 00\n\
	0x00481AA0: 8B 44 24 04 85 C0 0F 84 87 01 00 00 8B 40 0C 83 F8 4E 0F 84 7B 01 00 00 83 F8 24 0F 84 72 01 00 00 83 F8 6F 0F 87 7F 01 00 00 33 D2 8A 90 F4 1C 48 00\n\
	0x00481D70: 8B 44 24 04 85 C0 0F 84 FA 00 00 00 8B 40 0C 83 F8 4E 0F 84 EE 00 00 00 83 F8 24 0F 84 E5 00 00 00 83 F8 6F 0F 87 EF 00 00 00 33 D2 8A 90 34 1F 48 00\n\
	0x00481FB0: 8B 44 24 04 85 C0 0F 84 87 01 00 00 8B 40 0C 83 F8 4E 0F 84 7B 01 00 00 83 F8 24 0F 84 72 01 00 00 83 F8 6F 0F 87 7F 01 00 00 33 D2 8A 90 04 22 48 00\n\
	0x00482280: 8B 44 24 04 85 C0 0F 84 87 01 00 00 8B 40 0C 83 F8 4E 0F 84 7B 01 00 00 83 F8 24 0F 84 72 01 00 00 83 F8 6F 0F 87 7F 01 00 00 33 D2 8A 90 D4 24 48 00\n\
	0x0048CB10: 8B 44 24 04 83 F8 6F 0F 87 A2 01 00 00 33 D2 8A 90 44 CD 48 00 FF 24 95 C4 CC 48 00 68 97 03 00 00 E8 BA 21 FF FF C2 04 00 68 98 03 00 00 E8 AD 21 FF\n\
	0x004C0440: 53 56 8B 74 24 0C 8B 46 0C 8B D9 8B 0D 60 C0 66 00 8B 49 18 57 50 E8 95 26 05 00 85 C0 0F 84 0D 01 00 00 8B 46 0C 8B 0D 60 C0 66 00 8B 49 18 50 E8 7B\n\
	0x00477A20: 83 7C 24 04 01 53 56 57 8B F9 BB 02 00 00 00 75 4A 8B 47 24 8B 88 6C 0B 00 00 6A 20 E8 FF A7 08 00 8B F0 85 F6 75 4A 8B 4F 24 8B 89 6C 0B 00 00 6A 10\n\
	0x004778B0: 83 EC 08 83 7C 24 0C 01 53 56 57 8B F9 C6 44 24 0C 00 0F 85 FB 00 00 00 8B 47 24 8B 88 6C 0B 00 00 6A 20 E8 68 A9 08 00 8B F0 85 F6 0F 85 FB 00 00 00\n\
	0x0049E370: 53 55 56 57 8B F9 8B 8F 64 0C 00 00 8A 81 A1 00 00 00 33 DB 33 ED 33 F6 84 C0 76 49 56 E8 DE 4E FD FF 3A 05 E9 42 63 00 75 07 BB 01 00 00 00 EB 19 8B\n\
	0x00470730: 56 8B F1 8B 4E 24 57 E8 34 DC 02 00 85 C0 74 5D 8B 46 24 8B 88 6C 0B 00 00 6A 10 E8 F0 1A 09 00 8B 4E 24 8B 89 6C 0B 00 00 6A 20 8B F8 E8 DE 1A 09 00\n\
	0x004BB4D0: 8B 44 24 08 83 F8 01 53 8B 5C 24 08 55 56 57 8B F1 74 0C 3A 9E AB 04 00 00 0F 84 71 01 00 00 85 C0 75 1C 8B 86 C8 0A 00 00 83 B8 EC 20 00 00 01 75 0D\n\
	0x0052FCC4: 28 0F 84 F9 00 00 00 68 83 03 00 00 8B CB E8 19 F0 F4 FF 85 C0 0F 85 E5 00 00 00 8B 8E 5C 21 00 00 57 E8 15 70 F7 FF 85 C0 7D 14 68 62 03 00 00 8B CB\n\
	0x00530F74: 83 7F 0C 28 74 52 8B 8E 5C 21 00 00 8B 89 64 0C 00 00 68 83 03 00 00 E8 60 DD F4 FF 85 C0 75 38 8B 8E 5C 21 00 00 57 E8 60 5D F7 FF 85 C0 7D 1A 8B 96\n\
	0x00530FC4: 6A 01 53 E8 04 A5 F8 FF C7 86 48 21 00 00 01 00 00 00 8B 86 5C 21 00 00 83 B8 CC 09 00 00 01 75 06 FF 86 48 21 00 00 5F 89 9E 40 21 00 00 89 9E 44 21\n";

	char addr[12]={0};
	char * cursor;
	int nLinebreak;
	sprintf( addr, "0x%08X", pNWSERVER );

	cursor = strstr( Sig, addr );

	if( cursor == NULL ){

		Log( "o FindAddress: no signature for 0x%08X\n", pNWSERVER );
		return 0;
	}

	for( int n=0;n<strlen( cursor );n++ ){

		if( cursor[n]=='\n' ){

			cursor[n]='\0';
			nLinebreak=n;
		}
	}

	AssemblyHelper ah;

	DWORD CheckAddr = ah.FindFunctionBySignature( &cursor[12] );

	int retries = 0;

	while( CheckAddr == 0 ){

		CheckAddr = ah.FindFunctionBySignature( &cursor[12] );
		if( retries++ == 10 )
			break;
	}

	if( CheckAddr != pNWSERVER )
		pNWSERVER = CheckAddr;

	cursor[nLinebreak]='\n';
	return pNWSERVER;
}

bool CNWNXWeapons::OverwriteFunction( DWORD pCode, PVOID pCallbackFunc, PVOID *pNextHook ){

	DWORD Alt = FindAddress( pCode );

	if( Alt == 0 ){

		Log( "! Hook: unable to find address!\n" );
		return false;
	}

	if( HookCode( (PVOID)pCode, pCallbackFunc, pNextHook ) ){

		if( Alt == pCode ) 
			Log( "o Hook nwserver: 0x%08X -> 0x%08X\n", (DWORD)pCode, (DWORD)pCallbackFunc );
		else
			Log( "o Hook nwmain: 0x%08X -> 0x%08X\n", (DWORD)pCode, (DWORD)pCallbackFunc );

		return true;
	}
	else{

		if( Alt == pCode ) 
			Log( "! Hook nwserver: 0x%08X -> 0x%08X\n", (DWORD)pCode, (DWORD)pCallbackFunc );
		else 
			Log( "! Hook nwmain: 0x%08X -> 0x%08X\n", (DWORD)pCode, (DWORD)pCallbackFunc );
	}
	return false;
}

void CNWNXWeapons::RemoveCopyItemAndModifyColorCap( void ){

	//Absorlute address
	DWORD * CopyItemAndModifyCap = (DWORD*)FindAddress( 0x0058A872 );

	if( CopyItemAndModifyCap == 0 ){

		Log( "! RemoveCopyItemAndModifyColorCap: NULL address!\n" );
		return;
	}

	DWORD DefaultPrivs;
	unsigned char * func = (unsigned char*)CopyItemAndModifyCap;

	//Make the memoryspace writeable
	VirtualProtect( CopyItemAndModifyCap, 1, PAGE_EXECUTE_READWRITE, &DefaultPrivs );
	
	//0x0058A871: BA [04] 00 00 00: mov edx 0x04
	func[0]=0xFF; //<- set cap to 255

	//Restore the privs back to default
	VirtualProtect( CopyItemAndModifyCap, 1, DefaultPrivs, NULL );
}

/*----------------
--Script interface
----------------*/

void CNWNXWeapons::ResetArguments( void ){

	for( int n=0;n<5;n++ ){

		OBJ_IDS[n]=OBJECT_INVALID;
	}
}

unsigned long CNWNXWeapons::OnRequestObject (char *gameObject, char* Request){

	//Log( "o OnRequestObject: 0x%08X: %s( )", gameObject, Request );

	int nIndex = atoi( Request );

	if( nIndex < 0 || nIndex >= 5 ){

		if( nIndex == 6 ){

			Ownership * OWN = GetOwnership( ((CNWSObject*)gameObject)->obj_generic.obj_id );

			if( OWN )
				return OWN->owner;
			else
				return OBJECT_INVALID;
		}
		else if( nIndex == 7 ){

			return RemoveOwnership( ((CNWSObject*)gameObject)->obj_generic.obj_id );
		}

		return OBJECT_INVALID;
	}

	return OBJ_IDS[nIndex];
}

char* CNWNXWeapons::OnRequest(char *gameObject, char* Request, char* Parameters){

	//Log( "o OnRequest: 0x%08X: %s( %s ) = ", gameObject, Request, Parameters );
	
	if( strncmp( Request, "GetData", 7 ) == 0 ){

		DWORD bType;
		int nField;

		sscanf( Parameters, "%u %i", &bType, &nField );

		sprintf( Parameters, "%u", GetWeaponData( bType, nField ) );
	}
	else if( strncmp( Request, "SetData", 7 ) == 0 ){

		DWORD bType;
		int nField;
		WORD Data;

		sscanf( Parameters, "%u %i %u", &bType, &nField, &Data );

		SetWeaponData( bType, nField, Data );
	}
	else if( strncmp( Request, "GetProficiencies", 16 ) == 0 ){

		DWORD bType;
		sscanf( Parameters, "%u", &bType );
		CreateProficienciesList( Parameters, bType );
	}
	else if( strncmp( Request, "AddProficiencies", 16 ) == 0 ){

		DWORD bType;
		DWORD Feat;
		sscanf( Parameters, "%u %u", &bType, &Feat );
		WeaponBehavior::WeaponEntry * WE = WB->GetWeaponType( bType );

		if( WE == NULL ){

			strcpy( Parameters, "0" );
		}
		else{

			WB->AddProficiencies( bType, Feat ); 
			strcpy( Parameters, "1" );
		}

	}
	else if( strncmp( Request, "RemoveProficiencies", 19 ) == 0 ){

		DWORD bType;
		DWORD Feat;
		sscanf( Parameters, "%u %u", &bType, &Feat );
		WeaponBehavior::WeaponEntry * WE = WB->GetWeaponType( bType );

		if( WE == NULL ){

			strcpy( Parameters, "0" );
		}
		else{

			WB->RemoveProficiencies( bType, Feat ); 
			strcpy( Parameters, "1" );
		}
	}
	else if( strncmp( Request, "SetDevCrit", 10 ) == 0 ){

		int nSet = atoi( Parameters );
		DevCritBehavior = nSet;
		Log( "o Set Devastating Critical Hit Behavior: %u\n", DevCritBehavior );
	}
	else if( strncmp( Request, "SetOwn", 6 ) == 0 ){

		nwn_objid_t itm;
		DWORD bound;

		sscanf( Parameters, "%x %u", &itm, &bound );

		SetOwnership( itm, ((CNWSObject*)gameObject)->obj_generic.obj_id, (bound>=1) ); 
	}
	else{

		strcpy( Parameters, "0" );
	}

	return NULL;
}

WORD CNWNXWeapons::GetWeaponData( DWORD uiWeapon, int nField ){
	
	switch( nField ){

		case 0: return WB->GetWeaponFinessable( uiWeapon );
		case 2: return WB->GetWeaponFocusFeat( uiWeapon );
		case 3: return WB->GetEpicWeaponFocusFeat( uiWeapon );
		case 4: return WB->GetWeaponSpecializationFeat( uiWeapon );
		case 5: return WB->GetEpicWeaponSpecializationFeat( uiWeapon );
		case 6: return WB->GetWeaponOfChoiceFeat( uiWeapon );
		case 7: return WB->GetImprovedCriticalFeat( uiWeapon );
		case 8: return WB->GetOverwhelmingCirticalHitFeat( uiWeapon );
		case 9: return WB->GetDevastatingCirticalHitFeat( uiWeapon );
		case 10: return WB->GetCriticalHitDamageMod( uiWeapon );
		case 11: return WB->GetCriticalHitThreat( uiWeapon );
		default:break;
	}
	
	return 0;
}

void CNWNXWeapons::SetWeaponData( DWORD uiWeapon, int nField, WORD nData ){

	WeaponBehavior::WeaponEntry * WE = WB->GetWeaponType( uiWeapon );
	if( !WE )
		WE = WB->AddWeaponType( uiWeapon );

	if( WE ){

		switch( nField ){

			case 0: WE->FINESSABLE = (nData >= 1 );break;
			case 2: WE->WEAPON_FOCUS_FEAT = nData;break;
			case 3: WE->EPIC_WEAPON_FOCUS_FEAT = nData;break;
			case 4: WE->WEAPON_SPECIALIZATION_FEAT = nData;break;
			case 5: WE->EPIC_WEAPON_SPECIALIZATION_FEAT = nData;break;
			case 6: WE->WEAPON_OF_CHOICE_FEAT = nData;break;
			case 7: WE->IMPROVED_CIRITCAL_FEAT = nData;break;
			case 8: WE->OVERWHELMING_CRITICAL_FEAT = nData;break;
			case 9: WE->DEVASTATING_CRITICAL_FEAT = nData;break;
			case 10: WE->CriticalDamageMod = (BYTE)nData;break;
			case 11: WE->CriticalHitThreat = (BYTE)nData;break;
			default:break;
		}
	}
}

void CNWNXWeapons::CreateProficienciesList( char * out, DWORD baseItem ){

	WeaponBehavior::WeaponEntry * WE = WB->GetWeaponType( baseItem );
	char test[255]={0};
	if( WE == NULL )
		strcpy( out, "0" );
	else if( WE->Proficiencies == NULL )
		strcpy( out, "0" );
	else{

		for( int n=0; n<WE->numProficiencies;n++ ){

			sprintf( test, "%s %u", test, WE->Proficiencies[n] );
		}

		strcpy( out, test );
	}
}

DWORD * CNWNXWeapons::GetDefaultBaseItemTypeInfo( DWORD baseItem, DWORD offset ){

	DWORD (__thiscall *CNWBaseItemArray__GetBaseItem)(void *pTHIS, DWORD arg1) = (DWORD (__thiscall*)(void *pTHIS, DWORD arg1))0x00512AF0;

	DWORD * g_pRules = (DWORD*)0x0066C060;

	DWORD ptrbaseItem = CNWBaseItemArray__GetBaseItem( (void*)(*(DWORD *)(*g_pRules + 0x18)), baseItem );

	if( ptrbaseItem )
		return (DWORD *)(ptrbaseItem + offset);

	return NULL;
}

void CNWNXWeapons::SetOwnership( nwn_objid_t item, nwn_objid_t pc, byte bound ){

	Ownership * Ownr=NULL;
	for( int n=0;n<ownLen;n++ ){

		if( own[n].item == item ){

			Ownr=&own[n];
			break;
		}
	}

	if( !Ownr ){

		for( int n=0;n<ownLen;n++ ){

			if( own[n].item == OBJECT_INVALID ){

				Ownr=&own[n];
				break;
			}
		}

		if( !Ownr ){

			own	= (Ownership*)realloc( own, sizeof( Ownership )*(ownLen+1) );
			Ownr = &own[ownLen];
			ownLen++;
		}
	}

	if( own ){

		Ownr->bound=bound;
		Ownr->item=item;
		Ownr->owner=pc;
	}
}

Ownership * CNWNXWeapons::GetOwnership( nwn_objid_t item ){

	if( item == OBJECT_INVALID )
		return NULL;

	Ownership * Ownr=NULL;
	for( int n=0;n<ownLen;n++ ){

		if( own[n].item == item ){

			Ownr=&own[n];
			break;
		}
	}

	return Ownr;
}

nwn_objid_t CNWNXWeapons::RemoveOwnership( nwn_objid_t item ){

	Ownership * Ownr = GetOwnership( item );

	if( Ownr ){

		Ownr->item = OBJECT_INVALID;
		return Ownr->owner;
	}

	return OBJECT_INVALID;
}