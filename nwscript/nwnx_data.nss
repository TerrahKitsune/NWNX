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

//Save an object to a file. This only works on items and creatures.
//Path should be the full path along with filename.
//Extension will be set automaticly, .bic for creatures and .uti for items.
int DATA_SaveToFile( object oObject, string sPath );

//Retrive an uti file or bicfile, sFile should be the full path
//Items will attempt spawning in oTargets inventory otherwise its created on lTarget
object DATA_GetFromFile( string sFile, object oTarget, location lTarget );

object DATA_GetFromFile( string sFile, object oTarget, location lTarget ){
    return RetrieveCampaignObject( "DATA", sFile, lTarget, oTarget );
}

int DATA_SaveToFile( object oObject, string sPath ){
    return StoreCampaignObject( "DATA", sPath, oObject );
}

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
