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

#include "NWNXStack.h"
#include "../NWNXdll/madCHook.h"
#include "../NWNXdll/IniFile.h"
#include <exception>
#pragma comment(lib, "madChook.lib")

CNWNXStack::~CNWNXStack(){OnRelease();}

extern CNWNXStack stack;

void Dump(){

	((MyStackWalker*)stack.MSW)->ShowCallstack();
	abort();
}

void MyStackWalker::OnOutput(LPCSTR szText){

	pThis->Log( szText );
	
	StackWalker::OnOutput(szText); 
}

BOOL CNWNXStack::OnRelease( ){

	if( isOn ){

		PVOID * ptr = new PVOID[listlen];

		WORD ret = CaptureStackBackTrace( 0,listlen, ptr, 0 );

		Log( "\nDump addrs:\n" );
		for( WORD n=0;n<ret;n++ ){

			Log( "%u: %08X\n", n, ptr[n] );
		}

		//((MyStackWalker*)MSW)->ShowCallstack();
		delete (MyStackWalker*)MSW;
		delete []ptr;
	}
	
	return CNWNXBase::OnRelease();
}

BOOL CNWNXStack::OnCreate(const char* LogDir){

	char log[MAX_PATH];
	char logrotate[MAX_PATH];

	sprintf( log, "%s\\nwnx_stack.txt", LogDir);
	sprintf( logrotate, "%s\\1\\nwnx_stack.txt", LogDir);

	MoveFile( log, logrotate );

	// call the base class function
	if( !CNWNXBase::OnCreate( log ) )
		return false;

	WriteLogHeader( );

	CIniFile ini( "NWNX.ini" );

	if( !(isOn=ini.ReadBool( "STACK", "on", true )) ){
		Log( "o Stackdump is tured off!\n" );
		return false;
	}

	listlen = ini.ReadInteger( "STACK", "list", 32 );

	MSW = new MyStackWalker;
	((MyStackWalker*)MSW)->pThis=this;
	std::set_terminate( &Dump );

	return true;
}

void CNWNXStack::WriteLogHeader( ){

	fprintf( m_fFile, "NWNXStack v0.1 created by Terra_777\n\n" );
	fflush( m_fFile );	
}

void CNWNXStack::Log( const char * formatting, ... ){

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

unsigned long CNWNXStack::OnRequestObject (char *gameObject, char* Request){

	Log( "o OnRequestObject: 0x%08X: %s( )", gameObject, Request );

	return OBJECT_INVALID;
}

char* CNWNXStack::OnRequest(char *gameObject, char* Request, char* Parameters){

	Log( "o OnRequest: 0x%08X: %s( %s )\n", gameObject, Request, Parameters );

	return NULL;
}

/*----------------
--Patch functions
----------------*/