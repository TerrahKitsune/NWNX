//void main(){}

//Opens a sqlite file
//You can have multiple of these open
//returns true/false for success or failure
int DATA_OpenSQLite( string sSQLiteFile );

//Querry an open sqlite file
//returns true/false for success or failure
int DATA_ExecSQLite( string sSQLiteFile, string sQuerry );

//Querry and fetch an open sqlite file
//Works the same as DATA_ExecSQLite but this prepares a resultset
//This also sets the cursor to the first row
//use StepSQLite to advance to the next row
//returns true/false for success or failure
int DATA_FetchSQLite( string sSQLiteFile, string sQuerry );

//Step to the next row in the resultset
//this should only be called after a FetchSQLite
//Returns true if successful, false if there is no more rows
int DATA_StepSQLite( string sSQLiteFile );

//Get the data in the column of the current row
//Returns an empty string on failure
//sBuffer: should be as long (or longer) then the result you're expecting
string DATA_GetSQLite( string sSQLiteFile, int nColumn, string sBuffer );

//Close the SQLite file
//returns true/false for success or failure
int DATA_CloseSQLite( string sSQLiteFile );

//Turn on or off logging
//nSet should be true or false
void DATA_SetLogging( int nSet );

void DATA_SetLogging( int nSet ){

    SetLocalString( OBJECT_SELF, "NWNX!DATA!7", IntToString( nSet ) );
    DeleteLocalString( OBJECT_SELF, "NWNX!DATA!7" );
}

int DATA_CloseSQLite( string sSQLiteFile ){

    SetLocalString( OBJECT_SELF, "NWNX!DATA!6", sSQLiteFile );
    int nReturn = StringToInt( GetLocalString( OBJECT_SELF, "NWNX!DATA!6" ) );
    DeleteLocalString( OBJECT_SELF, "NWNX!DATA!6" );
    return nReturn;
}

string DATA_GetSQLite( string sSQLiteFile, int nColumn, string sBuffer ){

    SetLocalString( OBJECT_SELF, "NWNX!DATA!5", sSQLiteFile+" "+IntToString( nColumn )+" "+sBuffer );
    string nReturn = GetLocalString( OBJECT_SELF, "NWNX!DATA!5" );
    DeleteLocalString( OBJECT_SELF, "NWNX!DATA!5" );
    return nReturn;
}

int DATA_StepSQLite( string sSQLiteFile ){

    SetLocalString( OBJECT_SELF, "NWNX!DATA!4", sSQLiteFile );
    int nReturn = StringToInt( GetLocalString( OBJECT_SELF, "NWNX!DATA!4" ) );
    DeleteLocalString( OBJECT_SELF, "NWNX!DATA!4" );
    return nReturn;
}

int DATA_FetchSQLite( string sSQLiteFile, string sQuerry ){

    SetLocalString( OBJECT_SELF, "NWNX!DATA!3", sSQLiteFile+" "+sQuerry );
    int nReturn = StringToInt( GetLocalString( OBJECT_SELF, "NWNX!DATA!3" ) );
    DeleteLocalString( OBJECT_SELF, "NWNX!DATA!3" );
    return nReturn;
}

int DATA_ExecSQLite( string sSQLiteFile, string sQuerry ){

    SetLocalString( OBJECT_SELF, "NWNX!DATA!2", sSQLiteFile+" "+sQuerry );
    int nReturn = StringToInt( GetLocalString( OBJECT_SELF, "NWNX!DATA2" ) );
    DeleteLocalString( OBJECT_SELF, "NWNX!DATA!2" );
    return nReturn;
}

int DATA_OpenSQLite( string sSQLiteFile ){

    SetLocalString( OBJECT_SELF, "NWNX!DATA!1", sSQLiteFile );
    int nReturn = StringToInt( GetLocalString( OBJECT_SELF, "NWNX!DATA!1" ) );
    DeleteLocalString( OBJECT_SELF, "NWNX!DATA!1" );
    return nReturn;
}
