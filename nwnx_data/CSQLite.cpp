#include "CSQLite.h"

CSQLite::CSQLite( const char * databas ){

	stmt=NULL;

	if( sqlite3_open( databas ,&db ) != SQLITE_OK ){

		db=NULL;
	}
}
	
CSQLite::~CSQLite(){

	if( db )
		sqlite3_close(db);
}

bool CSQLite::Exec( const char * querry ){

	if( !db )
		return false;

	return sqlite3_exec( db, querry, NULL, NULL, NULL ) == SQLITE_OK;
}

bool CSQLite::Fetch( const char * querry ){

	if( !db )
		return false;
	
	return sqlite3_prepare_v2(db,querry,-1,&stmt,0) == SQLITE_OK;
}

bool CSQLite::Step( void ){

	return sqlite3_step(stmt) == SQLITE_ROW;
}

bool CSQLite::Get( int nColumn, char * data, int dataLen ){

	if( !stmt )
		return false;

	if( sqlite3_data_count( stmt ) < nColumn )
		return false;

	int nDataMax = sqlite3_column_bytes( stmt, nColumn );
	if( nDataMax > dataLen )
		nDataMax = dataLen;

	const unsigned char * row = sqlite3_column_text(stmt, nColumn);
	if( row ){

		memcpy( data, row, dataLen );

		return true;
	}
	
	return false;
}

const char * CSQLite::GetLastError( ){

	return sqlite3_errmsg( db );		
}