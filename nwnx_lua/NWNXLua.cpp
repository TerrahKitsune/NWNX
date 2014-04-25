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

#include "NWNXLua.h"
#include "NWNLuaLib.h"
#include "../NWNXdll/IniFile.h"
CNWNXLua::~CNWNXLua(){OnRelease();}

BOOL CNWNXLua::OnRelease( ){

	if( L )
		lua_close( L );

	if( hFind != INVALID_HANDLE_VALUE ){
		FindClose( hFind );
		hFind=INVALID_HANDLE_VALUE;
	}

	if (MainLoopFunc)
		delete[]MainLoopFunc;

	MainLoopFunc = NULL;

	return CNWNXBase::OnRelease();
}

BOOL CNWNXLua::OnCreate(const char* LogDir){

	L_Current=NULL;
	logging=1;
	char log[MAX_PATH];
	char logmove[MAX_PATH];
	MainLoopFunc = NULL;
	mainloophooked = false;
	sprintf( log, "%s\\nwnx_lua.txt", LogDir);
	sprintf( logmove, "%s\\1\\nwnx_lua.txt", LogDir);

	MoveFile( log, logmove );

	// call the base class function
	if( !CNWNXBase::OnCreate( log ) )
		return false;

	WriteLogHeader( );

	L = lua_newstate( nwn_alloc, NULL );
	if( !L ){
		Log( "! Unable to open lua state!\n" );
		return false;
	}

	hFind = INVALID_HANDLE_VALUE;

	luaL_openlibs(L);

	CIniFile ini("NWNX.ini");
	ini.ReadString("LUA", "StartScript", log, MAX_PATH, "");

	if (log[0]!=0)
		luaL_dofile(L, log);

	RegisterNWNLib(L);

	return true;
}

void CNWNXLua::WriteLogHeader( ){
	
	fprintf( m_fFile, "NWNXLua v0.5 created by Terra_777\n" );
	fprintf( m_fFile, "Copyright © 1994–2013 Lua.org, PUC-Rio. Please visit: http://www.lua.org/ \n\n" );
	fflush( m_fFile );
}

void CNWNXLua::Log( const char * formatting, ... ){

	if( logging == 0 )
		return;

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

unsigned long CNWNXLua::OnRequestObject (char *gameObject, char* Request){

	Log("o OnRequestObject: 0x%08X: %s( ): %08X\n", gameObject, Request, LastObjid);

	return LastObjid;
}

char* CNWNXLua::OnRequest(char *gameObject, char* Request, char* Parameters){

	size_t len = strlen(Parameters);
	char * f = strstr(Parameters, "|");
	char * tmp = f;
	while (f){

		tmp = f;

		f = strstr(f + 1, "|");
	}

	if (tmp)
		*tmp = 0;

	Log( "o OnRequest: 0x%08X: %s( %s )\n", gameObject, Request, Parameters );

	if( strcmp( Request, "sexec" ) == 0 ){
		
		ExecuteString( ((CNWSObject*)gameObject)->obj_generic.obj_id , Parameters );
	}
	else{
		ExecuteFunction(((CNWSObject*)gameObject)->obj_generic.obj_id, Request, Parameters,len);
	}

	return NULL;
}

/*----------------
--Patch functions
----------------*/

void CNWNXLua::ExecuteFunction(nwn_objid_t OBJECT_SELF, char * func, char * str, size_t len){

	LastObjid = OBJECT_INVALID;

	char buf[12];
	sprintf(buf, "%x", OBJECT_SELF);
	lua_pushstring(L, buf);
	lua_setglobal(L, "OBJECT_SELF");

	lua_getglobal(L, func);
	lua_pushstring(L, str);

	if (lua_pcall(L, 1, 1, 0)){

		const char * error = lua_tostring(L, -1);
		strncpy(str, error, len);
		str[len] = 0;
		int temp = logging;
		logging = 0;
		Log("! ExecuteFunction (%s): %s\n", func, error);
		logging = temp;
		lua_pop(L, 1);
	}
	else{
		const char * ret = lua_tostring(L, -1);
		if (ret){

			strncpy(str, ret, len);
			str[len] = 0;
		}
		Log("o ExecuteFunction (%s): %s\n", func, ret);
		lua_pop(L, 1);
	}
}

void CNWNXLua::ExecuteString( nwn_objid_t OBJECT_SELF, char * str ){

	LastObjid = OBJECT_INVALID;

	char buf[12];
	sprintf( buf, "%x", OBJECT_SELF );
	lua_pushstring( L, buf );
	lua_setglobal( L, "OBJECT_SELF" );

	if( luaL_loadstring( L, str ) || lua_pcall( L, 0,0,0 ) ){

		size_t len = strlen( str );
		const char * error = lua_tostring( L, -1 );
		strncpy( str, error, len );
		str[len] = 0;
		int temp = logging;
		logging=0;
		Log( "! ExecuteString: %s\n", error );
		logging=temp;
		lua_pop( L, 1 );
	}
	else{
		
		Log("o ExecuteString: %s\n", str);
	}
}

void CNWNXLua::lua_pushvector(lua_State * L, Vector v){

	lua_createtable(L, 0, 3);

	lua_pushstring(L, "x");
	lua_pushnumber(L, v.x);
	lua_settable(L, -3);

	lua_pushstring(L, "y");
	lua_pushnumber(L, v.y);
	lua_settable(L, -3);

	lua_pushstring(L, "z");
	lua_pushnumber(L, v.z);
	lua_settable(L, -3);
}

Vector CNWNXLua::lua_tovector(lua_State * L, int idx){

	Vector v = { 0 };

	if (!lua_istable(L,idx)){
		return v;
	}

	stackDump(L);

	lua_pushvalue(L, idx);

	lua_pushstring(L, "x");
	lua_gettable(L, -2);
	v.x = lua_tonumber(L, -1);

	lua_pushstring(L, "y");
	lua_gettable(L, -2);
	v.y = lua_tonumber(L, -1);

	lua_pushstring(L, "z");
	lua_gettable(L, -2);
	v.z = lua_tonumber(L, -1);

	lua_pop(L, 1);

	stackDump(L);

	return v;
}

/*int CNWNXLua::FireEvent(nwn_objid_t OBJECT_SELF){

	LastObjid = OBJECT_INVALID;

	InScript = true;
	
	lua_getglobal(L, Script[EP->EVENT].CStr());
	lua_pushinteger(L, EP->EVENT);
	char buf[12];
	sprintf(buf, "%x", OBJECT_SELF);
	lua_pushstring(L, buf);

	lua_createtable(L, 5, 6);

	lua_pushstring(L, "MainTarget");
	sprintf(buf, "%x", EP->oTarget[0]);
	lua_pushstring(L, buf);
	lua_settable(L, -3);

	lua_pushstring(L, "SecondaryTarget");
	sprintf(buf, "%x", EP->oTarget[1]);
	lua_pushstring(L, buf);
	lua_settable(L, -3);

	for (int n = 0; n < 5; n++){
		lua_pushinteger(L, n);
		lua_pushinteger(L, EP->nData[n]);
		lua_settable(L, -3);
	}

	lua_pushstring(L, "MainLocation");
	lua_pushvector(L, EP->lTarget[0]);
	lua_settable(L, -3);

	lua_pushstring(L, "SecondaryLocation");
	lua_pushvector(L, EP->lTarget[1]);
	lua_settable(L, -3);

	lua_pushstring(L, "MainStr");
	lua_pushstring(L, EP->sData[0].CStr() == NULL ? "" : EP->sData[0].CStr());
	lua_settable(L, -3);

	lua_pushstring(L, "SecondaryStr");
	lua_pushstring(L, EP->sData[1].CStr() == NULL ? "" : EP->sData[1].CStr());
	lua_settable(L, -3);

	int ByPass = 0x80000000;

	if (lua_pcall(L, 3, 1, 0)){

		const char * error = lua_tostring(L, -1);
		int temp = logging;
		logging = 0;
		Log("! ExecuteString: %s\n", error);
		logging = temp;
	}
	else{

		if (lua_tointeger(L, 1))
			ByPass = lua_tointeger(L, 1);
		else
			ByPass = 0x80000000;
	}

	lua_settop(L, 0);
	
	InScript = false;

	for (int n = 0; n<2; n++){

		if (EP->sData[n].text)
			mem.nwnx_free(EP->sData[n].text);
	}

	memset(EP, 0, sizeof(EventParams));

	for (int n = 0; n<2; n++){
		EP->oTarget[n] = OBJECT_INVALID;
	}

	return ByPass;
}*/