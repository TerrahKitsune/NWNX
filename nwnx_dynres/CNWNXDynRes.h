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

#if !defined(NWNXDYNRES_H_)
#define NWNXDYNRES_H_
#define _CRT_SECURE_NO_WARNINGS

#include "../NWSERVER/types.h"
#include "../NWNXdll/NWNXBase.h"
#include "FileManager.h"
#include "ERF.h"
#include <map>

#define OBJECT_INVALID 0x7F000000
void Hook();

class FileEntry{

public:
	
	FileEntry(){
		memset(this, 0, sizeof(FileEntry));
		IsArea = false;
	}

	~FileEntry(){
		if (file)
			nwnxfree(file);
		if (fullpath)
			nwnxfree(fullpath);

		memset(this, 0, sizeof(FileEntry));
	}

	void nwnxfree(void * mem){

		void(__cdecl *nwnx_free)(void * ptr);
		nwnx_free = (void(__cdecl *)(void *))0x0040D560;

		nwnx_free(mem);
		if (cache)
			nwnx_free(cache);
	}

	char * file;
	char * fullpath;
	DWORD LoadTimes;
	time_t Version;
	DWORD ERFOffset;
	DWORD ERFSize;
	bool IsArea;
	void * cache;
	DWORD cachesize;
};

class CNWNXDynRes : public CNWNXBase  
{
public:

	~CNWNXDynRes(){OnRelease();}
	BOOL OnCreate(const char* LogDir);
	char* OnRequest(char *gameObject, char* Request, char* Parameters);
	unsigned long OnRequestObject (char *gameObject, char* Request);
	BOOL OnRelease();
	void Log( const char * formatting, ... );
	void WriteLogHeader( );
	const char * GetExtensionFromTypeID( unsigned short uType );
	void * SetResEngienData( CRes * res, void * data, unsigned int size  );
	int ScanFolder( const char * folder );
	DWORD CalculateID( const char * file );
	FileEntry * GetExternalFileToLoad( const char * file );
	void * LoadFile( FileEntry * file, unsigned long * uLen );
	bool AddFile( char* Parameters );
	bool RemoveFile( char* Parameters );
	bool Exists( char* Parameters );
	void StringCopyToLower( char * dest, const char * src, int nCnt=0 );
	FileEntry * Find(const char * file);
	void DumpResFile( );
	int GetFileExists( const char * file );
	time_t GetLastWriteTime( const char * file );
	DWORD AddCapsuleToList( const char * fullFile );
	DWORD RecomputeCapsule( const char * fullFile );
	DWORD DeleteCapsule( const char * fullFile );
	bool AddToAreaListCheck( const char * file );
	void GetFirstNextArea( char * param );
	void SetFilter( char * param );
	bool IsInFilter( const char * ext );
	int CacheFile(const char * file, bool dolog = true);
	void GetFirstNextFileInCapsule( char * param, bool first );
	void ExtractFromCapsule( char * param );
	char * GetFileNameFromKey( KeyEntry * key, char * buffer );
	long GetFileSize(const char * fullFile);
	size_t GetFileListSize();

	FILE * LoadList;
	int logz;
	CNWNXMemory mem;
	CFileManager FM;

	std::map<FileEntry*,DWORD> FileList;
	std::map<char *,size_t> CapsuleList;

	std::map<FileEntry*, DWORD>::iterator FirstNext;
	std::map<FileEntry*, DWORD>::iterator AreaIterator;

	void * LastExists;
	char * filter;

	unsigned short * idLookup;
	CExoString * extLookup;
	ERF * ERFCurrent;
	DWORD FirstNextErf;

	unsigned long long int totaldemand;

	bool HasFile;
	char LastFile[MAX_PATH];

protected:

private:

};

#endif