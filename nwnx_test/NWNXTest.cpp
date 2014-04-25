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

#include "NWNXTest.h"
#include "../NWNXdll/madCHook.h"

#pragma comment(lib, "madChook.lib")

extern CNWNXTest test;

int (__stdcall *NWSERVER_bindNext)( SOCKET s, const struct sockaddr * name, int namelen );
int __stdcall NWSERVER_bind( SOCKET s, const struct sockaddr * name, int namelen ){
	
	test.Log( "o Test\n" );

	return NWSERVER_bindNext( s, name, namelen );
}




CNWNXTest::~CNWNXTest(){OnRelease();}

BOOL CNWNXTest::OnRelease( ){

	/*time( &rawtime );

	WriteToFLog( "[STOP] SESSION END! Ran for %ld minutes!\n\n", ((rawtime-start)/60) );

	if( flog ){
		fflush( flog );
		fclose( flog );
	}*/

	return CNWNXBase::OnRelease();
}

BOOL CNWNXTest::OnCreate(const char* LogDir){

	char log[MAX_PATH];
	
	sprintf( log, "%s\\nwnx_test.txt", LogDir);

	// call the base class function
	if( !CNWNXBase::OnCreate( log ) )
		return false;

	WriteLogHeader( );

	/*flog = fopen( "test_logging.txt", "a" );
	if( !flog )
		flog = fopen( "test_logging.txt", "w" );

	WriteToFLog( "[START] SESSION START!\n" );
	time( &start );


	if( !flog ){

		Log( "! Failed to open test_logging.txt!\n" );
	}*/

	if( HookCode( (PVOID)0x006332CC, NWSERVER_bind, (PVOID*)&NWSERVER_bindNext ) )
		Log( "o Hooked NWServer bind!\n" );
	else
		Log( "! Failed to hook NWServer bind!\n" );

	return true;
}

void CNWNXTest::WriteLogHeader( ){

	fprintf( m_fFile, "NWNXTest v0.0 created by Terra_777\n\n" );
	fflush( m_fFile );	
}

void CNWNXTest::Log( const char * formatting, ... ){

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

unsigned long CNWNXTest::OnRequestObject (char *gameObject, char* Request){

	Log( "o OnRequestObject: 0x%08X: %s( )", gameObject, Request );

	return OBJECT_INVALID;
}

char* CNWNXTest::OnRequest(char *gameObject, char* Request, char* Parameters){

	Log( "o OnRequest: 0x%08X: %s( %s )\n", gameObject, Request, Parameters );

	return NULL;
}

/*----------------
--Patch functions
----------------*/

void CNWNXTest::WriteToFLog( const char * formatting, ... ){

	va_list argList;

	if( flog ){  

		// build up the string
		va_start( argList, formatting );
		vsprintf( acBuffer, formatting, argList );
		va_end( argList );

		time (&rawtime);
		timeinfo = localtime (&rawtime);
		strftime( timestamp, 64,"%Y-%m-%d %H:%M",timeinfo);

		fprintf( flog, "[%s] %s", timestamp, acBuffer );
		fflush( flog );
	}
}