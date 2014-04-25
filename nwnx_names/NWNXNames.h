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

#if !defined(NWNXNAMES_H_)
#define NWNXNAMES_H_
#pragma once
#include "../NWNXdll/NWNXBase.h"
#include "../NWSERVER/types.h"
#include "nwnx_memory.h"
#include "CustomNames.h"

void ApplyHooks( );

struct ExoStr{

	char * text;
	DWORD len;		
};

class CNWNXNames : public CNWNXBase  
{
public:

	~CNWNXNames(){OnRelease();}

	BOOL OnCreate(const char* LogDir);
	char* OnRequest(char *gameObject, char* Request, char* Parameters);
	unsigned long OnRequestObject (char *gameObject, char* Request);
	void WriteLogHeader();
	BOOL OnRelease();
	void Log( const char * formatting, ... );
	bool OverwriteFunction( DWORD pCode, PVOID pCallbackFunc, PVOID *pNextHook );
	void SendNameUpdate( DWORD SendToID, DWORD ObjectToUpdateID );
	void AddName( DWORD objID, char * input );
	void GetName( DWORD objID, char * input );
	void RemoveName( DWORD objID, char * input );
	void SetTemporaryName( CNWSCreature * cre, nwn_objid_t acmIndex, bool restore );
	void SetPermanentName( CNWSCreature * cre, char * input );
	void GetPermanentName( CNWSCreature * cre, char * input );
	void UpdateObjectForAllPlayers( CNWSObject * obj );
	void GetSetFamilarCompanionName( char * input, int nGet );
	int SendMessageToPlayer( CNWSObject * ply, char * text );

	CNWNXMemory mem;
	DWORD LastID;

	DynamicNames * Array;
	int Len;
	int Alloc;

	DynamicNames * Resize( unsigned int newSize );
	DynamicNames * Add( DWORD ObjID );
	DynamicNames * Get( DWORD ObjID );
	bool Remove( DWORD ObjID );

	char * TempFirstName;
	char * TempLastName;
	int TempFirstNameLen;
	int TempLastNameLen;

protected:

private:

};

#endif