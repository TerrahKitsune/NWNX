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

#include "NWNXData.h"

BOOL CNWNXData::OnRelease( ){
	CloseAll();
	return CNWNXBase::OnRelease();
}

BOOL CNWNXData::OnCreate(const char* LogDir){

	char log[MAX_PATH];
	
	sprintf( log, "%s\\nwnx_data.txt", LogDir);

	// call the base class function
	if( !CNWNXBase::OnCreate( log ) )
		return false;

	WriteLogHeader( );

	DOArray=NULL;
	DOArrayLen=0;
	LastObject=NULL;
	nLog=1;
	return true;
}

void CNWNXData::WriteLogHeader( ){

	fprintf( m_fFile, "NWNXData v0.1 created by Terra_777\n\n" );
	fflush( m_fFile );	
}

void CNWNXData::Log( const char * formatting, ... ){

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

unsigned long CNWNXData::OnRequestObject (char *gameObject, char* Request){

	Log( "o OnRequestObject: 0x%08X: %s( )", gameObject, Request );

	return OBJECT_INVALID;
}

char* CNWNXData::OnRequest(char *gameObject, char* Request, char* Parameters){

	switch( atoi( Request ) ){
		
		case 1: OpenSQLLiteDatabase( Parameters ); break;
		case 2: SQLiteExec( Parameters, 0 ); break;
		case 3: SQLiteExec( Parameters, 1 ); break;
		case 4: SQLiteStep( Parameters );break;
		case 5: SQLiteGet( Parameters );break;
		case 6: sprintf( Parameters, "%i", CloseObject( Parameters, 1 ) );break;
		case 7: nLog=atoi( Parameters );break;
		default:break;
	}
	
	return NULL;
}

/*----------------
--Patch functions
----------------*/

DataObject * CNWNXData::AddDataObject( DataObject * obj ){

	DataObject * temp = GetDataObject( obj->name, obj->nType );
	if( temp ){
		LastObject=temp;
		return temp;
	}
	for( int n=0;n<DOArrayLen;n++ ){

		if( DOArray[n].nType == 0 ){
			temp=&DOArray[n];
			break;
		}
	}

	if( !temp ){

		temp = (DataObject *)realloc( DOArray, DOArrayLen+1 );
		if( temp )
			DOArray = temp;
		else
			return NULL;

		temp = &DOArray[DOArrayLen];
		DOArrayLen++;
	}

	memcpy( temp, obj, sizeof( DataObject ) );
	LastObject=temp;
	return LastObject;
}

DataObject * CNWNXData::GetDataObject( const char * name, int nType ){

	if( LastObject ){

		if( LastObject->nType = nType && strcmp( name, LastObject->name ) == 0 ){
			return LastObject;
		}
	}

	for( int n=0;n<DOArrayLen;n++ ){

		if( nType == DOArray[n].nType && strcmp( name, DOArray[n].name ) == 0 ){
			LastObject=&DOArray[n];
			return &DOArray[n];
		}
	}
	LastObject=NULL;
	return NULL;
}

bool CNWNXData::CloseObject( const char * name, int nType ){

	LastObject=NULL;

	int nIndex = -1;

	for( int n=0;n<DOArrayLen;n++ ){

		if( nType == DOArray[n].nType && strcmp( name, DOArray[n].name ) == 0 ){
			
			nIndex = n;
			break;
		}
	}

	if( nIndex == -1 )
		return false;

	//SQLITE
	if( DOArray[nIndex].nType == 1 ){
		
		if( nLog )
			Log( "o CloseObject: Closed SQLite %s\n", DOArray[nIndex].name );

		//Pop destructor on sqlite object
		CSQLite * sq = (CSQLite *)DOArray[nIndex].object;
		delete sq;
	}
	
	
	free( DOArray[nIndex].name );
	
	if( nIndex == DOArrayLen-1 ){

		memset( &DOArray[nIndex], 0, sizeof( DataObject ) );
	}
	else{

		memcpy( &DOArray[nIndex], &DOArray[nIndex+1], ((DOArrayLen-1)-nIndex)*sizeof( DataObject ) );
		memset( &DOArray[DOArrayLen-1], 0, sizeof( DataObject ) );
	}
	DOArrayLen--;

	return true;
}

void CNWNXData::CloseAll(){

	for( int n=0;n<DOArrayLen;n++ ){

		//Specific closure for different objects
		if( DOArray[n].nType == 1 ){
		
			if( nLog )
				Log( "o CloseAll( ): Closed sqlite %s\n", DOArray[n].name );

			//Pop destructor on sqlite object
			CSQLite * sq = (CSQLite *)DOArray[n].object;
			delete sq;
		}

		if( DOArray[n].name )
			free( DOArray[n].name );
	}

	free( DOArray );
	DOArray=NULL;
	DOArrayLen=0;

	if( nLog )
		Log( "o CloseAll( ): Closed all objects\n" );
}

bool CNWNXData::OpenSQLLiteDatabase( char * input ){

	if( GetDataObject( input, 1 ) ){

		if( nLog )
			Log( "o OpenSQLLiteDatabase: SQLite database %s already open\n", input );
		
		strcpy( input, "1" );
		return true;
	}
	
	CSQLite * sq = new CSQLite( input );
	if( sq ){
		
		if( !sq->db ){
			
			delete sq;
			if( nLog )
				Log( "! OpenSQLLiteDatabase: Failed to open SQLite database %s\n", input );
			strcpy( input, "0" );
			return false;
		}

		DataObject obj;

		obj.name = (char*)malloc( strlen( input )+1 );
		strcpy( obj.name, input );
		obj.nType=1;
		obj.object = sq;
		AddDataObject( &obj );

		if( nLog )
			Log( "o OpenSQLLiteDatabase: Opened SQLite database %s\n",obj.name );

		strcpy( input, "1" );
		return true;
	}

	if( nLog )
		Log( "! OpenSQLLiteDatabase: Failed to create SQLite object %s\n", input );

	strcpy( input, "0" );
	return false;
}

bool CNWNXData::SQLiteExec( char * input, int nFetch ){

	DWORD Len = strlen( input );
	char * db=NULL;
	char * querry=NULL;

	for( int n=0;n<Len;n++ ){

		if( input[n]==' ' ){
			input[n]='\0';
			db=input;
			querry=&input[n+1];
			break;
		}
	}

	if( !querry ){
		
		if( nLog ){
			if( nFetch )
				Log( "! SQLiteFetch( %s ): Cannot parse querry\n", db );
			else
				Log( "! SQLiteExec( %s ): Cannot parse querry\n", db );
		}

		strcpy( input, "0" );
		return false;
	}

	DataObject * DO = GetDataObject( db, 1 );
	if( !DO ){

		if( nLog ){
			if( nFetch )
				Log( "! SQLiteFetch( %s ): Database not open: %s\n", db );
			else
				Log( "! SQLiteExec( %s ): Database not open: %s\n", db );
		}
		strcpy( input, "0" );
		return false;
	}

	CSQLite * sq = (CSQLite *)DO->object;

	if( nFetch ){

		if( sq->Fetch( querry ) ){

			if( !sq->Step( ) ){

				if( nLog )
					Log( "o SQLiteFetch( %s ): no rows available: %s\n", db, querry );

				strcpy( input, "1" );
				return true;
			}

			if( nLog )
				Log( "o SQLiteFetch( %s ): executed querry successfully: %s\n", db, querry );
			strcpy( input, "1" );
			return true;
		}

		if( nLog )
			Log( "! SQLiteFetch( %s ): failed to execute: %s\n! Error: %s\n", db, querry, sq->GetLastError( ) );
		strcpy( input, "0" );
		return false;
	}

	if( sq->Exec( querry ) ){

		if( nLog )
			Log( "o SQLiteExec( %s ): executed querry successfully: %s\n", db, querry );
		strcpy( input, "1" );
		return true;
	}

	if( nLog )
		Log( "! SQLiteExec( %s ): failed to execute: %s\n! Error: %s\n", db, querry, sq->GetLastError( ) );
	strcpy( input, "0" );
	return false;
}

bool CNWNXData::SQLiteStep( char * input ){

	DataObject * DO = GetDataObject( input, 1 );
	if( !DO ){
		
		if( nLog )
			Log( "! SQLiteStep( %s ): Database not open\n", input );

		strcpy( input, "0" );
		return false;
	}

	CSQLite * sq = (CSQLite *)DO->object;
	if( sq->Step( ) ){
		
		if(nLog)Log( "o SQLiteStep( %s ): successfully stepped\n", input );
		strcpy( input, "1" );
		return true;
	}

	if(nLog)Log( "o SQLiteStep( %s ): no more rows\n", input );
	strcpy( input, "0" );
	return false;
}

bool CNWNXData::SQLiteGet( char * input ){

	char db[50];
	int column;
	
	DWORD len = strlen( input );

	sscanf( input, "%s %i", db, &column );

	DataObject * DO = GetDataObject( db, 1 );
	if( !DO ){
		
		if( nLog )
			Log( "! SQLiteGet( %s, %i ): Database not open\n", db, column );

		input[0]='\0';
		return false;
	}

	memset( input, 0, len );
	CSQLite * sq = (CSQLite *)DO->object;
	if( sq->Get( column, input, len ) ){
		if( nLog )
			Log( "o SQLiteGet( %s, %i ): %s\n", db, column, input );
		return true;
	}

	if( nLog )
		Log( "! SQLiteGet( %s, %i ): Unable to get data from column\n", db, column );

	input[0]='\0';
	return false;
}