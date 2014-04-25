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

#include "NWNXPalette.h"
#include "../NWNXdll/madCHook.h"

#pragma comment(lib, "madChook.lib")

extern CNWNXPalette palette;

BOOL CNWNXPalette::OnRelease( ){

	return CNWNXBase::OnRelease();
}

BOOL CNWNXPalette::OnCreate(const char* LogDir){

	char log[MAX_PATH];
	
	sprintf( log, "%s\\nwnx_palette.txt", LogDir);

	// call the base class function
	if( !CNWNXBase::OnCreate( log ) )
		return false;

	WriteLogHeader( );

	return true;
}

void CNWNXPalette::WriteLogHeader( ){

	fprintf( m_fFile, "NWNX Palette v0.1 created by Terra_777\n\n" );
	fflush( m_fFile );	
}

void CNWNXPalette::Log( const char * formatting, ... ){

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

unsigned long CNWNXPalette::OnRequestObject (char *gameObject, char* Request){

	Log( "o OnRequestObject: 0x%08X: %s( )", gameObject, Request );

	return OBJECT_INVALID;
}

char* CNWNXPalette::OnRequest(char *gameObject, char* Request, char* Parameters){

	Log( "o OnRequest: 0x%08X: %s( %s )", gameObject, Request, Parameters );
	
	return NULL;
}

/*----------------
--Patch functions
----------------*/

void CNWNXPalette::SendPaletteToPC( CNWSObject * PC ){

	CResRef ResRef;
	CNWSMessage * msg = (*NWN_AppManager)->app_server->GetNWSMessage();
	char * data;
	CNWSPlayer * ply = (*NWN_AppManager)->app_server->GetClientObjectByObjectId( PC->obj_generic.obj_id );

	if( !ply )
		return;

	CExoString str;
	ply->GetPlayerName( &str );

	CResGFF * creature = creature->NWNX_CreateOpenAndDemand( 2030, "ITP ", *ResRef.Set( "CREATUREPALCUS" ) );
	CResGFF * item = item->NWNX_CreateOpenAndDemand( 2030, "ITP ", *ResRef.Set( "ITEMPALCUS" ) );
	CResGFF * encounter = encounter->NWNX_CreateOpenAndDemand( 2030, "ITP ", *ResRef.Set( "ENCOUNTERPALCUS" ) );
	CResGFF * waypoint = waypoint->NWNX_CreateOpenAndDemand( 2030, "ITP ", *ResRef.Set( "WAYPOINTPALCUS" ) );
	CResGFF * trigger = trigger->NWNX_CreateOpenAndDemand( 2030, "ITP ", *ResRef.Set( "TRIGGERPALCUS" ) );
	CResGFF * portal = portal->NWNX_CreateOpenAndDemand( 2030, "ITP ", *ResRef.Set( "PORTALPALCUS" ) );
	CResGFF * placeable = placeable->NWNX_CreateOpenAndDemand( 2030, "ITP ", *ResRef.Set( "PLACEABLEPALCUS" ) );

	DWORD TotalSize = 29;

	TotalSize += creature->res_size;
	TotalSize += item->res_size;
	TotalSize += encounter->res_size;
	TotalSize += waypoint->res_size;
	TotalSize += trigger->res_size;
	TotalSize += portal->res_size;
	TotalSize += placeable->res_size;

	CNWMessage * mess = NWNX_CreateMessage( TotalSize );
		
	mess->WriteBYTE( 0, 8 );
	//Creature
	mess->WriteDWORD( creature ? creature->res_size : 0, 32 );
	if( creature ){
		mess->WriteVOIDPtr( creature, creature->res_size );
		(**(void (__thiscall ***)(CResGFF *, DWORD))creature)(creature, 1);
	}
	//item
	mess->WriteDWORD( item ? item->res_size : 0, 32 );
	if( item ){
		mess->WriteVOIDPtr( item, item->res_size );
		(**(void (__thiscall ***)(CResGFF *, DWORD))item)(item, 1);
	}
	//encounter
	mess->WriteDWORD( encounter ? encounter->res_size : 0, 32 );
	if( encounter ){
		mess->WriteVOIDPtr( encounter, encounter->res_size );
		(**(void (__thiscall ***)(CResGFF *, DWORD))encounter)(encounter, 1);
	}
	//waypoint
	mess->WriteDWORD( waypoint ? waypoint->res_size : 0, 32 );
	if( waypoint ){
		mess->WriteVOIDPtr( waypoint, waypoint->res_size );
		(**(void (__thiscall ***)(CResGFF *, DWORD))waypoint)(waypoint, 1);
	}
	//trigger
	mess->WriteDWORD( trigger ? trigger->res_size : 0, 32 );
	if( trigger ){
		mess->WriteVOIDPtr( trigger, trigger->res_size );
		(**(void (__thiscall ***)(CResGFF *, DWORD))trigger)(trigger, 1);
	}
	//portal
	mess->WriteDWORD( portal ? portal->res_size : 0, 32 );
	if( portal ){
		mess->WriteVOIDPtr( portal, portal->res_size );
		(**(void (__thiscall ***)(CResGFF *, DWORD))portal)(portal, 1);
	}
	//placeable
	mess->WriteDWORD( placeable ? placeable->res_size : 0, 32 );
	if( placeable ){
		mess->WriteVOIDPtr( placeable, placeable->res_size );
		(**(void (__thiscall ***)(CResGFF *, DWORD))placeable)(placeable, 1);
	}

	
	TotalSize=0;
	
	if( mess->GetWriteMessage( &data, &TotalSize ) ){

		msg->SendServerToPlayerMessage( ply->pl_id, 31, 6, data, TotalSize );
		NWNX_DestroyMessage( mess );

		Log( "o Sent palette (%u BYTES) to %s\n", TotalSize, str.CStr() );
	}
	else{
		Log( "! Unable to send palette to %s\n", str.CStr() );
	}
}