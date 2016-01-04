#include "LuaINI.h"
#include "..\NWNXdll\IniFile.h"

int INI_ReadString(lua_State *L){
		
	char buffer[255];
	CIniFile ini(luaL_checkstring(L,1));
	ini.ReadString(luaL_checkstring(L, 2), luaL_checkstring(L, 3), buffer, 254, luaL_checkstring(L, 4));
	lua_pop(L, lua_gettop(L));
	lua_pushstring(L, buffer);

	return 1;
}

int INI_ReadInt(lua_State *L){

	char buffer[255];
	CIniFile ini(luaL_checkstring(L, 1));
	long ret = ini.ReadInteger(luaL_checkstring(L, 2), luaL_checkstring(L, 3), lua_tointeger(L, 4));
	lua_pop(L, lua_gettop(L));
	lua_pushinteger(L, ret);

	return 1;
}

int INI_ReadBool(lua_State *L){

	char buffer[255];
	CIniFile ini(luaL_checkstring(L, 1));
	bool ret = ini.ReadBool(luaL_checkstring(L, 2), luaL_checkstring(L, 3), lua_toboolean(L, 4) > 0);
	lua_pop(L, lua_gettop(L));
	lua_pushboolean(L, ret);

	return 1;
}

void LoadIniLib(lua_State *L){

	register_c_function(L, "ini", "ReadString", INI_ReadString);
	register_c_function(L, "ini", "ReadInt", INI_ReadInt);
	register_c_function(L, "ini", "ReadBool", INI_ReadBool);
}