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
#include "BinaryTree.h"
#include "FileManager.h"
#include "LinkedList.h"
#include "ERF.h"

#define OBJECT_INVALID 0x7F000000
void Hook();

struct FileEntry{

	char * file;
	char * fullpath;
	DWORD LoadTimes;
	time_t Version;
	DWORD ERFOffset;
	DWORD ERFSize;
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
	BinaryTree::BTEntry * Find( const char * file );
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
	int CacheFile( const char * file );
	void GetFirstNextFileInCapsule( char * param, bool first );
	void ExtractFromCapsule( char * param );
	char * GetFileNameFromKey( KeyEntry * key, char * buffer );


	FILE * LoadList;
	int logz;
	CNWNXMemory mem;
	CFileManager FM;

	BinaryTree * FileLL;
	LinkedList * ERFList;
	LinkedList * AreaList;

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