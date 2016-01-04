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

#include "CNWNXDynRes.h"
#include "../NWNXdll/IniFile.h"
#include <stdio.h>
#include <math.h>
#include <list>

extern CNWNXDynRes DynRes;

int _stdcall FileScanProc( CFileManager * pThis, const char * file ){

	if( (pThis->GetCurrentFileData()->dwFileAttributes & (1 << 4 )) > 0 ){

		return 0;	
	}
	else{

		unsigned int Break = 0;
		unsigned int len = strlen( file );
		for( unsigned int n=0;n<len;n++ ){

			if( file[n]=='\\' || file[n]=='/' ){
				Break = n;
			}
		}
		if( !Break )
			return 0;

		FileEntry * FE = new FileEntry();
		
		if( !FE ){
			DynRes.Log( "! Failed to allocate linked list entry: %s\n", file ); 
			return 0;
		}

		FE->fullpath = (char*)DynRes.mem.nwnx_malloc( len+1 );
		DynRes.StringCopyToLower( FE->fullpath, file );
		FE->file = (char*)DynRes.mem.nwnx_malloc( (len-Break)+2 );
		DynRes.StringCopyToLower( FE->file, &file[Break+1] );

		DWORD ID=DynRes.CalculateID( FE->file );
		
		if( DynRes.Find( FE->file ) ){
			DynRes.Log( "! Scan: found duplicate file: %s\n", FE->file );	
		}

		if( strstr( FE->file, ".nss" ) || strstr( FE->file, ".txt" )  ){
		
			if( DynRes.LoadList ){
				fprintf( DynRes.LoadList, "IGNORED: %s -> %s\n", FE->fullpath, FE->file );
				fflush( DynRes.LoadList );
			}

			delete FE;

			return 0;
		}
		else if( strstr( FE->file, ".mod" ) || 
				strstr( FE->file, ".erf" ) ||
				strstr( FE->file, ".hak" ) || 
				strstr( FE->file, ".nwm" ) ){

			DynRes.CapsuleList[FE->fullpath] = strlen(FE->fullpath);

			if( DynRes.LoadList ){
				fprintf( DynRes.LoadList, "CAPSULE: %s -> %s\n", FE->fullpath, FE->file );
				fflush( DynRes.LoadList );
			}

			delete FE;

			return 0;
		}

		FE->Version = pThis->FileTimeToUnixTime( &pThis->GetCurrentFileData()->ftLastWriteTime );
		
		bool ok = true;

		try{
			DynRes.FileList[FE] = ID;
		}
		catch (std::bad_alloc){
			ok = false;
		}

		if (ok){
	
			if( DynRes.AddToAreaListCheck( FE->file ) ){
				FE->IsArea = true;
			}

			if(  DynRes.LoadList ){
				fprintf( DynRes.LoadList, "SCAN: %s -> %s\n", FE->fullpath, FE->file );
				fflush( DynRes.LoadList );
			}
		}
	}
	return 0;
}

DWORD CNWNXDynRes::AddCapsuleToList( const char * fullFile ){

	time_t Version = GetLastWriteTime( fullFile );
	if( Version == 0 )
		return 0;

	ERF * capsule = new ERF( fullFile );
	if( !capsule )
		return 0;
	else if( capsule->head.EntryCount <= 0 ){

		delete capsule;
		return 0;
	}

	char ResRef[30];
	const char * ext=NULL;
	ResEntry res={0};
	int n=0;
	KeyEntry * key = capsule->GetKey( n, NULL );
	KeyEntry * ori = key;
	FileEntry * FE = 0;
	DWORD cnt = 0;

	if( !ori ){

		delete capsule;
		return cnt;
	}

	while( key ){
	
		ext = capsule->NwnGetResTypeExtension( key->ResType );

		if( IsInFilter( ext ) || key->ResRef[0]=='\0' ){

			key = capsule->GetKey( ++n, ori );	
			continue;
		}

		res = capsule->GetResEntry( key );

		if( res.OffsetToResource == 0 || res.ResourceSize == 0 ){

			delete ori;
			delete capsule;
			return cnt;
		}

		memset( ResRef, 0, 30 );

		for( int i=0;i<16;i++ ){
			if( key->ResRef[i] == '\0' )
				break;
			ResRef[i]=tolower(key->ResRef[i]);
		}
		strcat( ResRef, ext );

		FileEntry * f = Find( ResRef );

		if( f ){

			if( f->Version == 1 ){

				if( DynRes.LoadList ){

					fprintf( DynRes.LoadList, "CAPSULE ALREADY CACHED: %s -> %s\n", ResRef, fullFile );
					fflush( DynRes.LoadList );
				}
		
			}
			else if( DynRes.LoadList ){

				fprintf( DynRes.LoadList, "CAPSULE DUPLICATE: %s -> %s\n", ResRef, fullFile );
				fflush( DynRes.LoadList );
			}
			key = capsule->GetKey( ++n, ori );	
			continue;
		}

		FE = new FileEntry();

		if( !FE ){

			delete ori;
			delete capsule;
			return cnt;
		}

		FE->ERFOffset = res.OffsetToResource;
		FE->ERFSize = res.ResourceSize;
		FE->Version = Version;
		FE->file = (char*)mem.nwnx_malloc( strlen( ResRef )+1 );
		FE->fullpath = (char*)mem.nwnx_malloc( strlen( fullFile )+1 );

		if( !FE->file || !FE->fullpath ){

			delete FE;
			delete ori;
			delete capsule;
			return cnt;
		}

		strcpy( FE->file, ResRef );
		strcpy( FE->fullpath, fullFile );

		bool ok = true;

		try{
			DynRes.FileList[FE] = CalculateID(FE->file);
		}
		catch (std::bad_alloc){
			ok = false;
		}

		if (ok){

			cnt++;

			if( DynRes.AddToAreaListCheck( FE->file ) ){

				FE->IsArea = true;
			}

			if( DynRes.LoadList ){

				fprintf( DynRes.LoadList, "CAPSULE ADDED: %s -> %s\n", FE->file, fullFile );
				fflush( DynRes.LoadList );
			}
		}
		else{

			mem.nwnx_free( FE->file );
			mem.nwnx_free( FE->fullpath );
			free( FE );
			delete ori;
			delete capsule;
			return cnt;
		}

		key = capsule->GetKey( ++n, ori );			
	}

	delete ori;
	delete capsule;

	return cnt;
}

DWORD CNWNXDynRes::CalculateID( const char * file ){

	DWORD id = 0;
	int n=0;
	while( true ){

		if( file[n] == 0 )
			return id;

		id+=tolower(file[n++]);
	}

	return id;
}

int CNWNXDynRes::ScanFolder( const char * folder ){

	FM.RunForeach( &FileScanProc, folder, 1 );

	size_t n = GetFileListSize();

	DWORD cnt = 0;
	DWORD total = 0;
	for (std::map<char *, size_t>::iterator it = CapsuleList.begin(); it != CapsuleList.end(); ++it){

		if( DynRes.LoadList ){

			fprintf( DynRes.LoadList, "COMPUTING CAPSULE: %s\n", it->first );
			fflush( DynRes.LoadList );
		}
		cnt = AddCapsuleToList(it->first);
		total+=cnt;
		if( DynRes.LoadList ){

			fprintf(DynRes.LoadList, "CAPSULE COMPUTED: %s %u ENTRIES\n", it->first, cnt);
			fflush( DynRes.LoadList );
		}
	}

	Log( "o Scanned (%s): %u files!\n", folder, n );
	Log( "o Scanned (%s): %u encapsuled files!\n", folder, total );

	return n;
}

BOOL CNWNXDynRes::OnRelease( ){

	DumpResFile( );

	delete filter;

	return CNWNXBase::OnRelease();
}

BOOL CNWNXDynRes::OnCreate(const char* LogDir){

	char log[MAX_PATH];
	char path[MAX_PATH];

	sprintf( log, "%s\\nwnx_dynres.txt", LogDir);
	sprintf( path, "%s\\1\\nwnx_dynres.txt", LogDir);

	MoveFile( log, path );

	// call the base class function
	if( !CNWNXBase::OnCreate( log ) )
		return false;

	
	sprintf( log, "%s\\dynres_loadlist.txt", LogDir);
	sprintf( path, "%s\\1\\dynres_loadlist.txt", LogDir);

	MoveFile( log, path );

	memset( LastFile, 0, MAX_PATH );

	totaldemand=0;
	LastExists=NULL;
	ERFCurrent=NULL;
	filter = new char[512];

	FirstNext = FileList.begin();
	AreaIterator = FileList.begin();

	if( !filter )
		return false;

	CIniFile ini( "nwnx.ini" );
	ini.ReadString( "DYNRES", "root", path, 260, "" );
	logz = ini.ReadInteger( "DYNRES", "log", 0 );
	ini.ReadString( "DYNRES", "filter", filter, 512, "" );

	strcat( filter, ".itp.ifo.fac.nss.gff.res.bmp" );

	if( ini.ReadBool( "DYNRES", "LoadList", true ) ){

		LoadList = fopen( log, "w" );
	}
	else{
		LoadList = NULL;
	}

	WriteLogHeader( );

	if( path[0]!=0 ){


		if( path[strlen(path)-1] != '/' && path[strlen(path)-1] != '\\' )
			strcat( path, "/*.*" );
		else
			strcat( path, "*.*" );

		ScanFolder( path );
	}
	else{

		Log( "! No root folder is set!\n" );
	}
	
	idLookup = 0;
	extLookup = 0;

	Hook();

	return true;
}

void CNWNXDynRes::WriteLogHeader( ){

	fprintf( m_fFile, "NWNXDynRes v1.83 created by Terra_777\n\n" );
	fprintf( m_fFile, "Log level: %i\n", logz );
	fprintf( m_fFile, "ResMan: %08X\n", NWN_ResMan );
	fprintf( m_fFile, "Load List: %s\n\n", LoadList ? "ON" : "OFF" );
	fflush( m_fFile );	
}

void CNWNXDynRes::Log( const char * formatting, ... ){

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

unsigned long CNWNXDynRes::OnRequestObject (char *gameObject, char* Request){

	Log( "o OnRequestObject: 0x%08X: %s( )", gameObject, Request );

	return OBJECT_INVALID;
}

char* CNWNXDynRes::OnRequest(char *gameObject, char* Request, char* Parameters){
	
	DWORD len = strlen( Parameters );
	for( DWORD n=0;n<len;n++ ){
		if( Parameters[n]=='/' ){
			Parameters[n]='\\';
		}
	}

	if( strcmp( Request, "NXT" ) == 0 ){
	
		FirstNext++;

		if (FirstNext!=FileList.end())
			sprintf( Parameters, "%s", FirstNext->first->file );
		else
			Parameters[0]='\0';
	}
	else if( strcmp( Request, "ARE" ) == 0 ){

		GetFirstNextArea( Parameters );
	}
	else if( strcmp( Request, "EFN" ) == 0 ){

		GetFirstNextFileInCapsule( Parameters,  false );
	}
	else if( strcmp( Request, "ADD" ) == 0 ){

		sprintf( Parameters, "%i", AddFile( Parameters ) );
	}
	else if( strcmp( Request, "REM" ) == 0 ){
		sprintf( Parameters, "%i", RemoveFile( Parameters ) );		
	}
	else if( strcmp( Request, "GET" ) == 0 ){
		Exists( Parameters );
	}
	else if( strcmp( Request, "FST" ) == 0 ){

		FirstNext = FileList.begin();
		if (FirstNext!=FileList.end())
			sprintf(Parameters, "%s", FirstNext->first->file);
		else
			Parameters[0]='\0';
	}
	else if( strcmp( Request, "EXI" ) == 0 ){
		sprintf( Parameters, "%i", GetFileExists( Parameters ) );
	}
	else if( strcmp( Request, "GIL" ) == 0 ){
		strncpy( Parameters, filter, strlen( Parameters ) );
	}
	else if( strcmp( Request, "FIL" ) == 0 ){
		sprintf( Parameters, "%i", GetFileExists( Parameters ) );
	}
	else if( strcmp( Request, "CAC" ) == 0 ){
		sprintf( Parameters, "%i", CacheFile( Parameters ) );
	}
	else if( strcmp( Request, "LST" ) == 0 ){
		sprintf( Parameters, "%s", LastFile );
	}
	else if( strcmp( Request, "EFF" ) == 0 ){

		GetFirstNextFileInCapsule( Parameters,  true );
	}
	else if( strcmp( Request, "EXT" ) == 0 ){
		ExtractFromCapsule( Parameters );
	}
	else if (strcmp(Request, "RAW") == 0){

		unsigned long len;
		FileEntry * eFile = GetExternalFileToLoad(Parameters);
		if (eFile){

			char * file = (char*)LoadFile(eFile, &len);
			if (!file){
				return NULL;
			}
			else if (file[len - 1] != '\0'){
				char * temp = (char*)mem.nwnx_malloc(len+1);
				memcpy(temp,file,len);
				temp[len] = '\0';
				mem.nwnx_free(file);
				file = temp;
			}

			return file;
		}
	}
	else{
		Log( "! Function %s not found!\n", Request );
	}
	return NULL;
}

/*----------------
--Patch functions
----------------*/

size_t CNWNXDynRes::GetFileListSize(){

	size_t cnt = 0;

	for (std::map<FileEntry*, DWORD>::iterator it = DynRes.FileList.begin(); it != DynRes.FileList.end(); ++it){
		cnt++;
	}

	return cnt;
}

int CNWNXDynRes::CacheFile( const char * file, bool dolog ){


	int nRet = 0;
	FileEntry * eFile = GetExternalFileToLoad( file );

	if (eFile){

		if (eFile->cache){

			mem.nwnx_free(eFile->cache);
			eFile->cachesize = 0;
		}

		eFile->cache = DynRes.LoadFile(eFile, &eFile->cachesize);
		if (eFile->cache)
			nRet = 1;
	}

	if (dolog){

		if (nRet == 0)
			Log("o NWScript unable to cache %s\n", file);
		else
			Log("o NWScript cached: %s from %s\n", eFile->file, eFile->fullpath);
	}

	return nRet;
}

void CNWNXDynRes::SetFilter( char * param ){

	char * temp = new char[strlen(param)+40];
	delete []filter;
	filter = temp;
	strcpy( filter, param );
	strcat( filter, ".itp.ifo.fac.nss.gff.res.bmp" );
}

bool CNWNXDynRes::IsInFilter( const char * ext ){

	if( !ext )
		return true;

	if( strstr( filter, ext ) )
		return true;

	return false;
}

const char * CNWNXDynRes::GetExtensionFromTypeID( unsigned short uType ){

	if( !idLookup || !extLookup ){

		DWORD dIDs = *(DWORD*)(*(DWORD*)((*(DWORD*)0x0066C040)+0x18)+0x4);
		DWORD dExt = *(DWORD*)(*(DWORD*)((*(DWORD*)0x0066C040)+0x18)+0x8);

		idLookup = (unsigned short*)dIDs;
		extLookup = (CExoString*)dExt;
	}
	int n=0;

	while( extLookup[n].len > 0 ){

		if( idLookup[n] == uType )
			return extLookup[n].text;

		n++;
	}

	return NULL;
}

void * CNWNXDynRes::SetResEngienData( CRes * res, void * data, unsigned int size ){

	//Free the old memory if any
	if( res->fileContents )
		mem.nwnx_free( res->fileContents );

	res->fileContents = data;
	res->fileCLen = size;
	res->bHasContents = 1;
	res->fileType |= 0x10;
	res->bIsReady = 1;

	res->resSize2 = size;
	res->pResData2 = data;
	res->bIsReady2 = 1;
	
	//Set the res object
	//SetResObject( this, Resources.ResRef,  Resources.LastType );	

	//call the respective OnServiced events
	int (__fastcall *CRes_OnServiced)(CRes * cRes) = (int (__fastcall *)(CRes* cRes))(DWORD*)(*(DWORD*)(res->CRes_vftable+0x10));
	res->fileType = res->fileType & 0xFFFFFFFB | (CRes_OnServiced( res ) != 0 ? 4 : 0);
	res->fileType &= 0xFFFFFFEF;

	return res->fileContents;
}

FileEntry * CNWNXDynRes::GetExternalFileToLoad( const char * file ){
	
	return Find(file);
}

long CNWNXDynRes::GetFileSize(const char * fullFile){
	FILE *p_file = NULL;
	p_file = fopen(fullFile, "rb");
	if (!p_file)
		return 0;
	fseek(p_file, 0, SEEK_END);
	long size = ftell(p_file);
	fclose(p_file);
	return size;
}

DWORD CNWNXDynRes::RecomputeCapsule( const char * fullFile ){

	std::list<char*> cachelist;
	char * test;

	for (std::map<FileEntry*, DWORD>::iterator it = DynRes.FileList.begin(); it != DynRes.FileList.end(); ++it){

		if (it->first->Version != 1 && strcmp(it->first->fullpath, fullFile) == 0 ){
			
			if (it->first->cache){
				test = new char[strlen(it->first->file)+1];
				strcpy(test, it->first->file);
				cachelist.push_back(test);
			}

			if (it->first)
				delete it->first;

			it = FileList.erase(it);		
		}
	}

	//wait until system done writing
	time_t current = GetFileSize(fullFile);
	time_t now;
	Sleep(100);
	now = GetFileSize(fullFile);

	while (current != now){
		current = now;
		Sleep(100);
		now = GetFileSize(fullFile);
	}

	DWORD cnt = AddCapsuleToList(fullFile);
	std::list<char*>::iterator it;
	

	if (cnt>0){

		for (it = cachelist.begin(); it != cachelist.end(); ++it){
			test = *it;
			CacheFile(test,false);
			delete[]test;
			*it = NULL;
		}
	}
	else{

		for (it = cachelist.begin(); it != cachelist.end(); ++it){
			test = *it;
			delete[]test;
			*it = NULL;
		}
	}

	cachelist.clear();

	return cnt;
}

void * CNWNXDynRes::LoadFile( FileEntry * file, unsigned long * uLen ){

	if (!file || !file->fullpath)
		return NULL;

	if( file->Version == 1 ){

		void * data = mem.nwnx_malloc( file->ERFSize );

		if( !data )
			return NULL;

		memcpy( data, (void*)file->ERFOffset, file->ERFSize );
		*uLen=file->ERFSize;
		return data;
	}

	//Open the file
	FILE * pFile = fopen( file->fullpath, "rb" );
	*uLen=0;

	//Failed to open
	if( pFile == NULL ){
		return NULL;
	}

	unsigned long nSize;

	if( file->ERFSize > 0 ){

		if( GetLastWriteTime( file->fullpath ) != file->Version ){

			fclose( pFile );

			if( DynRes.LoadList ){

				fprintf( DynRes.LoadList, "CAPSULE MISSMATCH: %s -> %s\n", file->file, file->fullpath );
				fflush( DynRes.LoadList );
			}

			char * temp = (char*)mem.nwnx_malloc( strlen( file->fullpath )+1 );
			strcpy( temp, file->fullpath );
			DWORD test = RecomputeCapsule( temp );

			if( DynRes.LoadList ){

				fprintf( DynRes.LoadList, "CAPSULE RECOMPUTED: %s %u ENTRIES\n", temp, test );
				fflush( DynRes.LoadList );
			}

			file = DynRes.GetExternalFileToLoad( DynRes.LastFile );

			mem.nwnx_free( temp );

			return LoadFile( file, uLen );
		}

		fseek( pFile, file->ERFOffset, SEEK_SET );
		nSize = file->ERFSize;
	}
	else{

		//Check length
		fseek( pFile, 0, SEEK_END );
		nSize = ftell( pFile );
		rewind( pFile );
	}

	//Close if the file is empty
	if( nSize <= 0 ){
		
		fclose( pFile );
		return NULL;
	}

	//New memory and fill it with the file
	void * vNew = mem.nwnx_malloc( nSize );

	//Bail
	if( vNew == NULL ){

		fclose( pFile );
		return NULL;
	}

	if( fread( vNew, 1, nSize, pFile ) < nSize ){

		mem.nwnx_free( vNew );
		fclose( pFile );
		return NULL;
	}

	//Close
	fclose( pFile );

	*uLen = nSize;
	return vNew;
}

bool CNWNXDynRes::AddFile( char* Parameters ){

	unsigned int len = strlen( Parameters );
	char * fullfile = NULL;
	for( unsigned int n=0;n<len;n++ ){

		if( Parameters[n]=='|' ){

			Parameters[n]='\0';
			fullfile=&Parameters[n+1];
			break;
		}
	}

	if( !fullfile )
		return false;

	char * node= NULL;

	if( strstr( Parameters, ".mod" ) || 
		strstr( Parameters, ".erf" ) ||
		strstr( Parameters, ".hak" ) || 
		strstr( Parameters, ".nwm" ) ){

		for (std::map<char *, size_t>::iterator it = CapsuleList.begin(); it != CapsuleList.end(); ++it){

			if( strcmp( fullfile, it->first )==0 ){
				node = it->first;
				break;
			}
		}

		if( node ){

			DeleteCapsule( node );
		}
		else{

			DWORD len = strlen( fullfile );
			char * st = (char*)mem.nwnx_malloc( len+1 );
			strcpy( st, fullfile );

			CapsuleList[st] = len;
		}

		if( DynRes.LoadList ){

			fprintf( DynRes.LoadList, "NWSCRIPT: %s -> %s\n", fullfile, Parameters );
			fflush( DynRes.LoadList );
		}

		Log( "o NWScript Add: %s (%u): %s\n", Parameters, CalculateID( Parameters ), fullfile );

		return AddCapsuleToList( fullfile ) > 0;
	}
	
	FileEntry * fnode = Find( Parameters );

	if (fnode){
		delete fnode;
		FileList.erase(fnode);
	}

	FileEntry * FE = new FileEntry();
		
	if( !FE )
		return false;

	//From file
	if( strstr( fullfile, ".mod" ) || 
		strstr( fullfile, ".erf" ) ||
		strstr( fullfile, ".hak" ) || 
		strstr( fullfile, ".nwm" ) ){

		time_t vVer = GetLastWriteTime( fullfile );
		
		if( vVer <= 0 ){
			free( FE );
			Log( "o NWScript Add: File not found %s\n", fullfile );
			return false;
		}

		char * temp = strstr( Parameters, "." );
		if( !temp ){
			free( FE );
			return false;
		}
		
		DWORD n=0;
		while( temp[++n]!='\0' ){
			temp[n]=tolower( temp[n] );
		}

		ERF * eFile = new ERF( fullfile ); 
		
		if( !eFile ){
			free( FE );
			return false;
		}
		else if( eFile->head.EntryCount <= 0 ){
			
			delete eFile;
			free( FE );
			return false;
		}

		n=0;
		KeyEntry * node = eFile->GetKey(n,NULL);
		KeyEntry * ori = node;
		
		if( !ori ){
			delete eFile;
			free( FE );
			return false;
		}		

		*temp='\0';

		while( node ){

			if( strcmp( GetExtensionFromTypeID( node->ResType ), (temp+1) ) == 0 && strncmp( Parameters, node->ResRef, 16 )==0  ){
		
				break;				
			}
			node = eFile->GetKey(++n,ori);
		}

		*temp='.';

		if( node ){

			ResEntry RE = eFile->GetResEntry( node );
			FE->ERFOffset = RE.OffsetToResource;
			FE->ERFSize = RE.ResourceSize;
			FE->Version = vVer;
			delete ori;
			delete eFile;
			
		}
		else{

			Log( "o NWScript Add: %s not found within %s\n", Parameters, fullfile );
			delete ori;
			delete eFile;
			free( FE );		
			return false;
		}
		
	}
	//End

	FE->fullpath = (char*)DynRes.mem.nwnx_malloc( strlen( fullfile )+1 );
	StringCopyToLower( FE->fullpath, fullfile );

	FE->file = (char*)DynRes.mem.nwnx_malloc( strlen( Parameters )+1 );
	StringCopyToLower( FE->file, Parameters );
		
	DWORD ID=DynRes.CalculateID( FE->file );

	bool ok = true;
	try{
		FileList[FE] = ID;
	}
	catch (std::bad_alloc){
		ok = false;
	}

	if (ok){

		if( AddToAreaListCheck( FE->file ) ){
			FE->IsArea = true;
		}

		if( DynRes.LoadList ){
			fprintf( DynRes.LoadList, "NWSCRIPT: %s -> %s\n", FE->fullpath, FE->file );
			fflush( DynRes.LoadList );
		}
	}

	Log( "o NWScript Add: %s (%u): %s\n", FE->file, ID, FE->fullpath );

	return true;
}

bool CNWNXDynRes::AddToAreaListCheck( const char * file ){

	if( !strstr( file, ".are" ) ){
		return false;
	}

	return true;
}

FileEntry * CNWNXDynRes::Find( const char * file ){
	
	DWORD ID = CalculateID( file );
	
	for (std::map<FileEntry*, DWORD>::iterator it = FileList.begin(); it != FileList.end(); ++it){
	
		if (it->second == ID && strcmp(it->first->file, file) == 0)
			return it->first;
	}

	return NULL;
}

bool CNWNXDynRes::Exists( char* Parameters ){

	FileEntry * node = Find( Parameters );
	
	if( node ){

		Log("o NWScript Exists: %s: %s\n", node->file, node->fullpath);
		strcpy( Parameters, node->fullpath );
		return true;
	}
	return false;
}

DWORD CNWNXDynRes::DeleteCapsule( const char * fullFile ){

	DWORD deleted=0;

	for (std::map<FileEntry*, DWORD>::iterator it = FileList.begin(); it != FileList.end(); ++it){

		if( strcmp( it->first->fullpath, fullFile ) == 0 ){

			delete it->first;
			it = FileList.erase(it);
			deleted++;
		}
	}

	return deleted;
}

bool CNWNXDynRes::RemoveFile( char* Parameters ){

	if( strstr( Parameters, ".mod" ) || 
		strstr( Parameters, ".erf" ) ||
		strstr( Parameters, ".hak" ) || 
		strstr( Parameters, ".nwm" ) ){

		char * test = (char*)mem.nwnx_malloc( strlen( Parameters )+2 );
		if( !test )
			return false;
		sprintf( test, "%c%s", '\\',Parameters );

		char * node = NULL;

		for (std::map<char *, size_t>::iterator it = CapsuleList.begin(); it != CapsuleList.end(); ++it){

			if( strstr( it->first, test ) ){
				node = it->first;
				break;
			}
		}

		mem.nwnx_free( test );

		if( !node )
			return false;

		DWORD cnt = DeleteCapsule(test);
		
		Log("o NWScript Remove Capsule: %s: %u\n", test, cnt);

		CapsuleList.erase(test);

		return cnt > 0; 
	}

	FileEntry * node = Find( Parameters );
	if( node ){

		Log( "o NWScript Remove: %s: %s\n", node->file, node->fullpath );
		delete node;
		FileList.erase(node);
		return true;
	}

	return false;
}

void CNWNXDynRes::StringCopyToLower( char * dest, const char * src, int nCnt ){

	int n=0;
	while( true ){

		if( src[n]=='\0' ){
			dest[n]='\0';
			break;
		}
		else if( n==nCnt && n>0 )
			break;

		dest[n]=tolower(src[n]);
		n++;
	}
}

void CNWNXDynRes::DumpResFile( ){

	if( !LoadList ){
		return;
	}

	fprintf( LoadList, "\n\nTimes loaded:\n\n" );
	if (GetFileListSize()<=0){
		fprintf( LoadList, "File loadlist doesnt exist\n" );
		fflush( LoadList );
		return;
	}

	unsigned long long int Total = 0;
	unsigned int ResCnt = 0;
	unsigned int ResCntUsed = 0;
	unsigned int Missing = 0;
	unsigned int NeverUsed = 0;

	for (std::map<FileEntry*, DWORD>::iterator it = DynRes.FileList.begin(); it != DynRes.FileList.end(); ++it){

		FileEntry * FE = it->first;

		if( FE->LoadTimes == 0xFFFFFFFF ){
			fprintf( LoadList, "%s: MISSING\n", FE->file );
			Missing++;
		}
		else{
			fprintf( LoadList, "%s: %u\n", FE->file, FE->LoadTimes );			
			Total+=FE->LoadTimes;		
			if( FE->LoadTimes == 0 ){
				NeverUsed++;
			}
			else
				ResCntUsed++;
		}
		ResCnt++;

		fflush( LoadList );
	}
	
	double dtotal,dpercentage;

	fprintf( LoadList, "\nResource List Entries: %u\n", ResCnt );
	fprintf( LoadList, "Unique Resources Used: %u\n", ResCntUsed );
	fprintf( LoadList, "Missing Files: %u\n", Missing );
	fprintf( LoadList, "Never Used: %u\n", NeverUsed );

	dtotal = (double)(Total+totaldemand);
	dpercentage = (double)Total;
	fprintf( LoadList, "External Loads: %llu (%u%%)\n", Total, (unsigned int)floor(((dpercentage/dtotal)*100)+0.5) );

	dpercentage = (double)totaldemand;
	fprintf( LoadList, "Interal Loads: %llu (%u%%)\n", totaldemand, (unsigned int)floor(((dpercentage/dtotal)*100)+0.5) );
	fflush( LoadList );
	fclose( LoadList );
}

int CNWNXDynRes::GetFileExists( const char * file  ){

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile( file, &FindFileData );
	if( hFind != INVALID_HANDLE_VALUE && ( FindFileData.nFileSizeLow > 0 || FindFileData.nFileSizeHigh > 0 ) ){
			
		FindClose( hFind) ;
		return 1;
	}

	return 0;
}

time_t CNWNXDynRes::GetLastWriteTime( const char * file ){

	HANDLE hFile = CreateFile( file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
    FILETIME ftCreate, ftAccess, ftWrite;
	if(hFile == INVALID_HANDLE_VALUE)
		return 0;

    // Retrieve the file times for the file.
    if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)){

		CloseHandle(hFile); 
		return 0;
	}
	
	CloseHandle(hFile);

	return FM.FileTimeToUnixTime( &ftWrite );	
}

void CNWNXDynRes::GetFirstNextArea( char * param ){

	FileEntry * node = NULL;

	if (atoi(param) > 0){
		AreaIterator = FileList.begin();
	}
	else
		AreaIterator++;
	
	while (AreaIterator!=FileList.end()){

		if (AreaIterator->first->IsArea){
			node = AreaIterator->first;
			break;
		}
		else{
			AreaIterator++;
		}
	}

	if( node ){

		strcpy( param, node->file );
		char * dot = strstr( param, "." );
		if( dot )
			*dot='\0';
	}
	else{

		param[0]=0;
	}
}

char * CNWNXDynRes::GetFileNameFromKey( KeyEntry * key, char * buffer ){

	memset( buffer, 0, 30 );

		for( int n=0;n<16;n++ ){

			if( key->ResRef[n] == '\0' ){
				buffer[n]='.';
				break;
			}
			else{
				buffer[n]=(char)tolower(key->ResRef[n]);
				buffer[n+1]='.';
			}
		}

	strcat( buffer, GetExtensionFromTypeID( key->ResType ) );
	return buffer;
}

void CNWNXDynRes::GetFirstNextFileInCapsule( char * param, bool first ){

	if( first ){

		if( ERFCurrent )
			delete ERFCurrent;

		ERFCurrent = new ERF( param );
		if( !ERFCurrent ){
			param[0]=0;
			return;
		}
		else if( ERFCurrent->head.EntryCount <= 0 ){
			param[0]=0;
			return;
		}
		FirstNextErf=0;
	}
	else if( !ERFCurrent || ERFCurrent->head.EntryCount <= 0 ){
		param[0]=0;
		return;
	}
	else if( ++FirstNextErf >= ERFCurrent->head.EntryCount ){
		param[0]=0;
		delete ERFCurrent;
		ERFCurrent=NULL;
		return;
	}

	memset( param, 0, 30 );

	KeyEntry * keynode = ERFCurrent->GetKey( FirstNextErf, NULL );
	if( !keynode ){
		return;
	}

	GetFileNameFromKey( keynode, param );

	delete keynode;
}

void CNWNXDynRes::ExtractFromCapsule( char * param ){

	char * fullfile;
	char * newName;
	char * filen = param;

	fullfile = strstr( param, "|" );
	if( !fullfile ){
		param[0]=0;
		return;
	}
	else{
		*fullfile='\0';
		fullfile++;
	}

	newName = strstr( fullfile, "|" );
	if( !newName ){
		param[0]=0;
		return;
	}
	else{
		*newName='\0';
		newName++;
	}


	ERF * erfNode = new ERF( fullfile );
	if( !erfNode ){
		Log( "! NWScript ExtractFromCapsule: unable to open: %s\n", fullfile );
		param[0]=0;
		return;
	}
	else if( erfNode->head.EntryCount <= 0 ){
		Log( "! NWScript ExtractFromCapsule: unable to open: %s\n", fullfile );
		delete erfNode;
		param[0]=0;
		return;
	}

	int ok=0;
	char full[MAX_PATH];
	DWORD n=0;
	KeyEntry * ori = erfNode->GetKey( n, NULL );
	KeyEntry * keynode = ori;
	while( keynode ){

		if( strcmp( GetFileNameFromKey( keynode, full ), filen ) == 0 ){
			break;
		}

		keynode = erfNode->GetKey( ++n, ori );
	}

	if( keynode ){

		char * dot;
		char * temp = strstr( newName, "." );
		while( temp ){
			dot = temp;
			temp = strstr( temp+1, "." );
		}

		if( dot ){
			*(dot+1)='\0';
		}
		else{
			strcat( newName, "." );
		}
		strcat( newName, GetExtensionFromTypeID( keynode->ResType ) );

		FILE * newFile = fopen( newName, "wb" );
		if( newFile ){

			bool Broken = false;
			void * data = erfNode->GetResource( keynode );
			ResEntry res = erfNode->GetResEntry( keynode );
			if( data ){

				if( fwrite( data, 1, res.ResourceSize, newFile ) != res.ResourceSize ){

					Broken=true;
				}
				else{
					Log( "o NWScript ExtractFromCapsule: Extracted %s to %s from %s\n", filen, fullfile, newName );
					ok=1;
				}
				delete []data;				
			}
			else
				Log( "! NWScript ExtractFromCapsule: no resource: %s\n", filen );

			fclose( newFile );

			if( Broken ){
				remove( newName );
				Log( "! NWScript ExtractFromCapsule: unable to write complete file: %s\n", newName ); 
			}
		}
		else{

			Log( "! NWScript ExtractFromCapsule: unable to create file: %s\n", newName );
		}

	}
	else{
		Log( "! NWScript ExtractFromCapsule: resource not found: %s\n", filen );
	}

	sprintf( param, "%i", ok );
	delete ori;
	delete erfNode;
}