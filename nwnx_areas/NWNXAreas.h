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

#if !defined(NWNXMEFFECTS_H_)
#define NWNXMEFFECTS_H_
#define _CRT_SECURE_NO_WARNINGS

#include "../NWSERVER/types.h"
#include "../NWNXdll/NWNXBase.h"

class CNWNXAreas : public CNWNXBase  
{
public:

	~CNWNXAreas();
	BOOL OnCreate(const char* LogDir);
	char* OnRequest(char *gameObject, char* Request, char* Parameters);
	unsigned long OnRequestObject (char *gameObject, char* Request);
	BOOL OnRelease();
	void Log( const char * formatting, ... );
	void WriteLogHeader( );
	nwn_objid_t CreateArea( const char * resref );
	int DestroyArea( nwn_objid_t nAreaID );
	void RenameArea( CNWSArea * are, char * str );
	long double GetHeight( CNWSArea * are, char * str );

	void SendNameUpdate(DWORD SendToID, DWORD ObjectToUpdateID);
	void UpdateObjectForAllPlayers(nwn_objid_t obj);
	void ResendClientArea(nwn_objid_t obj);

	void AddAreaToCreatures(CNWSModule *pModule, DWORD nAreaID);
	void AddAreaToCreature(CNWSModule *pModule, CNWSCreature *pObject, DWORD nAreaID);
	void RemoveAreaForCreatures(CNWSModule *pModule, DWORD nAreaID, CNWSArea * are);
	void RemoveAreaFromCreature(CNWSCreature *pObject, DWORD nAreaID);
	void UpdateAreasForDMs();
	bool FixCreature(CNWSModule* mod, CNWSCreature* pObject, bool log=true);
	bool FixAllCreatures(CNWSModule* mod);
	bool HasPlayers(CNWSArea * are);
	bool CheckHasPlayersInTrans();

	void GetSafeLocation(CScriptLocation * loc);

	void DestoryObject(nwn_objid_t id);

	nwn_objid_t Last;
	CNWNXMemory mem;
	DWORD test;

	bool backlog;
	bool HasUpdated;
	bool IsK;

protected:

private:

};

#endif