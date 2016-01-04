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

#include "NWNXUbab.h"
#include "../NWNXdll/madCHook.h"
#include "../NWNXdll/IniFile.h"
#pragma comment(lib, "madChook.lib")

extern CNWNXUbab ubab;

bool CNWNXUbab::BaseTypeAllowedUbab( DWORD basetype ){

	for( DWORD n=0;n<size;n++ ){

		if( ubabable[n]==basetype )
			return true;
	}

	return false; 
}

int (__fastcall * GetCanUseMonkAttackTableOriginal)( CNWSCreatureStats *, void*, int );
int __fastcall GetCanUseMonkAttackTable( CNWSCreatureStats * pThis, void*, int arg1 ){

	if( pThis == NULL )
		return 0;

	if( pThis->cs_original->GetUseMonkAbilities( ) ){

		CNWSItem * itmRightHand = pThis->cs_original->cre_equipment->GetItemInSlot( 16 );

		//Handle unarmed-only d20
		if( arg1 && !itmRightHand )
			return 1;

		CNWSItem * itmLeftHand = pThis->cs_original->cre_equipment->GetItemInSlot( 32 );

		DWORD baseItemR = itmRightHand == NULL ? 256 : itmRightHand->it_baseitemtype;
		DWORD baseItemL = itmLeftHand == NULL ? 256 : itmLeftHand->it_baseitemtype;

		return ( ubab.BaseTypeAllowedUbab( baseItemR ) == 1 && ubab.BaseTypeAllowedUbab( baseItemL ) == 1 );
	}

	return 0;
}

int (__fastcall * GetCanUseMonkAbilitiesOriginal)( CNWSCreature *, void * );
int __fastcall GetCanUseMonkAbilities( CNWSCreature * pThis, void * ){

	if( pThis == NULL || pThis->cre_stats->GetNumLevelsOfClass( 5 ) <= 0 )
		return 0;

	CNWSItem * itmChest = pThis->cre_equipment->GetItemInSlot( 2 );

	int armorAC = itmChest == NULL ? 0 : itmChest->ComputeArmorClass( );

	if( armorAC > 0 )
		return 0;

	CNWSItem * itmHand = pThis->cre_equipment->GetItemInSlot( 32 );
	DWORD baseItem = itmHand == NULL ? 256 : itmHand->it_baseitemtype;

	if (baseItem == 14 || baseItem == 56 || baseItem == 57)
		return 0;

	return 1;
}

void (__fastcall * SetCombatModeOriginal)( CNWSCreature *, void*, unsigned char, int );
void __fastcall SetCombatMode( CNWSCreature * pThis, void*, unsigned char a1, int a2 ){

	if( a1 == 5 ){

		CNWSItem * itmRightHand = pThis->cre_equipment->GetItemInSlot( 16 );
		CNWSItem * itmLeftHand = pThis->cre_equipment->GetItemInSlot( 32 );

		DWORD baseItemR = itmRightHand == NULL ? 256 : itmRightHand->it_baseitemtype;
		DWORD baseItemL = itmLeftHand == NULL ? 256 : itmLeftHand->it_baseitemtype;
	
		*ubab.UbabOnhand=ubab.BaseTypeAllowedUbab( baseItemR ) == 1 ? 0 : 1;
		*ubab.UbabOffHand=ubab.BaseTypeAllowedUbab( baseItemL ) == 1 ? 0 : 1;	
	}
	
	SetCombatModeOriginal( pThis, NULL, a1, a2 );
}

bool CNWNXUbab::OverwriteFunction( DWORD pCode, PVOID pCallbackFunc, PVOID *pNextHook, const char * name ){

	if( HookCode( (PVOID)pCode, pCallbackFunc, pNextHook ) ){

		Log( "o Hook nwmain (%s): 0x%08X -> 0x%08X\n", name, (DWORD)pCode, (DWORD)pCallbackFunc );
		return true;
	}
	else
		Log( "! Hook nwmain (%s): Failed\n", name );

	return false;
}

BOOL CNWNXUbab::OnRelease( ){

	Log( "o Plugin unloaded\n" );

	return CNWNXBase::OnRelease();
}

BOOL CNWNXUbab::OnCreate(const char* LogDir){

	CIniFile ini( "nwnx.ini" );

	char log[MAX_PATH];
	sprintf( log, "%s\\nwnx_ubab.txt", LogDir);


	// call the base class function
	if( !CNWNXBase::OnCreate( log ) )
		return false;

	WriteLogHeader( );

	ubabable=NULL;
	size=0;

	int nLoop=0;
	char number[12];

	int cursor;
	Log( "\nUbabable list:\n" );
	cursor = ini.ReadInteger( "UBAB", itoa( ++nLoop, number, 10 ), 257 ); 
	while( cursor < 257 ){

		ubabable = (DWORD*)realloc( ubabable, sizeof( DWORD )*(size+1) );	
		ubabable[size]=(DWORD)cursor;
		Log( "% 4u\n", ubabable[size++] ); 
		cursor = ini.ReadInteger( "UBAB", itoa( ++nLoop, number, 10 ), 257 ); 
	}
	Log( "\n" );

		DWORD memDefault;
	unsigned char * ASM;

	ASM = (unsigned char*)0x00530F74;
	VirtualProtect( ASM, 2, PAGE_EXECUTE_READWRITE, &memDefault );
	ASM[0]=0xEB;
	ASM[1]=0x56;
	VirtualProtect( ASM, 2, memDefault, NULL );

	ASM = (unsigned char*)0x004BB5A8;
	VirtualProtect( ASM, 4, PAGE_EXECUTE_READWRITE, &memDefault );
	ASM[0]=0x83;
	ASM[1]=0xF9;
	ASM[2]=0x00;
	ASM[3]=0x90;
	VirtualProtect( ASM, 4, memDefault, NULL );

	UbabOnhand=&ASM[2];
	VirtualProtect( UbabOnhand, 1, PAGE_EXECUTE_READWRITE, NULL );

	ASM = (unsigned char*)0x004BB5B9;
	VirtualProtect( ASM, 4, PAGE_EXECUTE_READWRITE, &memDefault );
	ASM[1]=0xF9;
	ASM[2]=0x00;
	VirtualProtect( ASM, 4, memDefault, NULL );

	UbabOffHand=&ASM[2];
	VirtualProtect( UbabOffHand, 1, PAGE_EXECUTE_READWRITE, NULL );

	OverwriteFunction( 0x0049E370, GetCanUseMonkAbilities, (PVOID*)&GetCanUseMonkAbilitiesOriginal, "GetCanUseMonkAbilities" );
	OverwriteFunction( 0x00470730, GetCanUseMonkAttackTable, (PVOID*)&GetCanUseMonkAttackTableOriginal, "GetCanUseMonkAttackTable" );
	OverwriteFunction( 0x004BB4D0, SetCombatMode, (PVOID*)&SetCombatModeOriginal, "SetCombatMode" );

	Log( "\n" );

	return true;
}

void CNWNXUbab::WriteLogHeader( ){

	fprintf( m_fFile, "NWNXUbab v1.2\n\n" );
	fflush( m_fFile );	
}

void CNWNXUbab::Log( const char * formatting, ... ){

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

unsigned long CNWNXUbab::OnRequestObject (char *gameObject, char* Request){

	Log( "! OnRequestObject: Unknown request: %s\n", Request );

	return OBJECT_INVALID;
}

char* CNWNXUbab::OnRequest(char *gameObject, char* Request, char* Parameters){
	
	
	if( strcmp( Request, "DIE" ) == 0 ){
		Log( "o Killing server!\n" );	
		exit( 0 );
	}
	else
		Log( "! OnRequest: Unknown request: %s\n", Request );	

	return NULL;
}

/*----------------
--Patch functions
----------------*/