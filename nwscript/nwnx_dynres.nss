//void main(){}

const string MEM = "                                                                                                                                                                                                                                                                   ";

//Adds a file to the dynamic loader
//sFile: must be resref+extension IE: "creature.utc"
//sFullPath: must be the path to a full file IE: "C:/nwn/stuff/creature.utc"
int DYNRES_AddFile( string sFile, string sFullPath );

//Removes a file to the dynamic loader
//sFile: resref+extension IE: "creature.utc"
int DYNRES_RemoveFile( string sFile );

//Returns the full file the loeader will load when sFile is requested
//sFile: resref+extension IE: "creature.utc"
string DYNRES_GetFullFile( string sFile );

//Get first file in the loader list
//!!Spawning resources or using other DYNRES functions will reset the first/next loop!!
string DYNRES_GetFirst( );

//Get the next file in the loader list
//!!Spawning resources or using other DYNRES functions will reset the first/next loop!!
string DYNRES_GetNext( );

//Returns true if the file exists, must be a full file path with extension
int DYNRES_FileExist( string sFullFile );

//This will get the first or next area in the list, only returns area resrefs
//nFirst must be true to reset the list
//returns empty string on error or end of list
string DYNRES_GetFirstNextArea( int nFirst );

//Returns the capsule filter, the filter is a list delmited by dots of extensions
string DYNRES_GetFilter( );

//Sets a new filter, this has to be a list delmited by dots
//Must start with a dot
//IE: ".ncs.txt.exe.utc.mod.etc.etc.etc"
void DYNRES_SetFilter( string sNewFilter );

//Caches a file, the file must be in the register
//sFile: resref+extension IE: "creature.utc"
//Returns true if the file was successfully cached
int DYNRES_CacheFile( string sFile );

//Creates sFile, sFile should be a complete path
//sFile will be overwritten if it exsists
//If an extension is provided it'll be overwritten with the object typing
//IE: If a file is specified as "C:/test.txt" for a creature the
//file will be changed to "C:/text.utc"
//This only works on creatures and items
//This function returns the size of the new file in bytes or 0 if it failed
//use DYNRES_LastFile to return the name of the file that was written
int DYNRES_SaveObjectToFile( string sFile, object oObject );

//This will return the last file that DYNRES handled
string DYNRES_LastFile( );

//Returns the first file inside sCapsule
//sCapsule must be a complete path
//Returns an empty string is none is found
string DYNRES_GetFirstFileInCapsule( string sCapsule );

//Get the next file in the capsule defined by DYNRES_GetFirstFileInCapsule
//Returns empty string if no more or invalid
string DYNRES_GetNextFileInCapsule( );

//Locates sFile in sCapsule and writes it to sNewFile
//sCapsule and sNewFile must be complete with filepath
//If sNewFile exists it'll be overwritten
//Returns true/false
int DYNRES_ExtractFromCapsule( string sFile, string sCapsule, string sNewFile );

//sFile: resref+extension IE: "script.lua"
//Returns the raw contents of a cached file
string DYNRES_GetRaw( string sFile );

string DYNRES_GetRaw( string sFile ){
    SetLocalString( OBJECT_SELF, "NWNX!DYNRES!RAW", sFile );
    string nRet = GetLocalString( OBJECT_SELF, "NWNX!DYNRES!RAW" );
    DeleteLocalString( OBJECT_SELF, "NWNX!DYNRES!RAW" );
    return nRet;
}

int DYNRES_ExtractFromCapsule( string sFile, string sCapsule, string sNewFile ){

    SetLocalString( OBJECT_SELF, "NWNX!DYNRES!EXT", sFile+"|"+sCapsule+"|"+sNewFile );
    string nRet = GetLocalString( OBJECT_SELF, "NWNX!DYNRES!EXT" );
    DeleteLocalString( OBJECT_SELF, "NWNX!DYNRES!EXT" );
    return StringToInt( nRet );
}

string DYNRES_GetNextFileInCapsule( ){

    SetLocalString( OBJECT_SELF, "NWNX!DYNRES!EFN", MEM );
    string nRet = GetLocalString( OBJECT_SELF, "NWNX!DYNRES!EFN" );
    DeleteLocalString( OBJECT_SELF, "NWNX!DYNRES!EFN" );
    return nRet;
}

string DYNRES_GetFirstFileInCapsule( string sCapsule ){

    SetLocalString( OBJECT_SELF, "NWNX!DYNRES!EFF", sCapsule );
    string nRet = GetLocalString( OBJECT_SELF, "NWNX!DYNRES!EFF" );
    DeleteLocalString( OBJECT_SELF, "NWNX!DYNRES!EFF" );
    return nRet;
}

string DYNRES_LastFile( ){

    SetLocalString( OBJECT_SELF, "NWNX!DYNRES!LST", MEM );
    string nRet = GetLocalString( OBJECT_SELF, "NWNX!DYNRES!LST" );
    DeleteLocalString( OBJECT_SELF, "NWNX!DYNRES!LST" );
    return nRet;
}

int DYNRES_SaveObjectToFile( string sFile, object oObject ){

    return StoreCampaignObject( "DYNRES", sFile+"|                               ", oObject );
}

int DYNRES_CacheFile( string sFile ){

    SetLocalString( OBJECT_SELF, "NWNX!DYNRES!CAC", sFile );
    int nRet = StringToInt( GetLocalString( OBJECT_SELF, "NWNX!DYNRES!CAC" ) );
    DeleteLocalString( OBJECT_SELF, "NWNX!DYNRES!CAC" );
    return nRet;
}

void DYNRES_SetFilter( string sNewFilter ){
    SetLocalString( OBJECT_SELF, "NWNX!DYNRES!FIL", sNewFilter );
    DeleteLocalString( OBJECT_SELF, "NWNX!DYNRES!FIL" );
}

string DYNRES_GetFilter( ){

    SetLocalString( OBJECT_SELF, "NWNX!DYNRES!GIL", MEM );
    string nRet = GetLocalString( OBJECT_SELF, "NWNX!DYNRES!GIL" );
    DeleteLocalString( OBJECT_SELF, "NWNX!DYNRES!GIL" );
    return nRet;
}

string DYNRES_GetFirstNextArea( int nFirst ){

    SetLocalString( OBJECT_SELF, "NWNX!DYNRES!ARE", IntToString(nFirst)+"                              " );
    string nRet = GetLocalString( OBJECT_SELF, "NWNX!DYNRES!ARE" );
    DeleteLocalString( OBJECT_SELF, "NWNX!DYNRES!ARE" );
    return nRet;
}

int DYNRES_FileExist( string sFullFile ){

    SetLocalString( OBJECT_SELF, "NWNX!DYNRES!EXI", sFullFile );
    int nRet = StringToInt( GetLocalString( OBJECT_SELF, "NWNX!DYNRES!EXI" ) );
    DeleteLocalString( OBJECT_SELF, "NWNX!DYNRES!EXI" );
    return nRet;
}

int DYNRES_AddFile( string sFile, string sFullPath ){

    SetLocalString( OBJECT_SELF, "NWNX!DYNRES!ADD", sFile+"|"+sFullPath );
    int nRet = StringToInt( GetLocalString( OBJECT_SELF, "NWNX!DYNRES!ADD" ) );
    DeleteLocalString( OBJECT_SELF, "NWNX!DYNRES!ADD" );
    return nRet;
}

int DYNRES_RemoveFile( string sFile ){

    SetLocalString( OBJECT_SELF, "NWNX!DYNRES!REM", sFile );
    int nRet = StringToInt( GetLocalString( OBJECT_SELF, "NWNX!DYNRES!REM" ) );
    DeleteLocalString( OBJECT_SELF, "NWNX!DYNRES!REM" );
    return nRet;
}

string DYNRES_GetFullFile( string sFile ){

    SetLocalString( OBJECT_SELF, "NWNX!DYNRES!GET", sFile+MEM );
    string nRet = GetLocalString( OBJECT_SELF, "NWNX!DYNRES!GET" );
    DeleteLocalString( OBJECT_SELF, "NWNX!DYNRES!GET" );
    return nRet;
}

string DYNRES_GetFirst( ){

    SetLocalString( OBJECT_SELF, "NWNX!DYNRES!FST", MEM );
    string nRet = GetLocalString( OBJECT_SELF, "NWNX!DYNRES!FST" );
    DeleteLocalString( OBJECT_SELF, "NWNX!DYNRES!FST" );
    return nRet;
}

string DYNRES_GetNext( ){

    SetLocalString( OBJECT_SELF, "NWNX!DYNRES!NXT", MEM );
    string nRet = GetLocalString( OBJECT_SELF, "NWNX!DYNRES!NXT" );
    DeleteLocalString( OBJECT_SELF, "NWNX!DYNRES!NXT" );
    return nRet;
}