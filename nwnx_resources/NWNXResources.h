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

#ifndef NWNXRESRES_H_
#define NWNXRESRES_H_

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include "../NWNXdll/NWNXBase.h"
#include "../NWNXdll/IniFile.h"
#include "ResHooks.h"
#include <string.h>

typedef struct CExoStr{

	char * cStr;
	unsigned int nLen;
}CExoStr;

typedef struct NWRes{

	char * cResRef;
	unsigned char Behavior;
	unsigned int Data;
	void * vPtr;

}NWRes;

class CNWNXResources : public CNWNXBase  
{
public:

	~CNWNXResources(){OnRelease();}

	//On create is called when the module is loaded
	BOOL OnCreate(const char* LogDir);
	//This is called when SetLocalString is called as per a request
	//IE: SetLocalInt( GetModule(), "NWNX!RESOURCES!Request", "Parameters" );
	//Parameters can be fetched with GetLocalInt( GetModule(), "NWNX!RESOURCES!Request" );
	char* OnRequest(char *gameObject, char* Request, char* Parameters);
	//This is is proced when getlocal object is called
	//IE: GetLocalObject( GetModule(), "NWNX!RESOURCES!Request" );
	//Return value should be a object ID
	unsigned long OnRequestObject (char *gameObject, char* Request);
	//Is called before the module is unloaded
	BOOL OnRelease();

	//Used to extract data out of the raw buffer
	void * ExtractData( int * Behavior, char * ResRef, int * DataLen, unsigned char * Raw );

	const char * GetPathFromType( unsigned short uType );
	void SetPathRepository( CIniFile * ini );
	bool GetTypeAllowed( unsigned short uType );

	//Logs to nwnx_resources.txt in logs.
	void Log( int nDebugLevel, const char *pcMsg, ... );

	//Gets the extention char from uType
	//pointer returned should not be deallocated as the string belongs to nwserver
	char * GetExtensionFromTypeID( unsigned short uType );

	//Formats and returns the full path based on the ini settings
	char * GetPath( );

	//nwns malloc routine
	void * (__cdecl *nwnx_malloc)(size_t cb);
	//nwns free routine
	void (__cdecl *nwnx_free)(void * cb);

	//Log prio
	int priority;
	
	//File-type lookup
	unsigned short * idLookup;
	CExoStr * extLookup;
	CExoStr * extSubFldr;
	int ExtLen;
	NWRes * ResTable;
	int ResTLen;

	void ReallocResTable( int nNewSize );
	NWRes * AddToResTable( const char * ResRef, unsigned char Behavior );
	NWRes * GetBehaviorForRes( const char * ResRef );

	char ResRef[20];
	char ResFolder[260];
	char LastFile[260];
	unsigned short LastType;

	bool ignoreMode;
	bool useExtFolder;
	bool BaseRes;
	int Behavior;

private:
	unsigned int ftLoop;
	
};
#endif