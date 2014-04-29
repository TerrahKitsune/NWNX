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

#include "NWNXAmia.h"
#include "../NWNXdll/madCHook.h"
#include "../NWNXdll/IniFile.h"
#pragma comment(lib, "madChook.lib")

bool CNWNXAmia::OverwriteFunction( DWORD pCode, PVOID pCallbackFunc, PVOID *pNextHook, const char * name ){

	if( HookCode( (PVOID)pCode, pCallbackFunc, pNextHook ) ){

		Log( "o Hook nwmain (%s): 0x%08X -> 0x%08X\n", name, (DWORD)pCode, (DWORD)pCallbackFunc );
		return true;
	}
	else
		Log( "! Hook nwmain (%s): Failed\n", name );

	return false;
}

BOOL CNWNXAmia::OnRelease( ){

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	strftime( timestamp, 64,"%Y-%m-%d %H:%M",timeinfo);
	Log( "o Server stop: %s Ran for %ld minutes!\n", timestamp, ((rawtime-start)/60) );

	return CNWNXBase::OnRelease();
}

BOOL CNWNXAmia::OnCreate(const char* LogDir){

	CIniFile ini( "nwnx.ini" );

	time( &start );

	char log[MAX_PATH];
	char logold[MAX_PATH];	
	sprintf( log, "%s\\nwnx_amia.txt", LogDir);
	sprintf( logold, "%s\\1\\nwnx_amia.txt", LogDir);

	MoveFile( log, logold );

	// call the base class function
	if( !CNWNXBase::OnCreate( log ) )
		return false;

	WriteLogHeader( );

	if( !ini.ReadBool( "AMIA", "CrashBox", false ) ){

		SetErrorMode( SetErrorMode(0) | SEM_NOGPFAULTERRORBOX | SEM_NOALIGNMENTFAULTEXCEPT );
		Log( "o Crashbox: Disabled\n\n" );
	}
	else
		Log( "o Crashbox: Enabled\n\n" );

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	strftime( timestamp, 64,"%Y-%m-%d %H:%M",timeinfo);
	Log( "\no Server start: %s\n", timestamp );

	return true;
}

void CNWNXAmia::WriteLogHeader( ){

	fprintf( m_fFile, "NWNXAmia v1.3\n\n" );
	fflush( m_fFile );	
}

void CNWNXAmia::Log( const char * formatting, ... ){

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

unsigned long CNWNXAmia::OnRequestObject (char *gameObject, char* Request){

	Log( "! OnRequestObject: Unknown request: %s\n", Request );

	return OBJECT_INVALID;
}

char* CNWNXAmia::OnRequest(char *gameObject, char* Request, char* Parameters){

	if( strcmp( Request, "KEEPALIVE" ) == 0 ){

		time (&rawtime);
		timeinfo = localtime (&rawtime);
		strftime( timestamp, 64,"%Y-%m-%d %H:%M",timeinfo);

		Log( "o KEEPALIVE: %s\n", timestamp );	

	}
	else if( strcmp( Request, "KILLSERVER" ) == 0 ){
		Log( "o KILLSERVER\n" );
		exit( 0 );
	}
	else if( strcmp( Request, "GETSERVERTIME" ) == 0 ){

		time (&rawtime);
		Log( "o GETSERVERTIME: %ld\n", rawtime );
		sprintf( Parameters, "%ld", rawtime );
	}
	else{
		Log( "! OnRequest: Unknown request: %s\n", Request );
	}
	
	return NULL;
}

/*----------------
--Patch functions
----------------*/