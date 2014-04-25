#ifndef __SQLITE__
#define __SQLITE__

#include "sqlite3.h"
#include <stdlib.h>
#include <string.h>

class CSQLite{

public:
	CSQLite( const char * databas );
	~CSQLite();
	bool Exec( const char * querry );
	bool Fetch( const char * querry );
	bool Step( void );
	bool Get( int nColumn, char * data, int dataLen );
	const char * GetLastError( );

	sqlite3	*db;			/* den öppnade databasen */
	sqlite3_stmt *stmt;		/* kursorn vid SELECT-satser */	
};

#endif
