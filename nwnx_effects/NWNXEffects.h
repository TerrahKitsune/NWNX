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
#include "nwnx_memory.h"

#define OBJECT_INVALID 0x7F000000

struct EffectScript{

	DWORD effectID;
	char script[18]; 
};

class CNWNXEffects : public CNWNXBase  
{
public:

	~CNWNXEffects();
	BOOL OnCreate(const char* LogDir);
	char* OnRequest(char *gameObject, char* Request, char* Parameters);
	unsigned long OnRequestObject (char *gameObject, char* Request);
	BOOL OnRelease();
	void Log( const char * formatting, ... );
	void WriteLogHeader( );
	UINT64 GetEffectID( CGameEffect * effect );
	void NormalizeLink( CGameEffect * effect );
	CGameEffect * CreateNewEffect( );
	int CalculateUnconditionalAC( CNWSCreature * cre );
	int CalculateUnconditionalAB( CNWSCreature * cre );

	DWORD last;
	CNWNXMemory mem;

	int bLog;

	DWORD Order;
	void * data;
	int param;

	bool ACHook;
	bool ABHook;

	EffectScript * ESArray;
	DWORD ESArrayLen;

	void AddScript( const char * script );
	void PrimeScript( CGameEffect * effect );
	void ExecuteScript( CGameEffect * effect, nwn_objid_t object_Self );

protected:

private:

};

#endif