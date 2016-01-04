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
#include "LuaINI.h"

void register_c_function(lua_State *L, const char * tableName, const char * funcName, lua_CFunction funcPointer)
{
	lua_getglobal(L, tableName);
	if (!lua_istable(L, -1))
	{
		lua_createtable(L, 0, 1);
		lua_setglobal(L, tableName);
		lua_getglobal(L, tableName);
	}

	lua_pushstring(L, funcName);
	lua_pushcfunction(L, funcPointer);
	lua_settable(L, -3);

	lua_pop(L, 1);
}

CNWNXLua::~CNWNXLua(){OnRelease();}

void CNWNXLua::SendNameUpdate(DWORD SendToID, DWORD ObjectToUpdateID){

	CNWSPlayer * pc = (*NWN_AppManager)->app_server->GetClientObjectByObjectId(SendToID);
	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject(ObjectToUpdateID);

	if (!pc || !obj || obj->obj_generic.obj_type2 != CGameObject__OBJECT_TYPE_CREATURE)
		return;

	CLastUpdateObject * luo = (CLastUpdateObject *)mem.nwnx_malloc(sizeof(CLastUpdateObject));

	if (!luo)
		return;

	int(__thiscall *CLastUpdateObject__CLastUpdateObject)(CLastUpdateObject *pThis) = (int(__thiscall *)(CLastUpdateObject *))0x0046ACF0;

	CLastUpdateObject__CLastUpdateObject(luo);

	char * pData;
	DWORD length;

	CNWSMessage * exoMess = (*NWN_AppManager)->app_server->GetNWSMessage();
	CNWMessage * nwMess = (CNWMessage *)exoMess;

	nwMess->CreateWriteMessage(0x400, pc->pl_id, 1);
	exoMess->WriteGameObjUpdate_UpdateObject(pc, obj, luo, 0, 0x400);
	nwMess->GetWriteMessage(&pData, &length);

	if (length)
		exoMess->SendServerToPlayerMessage(pc->pl_id, 5, 1, pData, length);

	mem.nwnx_free(luo);
}

void CNWNXLua::UpdateObjectForAllPlayers(nwn_objid_t obj){

	nwn_objid_t oPC = (*NWN_AppManager)->app_server->GetFirstPCObject();
	while (oPC != OBJECT_INVALID){

		this->SendNameUpdate(oPC, obj);

		oPC = (*NWN_AppManager)->app_server->GetNextPCObject();
	}
}

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

	free(targettable);

	return CNWNXBase::OnRelease();
}

BOOL CNWNXLua::OnCreate(const char* LogDir){

	isinhook = false;
	creaturehandlerfunc = -1;
	visibilityfunc=-1;
	mainloop = -1;
	canequipfunc = -1;
	costfunc = -1;
	weightfunc = -1;
	lives = 10;
	CNT = 0;
	MAX = 100;
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

	firstnext = NULL;
	Skills = 28;

	RegisterNWNLib(L);
	LoadIniLib(L);

	targettable = (char*)malloc(10);
	strcpy(targettable, "localdata");

	return true;
}

void CNWNXLua::WriteLogHeader( ){
	
	fprintf( m_fFile, "NWNXLua v1.08 created by Terra_777\n" );
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

char* CNWNXLua::OnGetString(char* Request, char* Parameters){

	char * ret = NULL;
	bool exists = true;

	if (strcmp(Request, "giv") == 0){
		ret = GetKeyValue(Parameters,true);
	}
	else if (strcmp(Request, "gkv") == 0){
		ret = GetKeyValue(Parameters,false);
	}
	else if (strcmp(Request, "lkv") == 0){
		ret = GetKeyValueIndex(Parameters);
	}
	else if (strcmp(Request, "run") == 0){
		ret = ExecuteScalarString(Parameters);
	}
	else if (strcmp(Request, "skv") == 0){
		InsertKeyValue(Parameters);
	}
	else if (strcmp(Request, "dkv") == 0){
		DeleteKey(Parameters);
	}
	else if (strcmp(Request, "st") == 0){
		free(targettable);
		targettable = (char*)malloc(strlen(Parameters)+1);
		strcpy(targettable, Parameters);
	}
	else if (strcmp(Request, "gt") == 0){

		size_t len = strlen(targettable);
		ret = (char*)mem.nwnx_malloc(len + 1);
		if (ret){
			strncpy(ret, targettable, len);
			ret[len] = '\0';
		}	
	}
	else{
		exists = false;
	}

	if (exists)
		Log("o %s.%s( %s ): %s\n", targettable, Request, Parameters, ret);
	else
		Log("! %s.%s( %s ): %s\n", targettable, Request, Parameters, ret);

	return ret;
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
		
		ExecuteString( ((CNWSObject*)gameObject)->obj_generic.obj_id , Parameters, len );
	}
	else{
		ExecuteFunction(((CNWSObject*)gameObject)->obj_generic.obj_id, Request, Parameters,len);
	}

	return NULL;
}

/*----------------
--Patch functions
----------------*/

void CNWNXLua::stackDump(lua_State *L) {
	int i;
	int top = lua_gettop(L);
	Log("o Dumping stack: %d\n", top);
	for (i = 1; i <= top; i++) {  /* repeat for each level */
		int t = lua_type(L, i);
		switch (t) {

		case LUA_TSTRING:  /* strings */
			Log("`%s'", lua_tostring(L, i));
			break;

		case LUA_TBOOLEAN:  /* booleans */
			Log(lua_toboolean(L, i) ? "true" : "false");
			break;

		case LUA_TNUMBER:  /* numbers */
			Log("%g", lua_tonumber(L, i));
			break;

		default:  /* other values */
			Log("%s", lua_typename(L, t));
			break;

		}
		Log("  ");  /* put a separator */
	}
	Log("\n");  /* end the listing */
}

char * CNWNXLua::GetKeyValueIndex(char * key){

	int index = atoi(key);

	if (index < 0)
		return NULL;

	lua_pop(L, lua_gettop(L));

	lua_getglobal(L, "nwscript");
	if (!lua_istable(L, -1))
	{
		lua_pop(L, 1);
		return NULL;
	}

	lua_pushstring(L, targettable);
	lua_gettable(L, -2);

//Table 1
//Table 2

if (!lua_istable(L, -1))
{
	lua_pop(L, 2);
	return NULL;
}

lua_pushnil(L);

int n = 0;

while (lua_next(L, -2)) {

	if (n == index){

		size_t len;
		const char * key = luaL_tolstring(L, -2, &len);

		char * ret = (char*)mem.nwnx_malloc(len + 1);
		if (ret){
			strncpy(ret, key, len);
			ret[len] = '\0';
		}

		lua_pop(L, lua_gettop(L));
		return ret;
	}
	else{
		n++;
	}

	lua_pop(L, 1);
}

lua_pop(L, lua_gettop(L));
return NULL;
}

char * CNWNXLua::GetKeyValue(char * key, bool isIndex){

	lua_pop(L, lua_gettop(L));

	lua_getglobal(L, "nwscript");

	//Table 1

	if (!lua_istable(L, -1))
	{
		lua_pop(L, 1);

		//Emtpy

		lua_createtable(L, 0, 1);
		lua_setglobal(L, "nwscript");
		lua_getglobal(L, "nwscript");
	}

	//Table 1

	lua_pushstring(L, targettable);
	lua_gettable(L, -2);

	//Table 1
	//Table 2

	if (!lua_istable(L, -1))
	{
		lua_pop(L, 1);

		//Table 1

		lua_pushstring(L, targettable);
		lua_createtable(L, 0, 1);
		lua_settable(L, -3);

		//Table 1 [test]=tbl

		lua_pushstring(L, targettable);
		lua_gettable(L, -2);
	}

	//Table 1 [test]=tbl
	//Table 2 = tbl

	if (isIndex)
		lua_pushinteger(L, atoi(key));	
	else
		lua_pushstring(L, key);
	lua_gettable(L, -2);

	if (lua_isnil(L, -1)){
		key[0] = '\0';
		lua_pop(L, 2);
		return NULL;
	}
	else{


		char buffer[100];
		size_t len;
		const char * data = NULL;

		int type = lua_type(L, -1);

		if (type == LUA_TBOOLEAN){		
			sprintf(buffer, "%d", lua_toboolean(L, -1));
			data = buffer;
			len = strlen(data);
		}
		else if (type == LUA_TNUMBER){
			
			sprintf(buffer, "%f", lua_tonumber(L, -1));
			data = buffer;
			len = strlen(data);
		}
		else{
			data = luaL_tolstring(L, -1, &len);
		}

		if (data){
			
			char * ret = (char*)mem.nwnx_malloc(len + 1);
			if (ret){
				strncpy(ret, data, len);
				ret[len] = '\0';
			}

			lua_pop(L, 4);
			return ret;
		}
		else{
			lua_pop(L, 4);
			return NULL;
		}
	}
}

void CNWNXLua::DeleteKey(const char * key){

	lua_pop(L, lua_gettop(L));

	lua_getglobal(L, "nwscript");

	//Table 1

	if (!lua_istable(L, -1))
	{
		lua_pop(L, 1);

		//Emtpy

		lua_createtable(L, 0, 1);
		lua_setglobal(L, "nwscript");
		lua_getglobal(L, "nwscript");
	}

	//Table 1

	lua_pushstring(L, targettable);
	lua_gettable(L, -2);

	//Table 1
	//Table 2

	if (!lua_istable(L, -1))
	{
		lua_pop(L, 1);

		//Table 1

		lua_pushstring(L, targettable);
		lua_createtable(L, 0, 1);
		lua_settable(L, -3);

		//Table 1 [test]=tbl

		lua_pushstring(L, targettable);
		lua_gettable(L, -2);
	}

	//Table 1 [test]=tbl
	//Table 2 = tbl

	lua_pushstring(L, key);
	lua_pushnil(L);
	lua_settable(L, -3);

	lua_pop(L, 2);
}

void CNWNXLua::InsertKeyValue(char * keyvalue){

	char * key = keyvalue;
	char * value = strstr(keyvalue, "|");

	*value = '\0';
	value++;

	lua_pop(L, lua_gettop(L));

	lua_getglobal(L, "nwscript");

	//Table 1

	if (!lua_istable(L, -1))
	{
		lua_pop(L,1);

		//Emtpy

		lua_createtable(L, 0, 1);
		lua_setglobal(L, "nwscript");
		lua_getglobal(L, "nwscript");
	}

	//Table 1
	lua_pushstring(L, targettable);
	lua_gettable(L, -2);

	//Table 1
	//Table 2

	if (!lua_istable(L, -1))
	{		
		lua_pop(L, 1);

		//Table 1

		lua_pushstring(L, targettable);
		lua_createtable(L, 0, 1);
		lua_settable(L, -3);

		//Table 1 [test]=tbl

		lua_pushstring(L, targettable);
		lua_gettable(L, -2);
	}

	//Table 1 [test]=tbl
	//Table 2 = tbl

	lua_pushstring(L, key);
	lua_pushstring(L, value);
	lua_settable(L, -3);

	lua_pop(L, 2);

	//Empty
}

void CNWNXLua::ExecuteFunction(nwn_objid_t OBJECT_SELF, char * func, char * str, size_t len){

	lua_pop(L, lua_gettop(L));

	LastObjid = OBJECT_INVALID;

	char buf[12];
	sprintf(buf, "%x", OBJECT_SELF);
	lua_pushstring(L, buf);
	lua_setglobal(L, "OBJECT_SELF");

	lua_getglobal(L, func);
	lua_pushstring(L, str);
	CNT = 0;
	if (lua_pcall(L, 1, 1, 0)){

		const char * error = lua_tostring(L, -1);
		strncpy(str, error, len);
		str[len] = 0;
		int temp = logging;
		logging = 1;
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
		else
			str[0] = 0;
		Log("o ExecuteFunction (%s): %s\n", func, ret);
		lua_pop(L, 1);
	}
}

void CNWNXLua::ExecuteString( nwn_objid_t OBJECT_SELF, char * str, size_t len ){

	lua_pop(L, lua_gettop(L));

	LastObjid = OBJECT_INVALID;

	char buf[12];
	sprintf( buf, "%x", OBJECT_SELF );
	lua_pushstring( L, buf );
	lua_setglobal( L, "OBJECT_SELF" );
	CNT = 0;
	if( luaL_loadstring( L, str ) || lua_pcall( L, 0,0,0 ) ){

		const char * error = lua_tostring( L, -1 );
		strncpy( str, error, len );
		str[len] = 0;
		int temp = logging;
		logging=1;
		Log( "! ExecuteString: %s\n", error );
		logging=temp;
		lua_pop( L, 1 );
	}
	else{
		
		Log("o ExecuteString: %s\n", str);
		str[0] = 0;
	}
}

char * CNWNXLua::ExecuteScalarString(const char * str){

	lua_pop(L, lua_gettop(L));

	LastObjid = OBJECT_INVALID;

	CNT = 0;
	char * ret=NULL;

	if (luaL_loadstring(L, str) || lua_pcall(L, 0, 1, 0)){

		size_t len;
		const char * error = lua_tolstring(L, -1, &len);
		if (error){
			ret = (char*)mem.nwnx_malloc(len+2);	
			if (ret){
				ret[0] = '%';
				strncpy(&ret[1], error, len);
				ret[len] = '\0';
				int temp = logging;
				logging = 1;
				Log("! ExecuteScalarString: %s\n", error);
				logging = temp;
			}
		}
		else{
			Log("! ExecuteScalarString: no error message!\n");
		}

		lua_pop(L, lua_gettop(L));
		return ret;
	}
	else{

		if (lua_type(L, -1) == LUA_TBOOLEAN){

			ret = (char*)mem.nwnx_malloc(16);
			sprintf(ret, "%d", lua_toboolean(L, -1));
		}
		else if (lua_type(L, -1) == LUA_TNUMBER){

			ret = (char*)mem.nwnx_malloc(32);
			sprintf(ret, "%f", lua_tonumber(L, -1));
		}
		else{

			size_t len;
			const char * returnstr = luaL_tolstring(L, -1, &len);

			if (returnstr){
				ret = (char*)mem.nwnx_malloc(len + 1);
				if (ret){

					strncpy(ret, returnstr, len);
					ret[len] = '\0';
				}
			}
		}

		Log("o ExecuteString: %s\n", str);
	}

	lua_pop(L, lua_gettop(L));
	return ret;
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

	//stackDump(L);

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

	//stackDump(L);

	return v;
}

void CNWNXLua::PushEventString(int event){
	switch (event){
	case 0x1B:lua_pushstring(L, "boot pc"); break;
	case 0x05:lua_pushstring(L, "apply effect"); break;
	case 0x0E:lua_pushstring(L, "remove effect"); break;
	case 0x04:lua_pushstring(L, "remove from area"); break;
	case 0x09:lua_pushstring(L, "play animation"); break;
	case 0x01:lua_pushstring(L, "timed event"); break;
	case 0x14:lua_pushstring(L, "broadcast aoo"); break;
	case 0x15:lua_pushstring(L, "broadcast safe projectile"); break;
	case 0x16:lua_pushstring(L, "feedback message"); break;
	case 0x0F:lua_pushstring(L, "on melee attacked"); break;
	case 0x08:lua_pushstring(L, "spell impact"); break;
	case 0x13:lua_pushstring(L, "item on hit spell impact"); break;
	case 0x0B:lua_pushstring(L, "destroy object"); break;
	case 0x12:lua_pushstring(L, "forced action"); break;
	case 0x0A:lua_pushstring(L, "signal event"); break;
	case 0x18:lua_pushstring(L, "summon creature"); break;
	case 0x1A:lua_pushstring(L, "whirlwind attack"); break;
	case 0x10:lua_pushstring(L, "decrement stacksize"); break;
	case 0x11:lua_pushstring(L, "spawn body bag"); break;
	case 0x02:lua_pushstring(L, "entered trigger"); break;
	case 0x03:lua_pushstring(L, "left trigger"); break;
	case 0x06:lua_pushstring(L, "close object"); break;
	case 0x07:lua_pushstring(L, "open object"); break;
	case 0x0C:lua_pushstring(L, "unlock object"); break;
	case 0x0D:lua_pushstring(L, "lock object"); break;
	case 0x17:lua_pushstring(L, "ability effect applied"); break;
	case 0x19:lua_pushstring(L, "acquire item"); break;
	default:lua_pushstring(L, "unknown event"); break;
	}
}

void CNWNXLua::EventHandler(nwn_objid_t OBJECT_SELF, int event, nwn_objid_t target, DWORD * data){

	lua_pop(L, lua_gettop(L));

	lua_rawgeti(L, LUA_REGISTRYINDEX, creaturehandlerfunc);

	int subdata = -1;
	const char * substring = NULL;

	if (data){

		if (event == 0x0A){
			WORD eventtype = *(WORD*)data;
			subdata = eventtype;
		}
		else if (event == 0x01){
			substring = (char*)*(DWORD*)((BYTE*)data + 0x18);
		}
		else if (event == 0x05 || event == 0x0E){
			CGameEffect * effect = (CGameEffect *)data;
			subdata = effect->eff_id_02;
		}
		else if (event == 0x08 || event == 0x13){
			DWORD spellid = *(DWORD*)data;
			subdata = spellid;
			target = *(DWORD*)((BYTE*)data + 0x0C);
		}
	}


	char buf[12];
	sprintf(buf, "%x", OBJECT_SELF);
	lua_pushstring(L, buf);

	sprintf(buf, "%x", target);
	lua_pushstring(L, buf);

	lua_pushinteger(L, event);

	if (substring)
		lua_pushstring(L, substring);
	else
		lua_pushinteger(L, subdata);

	if (lua_pcall(L, 4, 0, 0)){

		int temp = logging;
		logging = 1;
		Log("! CreatureEventHandler (%08X, %08X): %s\n", OBJECT_SELF, target,lua_tostring(L, -1));
		logging = temp;
		lua_pop(L, 1);
	}
}