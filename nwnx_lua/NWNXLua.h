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

#if !defined(NWNXLUA_H_)
#define NWNXLUA_H_
#define _CRT_SECURE_NO_WARNINGS

#include "../NWSERVER/types.h"
#include "../NWNXdll/NWNXBase.h"

#define lua_c

extern "C"{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "luaconf.h"
}
#pragma comment(lib, "lua52.lib")

#define OBJECT_INVALID 0x7F000000

struct EventParams{

	int EVENT;
	int BYPASS;
	nwn_objid_t oTarget[2];
	Vector lTarget[2];
	int nData[5];
	CExoString sData[2];
};

class CNWNXLua : public CNWNXBase  
{
public:

	~CNWNXLua();
	
	BOOL OnCreate(const char* LogDir);
	char* OnRequest(char *gameObject, char* Request, char* Parameters);
	unsigned long OnRequestObject (char *gameObject, char* Request);
	BOOL OnRelease();
	void Log( const char * formatting, ... );
	void WriteLogHeader( );
	void ExecuteString( nwn_objid_t OBJECT_SELF, char * str );
	void ExecuteFunction(nwn_objid_t OBJECT_SELF, char * func, char * str, size_t len);
	void HookVM();
	CNWNXMemory mem;

	lua_State *L;
	lua_State *L_Current;
	int L_Pushed;

	int logging;

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	char * MainLoopFunc;
	bool mainloophooked;

	void lua_pushvector(lua_State * L, Vector v);
	Vector lua_tovector(lua_State * L, int idx);

	nwn_objid_t LastObjid;

protected:

private:

};

#endif