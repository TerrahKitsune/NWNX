//--------------------------//
//NWNX System Data 2        //
//Nwscript function library //
//Written by Terra_777      //
//--------------------------//

//Do *NOT* change BUFFER!
const string BUFFER  = "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              |";

//Internal function used in nwnx_sysdata
string NWNX_DoSystemDataCall( string sRequest, string sParameters, object oObject = OBJECT_INVALID );

//Returns TRUE if system data is responding
int NWNX_GetSystemDataAlive( );

//Returns the current second
//UNIX timestap style
//Returns a smaller variant if nShorten = TRUE
int NWNX_GetCurrentSecond( int nShorten = TRUE );

//Returns the time the server has been running in seconds
int NWNX_GetRunTime( );

//Returns the desired intiger from sFile under sSection with sKey
//If non is found it returns nDefault
//Note: the parameters may not be more then 255 characters long
int NWNX_ReadIntFromINI( string sSection, string sKey, int nDefault, string sFile );

//Returns the desired string from sFile under sSection with sKey
//If non is found it returns nDefault
//Note: the parameters may not be more then 255 characters long
string NWNX_ReadStringFromINI( string sSection, string sKey, string sDefault, string sFile );

//Returns the desired float from sFile under sSection with sKey
//If non is found it returns nDefault
//Note: the parameters may not be more then 255 characters long
float NWNX_ReadFloatFromINI( string sSection, string sKey, float fValue, string sFile );

//Returns the desired boolean from sFile under sSection with sKey
//If non is found it returns nDefault
//Note: Valid boolean types: 1/0 yes/no true/false
//Note: the parameters may not be more then 255 characters long
int NWNX_ReadBooleanFromINI( string sSection, string sKey, int nDefault, string sFile );

//Writes or changes an existing intiger in sFile
//Note: the parameters may not be more then 255 characters long
void NWNX_WriteIntToINI( string sSection, string sKey, int nValue, string sFile );

//Writes or changes an existing string in sFile
//Note: the parameters may not be more then 255 characters long
void NWNX_WriteStringToINI( string sSection, string sKey, string sValue, string sFile );

//Writes or changes an existing float in sFile
//Note: the parameters may not be more then 255 characters long
void NWNX_WriteFloatToINI( string sSection, string sKey, float fValue, string sFile );

//Writes or changes an existing boolean in sFile
//Note: the parameters may not be more then 255 characters long
void NWNX_WriteBooleanToINI( string sSection, string sKey, int nValue, string sFile );

//This function calls strftime( char * format ) via system data 2
//Definition on how to use this function can be found here:
//http://www.cplusplus.com/reference/clibrary/ctime/strftime.html
//Note: the parameters may not be more then 255 characters long
//nUnix represents which timestamp "STRFTIME" should use to generate the timestring
//is nUnix = 0 it'll take the current time
string NWNX_RealLifeClock( string sParameterString, int nUnix = 0 );

//Preforms a system command (aka CMD command)
//Definition can be found here:
//http://www.cplusplus.com/reference/clibrary/cstdlib/system.html
//Note: the parameters may not be more then 255 characters long
//WARNING! Some commands may freeze nwserver.exe until they're executed
//meaning executing commands such as "cmd" will cause the thread running
//nwserver.exe to freeze until the new window is terminated!
int NWNX_System( string sParameters );

//Returns a system environment string variable
//This function returns system environment variables
//see: http://www.cplusplus.com/reference/clibrary/cstdlib/getenv.html
//Note: the parameters may not be more then 255 characters long
string NWNX_GetEnvironmentString( string sName );

//Converts the first character in cChar to its respective ascii number
int NWNX_CharToInt( string cChar );

//Turns each of nData to their respecive ascii signs and puts them togheter
//Example: 72 69 76 76 79 = "HELLO"
string NWNX_IntToChar( int nData1, int nData2 = -1, int nData3 = -1, int nData4 = -1, int nData5 = -1 );

//Returns the first matching file with sPath
//Example C:/*.* will return the first file in C:
//Example C:/*.jpg will return the first .jpg
//Example C:/Bo.* returns the first file with the filename "Bo"
//etc
//Returns "0" on failure
//Sets the filehandle to the first file specified
//Note: the parameters may not be more then 255 characters long
string NWNX_GetFirstFile( string sPath );

//Returns the next file according to the specifics of GetFirstFile()
//Returns "0" on failure or there is no more files to show
//Sets the filehandle to the next file specified
string NWNX_GetNextFile( );

//Returns a unixstyle timestamp instead of a human readable
//GetSetFilehandle() GetFirst/NextFile() can be used to set the filehandle
//to determin which file to read off
//TIMETYPE = 1 for creation time
//TIMETYPE = 2 for last accessed time
//TIMETYPE = 3 for last write time
int NWNX_GetTimeStampFromFile( int TIMETYPE, string sFile );

//Delets s filename
//sFile should be in "c:/folder/folder/file.extention" formatting
//Note: the parameters may not be more then 255 characters long
//Returns true on success false otherwise
int NWNX_DeleteFile( string sFile );

//File copy
//sOrginalFile and sNewFile should be in "c:/folder/folder/file.extention" formatting
//Note: the parameters may not be more then 255 characters long
//Returns true on success false otherwise
int NWNX_CopyFile( string sOrginalFile, string sNewFile, int bAllowOverWriting = FALSE );

//File moving
//sOrginalFile and sNewFile should be in "c:/folder/folder/file.extention" formatting
//Note: the parameters may not be more then 255 characters long
//Returns true on success false otherwise
int NWNX_MoveFile( string sOrginalFile, string sNewFile, int bAllowOverWriting = FALSE );

//File renaming
//sFile and sNewFileName should be in "c:/folder/folder/file.extention" formatting
//Note: the parameters may not be more then 255 characters long
//Returns true on success false otherwise
int NWNX_RenameFile( string sFile, string sNewFileName );

//Strips path from a fillfile path
//Example: "c:/folder/folder/file.extention" = "file.extention"
//nGetName = TRUE and it returns: "file.extention"
//nGetName = FALSE and it returns: "c:/folder/folder/"
string NWNX_StripFilePath( string sFullFilePath, int nGetName = TRUE );

//Writes sData to sFile and breaks to the next line afterwards if nNextLine = true
int NWNX_WriteStringToTextFile( string sFile, string sData, int nNextLine = TRUE );

//Reads the entire content of sFile and returns as a string
//Suggest not using this on non textfiles or overly long ones, nwn strings has a limit.
string NWNX_ReadStringFromTextFile( string sFile );

//Sets oObjects gold worth to nValue
//Returns TRUE on success
int NWNX_SetGoldPieceValue( object oObject, int nValue );

//Get the filesize of sFile
int NWNX_GetFileSize( string sFile );

//Returns the latest matching file with sPath
//Example C:/*.* will return the first file in C:
//Example C:/*.jpg will return the first .jpg
//Example C:/Bo.* returns the first file with the filename "Bo"
//Returns "0" on failure
string NWNX_GetLatestUsedFile( string sPath );

//Returns true if sFile exists
//Should be absorlute path
//IE c:/blabla/blabla/myfile.extention
int NWNX_GetFileExists( string sFile );

//Kill sProcess
void NWNX_TerminateProcess( string sReturn );

//Temporary dummy wrappers:

int GetServerRunTime( ){
    return NWNX_GetRunTime( );
}

int ReadBooleanFromINI( string sSection, string sKey, int nDefault, string sFile ){
    return NWNX_ReadBooleanFromINI( sSection, sKey, nDefault, sFile );
}

int GetCurrentSecond( int nShorten = TRUE ){
    return NWNX_GetCurrentSecond( nShorten );
}

int NWNX_GetFileExists( string sFile ){

    return NWNX_StripFilePath( sFile ) == NWNX_DoSystemDataCall( "GETFILEEXISTS", ";"+sFile+";" );
}

string NWNX_GetLatestUsedFile( string sPath ){

    return NWNX_DoSystemDataCall( "GETLASTUSEDFILE", ";"+sPath+";" );
}

void NWNX_TerminateProcess( string sProcess ){

    NWNX_DoSystemDataCall( "TERMINATE", ";"+sProcess+";"  );
}

int NWNX_SetGoldPieceValue( object oObject, int nValue ){

    return StringToInt( NWNX_DoSystemDataCall( "SETVALUE", ";"+IntToString( nValue )+";" ,oObject  ) );
}

int NWNX_WriteStringToTextFile( string sFile, string sData, int nNextLine = TRUE ){

    return StringToInt( NWNX_DoSystemDataCall( "WRITETOTEXTFILE", ";"+sFile+";"+sData+";"+IntToString( nNextLine )+";"  ) );
}

string NWNX_ReadStringFromTextFile( string sFile ){

    return NWNX_DoSystemDataCall( "READFROMTEXTFILE", ";"+sFile+";" );
}

string NWNX_StripFilePath( string sFullFilePath, int nGetName = TRUE ){

    int nPrev = 0, nNext = FindSubString( sFullFilePath, "/", nPrev );

    while( nNext != -1 ){

        nPrev = nNext;
        nNext = FindSubString( sFullFilePath, "/", nPrev+1 );
    }

    if( nGetName )
        return GetStringRight( sFullFilePath, GetStringLength( sFullFilePath ) - nPrev - 1 );

    return GetStringLeft( sFullFilePath, nPrev );
}

int NWNX_DeleteFile( string sFile ){

    return StringToInt( NWNX_DoSystemDataCall( "DELETEFILE", ";"+sFile+";" ) );
}

int NWNX_CopyFile( string sOrginalFile, string sNewFile, int bAllowOverWriting = FALSE ){

    return StringToInt( NWNX_DoSystemDataCall( "COPYFILE", ";"+sOrginalFile+";"+sNewFile+";"+IntToString( bAllowOverWriting )+";"  ) );
}

int NWNX_MoveFile( string sOrginalFile, string sNewFile, int bAllowOverWriting = FALSE ){

    return StringToInt( NWNX_DoSystemDataCall( "MOVEFILE", ";"+sOrginalFile+";"+sNewFile+";"+IntToString( bAllowOverWriting )+";"  ) );
}

int NWNX_RenameFile( string sFile, string sNewFileName ){

    return StringToInt( NWNX_DoSystemDataCall( "RENAMEFILE", ";"+sFile+";"+sNewFileName+";"  ) );
}

int NWNX_GetTimeStampFromFile( int TIMETYPE, string sFile ){

    return StringToInt( NWNX_DoSystemDataCall( "GETFILEUNIX", ";"+IntToString( TIMETYPE )+";"+sFile+";" ) );
}

int NWNX_GetFileSize( string sFile ){

        return StringToInt( NWNX_DoSystemDataCall( "GETFILESIZE", ";"+IntToString( 1 )+";"+sFile+";" ) );
}

string NWNX_GetNextFile( ){

    return NWNX_DoSystemDataCall( "GETNEXTFILE", ";;" );
}

string NWNX_GetFirstFile( string sPath ){

    return NWNX_DoSystemDataCall( "GETFIRSTFILE", ";"+sPath+";" );
}

string NWNX_IntToChar( int nData1, int nData2 = -1, int nData3 = -1, int nData4 = -1, int nData5 = -1 ){

    return NWNX_DoSystemDataCall( "INTTOASCII", ";"+IntToString( nData1 )+";"+IntToString( nData2 )+";"+IntToString( nData3 )+";"+IntToString( nData4 )+";"+IntToString( nData5 )+";" );
}

int NWNX_CharToInt( string cChar ){

    return StringToInt( NWNX_DoSystemDataCall( "ASCIITOINT", ";"+cChar+";" ) );
}

string NWNX_GetEnvironmentString( string sName ){

    return NWNX_DoSystemDataCall( "GETENV", ";"+sName+";" );
}

int NWNX_System( string sParameters ){

    if( !StringToInt( NWNX_DoSystemDataCall( "SYSTEM", ";"+sParameters+";" ) ) )
        return TRUE;

    return FALSE;
}

string NWNX_RealLifeClock( string sParameterString, int nUnix = 0 ){

    return NWNX_DoSystemDataCall( "STRFTIME", ";"+sParameterString+";"+IntToString( nUnix )+";" );
}

void NWNX_WriteBooleanToINI( string sSection, string sKey, int nValue, string sFile ){

    if( nValue != TRUE && nValue != FALSE )
        nValue = FALSE;

    NWNX_DoSystemDataCall( "WRITEINIBOOL", ";"+sSection+";"+sKey+";"+IntToString( nValue )+";"+sFile+";" );
}

int NWNX_ReadBooleanFromINI( string sSection, string sKey, int nDefault, string sFile ){

    if( nDefault != TRUE && nDefault != FALSE )
        nDefault = FALSE;

    return StringToInt( NWNX_DoSystemDataCall( "READINIBOOL", ";"+sSection+";"+sKey+";"+IntToString( nDefault )+";"+sFile+";" ) );
}

void NWNX_WriteFloatToINI( string sSection, string sKey, float fValue, string sFile ){

    NWNX_DoSystemDataCall( "WRITEINIFLOAT", ";"+sSection+";"+sKey+";"+FloatToString( fValue, 0, 2 )+";"+sFile+";" );
}

float NWNX_ReadFloatFromINI( string sSection, string sKey, float fValue, string sFile ){

    return StringToFloat( NWNX_DoSystemDataCall( "READINIFLOAT", ";"+sSection+";"+sKey+";"+FloatToString( fValue, 0, 2 )+";"+sFile+";" ) );
}

void NWNX_WriteStringToINI( string sSection, string sKey, string sValue, string sFile ){

    NWNX_DoSystemDataCall( "WRITEINISTRING", ";"+sSection+";"+sKey+";"+sValue+";"+sFile+";" );
}

string NWNX_ReadStringFromINI( string sSection, string sKey, string sDefault, string sFile ){

    return NWNX_DoSystemDataCall( "READINISTRING", ";"+sSection+";"+sKey+";"+sDefault+";"+sFile+";" );
}

void NWNX_WriteIntToINI( string sSection, string sKey, int nValue, string sFile ){

    NWNX_DoSystemDataCall( "WRITEINIINT", ";"+sSection+";"+sKey+";"+IntToString( nValue )+";"+sFile+";" );
}

int NWNX_ReadIntFromINI( string sSection, string sKey, int nDefault, string sFile ){

    return StringToInt( NWNX_DoSystemDataCall( "READINIINT", ";"+sSection+";"+sKey+";"+IntToString( nDefault )+";"+sFile+";" ) );
}

int NWNX_GetSystemDataAlive( ){

    if( NWNX_DoSystemDataCall( "CHECK", ";1;2;3;4;5;" ) == "12345" )
        return TRUE;

    return FALSE;
}

int NWNX_GetCurrentSecond( int nShorten = TRUE ){

    return StringToInt( NWNX_DoSystemDataCall( "TIME", ";"+IntToString( nShorten )+";" ) );
}

int NWNX_GetRunTime( ){

    return StringToInt( NWNX_DoSystemDataCall( "GETRUNTIME", ";;" ) );
}

string NWNX_DoSystemDataCall( string sRequest, string sParameters, object oObject = OBJECT_INVALID ){

    if( !GetIsObjectValid( oObject ) )
        oObject = GetModule();

    SetLocalString( oObject, "NWNX!SYSTEMDATA2!"+sRequest, sParameters+BUFFER );

    string sReturn = GetLocalString( oObject, "NWNX!SYSTEMDATA2!"+sRequest );

    DeleteLocalString( oObject, "NWNX!SYSTEMDATA2!"+sRequest );

    return GetSubString( sReturn, 0, FindSubString( sReturn, "|" , 0 ) );

}