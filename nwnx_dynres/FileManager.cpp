#include "FileManager.h"

CFileManager::CFileManager(){

	GetCurrentDirectory( MAX_PATH, BasePath );
	FILES_COUNTED=0;
	FirstNextHandle=NULL;
	CustomData=NULL;
	CustomDataLength=0;
	for( unsigned int n=0;n<strlen( BasePath );n++ ){

		if( BasePath[n]=='/' )
			BasePath[n]='\\';
	}
}

CFileManager::CFileManager( const char * path ){

	strcpy( BasePath, path );
	FILES_COUNTED=0;
	FirstNextHandle=NULL;
	CustomData=NULL;
	CustomDataLength=0;
	for( unsigned int n=0;n<strlen( BasePath );n++ ){

		if( BasePath[n]=='/' )
			BasePath[n]='\\';
	}

}

CFileManager::~CFileManager(){

	if( CustomData )
		free( CustomData );
}

char CFileManager::GetFirstNextDriveLetter( bool first ){

	if( first ){
		FirstNext=0;
		DriveLetterMask = GetLogicalDrives();
	}
	else
		FirstNext++;

	char creturn = 0;

	for( int n=FirstNext;n<32;n++ ){

		if( ((DriveLetterMask & (1 << n )) > 0) ){

			creturn = 'A';

			creturn += n;

			FirstNext=n;

			return creturn;
		}
	}

	return creturn;	
}

WIN32_FIND_DATA * CFileManager::GetCurrentFileData( void ){

	return &LAST_FILE_DATA;
}

WIN32_FIND_DATA * CFileManager::GetFirstNextFile( const char * fileFilter, bool first ){

	bool Success;

	if( first ){

		if( FirstNextHandle ){		
			FindClose( FirstNextHandle );
			FirstNextHandle=NULL;
		}

		char Path[MAX_PATH];
		strcpy( Path, fileFilter );
		if( !FormatPath( Path ) )
			return 0;


		FirstNextHandle = FindFirstFile( Path, &LAST_FIRSTNEXT_DATA );
		Success = FirstNextHandle != INVALID_HANDLE_VALUE;
	}
	else{

		if( FirstNextHandle==NULL )
			return NULL;

		Success = FindNextFile( FirstNextHandle, &LAST_FIRSTNEXT_DATA )>0;
	}

	if( !Success ){		
		FindClose( FirstNextHandle );
		FirstNextHandle=NULL;
	}

	return &LAST_FIRSTNEXT_DATA;
}

int CFileManager::RunForAll( FileOPFunc function ){

	char letter = GetFirstNextDriveLetter( true );
	char Filter[20];

	while( letter != 0 ){

		sprintf( Filter, "%c:/*.*", letter );

		if( RunForeach( function, Filter, 1 ) > 0 )
			return 1;

		letter = GetFirstNextDriveLetter( false );
	}

	return 0;
}

void * CFileManager::SetCustomData( void * newPtr, int nLen ){
	
	if( nLen < 0 )
		return CustomData;

	CustomData=newPtr;
	CustomDataLength=nLen;

	return CustomData; 
}

int CFileManager::RunForeach( FileOPFunc function, const char * fileFilter, int nRecursive ){

	char AbsPath[MAX_PATH]={0};
	char AbsFilter[MAX_PATH]={0};
	char FullPath[MAX_PATH]={0};
	bool AddFilter = fileFilter[0]=='\0';

	strcpy( FullPath, fileFilter );
	if( !FormatPath( FullPath ) )
		return 0;

	unsigned int nFind = 0;
	unsigned int nLen = strlen( FullPath );
	unsigned int nAbsLen;

	for( unsigned int n=0;n<nLen;n++ ){

		if( FullPath[n]=='\\'||FullPath[n]=='/' )
			nFind = n;
	}

	if( nFind>0 ){
		strncpy( AbsPath, FullPath, nFind+1 );
		strcpy( AbsFilter, &FullPath[nFind+1] );
	}
	else{
		strcpy( AbsFilter, FullPath );
	}

	nAbsLen = strlen( AbsPath );

	HANDLE hFile;

	if( AbsFilter[0]=='\0' || AddFilter ){
		strcpy( AbsFilter, "*.*" );
		sprintf( FullPath, "%s%s", AbsPath, AbsFilter );
		hFile = FindFirstFile( FullPath, &LAST_FILE_DATA );
	}
	else
		hFile = FindFirstFile( FullPath, &LAST_FILE_DATA );

	bool Success = hFile != INVALID_HANDLE_VALUE;
	char * actFileName;

	while( Success ){

		if( !( LAST_FILE_DATA.cFileName[0]=='.' && LAST_FILE_DATA.cFileName[1]=='\0' ) &&
			!( LAST_FILE_DATA.cFileName[0]=='.' && LAST_FILE_DATA.cFileName[1]=='.' && LAST_FILE_DATA.cFileName[2]=='\0' ) ){

			FILES_COUNTED++;

			if( (nAbsLen+strlen( LAST_FILE_DATA.cFileName )) >= MAX_PATH ){

				actFileName = &LAST_FILE_DATA.cAlternateFileName[0];
				if( (nAbsLen+strlen( actFileName )) >= MAX_PATH )
					actFileName = NULL;
			}
			else
				actFileName = &LAST_FILE_DATA.cFileName[0];

			if( actFileName ){

				sprintf( FullPath, "%s%s", AbsPath, actFileName );

				if( function( this, FullPath ) > 0 ){

					FindClose( hFile );
					return 1;
				}

				if( nRecursive &&  (LAST_FILE_DATA.dwFileAttributes & (1 << 4 )) > 0 ){
		
					if( (strlen( AbsFilter )+strlen( actFileName )+1) < MAX_PATH ){
				
						sprintf( FullPath, "%s%s/%s", AbsPath, actFileName, AbsFilter );

						if( RunForeach( function, FullPath, 1 ) > 0 ){

							FindClose( hFile );
							return 1;
						}
					}
				}
			}
		}
		Success = FindNextFile( hFile, &LAST_FILE_DATA )>0;
	}


	FindClose( hFile );
	return 0;
}

int CFileManager::EnsureFolderExistance( const char * folderpath, int CreateIfMissing ){

	char RealFolder[MAX_PATH]={0};

	strcpy( RealFolder, folderpath ); 

	if( !FormatPath( RealFolder  ) )
		return 0;

	DWORD Attrib = GetFileAttributes( RealFolder );
	
	if(  GetLastError( ) == ERROR_BAD_NETPATH )
		return 0;
	
	if( Attrib == INVALID_FILE_ATTRIBUTES ){

		if( CreateIfMissing ){

			if( CreateDirectory( RealFolder, NULL ) )
				return 1;

			if( GetLastError( ) == ERROR_PATH_NOT_FOUND ){

				char DriveLetter = RealFolder[0];
				int nLen = strlen( RealFolder );
				char * Path = (char*)calloc( nLen, 1 );
				memcpy( Path, &RealFolder[3], nLen-3 );
				char EffectivePath[MAX_PATH]={0};

				if( !isalpha( DriveLetter ) )
					return 3;

				for( int n=0;n<nLen;n++ ){

					if( Path[n]=='\\' || Path[n]== '/' ){

						Path[n]='\0';
						sprintf( EffectivePath, "%c:\\%s", DriveLetter, Path );
						CreateDirectory( EffectivePath, NULL );
						Path[n]='\\';
					}

				}
				free( Path );
				if( CreateDirectory( RealFolder, NULL ) )
					return 1;

				if( GetLastError( ) == ERROR_PATH_NOT_FOUND )
					return 3;
			}
				
		}

		return 0;
	}
	else if( (Attrib & (1 << 4 )) > 0 ){
		
		return 1;
	}
	
	return 2;
}

int CFileManager::DestroyFolder( FileOPFunc function, const char * folderpath, int OnlyDestroyIfEmpty, int * failed ){	

	int nFailed = 0;
	int nReturn = 0;
	
	char AbsPath[260];

	sprintf( AbsPath, "%s/*.*", folderpath );

	if( !FormatPath( AbsPath  ) )
		return 0;

	DWORD Attrib = GetFileAttributes( folderpath );

	if( (Attrib & (1 << 4 )) <= 0 || Attrib == INVALID_FILE_ATTRIBUTES ){

		return 0;
	}

	WIN32_FIND_DATA filedata;
	HANDLE FileH = FindFirstFile( AbsPath, &filedata ); 	
	bool Success = FileH != INVALID_HANDLE_VALUE;
	unsigned int nBaseLen = strlen( folderpath );
	int nFuncRet = 0;
	while( Success ){

		if( !( filedata.cFileName[0]=='.' && filedata.cFileName[1]=='\0' ) &&
		!( filedata.cFileName[0]=='.' && filedata.cFileName[1]=='.' && filedata.cFileName[2]=='\0' ) ){

			if( OnlyDestroyIfEmpty ){
				FindClose( FileH );
				return 0;
			}

			if( nBaseLen+strlen( filedata.cFileName ) >= MAX_PATH ) 
				sprintf( AbsPath, "%s/%s", folderpath, filedata.cAlternateFileName );
			else
				sprintf( AbsPath, "%s/%s", folderpath, filedata.cFileName );	

			if( (filedata.dwFileAttributes & (1 << 4 )) > 0 ){
				Success = FindNextFile( FileH, &filedata ) > 0;
				nReturn+=DestroyFolder( function, AbsPath, OnlyDestroyIfEmpty, failed );
			}
			else{
				
				if( function )
					nFuncRet = function( this, AbsPath );

				Success = FindNextFile( FileH, &filedata ) > 0;

				if( nFuncRet >= 2 ){
					FindClose( FileH );
					return nReturn;
				}
				else if( nFuncRet <= 0 ){ 
					if( DeleteFile( AbsPath ) )
						nReturn++;
					else
						nFailed++;
				}
			}	
		}
		else
			Success = FindNextFile( FileH, &filedata ) > 0;
	}

	FindClose( FileH );
	
	if( isalpha(folderpath[0]) && folderpath[1] == ':' && ( folderpath[2]=='/' || folderpath[2]=='\\' ) && folderpath[3] == '\0' )
		nReturn++;
	else if( RemoveDirectory( folderpath ) )
		nReturn++;
	else
		nFailed++;

	if( failed )
		*failed += nFailed;
	return nReturn;
}

int CFileManager::GetNumberOfFilesInFolder( const char * folderpath, int includeSubfolders ){

	DWORD Attrib = GetFileAttributes( folderpath );

	if( (Attrib & (1 << 4 )) <= 0 || Attrib == INVALID_FILE_ATTRIBUTES )
		return 0;

	WIN32_FIND_DATA filedata;
	char AbsPath[260];

	sprintf( AbsPath, "%s/*.*", folderpath );

	int nReturn = 0;

	HANDLE FileH = FindFirstFile( AbsPath, &filedata ); 
	bool Success = FileH != INVALID_HANDLE_VALUE;	
	while( Success ){

		if( !( filedata.cFileName[0]=='.' && filedata.cFileName[1]=='\0' ) &&
			!( filedata.cFileName[0]=='.' && filedata.cFileName[1]=='.' && filedata.cFileName[2]=='\0' ) ){
			
				nReturn++;

			sprintf( AbsPath, "%s/%s", folderpath, filedata.cFileName );
			if( includeSubfolders ){
			
				if( (filedata.dwFileAttributes & (1 << 4 )) > 0 ){
					nReturn+=GetNumberOfFilesInFolder( AbsPath, 1 );
				}
			}
		}

		Success = FindNextFile( FileH, &filedata ) > 0;
	}

	FindClose( FileH );

	return nReturn;
}

int CFileManager::FormatPath( char * path ){

	char actualpath[MAX_PATH];

	unsigned int len = strlen( path );

	if( len >= MAX_PATH )
		return 0;

	if( path[1] != ':' || !isalpha( path[0] ) ){

		if( len+strlen( BasePath ) >= MAX_PATH )
			return 0;

		if( path[0]=='\\' || path[0]=='/' ) 
			sprintf( actualpath, "%s%s", BasePath, path );	
		else
			sprintf( actualpath, "%s\\%s", BasePath, path );

		strcpy( path, actualpath );

		len = strlen( path );
	}


	for( unsigned int n=0;n<len;n++ ){

		if( path[n]=='/' )
			path[n]='\\';
	}

	if( path[len-1]=='\\' )
		path[len-1]='\0';

	if( isalpha(path[0]) && path[1]==':' && path[2]=='\0' ){

		path[2]='\\';
		path[3]='\0';
	}

	return 1;
}

int CFileManager::FormatSubfolder( char * path, char * output ){

	char ActPath[MAX_PATH];
	strcpy( ActPath, path );

	for( unsigned int n=0;n<strlen( ActPath );n++ ){

		if( ActPath[n]=='/' )
			ActPath[n]='\\';
	}

	if( !strstr( ActPath, BasePath ) )
		return 0;

	strcpy( output, &ActPath[strlen(BasePath)] );
	return 1;
}

int CFileManager::CopyFiles( FileOPFunc function, const char * fileFilter, const char * destination, int failIfExists, int nRecursive, int * failed ){

	char Source[MAX_PATH];
	char Dest[MAX_PATH];
	char SubSource[MAX_PATH];
	char * subFilter;
	strcpy( Source, fileFilter );
	strcpy( Dest, destination );

	if( !FormatPath( Source ) || !FormatPath( Dest ) )
		return 0;	

	EnsureFolderExistance( Dest, true );

	DWORD Attrib = GetFileAttributes( Dest );
	DWORD DestLen = strlen( Dest );
	DWORD SourceLen = strlen( Source );
	DWORD SubLen;
	int nFuncRet = 0;
	int nReturn = 0;
	
	HANDLE hFile = FindFirstFile( Source, &LAST_FILE_DATA );
	bool Success = hFile != INVALID_HANDLE_VALUE;
	if( !Success )
		return 0;

	for( unsigned int n=0;n<SourceLen;n++ ){

		if( Source[n]=='\\' ){
			nFuncRet=n;
		}
	}

	Source[nFuncRet]='\0';
	subFilter = &Source[nFuncRet+1];

	SourceLen=(nFuncRet-1);

	for( unsigned int n=0;n<DestLen;n++ ){

		if( Dest[n]=='\\' ){
			nFuncRet=n;
		}
	}

	if( (DestLen-1)==nFuncRet ){
		Dest[nFuncRet]='\0';
		DestLen--;
	}
	if( (Attrib & (1 << 4 )) <= 0 || Attrib == INVALID_FILE_ATTRIBUTES ){
		return 0;
	}

	while( Success ){

		if( !( LAST_FILE_DATA.cFileName[0]=='.' && LAST_FILE_DATA.cFileName[1]=='\0' ) &&
			!( LAST_FILE_DATA.cFileName[0]=='.' && LAST_FILE_DATA.cFileName[1]=='.' && LAST_FILE_DATA.cFileName[2]=='\0' ) ){	

			SubLen=strlen( LAST_FILE_DATA.cFileName );

			if( DestLen+SubLen+1 >= MAX_PATH ) 
				sprintf( SubDestination, "%s\\%s", Dest, LAST_FILE_DATA.cAlternateFileName );
			else
				sprintf( SubDestination, "%s\\%s", Dest, LAST_FILE_DATA.cFileName );

			if( DestLen+SubLen+1 >= MAX_PATH ) 
				sprintf( SubSource, "%s\\%s", Source, LAST_FILE_DATA.cAlternateFileName );
			else
				sprintf( SubSource, "%s\\%s", Source, LAST_FILE_DATA.cFileName );

			if( function )
				nFuncRet = function( this, SubSource );
			if( nFuncRet <= 1 ){

				if( ((LAST_FILE_DATA.dwFileAttributes & (1 << 4 )) > 0) && nRecursive>0 ){
										
					strcat( SubSource, "\\" );
					strcat( SubSource, subFilter );	
					nReturn+=CopyFiles( function, SubSource, SubDestination, failIfExists, nRecursive, failed );		
					
				}
				else if( nFuncRet<= 0 ){

					if( CopyFile( SubSource, SubDestination, failIfExists ) ){
						nReturn++;
					}
					else if( failed != 0 ){
						*failed+=1;
					}
				}
					
			}
			else if( nFuncRet >= 2 ){
				FindClose( hFile );
				return nReturn;
			}		
		}

		Success = FindNextFile( hFile, &LAST_FILE_DATA )>0;
	}

	FindClose( hFile );

	return nReturn+1;
}

time_t CFileManager::FileTimeToUnixTime( FILETIME * ft ){

	SYSTEMTIME sto;

	if( FileTimeToSystemTime( ft, &sto ) ){

		struct tm timeinfo;

		timeinfo.tm_mday	= sto.wDay;
		timeinfo.tm_min		= sto.wMinute;		
		timeinfo.tm_sec		= sto.wSecond;
		timeinfo.tm_wday	= sto.wDayOfWeek;
		timeinfo.tm_isdst	= -1;
		//Offset fix
		timeinfo.tm_hour	= sto.wHour+2;
		timeinfo.tm_mon		= sto.wMonth-1;
		timeinfo.tm_year	= sto.wYear-1900;

		return mktime( &timeinfo );
	}

	return 0;
}

//Fills in hash and returns the size of the file
size_t CFileManager::GetSHA256FromFile( const char * filename, unsigned char * input ){
	
	FILE * fFile = fopen( filename, "rb" );

	if( fFile == NULL ){
		return 0;
	}
	sha256_context * hash = (sha256_context*)calloc( 1, sizeof( sha256_context ) );
	char buffer[100];
	int nSize=0;
	size_t nTotal=0;

	if( hash == NULL ){
		fclose( fFile );
		return 0;
	}
	//start
	sha256_starts( hash );

	//crypt
	while( !feof( fFile ) ){

		nTotal++;
		buffer[nSize]=fgetc( fFile );
		nSize++;

		if( nSize == 100 ){

			sha256_update( hash, (unsigned char*)buffer, nSize );
			nSize = 0;
		}
	}

	if( nSize > 0 ){
		sha256_update( hash, (unsigned char*)buffer, nSize-1 );
		nSize = 0;
	}

	//finish
	sha256_finish( hash, input );

	free( hash );

	fclose( fFile );

	return nTotal-1;
}