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

#include "NWNXEvents.h"
#include "EventHooks.h"
#include "../NWNXdll/IniFile.h"

BOOL CNWNXEvents::OnRelease( ){

	for( int n=0;n<2;n++ ){

		if( EP->sData[n].text )
			mem.nwnx_free( EP->sData[n].text );
	}
	for( int n=0;n<NUM_EVENTS;n++ ){

		if( Script[n].text )
			mem.nwnx_free( Script[n].text );

	}

	mem.nwnx_free( EP );
	mem.nwnx_free( Script );

	return CNWNXBase::OnRelease();
}

BOOL CNWNXEvents::OnCreate(const char* LogDir){

	char log[MAX_PATH];
	char path[MAX_PATH];

	sprintf( log, "%s\\nwnx_events.txt", LogDir);
	sprintf( path, "%s\\1\\nwnx_events.txt", LogDir);

	MoveFile( log, path );

	// call the base class function
	if( !CNWNXBase::OnCreate( log ) )
		return false;

	Script = (CExoString*)mem.nwnx_malloc( sizeof(CExoString)*NUM_EVENTS );	
	memset( Script, 0, sizeof(CExoString)*NUM_EVENTS );

	EP = (EventParams*)mem.nwnx_malloc( sizeof( EventParams ) );
	memset( EP, 0, sizeof( EventParams ) );
	EP->BYPASS = 0x80000000;
	for( int n=0;n<2;n++ ){
		EP->oTarget[n]=OBJECT_INVALID;
	}

	LOG=1;

	WriteLogHeader( );
	for( int n=0;n<NUM_EVENTS;n++ )
		HookEvents( n );

	CIniFile INI( "NWNX.ini" );
	
	LOG = INI.ReadInteger( "EVENTS", "log", 0 );

	return true;
}

void CNWNXEvents::WriteLogHeader( ){

	fprintf( m_fFile, "NWNXEvents v2.0 created by Terra_777\n\n" );
	fflush( m_fFile );	
}

void CNWNXEvents::Log( const char * formatting, ... ){

	if( LOG<=0 )
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

unsigned long CNWNXEvents::OnRequestObject (char *gameObject, char* Request){

	if( !InScript ){
		Log( "! %s: Call outside event script!\n", Request );
		return OBJECT_INVALID;
	}

	int nID = atoi( Request );
	if( nID >= 0 && nID < 2 ){
		return EP->oTarget[nID];
	}

	return OBJECT_INVALID;
}

char* CNWNXEvents::OnRequest(char *gameObject, char* Request, char* Parameters){

	if( !InScript ){
		Log( "! %s: Call outside event script!\n", Request );
		return NULL;
	}
	
	int nFunc = atoi( Request );
	int nParam = atoi( Parameters );

	if( nFunc == 0 ){

		sprintf( Parameters, "%i", EP->EVENT );
	}
	else if( nFunc == 1 ){

		if( nParam >= 0 && nParam < 5 ){ 
			sprintf( Parameters, "%i", EP->nData[nParam] );
		}
	}
	else if( nFunc == 2 ){

		if( nParam >= 0 && nParam < 2 ){ 
			sprintf( Parameters, "%f|%f|%f", EP->lTarget[nParam].x,EP->lTarget[nParam].y,EP->lTarget[nParam].z );
		}
	}
	else if( nFunc == 3 ){

		if( nParam >= 0 && nParam < 2 ){ 
			sprintf( Parameters, "%s", EP->sData[nParam].CStr() );
		}
	}
	else if( nFunc == 4 ){
		
		sscanf( Parameters, "%i", &EP->BYPASS );
	}

	return NULL;
}

/*----------------
--Patch functions
----------------*/

int CNWNXEvents::FireEvent( nwn_objid_t OBJECT_SELF ){

	InScript=true;
	(*NWN_VirtualMachine)->Runscript( &Script[EP->EVENT], OBJECT_SELF );
	InScript=false;

	int ByPass = EP->BYPASS;

	for( int n=0;n<2;n++ ){

		if( EP->sData[n].text )
			mem.nwnx_free( EP->sData[n].text );
	}

	memset( EP, 0, sizeof( EventParams ) );
	EP->BYPASS = 0x80000000;

	for( int n=0;n<2;n++ ){
		EP->oTarget[n]=OBJECT_INVALID;
	}

	return ByPass;
}