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
#define _CRT_SECURE_NO_WARNINGS
#include "NWNXMagic.h"
#include "../NWNXdll/madCHook.h"
#include "AssemblyHelper.h"
#include "../NWNXdll/iniFile.h"

#pragma comment(lib, "madChook.lib")

BOOL CNWNXMagic::OnRelease( ){
	
	return CNWNXBase::OnRelease();
}

BOOL CNWNXMagic::OnCreate(const char* LogDir){

	char log[MAX_PATH];
	
	sprintf( log, "%s\\nwnx_magic.txt", LogDir);

	// call the base class function
	if( !CNWNXBase::OnCreate( log ) )
		return false;

	WriteLogHeader( );
	
	//CIniFile iniFile ("nwnx.ini");
	ISNWSERVER = true;//iniFile.ReadBool( "MAGIC", "IsServer", false );

	RemoveSpellLikeLevelCap( );

	return true;
}

void CNWNXMagic::WriteLogHeader( ){

	fprintf( m_fFile, "NWNXMagic v1.0 created by Terra_777\n\n" );
	fflush( m_fFile );	
}

void CNWNXMagic::Log( const char * formatting, ... ){

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

void CNWNXMagic::RemoveSpellLikeLevelCap( void ){

	DWORD privs;
	unsigned char * func = (unsigned char*)0x00488DDF;

	VirtualProtect(func, 1, PAGE_EXECUTE_READWRITE, &privs);

	func[0] = 0xEB;

	VirtualProtect(func, 1, privs, NULL);
}

/*----------------
--Script interface
----------------*/

unsigned long CNWNXMagic::OnRequestObject (char *gameObject, char* Request){

	return OBJECT_INVALID;
}

char* CNWNXMagic::OnRequest(char *gameObject, char* Request, char* Parameters){

	for( unsigned int n=0; n<strlen( Parameters );n++ ){
		if( Parameters[n]=='~' )
			Parameters[n]='\0';
	}

	Log( "o %s( %08X: %s )\n", Request, ((CNWSCreature*)gameObject)->obj.obj_generic.obj_id, Parameters );

	if( strcmp( Request, "AddSpellLikeAbility" ) == 0 ){

		sprintf( Parameters, "%i", AddSpellLikeAbility( (CNWSCreature*)gameObject, Parameters ) );
	}
	else if( strcmp( Request, "GetSpellLikeAbility" ) == 0 ){

		GetSpellLikeAbility( (CNWSCreature*)gameObject, Parameters );
	}
	else if( strcmp( Request, "RemoveSpellLikeAbility" ) == 0 ){

		RemoveSpellLikeAbility( (CNWSCreature*)gameObject, Parameters );
	}
	else if( strcmp( Request, "PackSpellLikeAbilities" ) == 0 ){

		PackSpellLikeAbilities( (CNWSCreature*)gameObject, Parameters );
	}
	else if( strcmp( Request, "AddMemorizedSpell" ) == 0 ){

		AddMemorizedSpell( (CNWSCreature*)gameObject, Parameters );
	}
	else if( strcmp( Request, "SetMemorizedSpell" ) == 0 ){

		SetMemorizedSpell( (CNWSCreature*)gameObject, Parameters );
	}
	else if( strcmp( Request, "SetNumberOfSpellslots" ) == 0 ){

		SetNumberOfSpellslots( (CNWSCreature*)gameObject, Parameters );
	}
	else if( strcmp( Request, "GetSpellLikeLength" ) == 0 ){

		NWNXSpecialAbilities * Repository = (NWNXSpecialAbilities*)((CNWSCreature*)gameObject)->cre_stats->cs_specabil;
		if( Repository == NULL )
			strcpy( Parameters, "0" );
		else
			sprintf( Parameters, "%i", Repository->Len );
	}
	else if( strcmp( Request, "GetMemorizedSpell" ) == 0 ){

		GetMemorizedSpell( (CNWSCreature*)gameObject, Parameters );
	}
	else if( strcmp( Request, "ModifySpellsPerDay" ) == 0 ){

		ModifySpellsPerDay( (CNWSCreature*)gameObject, Parameters );
	}
	else if( strcmp( Request, "PackSpellLevelIntoString" ) == 0 ){

		PackSpellLevelIntoString( (CNWSCreature*)gameObject, Parameters );
	}
	else if( strcmp( Request, "UnPackSpellLevelString" ) == 0 ){

		UnPackSpellLevelString( (CNWSCreature*)gameObject, Parameters );
	}
	else if( strcmp( Request, "EmptySpellBook" ) == 0 ){

		EmptySpellBook( (CNWSCreature*)gameObject, Parameters );
	}
	else if( strcmp( Request, "AddKnownSpell" ) == 0 ){

		AddKnownSpell( (CNWSCreature*)gameObject, Parameters );
	}
	else if( strcmp( Request, "GetKnownSpell" ) == 0 ){

		sprintf( Parameters, "%i", GetKnownSpell( (CNWSCreature*)gameObject, Parameters ) );
	}
	else if( strcmp( Request, "RemoveKnownSpell" ) == 0 ){

		sprintf( Parameters, "%i", RemoveKnownSpell( (CNWSCreature*)gameObject, Parameters ) );
	}
	else if( strcmp( Request, "DumpSpells" ) == 0 ){
		
		DumpSpellData( (CNWSCreature*)gameObject );
	}

	return NULL;
}

void CNWNXMagic::CheckSpellLikeRepository( CNWSCreature * cre ){

	if( cre->cre_stats->cs_specabil == 0 ){

		cre->cre_stats->cs_specabil = (CExoArrayList_2<CNWSStats_SpellLikeAbility>*)mem.nwnx_malloc( sizeof( NWNXSpecialAbilities ) );
		NWNXSpecialAbilities * test = (NWNXSpecialAbilities*)cre->cre_stats->cs_specabil;

		test->Alloc=0;
		test->Len=0;
		test->Array=0;
	}
}

int CNWNXMagic::AddSpellLikeAbility( CNWSCreature * cre, char * SpellLikeString ){

	CheckSpellLikeRepository( cre );

	NWNXSpecialAbilities * Repository = (NWNXSpecialAbilities*)cre->cre_stats->cs_specabil;
	Repository->UnPackSpecialAbilitiesToString( SpellLikeString );		

	return Repository->Len-1;
}

int CNWNXMagic::GetSpellLikeAbility( CNWSCreature * cre, char * input ){

	NWNXSpecialAbilities * Repository = (NWNXSpecialAbilities*)cre->cre_stats->cs_specabil;

	int nIndex;
	sscanf( input, "%i", &nIndex );

	input[0]='\0';

	if( Repository == NULL || nIndex < 0 || nIndex >= Repository->Len ){

		return 0;
	}

	sprintf( input, "%u %u %u", Repository->Array[nIndex].Spell, Repository->Array[nIndex].Ready, Repository->Array[nIndex].Level );

	return 1;
}

int CNWNXMagic::RemoveSpellLikeAbility( CNWSCreature * cre, char * input ){

	NWNXSpecialAbilities * Repository = (NWNXSpecialAbilities*)cre->cre_stats->cs_specabil;

	unsigned int nIndex;
	sscanf( input, "%u", &nIndex );
	
	input[0]='\0';
	strcpy( input, "0" );

	if( Repository == NULL || nIndex < 0 || nIndex >= (unsigned int)Repository->Len ){

		return 0;
	}

	if( Repository->Remove( nIndex ) )
		strcpy( input, "1" );

	return 1;
}

int CNWNXMagic::PackSpellLikeAbilities( CNWSCreature * cre, char * input ){

	input[0]='\0';

	NWNXSpecialAbilities * Repository = (NWNXSpecialAbilities*)cre->cre_stats->cs_specabil;
	if( Repository == NULL )
		return 0;
	
	return Repository->PackSpecialAbilitiesToString( input );
}

int CNWNXMagic::SetNumberOfSpellslots( CNWSCreature * cre, char * input ){

	int nClass;
	int nLevel;
	int nSpellslots;

	sscanf( input, "%i %i %i", &nClass, &nLevel, &nSpellslots );

	NWNXSpellBook * Spells = (NWNXSpellBook*)&cre->cre_stats->cs_classes[nClass].cl_spells_mem[nLevel];

	if( nSpellslots < 0 ){

		sprintf( input, "%u", Spells->Len ); 
		return 1;
	}
	else if( nSpellslots == 0 ){

		Spells->EmptySpellbook( 0 );
		Spells->Len=0;
	}
	else if( (unsigned int)nSpellslots < Spells->Len ){

		Spell * slot;

		for( unsigned int n=nSpellslots;n<Spells->Len;n++ ){

			slot = Spells->GetSpell( n );
			if( slot != NULL )
				Spells->RemoveSpell( n );
		}

		Spells->Len = nSpellslots;
	}
	else if( (unsigned int)nSpellslots > Spells->Alloc ){

		Spells->Resize( nSpellslots+1 );
		Spells->Len = nSpellslots;
	}

	sprintf( input, "%u", Spells->Len ); 
	return 1;
}

int CNWNXMagic::AddMemorizedSpell( CNWSCreature * cre, char * input ){

	int nLevel;
	int nClass;
	Spell spData;

	sscanf( input, "%i %i %i %u %u %u", &nClass, &nLevel, &spData.SpellID, &spData.Ready, &spData.MetaMagic, &spData.Domain );

	NWNXSpellBook * Spells = (NWNXSpellBook*)&cre->cre_stats->cs_classes[nClass].cl_spells_mem[nLevel];

	if( Spells->Alloc <= 0 )
		Spells->Resize( 16 );

	if( Spells->SetSpell( spData ) )
		strcpy( input, "1" );
	else
		strcpy( input, "0" );

	return 1;
}

int CNWNXMagic::SetMemorizedSpell( CNWSCreature * cre, char * input ){

	int nLevel;
	int nClass;
	int nSlot;
	Spell spData;

	sscanf( input, "%i %i %i %i %u %u %u", &nClass, &nLevel, &nSlot, &spData.SpellID, &spData.Ready, &spData.MetaMagic, &spData.Domain );

	NWNXSpellBook * Spells = (NWNXSpellBook*)&cre->cre_stats->cs_classes[nClass].cl_spells_mem[nLevel];

	if( spData.Domain == 0 && spData.MetaMagic == 0 && spData.Ready == 0 && spData.SpellID == 0 ){
		Spells->RemoveSpell( nSlot );
	}
	else{
		Spells->SetSpell( spData, nSlot, 1 );
	}
	return 1;
}

int CNWNXMagic::GetMemorizedSpell( CNWSCreature * cre, char * input ){

	int nLevel;
	int nClass;
	int nSlot;
	Spell * spData;

	sscanf( input, "%i %i %i", &nClass, &nLevel, &nSlot );

	NWNXSpellBook * Spells = (NWNXSpellBook*)&cre->cre_stats->cs_classes[nClass].cl_spells_mem[nLevel];

	spData = Spells->GetSpell( nSlot );

	if( spData == NULL ){
		strcpy( input, "-1 0 0 0" );
	}
	else{
		sprintf( input, "%i %u %u %u", spData->SpellID, spData->Ready, spData->MetaMagic, spData->Domain );	
	}

	return 1;
}

int CNWNXMagic::ModifySpellsPerDay( CNWSCreature * cre, char * input ){

	int nClass;
	int nLevel;
	int nType;
	int nNew;

	uint8_t * ActualData;

	sscanf( input, "%i %i %i %i", &nClass, &nLevel, &nType, &nNew );

	if( nType <= 0 ){
		ActualData = cre->cre_stats->cs_classes->cl_spells_bonus;
	}
	else if( nType == 1 ){
		ActualData = cre->cre_stats->cs_classes->cl_spells_left;
	}
	else if( nType >= 2 ){
		ActualData = cre->cre_stats->cs_classes->cl_spells_max;
	}

	if( nNew < 0 ){

		sprintf( input, "%i", ActualData[nLevel] );
	}
	else{
		ActualData[nLevel]=nNew;
		sprintf( input, "%i", ActualData[nLevel] );
	}

	return 1;
}

void CNWNXMagic::DumpSpellData( CNWSCreature * cre ){

	NWNXSpellBook * Spells;

	Log( "ID: %08X type: %u\n", cre->obj.obj_generic.obj_id, cre->obj.obj_generic.obj_type2 );

	for( int n=0; n<cre->cre_stats->cs_classes_len; n++ ){

		Log( "Class %u\n", cre->cre_stats->cs_classes[n].cl_class );
		Log( "domain 1: %u\n", cre->cre_stats->cs_classes[n].cl_domain_1 );
		Log( "domain 2: %u\n", cre->cre_stats->cs_classes[n].cl_domain_2 );
		Log( "Level: %u\n", cre->cre_stats->cs_classes[n].cl_level );
		Log( "Negative Level: %u\n", cre->cre_stats->cs_classes[n].cl_negative_level );
		Log( "Specialist: %u\n", cre->cre_stats->cs_classes[n].cl_specialist );
		for( int i=0;i<10;i++ ){
			Log( "Spellbonus (%i) (0x%08X): %u\n", i, cre->cre_stats->cs_classes[n].cl_spells_bonus,cre->cre_stats->cs_classes[n].cl_spells_bonus[i] ); 
		}
		for( int j=0;j<10;j++ ){
			Log( "Spellsknown (%i) (0x%08X):", j,cre->cre_stats->cs_classes[n].cl_spells_known ); 
			for( int k=0;k<cre->cre_stats->cs_classes[n].cl_spells_known[j].len;k++ ){

				Log( " %u",cre->cre_stats->cs_classes[n].cl_spells_known[j].data[k] );			
			}
			Log( "\n" );
		}
		for( int h=0;h<10;h++ ){
			Log( "Spellsleft (%i) (0x%08X): %u\n", h, cre->cre_stats->cs_classes[n].cl_spells_left, cre->cre_stats->cs_classes[n].cl_spells_left[h] ); 
		}
		for( int l=0;l<10;l++ ){
			Log( "Spellsmax (%i) (0x%08X): %u\n", l, cre->cre_stats->cs_classes[n].cl_spells_max, cre->cre_stats->cs_classes[n].cl_spells_max[l] ); 
		}
		
		for( int z=0;z<10;z++ ){
			
			Spells = (NWNXSpellBook*)&cre->cre_stats->cs_classes[n].cl_spells_mem[z];
			Log( "Spellsmem (%i) (0x%08X) (%u) (%u):", z, Spells, Spells->Len, Spells->Alloc ); 
			for( unsigned int x=0;x<Spells->Len;x++ ){

				if( Spells->Matrix[x] != NULL )
					Log( " %i (%u %u %u)", Spells->Matrix[x]->SpellID, Spells->Matrix[x]->Ready, Spells->Matrix[x]->MetaMagic, Spells->Matrix[x]->Domain );
				else
					Log( " -1 (0 0 0)" );
			}
			Log( "\n" );
		}
	}
}

int CNWNXMagic::PackSpellLevelIntoString( CNWSCreature * cre, char * input ){

	int nClass;
	int nLevel;

	sscanf( input, "%i %i", &nClass, &nLevel );

	NWNXSpellBook * Spells = (NWNXSpellBook*)&cre->cre_stats->cs_classes[nClass].cl_spells_mem[nLevel];

	if( Spells->Alloc == 0 || Spells->Len == 0 ){

		input[0]='\0';
		return 0;
	}

	int nRet = Spells->PackSpellbookToString( input );
		
	return nRet;
}

int CNWNXMagic::UnPackSpellLevelString( CNWSCreature * cre, char * input ){

	int nClass;
	int nLevel;
	int nDatastart=0;
	int nFind = 0;

	sscanf( input, "%i %i", &nClass, &nLevel );

	for( unsigned int n=0;n<strlen( input );n++ ){

		if( input[n]==' ' && nFind == 1 ){
			nDatastart = n+1;
			break;
		}
		else if( input[n]==' ' ){
			nFind++;
		}
	}

	NWNXSpellBook * Spells = (NWNXSpellBook*)&cre->cre_stats->cs_classes[nClass].cl_spells_mem[nLevel];

	if( Spells->Alloc == 0 || Spells->Len == 0 )
		return 0;

	int nRet = Spells->UnPackSpellbookToString( &input[nDatastart], 1 );

	sprintf( input, "%i", nRet );

	return nRet;
}

int CNWNXMagic::EmptySpellBook( CNWSCreature * cre, char * input ){

	int nClass;
	int nLevel;
	int EatChargesOnly;

	sscanf( input, "%i %i %i", &nClass, &nLevel, &EatChargesOnly );

	NWNXSpellBook * Spells = (NWNXSpellBook*)&cre->cre_stats->cs_classes[nClass].cl_spells_mem[nLevel];

	if (!Spells || Spells->Alloc == 0 || Spells->Len == 0)
		return 0;

	Spells->EmptySpellbook( EatChargesOnly );

	strcpy( input, "1" );

	return 1;
}

int CNWNXMagic::AddKnownSpell( CNWSCreature * cre, char * input ){

	int nClass, nLevel, nSpell;
	sscanf( input, "%i %i %i", &nClass, &nLevel, &nSpell );

	KnownSpells * Spells = (KnownSpells*)&cre->cre_stats->cs_classes[nClass].cl_spells_known[nLevel];

	if( Spells->len+1 >= Spells->alloc ){

		void * temp = mem.nwnx_calloc( Spells->alloc+16, sizeof( DWORD ) );
		memcpy( temp, Spells->spell, sizeof( DWORD )*Spells->len );
		mem.nwnx_free( Spells->spell );
		Spells->spell = (DWORD*)temp;
	}

	Spells->spell[Spells->len++]=nSpell;

	return 1;
}

int CNWNXMagic::GetKnownSpell( CNWSCreature * cre, char * input ){

	int nClass, nLevel, nIndex;
	sscanf( input, "%i %i %i", &nClass, &nLevel, &nIndex );

	KnownSpells * Spells = (KnownSpells*)&cre->cre_stats->cs_classes[nClass].cl_spells_known[nLevel];

	if( (DWORD)nIndex >= Spells->len || nIndex < 0 )
		return -1;

	return Spells->spell[nIndex];
}

int CNWNXMagic::RemoveKnownSpell( CNWSCreature * cre, char * input ){

	int nClass, nLevel, nSpell, nIndex;
	sscanf( input, "%i %i %i", &nClass, &nLevel, &nSpell );

	KnownSpells * Spells = (KnownSpells*)&cre->cre_stats->cs_classes[nClass].cl_spells_known[nLevel];
	nIndex = -1;

	for( DWORD n = 0; n<Spells->len;n++ ){

		if( Spells->spell[n] == nSpell ){
			nIndex = (int)n;
			break;
		}
	}

	if( nIndex <= -1 )
		return 0;

	if( nIndex == Spells->len-1 ){

		memset( &Spells->spell[nIndex], 0, sizeof( DWORD ) );
		Spells->spell[nIndex]=0;
	}
	else{

		memcpy( &Spells->spell[nIndex], &Spells->spell[nIndex+1], ((Spells->len-1)-nIndex)*sizeof( DWORD ) );
		memset( &Spells->spell[Spells->len-1], 0, sizeof( DWORD ) );
		Spells->spell[Spells->len-1]=0;
	}
	Spells->len--;

	return 1;
}