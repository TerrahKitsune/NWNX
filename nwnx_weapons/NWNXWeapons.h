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

#if !defined(NWNXWEAPONS_H_)
#define NWNXWEAPONS_H_
#pragma once
#include "../NWNXdll/NWNXBase.h"
#include "WeaponBehavior.h"
#include "./NWSERVER/types.h"

void ApplyHooks( );

struct Ownership{

	nwn_objid_t item;
	nwn_objid_t owner;
	byte bound;
};

class CNWNXWeapons : public CNWNXBase  
{
public:

	~CNWNXWeapons(){OnRelease();}

	BOOL OnCreate(const char* LogDir);
	char* OnRequest(char *gameObject, char* Request, char* Parameters);
	unsigned long OnRequestObject (char *gameObject, char* Request);
	void WriteLogHeader();
	BOOL OnRelease();
	void Log( const char * formatting, ... );
	void RemoveCopyItemAndModifyColorCap( void );
	bool OverwriteFunction( DWORD pCode, PVOID pCallbackFunc, PVOID *pNextHook );
	WORD GetWeaponData( DWORD uiWeapon, int nField );
	void SetWeaponData( DWORD uiWeapon, int nField, WORD nData );
	void CreateProficienciesList( char * out, DWORD baseItem );
	void ResetArguments( void );
	DWORD FindAddress( DWORD pNWSERVER );
	DWORD * GetDefaultBaseItemTypeInfo( DWORD baseItem, DWORD offset );

	WeaponBehavior * WB;
	BYTE DevCritBehavior;
	BYTE ScriptRunning;
	DWORD OBJ_IDS[5];
	char * DevCritScript;
	bool ISNWSERVER;

	DWORD ownLen;
	Ownership * own;
	nwn_objid_t LastRqstID;

	void SetOwnership( nwn_objid_t item, nwn_objid_t pc, byte bound );
	Ownership * GetOwnership( nwn_objid_t item );
	nwn_objid_t RemoveOwnership( nwn_objid_t item );

protected:

private:

};

#endif