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

#include "CNWNXPoly.h"
#include "../NWNXdll/madCHook.h"

#pragma comment(lib, "madChook.lib")

extern CNWNXPoly poly;

int (__fastcall * CNWSCreature__PolymorphNext)( CNWSCreature * cre, void*, int nPoly, CGameEffect * eff, int nLocked );
int __fastcall CNWSCreature__Polymorph( CNWSCreature * cre, void*, int nPoly, CGameEffect * eff, int nLocked ){

	poly.Log( "o Polymorph: %08X %i\n", cre->obj.obj_generic.obj_id, eff->eff_integers[0] );

	poly.nType = 1;
	poly.nPoly = eff->eff_integers[0];
	(*NWN_VirtualMachine)->Runscript( &CExoString( "nwnx_poly" ), cre->obj.obj_generic.obj_id );

	return CNWSCreature__PolymorphNext( cre, NULL, nPoly, eff, nLocked );
}

int (__fastcall * CNWSCreature__UnPolymorphNext)( CNWSCreature * cre, void*, CGameEffect * eff );
int __fastcall CNWSCreature__UnPolymorph( CNWSCreature * cre, void*, CGameEffect * eff ){

	poly.Log( "o UnPolymorph: %08X %i\n", cre->obj.obj_generic.obj_id, eff->eff_integers[0] );

	poly.nType = 0;
	poly.nPoly = eff->eff_integers[0];
	(*NWN_VirtualMachine)->Runscript( &CExoString( "nwnx_poly" ), cre->obj.obj_generic.obj_id );

	return CNWSCreature__UnPolymorphNext( cre, NULL, eff );
}

BOOL CNWNXPoly::OnRelease( ){

	return CNWNXBase::OnRelease();
}

BOOL CNWNXPoly::OnCreate(const char* LogDir){

	char log[MAX_PATH];
	
	sprintf( log, "%s\\nwnx_polymorph.txt", LogDir);

	// call the base class function
	if( !CNWNXBase::OnCreate( log ) )
		return false;

	WriteLogHeader( );

	if( HookCode( (PVOID)0x004AD960, CNWSCreature__Polymorph, (PVOID*)&CNWSCreature__PolymorphNext ) )
		Log( "o Hooked int CNWSCreature::Polymorph( CNWSCreature * cre, int nPoly, CGameEffect * eff, int nLocked )\n" );
	else
		Log( "! Failed to hook int CNWSCreature::Polymorph( CNWSCreature * cre, int nPoly, CGameEffect * eff, int nLocked )\n" );
	
	if( HookCode( (PVOID)0x004AE6C0, CNWSCreature__UnPolymorph, (PVOID*)&CNWSCreature__UnPolymorphNext ) )
		Log( "o Hooked int CNWSCreature::UnPolymorph( CNWSCreature * cre, CGameEffect * eff )\n" );
	else
		Log( "! Failed to hook int CNWSCreature::UnPolymorph( CNWSCreature * cre, CGameEffect * eff )\n" );

	Log( "\n" );

	return true;
}

void CNWNXPoly::WriteLogHeader( ){

	fprintf( m_fFile, "NWNXPoly v1.0 created by Terra_777\n\n" );
	fflush( m_fFile );	
}

void CNWNXPoly::Log( const char * formatting, ... ){

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

unsigned long CNWNXPoly::OnRequestObject (char *gameObject, char* Request){

	return OBJECT_INVALID;
}

char* CNWNXPoly::OnRequest(char *gameObject, char* Request, char* Parameters){

	
	if( strcmp( Request, "GETTYPE" ) == 0 )
		sprintf( Parameters, "%i", nType );
	else if( strcmp( Request, "GETPOLY" ) == 0 )
		sprintf( Parameters, "%i", nPoly );


	return NULL;
}

/*----------------
--Patch functions
----------------*/