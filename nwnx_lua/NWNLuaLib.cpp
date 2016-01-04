#include "Hooks.h"
#include "NWNXSpecialAbilities.h"
#include <time.h>

void stackDump (lua_State *L) {
      int i;
      int top = lua_gettop(L);
		lua.Log( "o Dumping stack: %d\n", top );
      for (i = 1; i <= top; i++) {  /* repeat for each level */
        int t = lua_type(L, i);
        switch (t) {
    
          case LUA_TSTRING:  /* strings */
			  lua.Log("`%s'", lua_tostring(L, i));
            break;
    
          case LUA_TBOOLEAN:  /* booleans */
            lua.Log(lua_toboolean(L, i) ? "true" : "false");
            break;
    
          case LUA_TNUMBER:  /* numbers */
            lua.Log("%g", lua_tonumber(L, i));
            break;
    
          default:  /* other values */
            lua.Log("%s", lua_typename(L, t));
            break;
    
        }
        lua.Log("  ");  /* put a separator */
      }
      lua.Log("\n");  /* end the listing */
   }

time_t FILETIME_to_time_t(const FILETIME *lpFileTime) {

	time_t result;

	SYSTEMTIME st;

	struct tm tmp;

	FileTimeToSystemTime(lpFileTime, &st);

	memset(&tmp, 0, sizeof(struct tm));

	tmp.tm_mday = st.wDay;
	tmp.tm_mon = st.wMonth - 1;
	tmp.tm_year = st.wYear - 1900;

	tmp.tm_sec = st.wSecond;
	tmp.tm_min = st.wMinute;
	tmp.tm_hour = st.wHour;

	result = mktime(&tmp);

	return result;
}

bool get_file_information(const char * path, WIN32_FIND_DATA* data)
{
	HANDLE h = FindFirstFile(path, data);
	if (h == INVALID_HANDLE_VALUE) {
		return false;
	}
	else {
		FindClose(h);
		return true;
	}
}

int (__thiscall * CNWSMessage__SendServerToPlayerChat_Party)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043D790;
int (__thiscall * CNWSMessage__SendServerToPlayerChat_Talk)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043D9A0;
int (__thiscall * CNWSMessage__SendServerToPlayerChat_Tell)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043DC90;
int (__thiscall * CNWSMessage__SendServerToPlayerChat_Shout)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043DF80;
int (__thiscall * CNWSMessage__SendServerToPlayerChat_DM_Silent_Shout)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043E5C0;
int (__thiscall * CNWSMessage__SendServerToPlayerChat_Whisper)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043E4E0;
int (__thiscall * CNWSMessage__SendServerToPlayerChat_DM_Talk)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043DA80;
int (__thiscall * CNWSMessage__SendServerToPlayerChat_DM_Whisper)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043E7D0;

void SendMessageToPC(nwn_objid_t obj, const char * msg){

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(obj);
	if (!cre)
		return;

	CNWCCMessageData * ccmsg = (CNWCCMessageData *)lua.mem.nwnx_malloc(0x34);
	CExoString str;
	memset(ccmsg, 0, 0x34);
	memset(&str, 0, sizeof(CExoString));
	str.CExoStringCpy(msg);
	ccmsg->SetString(0, str);
	cre->SendFeedbackMessage(204, ccmsg, 0);
	lua.mem.nwnx_free(ccmsg);
}

int NWNX_SetObject(lua_State * L){

	sscanf(luaL_checkstring(L, 1), "%x", &lua.LastObjid);
	lua_pop(L, 1);

	//lua.Log("o OBJECT %08X\n", lua.LastObjid);

	return 0;
}

int NWNX_RegisterMainLoopFunc(lua_State * L){

	if (lua_type(L, 1) != LUA_TFUNCTION){
		lua_pop(L, 1);
		luaL_error(L, "no valid function given!");
		return 0;
	}

	if (lua.mainloop == -1){
		ApplyMainLoopHook();
	}
	else{
		luaL_unref(L, LUA_REGISTRYINDEX, lua.mainloop);
	}

	lua.mainloop = luaL_ref(L, LUA_REGISTRYINDEX);

	return 0;
}

int NWNX_GetBlind( lua_State * L ){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf( pc, "%x", &ID );
	lua_pop( L, 1 );

	CNWSCreature * obj = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID( ID );

	if( !obj || ID == OBJECT_INVALID ){
		lua_pushnil( L );	
	}
	else{
		lua_pushboolean( L, obj->GetBlind( ) );
	}

	return 1;
}

int NWNX_GetInvisible( lua_State *L ){

	nwn_objid_t ID;
	nwn_objid_t trg;
	const char *pc = luaL_checkstring(L, 1);
	sscanf( pc, "%x", &ID );
	pc = luaL_checkstring(L, 2);
	sscanf( pc, "%x", &trg );
	
	int DC = lua_tointeger( L, 3 );

	lua_pop( L, 3 );

	CNWSCreature * obj = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID( ID );
	CNWSObject * target = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( trg );

	if( !obj || ID == OBJECT_INVALID || !target || trg == OBJECT_INVALID ){
		lua_pushboolean( L, false );
		return 1;
	}

	lua_pushboolean( L, obj->GetInvisible( target, DC ) );

	return 1;
}

int NWNX_GetVisibleListElement( lua_State *L ){

	nwn_objid_t ID;
	nwn_objid_t trg;
	const char *pc = luaL_checkstring(L, 1);
	sscanf( pc, "%x", &ID );
	pc = luaL_checkstring(L, 2);
	sscanf( pc, "%x", &trg );
	
	lua_pop( L, 2 );

	CNWSCreature * obj = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID( ID );
	CNWSObject * target = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( trg );

	if( !obj || ID == OBJECT_INVALID || !target || trg == OBJECT_INVALID ){
		lua_pushnil( L );
		return 1;
	}

	CNWVisibilityNode * node = obj->GetVisibleListElement( trg );

	if( !node ){
		lua_pushnil( L );
		return 1;
	}

	lua_pushunsigned( L, node->flags );

	return 1;
}

int NWNX_SetVisibleListElement( lua_State *L ){

	nwn_objid_t ID;
	nwn_objid_t trg;
	const char *pc = luaL_checkstring(L, 1);
	sscanf( pc, "%x", &ID );
	pc = luaL_checkstring(L, 2);
	sscanf( pc, "%x", &trg );
	
	DWORD newFlag = lua_tounsigned( L, 3 );

	lua_pop( L, 3 );

	CNWSCreature * obj = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID( ID );
	CNWSObject * target = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( trg );

	if( !obj || ID == OBJECT_INVALID || !target || trg == OBJECT_INVALID ){
		lua_pushnil( L );
		return 1;
	}

	CNWVisibilityNode * node = obj->GetVisibleListElement( trg );

	if( !node ){
		lua_pushnil( L );
		return 1;
	}

	lua_pushunsigned( L, node->flags );

	node->flags = newFlag;

	return 1;
}

int NWNX_SetCheckDistance( lua_State * L ){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf( pc, "%x", &ID );

	int nType = lua_tointeger( L, 2 );
	float fNew = lua_tonumber( L, 3 );

	lua_pop( L, 3 );

	CNWSCreature * obj = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID( ID );

	if( !obj || ID == OBJECT_INVALID ){
		lua_pushnil( L );		
	}
	else{

		float * fPTR = NULL;
		DWORD offs = (DWORD)obj;

		switch( nType ){

			case 1: fPTR = (float*)(offs + 0xA18); break;
			case 2: fPTR = (float*)(offs + 0xA14); break;
			case 3: fPTR = (float*)(offs + 0xA20); break;
			case 4: fPTR = (float*)(offs + 0xA1C); break;
		}

		if( !fPTR ){
			lua_pushnil( L );
		}
		else{

			lua_pushnumber( L, *fPTR );
			*fPTR = fNew;
		}
	}

	return 1;
}

int NWNX_GetCheckDistance( lua_State * L ){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf( pc, "%x", &ID );
	lua_pop( L, 1 );

	CNWSCreature * obj = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID( ID );

	if( !obj || ID == OBJECT_INVALID ){
		lua_pushnil( L );
		lua_pushnil( L );
		
	}
	else{
		lua_pushnumber( L, obj->GetSpotCheckDistance( ) );
		lua_pushnumber( L, obj->GetListenCheckDistance( ) );
		//lua_pushunsigned( L, *(DWORD *)(obj + 0x4BC) );
	}

	return 2;
}

int NWNX_ClearVisibleList( lua_State *L ){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf( pc, "%x", &ID );
	lua_pop( L, 1 );

	CNWSCreature * obj = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID( ID );
	
	if( !obj || ID == OBJECT_INVALID ){
		return 0;
	}

	obj->ClearVisibleList();

	return 0;
}

int NWNX_UpdateVisibleList( lua_State *L ){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf( pc, "%x", &ID );
	lua_pop( L, 1 );

	CNWSCreature * obj = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID( ID );
	
	if( !obj || ID == OBJECT_INVALID ){
		return 0;
	}

	obj->UpdateVisibleList();

	return 0;
}

int NWNX_AddToVisibleList( lua_State *L ){

	nwn_objid_t ID;
	nwn_objid_t trg;
	const char *pc = luaL_checkstring(L, 1);
	sscanf( pc, "%x", &ID );
	pc = lua_tostring(L, 2); 
	sscanf( pc, "%x", &trg );
	
	int a1 = lua_tointeger( L, 3 );
	int a2 = lua_tointeger( L, 4 );
	unsigned char a3 = (unsigned char)lua_tounsigned( L, 5 );
	int a4 = lua_tointeger( L, 6 );

	lua_pop( L, 6 );

	CNWSCreature * obj = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID( ID );
	CNWSObject * target = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( trg );

	if( !obj || ID == OBJECT_INVALID || !target || trg == OBJECT_INVALID ){
		lua_pushinteger( L, 0 );
		return 1;
	}

	lua_pushinteger( L, obj->AddToVisibleList( trg, a1,a2,a3,a4 ) );

	return 1;
}

int NWNX_RemoveFromVisibleList( lua_State *L ){

	nwn_objid_t ID;
	nwn_objid_t trg;
	const char *pc = luaL_checkstring(L, 1);
	sscanf( pc, "%x", &ID );
	pc = lua_tostring(L, 2); 
	sscanf( pc, "%x", &trg );
	
	lua_pop( L, 2 );

	CNWSCreature * obj = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID( ID );
	CNWSObject * target = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( trg );

	if( !obj || ID == OBJECT_INVALID || !target || trg == OBJECT_INVALID ){
		return 0;
	}

	obj->RemoveFromVisibleList( trg );

	return 0;
}

int NWNX_GetFirstFile( lua_State *L){

	if( lua.hFind != INVALID_HANDLE_VALUE ){
		FindClose( lua.hFind );
	}

	lua.hFind = FindFirstFile( lua_tostring( L, 1 ), &lua.FindFileData );

	lua_pop( L, 1 );

	if( lua.hFind == INVALID_HANDLE_VALUE ){

		lua_pushnil( L );	
		return 1;
	}
	else{

		lua_pushstring( L, lua.FindFileData.cFileName );
		lua_pushunsigned(L, lua.FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
	}

	return 2;
}

int NWNX_GetNextFile( lua_State *L){

	if( lua.hFind == INVALID_HANDLE_VALUE ){
		lua_pushnil( L );	
		return 1;
	}

	if( FindNextFile( lua.hFind, &lua.FindFileData ) == 0 ){

		FindClose( lua.hFind );
		lua.hFind=INVALID_HANDLE_VALUE;
		lua_pushnil( L );	
		return 1;
	}
	else{

		lua_pushstring( L, lua.FindFileData.cFileName );
		lua_pushunsigned(L, lua.FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
	}

	return 2;
}

static int NWNX_SetLog (lua_State *L){

	lua.logging = lua_tointeger( L, 1 );
	lua_pop( L, 1 );
	return 0;
}

static int NWNX_Print (lua_State *L) {
	int n = lua_gettop(L); 
	int i;
	const char *s;
	int temp = lua.logging;
	lua.logging=1;
	lua.Log("LUA PRINT: ");
	for (i=1; i<=n; i++) {

		s = lua_tostring(L, -1); 

		if (i>1)
			lua.Log("\t");

		lua.Log(s);
		lua_pop(L, 1);
	}
	lua.Log("\n");
	lua.logging=temp;
	return 0;
}

static int NWNX_msgall (lua_State *L) {

	const char *s = lua_tostring(L, 1); 

	nwn_objid_t oPC = (*NWN_AppManager)->app_server->GetFirstPCObject();

	while( oPC != OBJECT_INVALID ){

		SendMessageToPC( oPC, s );

		oPC = (*NWN_AppManager)->app_server->GetNextPCObject();
	}

	lua_pop( L, 1 );
	return 0;
}

int NWNX_msgpc (lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	const char *s = lua_tostring(L, 2); 
	sscanf( pc, "%x", &ID );
	SendMessageToPC( ID, s );
	lua_pop( L, 2 );
	return 0;
}

int NWNX_GetFirstPC(lua_State *L){
	nwn_objid_t oPC = (*NWN_AppManager)->app_server->GetFirstPCObject();
	char buf[12];
	sprintf( buf, "%x", oPC );
	lua_pushstring( L, buf );
	lua.firstnextlock = false;
	return 1;
}

int NWNX_GetNextPC(lua_State *L){
	nwn_objid_t oPC = (*NWN_AppManager)->app_server->GetNextPCObject();

	if (oPC == OBJECT_INVALID){

		if (lua.firstnextlock)
			luaL_error(L,"GetNextPC cannot has already returned OBJECT_INVALID!");
		else
			lua.firstnextlock = true;
	}

	char buf[12];
	sprintf( buf, "%x", oPC );
	lua_pushstring( L, buf );
	return 1; 
}

int NWNX_ExecuteScript(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 2);
	const char *s = lua_tostring(L, 1); 
	sscanf( pc, "%x", &ID );
	CExoString str;
	str.CExoStringCpy( s );
	int nRet = (*NWN_VirtualMachine)->Runscript( &str, ID );
	lua_pop( L, 2 );
	lua_pushinteger( L, nRet );
	return 1;	
}

int NWNX_IsValidObject(lua_State *L){

	if (!lua_isstring(L, 1)){

		lua_pop(L, 1);
		lua_pushinteger(L, 0);
		return 1;
	}

	nwn_objid_t ID;
	const char *pc = lua_tostring(L, 1);
	sscanf( pc, "%X", &ID );

	if( ID == OBJECT_INVALID ){
		lua_pushinteger( L, 0 );
		return 1;
	}

	lua_pop( L, 1 );
	CGenericObject * obj = (*NWN_AppManager)->app_server->GetGameObject( ID );
	if( obj && obj->obj_id != OBJECT_INVALID )
		lua_pushinteger( L, 1 );
	else
		lua_pushinteger( L, 0 );

	return 1;
}

int NWNX_SetLocalString(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	const char *sVar = luaL_checkstring(L, 2);
	const char *sData = lua_tostring(L, 3); 
	sscanf( pc, "%x", &ID );

	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	if( !obj || ID == OBJECT_INVALID ){
		lua_pop( L, 3 );
		return 0;
	}
	CExoString var;
	CExoString dat;

	var.CExoStringCpy( sVar );
	dat.CExoStringCpy( sData );

	obj->obj_vartable.SetString( var, dat );
	lua_pop( L, 3 );
	return 0;
}

int NWNX_GetLocalString(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	const char *sVar = luaL_checkstring(L, 2);
	sscanf( pc, "%x", &ID );

	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	if( !obj || ID == OBJECT_INVALID ){
		lua_pop( L, 2 );
		lua_pushstring( L, "" );
		return 1;
	}
	CExoString var;

	var.CExoStringCpy( sVar );

	CExoString * str = obj->obj_vartable.GetString( var );
	lua_pop( L, 2 );
	if( str )
		lua_pushstring( L, str->text );
	else
		lua_pushstring( L, "" );

	return 1;
}

int NWNX_DeleteLocalString(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	const char *sVar = luaL_checkstring(L, 2);
	sscanf( pc, "%x", &ID );

	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	if( !obj || ID == OBJECT_INVALID ){
		lua_pop( L, 2 );
		return 0;
	}
	CExoString var;

	var.CExoStringCpy( sVar );

	obj->obj_vartable.DestroyString( var );
	lua_pop( L, 2 );
	return 0;
}

int NWNX_SendChatMessage( lua_State *L ){

	nwn_objid_t ID,Send;
	const char *pc = luaL_checkstring(L, 1);
	const char *sMessage = lua_tostring(L, 3); 
	int nChannel = lua_tointeger(L, 4); 
	sscanf( pc, "%x", &ID );
	pc = luaL_checkstring(L, 2);
	sscanf( pc, "%x", &Send );

	CNWSPlayer * cli = (*NWN_AppManager)->app_server->GetClientObjectByObjectId( ID );
	CNWSMessage * mess = (*NWN_AppManager)->app_server->GetNWSMessage();
	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( Send );

	if( !cli || !obj || ID == OBJECT_INVALID ){
		lua_pop( L, 4 );
		lua_pushinteger( L, 0 );
		return 1;
	}

	CExoString msg;

	msg.CExoStringCpy( sMessage );

	int nRet = 0;

	switch( nChannel ){
		
		case 0: nRet = CNWSMessage__SendServerToPlayerChat_Talk( mess, cli->pl_id, Send, msg ); break; 
		case 1: nRet = CNWSMessage__SendServerToPlayerChat_Whisper( mess, cli->pl_id, Send, msg ); break;
		case 2: nRet = CNWSMessage__SendServerToPlayerChat_Party( mess, cli->pl_id, Send, msg ); break;
		case 3: nRet = CNWSMessage__SendServerToPlayerChat_Tell( mess, cli->pl_id, Send, msg ); break;
		case 4: nRet = CNWSMessage__SendServerToPlayerChat_Shout( mess, cli->pl_id, Send, msg ); break;
		case 5: nRet = CNWSMessage__SendServerToPlayerChat_DM_Silent_Shout( mess, cli->pl_id, Send, msg ); break;
		case 6: nRet = CNWSMessage__SendServerToPlayerChat_DM_Talk( mess, cli->pl_id, Send, msg ); break;
		case 7: nRet = CNWSMessage__SendServerToPlayerChat_DM_Whisper( mess, cli->pl_id, Send, msg ); break;
	}

	lua_pop( L, 4 );
	lua_pushinteger( L, nRet );
	return 1;
}

int NWNX_IntToObject( lua_State *L ){
	
	int nPC = lua_tointeger(L, 1); 
	char str[12];
	sprintf( str, "%x", str );
	nwn_objid_t ID;
	sscanf( str, "%x", &ID );
	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( ID );
	if( obj ){
		sprintf( str, "%x", obj->obj_generic.obj_id );
	}
	else
		sprintf( str, "%x", OBJECT_INVALID );

	lua_pop( L, 1 );

	lua_pushstring( L, str );
	return 1;
}

int NWNX_ObjectToInt( lua_State *L ){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf( pc, "%x", &ID );

	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( ID );
	
	lua_pop( L, 1 );

	if( obj ){
		lua_pushinteger( L, obj->obj_generic.obj_id );
	}
	else
		lua_pushinteger( L, OBJECT_INVALID );

	return 1;
}

int NWNX_SetLocalObject(lua_State *L){

	nwn_objid_t ID;
	nwn_objid_t data;
	const char *pc = luaL_checkstring(L, 1);
	const char *sVar = luaL_checkstring(L, 2);
	const char *sData = lua_tostring(L, 3); 
	sscanf( pc, "%x", &ID );
	sscanf( sData, "%x", &data );
	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	CNWSObject * trg = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( data );
	if( !obj || !trg || ID == OBJECT_INVALID ){
		lua_pop( L, 3 );
		return 0;
	}

	CExoString var;
	var.CExoStringCpy( sVar );

	obj->obj_vartable.SetObject( var, data );
	lua_pop( L, 3 );
	return 0;
}

int NWNX_GetLocalObject(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	const char *sVar = luaL_checkstring(L, 2);
	sscanf( pc, "%x", &ID );

	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	if( !obj || ID == OBJECT_INVALID ){
		lua_pop( L, 2 );
		lua_pushstring( L, "" );
		return 1;
	}
	CExoString var;

	var.CExoStringCpy( sVar );

	nwn_objid_t data = obj->obj_vartable.GetNWNObject( var );

	lua_pop( L, 2 );

	char buf[12];
	sprintf(buf, "%x", data);
	lua_pushstring(L, buf);

	return 1;
}

int NWNX_DeleteLocalObject(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	const char *sVar = luaL_checkstring(L, 2);
	sscanf( pc, "%x", &ID );

	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	if( !obj || ID == OBJECT_INVALID ){
		lua_pop( L, 2 );
		return 0;
	}
	CExoString var;

	var.CExoStringCpy( sVar );

	obj->obj_vartable.DestroyObject( var );
	lua_pop( L, 2 );
	return 0;
}

int NWNX_SetLocalInt(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	const char *sVar = luaL_checkstring(L, 2);

	sscanf( pc, "%x", &ID );
	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if( !obj || ID == OBJECT_INVALID ){
		lua_pop( L, 3 );
		return 0;
	}

	CExoString var;
	var.CExoStringCpy( sVar );

	obj->obj_vartable.SetInt( var, lua_tointeger( L, 3 ), 0 );
	lua_pop( L, 3 );
	return 0;
}

int NWNX_GetLocalInt(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	const char *sVar = luaL_checkstring(L, 2);
	sscanf( pc, "%x", &ID );

	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	if( !obj || ID == OBJECT_INVALID ){
		lua_pop( L, 2 );
		lua_pushstring( L, "" );
		return 1;
	}
	CExoString var;
	var.CExoStringCpy(sVar);
	lua_pop( L, 2 );

	lua_pushinteger( L, obj->obj_vartable.GetInt( var ) );

	return 1;
}

int NWNX_DeleteLocalInt(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	const char *sVar = luaL_checkstring(L, 2);
	sscanf( pc, "%x", &ID );

	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	if( !obj || ID == OBJECT_INVALID ){
		lua_pop( L, 2 );
		return 0;
	}
	CExoString var;

	var.CExoStringCpy( sVar );

	obj->obj_vartable.DestroyInt( var );
	lua_pop( L, 2 );
	return 0;
}

int NWNX_CallNWScriptFunction( lua_State *L ){

	DWORD proc;
	sscanf(luaL_checkstring(L, 1), "%x", &proc);
	lua_remove( L, 1 );

	int nArg1 = lua_tointeger( L, 1 );
	lua_remove( L, 1 );
	int nArg2 = lua_tointeger( L, 1 );
	lua_remove( L, 1 );

	if( proc < 0x00567DA0 || proc > 0x00598040 ){

		lua.Log( "o NWNX_CallNWScriptFunction: 0x%08X is outside safezone!\n", proc );
		lua_pushnil( L );
		return 1;
	}

	int (__thiscall *NWScriptFunction)( CVirtualMachine * pThis, int arg1, int arg2 ) = (int (__thiscall *)( CVirtualMachine *, int, int ))proc;

	lua.L_Current=L;
	lua.L_Pushed=0;
	int n = NWScriptFunction( (*NWN_VirtualMachine), nArg1, nArg2 ); 
	lua.L_Current=NULL;

	if( lua.L_Pushed <= 0 ){

		lua_pushnil( L );	
	}

	return lua.L_Pushed;
}

int NWNX_SetGetVoiceSet(lua_State *L){
	
	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);
		
	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	if (!cre){

		lua_pushinteger(L, -1);
		return 1;
	}

	int nCurrent = cre->cre_soundset;

	//lua.logging = 1;

	if (lua_gettop(L) == 1 || lua_isnil(L,2)){
		lua_pop(L, 1);
		lua_pushinteger(L, nCurrent);
		return 1;
	}

	//stackDump(L);

	int nNew = lua_tointeger(L, 2);
	lua_pop(L, 2);

	//lua.Log("Top: %d new: %d isnil: %d\n", lua_gettop(L), nNew, lua_isnil(L, 1));

	if (nNew < 0){

		lua_pushinteger(L, nCurrent);
		return 1;
	}

	cre->cre_soundset = nNew;
	lua_pushinteger(L, cre->cre_soundset);
	return 1;
}

void luadiehook(lua_State* L, lua_Debug *ar){

	if (lua.MAX > 0 && ++lua.CNT >= lua.MAX){
		luaL_error(L, "Error: too many instructions!");
	}
}

int NWNX_SetInstructionCap(lua_State *L){

	int nNew = lua_tointeger(L, 1);
	lua_pop(L, 1);
	if (nNew < 0)
		nNew = 0;

	lua.MAX = nNew;
	lua.CNT = 0;

	return 0;
}

int NWNX_GetInstructionCap(lua_State *L){

	lua_pushinteger(L, lua.MAX);
	return 1;
}

int NWNX_SetGetScript(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	int nType = lua_tointeger(L, 2);
	sscanf(pc, "%x", &ID);
	
	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	if (!cre || cre->obj.obj_generic.obj_type2 != CGameObject__OBJECT_TYPE_CREATURE){

		lua_pop(L, lua_gettop(L));
		lua_pushstring(L, "");
		return 1;
	}

	CExoString * script = NULL;

	switch (nType){

		case 1:script = &cre->cre_events.onAttacked; break;
		case 2:script = &cre->cre_events.onBlocked; break;
		case 3:script = &cre->cre_events.onDamaged; break;
		case 4:script = &cre->cre_events.onDeath; break;
		case 5:script = &cre->cre_events.onDialog; break;
		case 6:script = &cre->cre_events.onDisturbed; break;
		case 7:script = &cre->cre_events.onEndCombat; break;
		case 8:script = &cre->cre_events.onHeartbeat; break;
		case 9:script = &cre->cre_events.onPerception; break;
		case 10:script = &cre->cre_events.onRested; break;
		case 11:script = &cre->cre_events.onSpawn; break;
		case 12:script = &cre->cre_events.onSpellCastAt; break;
		case 13:script = &cre->cre_events.onUserDefined; break;
		default:break;
	}

	if (script == NULL){
		lua_pushstring(L, "");
		return 1;
	}

	if (lua_gettop(L)==2 || lua_isnil(L,3)){
	
		lua_pop(L, lua_gettop(L));
		lua_pushstring(L, script->text);
		return 1;
	}

	size_t len;
	const char * newScript = lua_tolstring(L, 3,&len);

	if (script->text)
		lua.mem.nwnx_free(script->text);
	script->text = NULL;
	script->len = 0;

	script->text = (char*)lua.mem.nwnx_malloc(len+1);
	script->len = len;

	strncpy(script->text,newScript,len);
	script->text[len] = '\0';

	lua_pop(L, lua_gettop(L));

	lua_pushstring(L, script->text);
	return 1;
}

int NWNX_SetGetMovementRate(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	if (!cre){

		lua_pushinteger(L, -1);
		return 1;
	}

	int nCurrent = cre->cre_stats->cs_movement_rate;

	//lua.logging = 1;

	if (lua_gettop(L) == 1 || lua_isnil(L, 2)){
		lua_pop(L, 1);
		lua_pushinteger(L, nCurrent);
		return 1;
	}

	//stackDump(L);

	int nNew = lua_tointeger(L, 2);
	lua_pop(L, 2);

	//lua.Log("Top: %d new: %d isnil: %d\n", lua_gettop(L), nNew, lua_isnil(L, 1));

	if (nNew < 0){

		lua_pushinteger(L, nCurrent);
		return 1;
	}

	cre->cre_stats->cs_movement_rate = nNew;
	lua_pushinteger(L, cre->cre_stats->cs_movement_rate);
	return 1;
}

int NWNX_SetAbilityScore(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);
	int nType = lua_tointeger(L, 2);
	int nNumber = lua_tointeger(L, 3);

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	if (!cre){
		lua_pop(L, lua_gettop(L));
		return 0;
	}


	if (nNumber < 0)
		nNumber = 0;
	else if (nNumber>254)
		nNumber = 254;

	switch (nType){

		case 0: cre->cre_stats->SetSTRBase(nNumber); break;
		case 1: cre->cre_stats->SetDEXBase(nNumber); break;
		case 2: cre->cre_stats->SetCONBase(nNumber,1); break;
		case 3: cre->cre_stats->SetINTBase(nNumber); break;
		case 4: cre->cre_stats->SetWISBase(nNumber); break;
		case 5: cre->cre_stats->SetCHABase(nNumber); break;
		default:break;
	}

	lua_pop(L, lua_gettop(L));
	return 0;
}

int NWNX_SetSkillScore(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);
	int nType = lua_tointeger(L, 2);
	int nNumber = lua_tointeger(L, 3);

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	if (!cre){
		lua_pop(L, lua_gettop(L));
		return 0;
	}


	if (nNumber < 0)
		nNumber = 0;
	else if (nNumber>254)
		nNumber = 254;

	if (nType < 0)
		cre->cre_stats->cs_skill_points = nNumber;
	else
		cre->cre_stats->cs_skills[nType] = nNumber;

	lua_pop(L, lua_gettop(L));
	return 0;
}

int NWNX_GetStoredSkillPoints(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	if (!cre){
		lua_pop(L, lua_gettop(L));
		lua_pushinteger(L,0);
		return 1;
	}

	lua_pushinteger(L, cre->cre_stats->cs_skill_points);
	return 1;
}

int NWNX_SetBaseDesc(lua_State *L){

	size_t len;
	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	const char *newDesc = luaL_checklstring(L, 2, &len);
	sscanf(pc, "%x", &ID);
	
	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	
	if (!cre){
		lua_pop(L, lua_gettop(L));
		return 0;
	}

	char * nDesc = (char*)lua.mem.nwnx_malloc(len+1);

	if (!nDesc){
		lua_pop(L, lua_gettop(L));
		return 0;
	}
	else{
		strncpy(nDesc, newDesc, len);
		nDesc[len] = '\0';
	}

	CExoLocString * Desc = &cre->cre_stats->cs_desc_base;

	Desc->AddString(0, nDesc);

	lua_pop(L, lua_gettop(L));
	return 0;
}

int NWNX_SetGetName(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	int Type = lua_tointeger(L, 2);
	sscanf(pc, "%x", &ID);

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	if (!cre ||ID == OBJECT_INVALID){
		lua_pop(L, lua_gettop(L));
		lua_pushstring(L, "");
		return 1;
	}

	CExoLocStringElement * name=NULL;

	if (Type == 1){

		name = cre->cre_stats->cs_lastname.GetLangEntry(0);
	}
	else{

		name = cre->cre_stats->cs_firstname.GetLangEntry(0);
	}

	if (lua_gettop(L) == 2 || lua_isnil(L, 3)){
		lua_pop(L, lua_gettop(L));
		if (name && name->Text.text)
			lua_pushstring(L, name->Text.text);
		else 
			lua_pushstring(L, "");
		return 1;
	}

	size_t len;
	const char * newname = lua_tolstring(L, 3,&len);

	if (!newname || len==0){

		if (name && name->Text.text){
			lua.mem.nwnx_free(name->Text.text);
			name->Text.text = NULL;
			name->Text.len = 0;
		}
		return 0;
	}

	char * newn = (char*)lua.mem.nwnx_malloc(len+1);
	if (newname){

		strncpy(newn, newname, len);
		newn[len] = '\0';

		if (Type == 1)
			cre->cre_stats->cs_lastname.AddString(0, newn);
		else
			cre->cre_stats->cs_firstname.AddString(0, newn);
	}
	
	lua.UpdateObjectForAllPlayers(ID);

	return 0;
}

int NWNX_GetABVs(lua_State *L){

	nwn_objid_t ID;
	nwn_objid_t TRG;
	const char *pc = luaL_checkstring(L, 1);
	const char *trg = luaL_checkstring(L, 2);
	sscanf(pc, "%x", &ID);
	sscanf(trg, "%x", &TRG);

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	CNWSCreature * target = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(TRG);

	if (!cre || !target){

		lua_pop(L, lua_gettop(L));
		lua_pushinteger(L, 0);
		return 1;
	}

	lua_pop(L, lua_gettop(L));
	lua_pushinteger(L, cre->cre_stats->GetAttackModifierVersus(target) );
	return 1;

}

int NWNX_GetACVs(lua_State *L){

	nwn_objid_t ID;
	nwn_objid_t TRG;
	const char *pc = luaL_checkstring(L, 1);
	const char *trg = luaL_checkstring(L, 2);
	sscanf(pc, "%x", &ID);
	sscanf(trg, "%x", &TRG);

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	CNWSCreature * target = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(TRG);

	if (!cre || !target){

		lua_pop(L, lua_gettop(L));
		lua_pushinteger(L, 0);
		return 1;
	}

	lua_pop(L, lua_gettop(L));
	lua_pushinteger(L, cre->cre_stats->GetArmorClassVersus( target, 0 ));
	return 1;

}

int NWNX_AddFeat(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	int nFeat = lua_tointeger(L, 2);
	int nLevel = lua_tointeger(L, 3);

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if (!cre || nFeat < 0 || nLevel < 0){

		lua_pop(L, lua_gettop(L));
		lua_pushinteger(L, 0);
		return 1;
	}

	if (nLevel > 0){

		if (nLevel > cre->cre_stats->cs_levelstat.len){
			lua_pop(L, lua_gettop(L));
			lua_pushinteger(L, 0);
			return 1;
		}
		
		CNWSStats_Level *ls = (CNWSStats_Level*)cre->cre_stats->cs_levelstat.data[nLevel - 1];

		ls->AddFeat(nFeat);
	}

	int ret = cre->cre_stats->AddFeat(nFeat);

	lua_pop(L, lua_gettop(L));
	lua_pushinteger(L, ret);
	return 1;	
}

int NWNX_RemoveFeat(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	int nFeat = lua_tointeger(L, 2);

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if (!cre || nFeat < 0){

		lua_pop(L, lua_gettop(L));
		return 0;
	}

	CNWSStats_Level *ls;

	UNNEST:

	for (int n = 0; n < cre->cre_stats->cs_levelstat.len; n++){

		ls = (CNWSStats_Level*)cre->cre_stats->cs_levelstat.data[n];
		for (int i = 0; i < ls->ls_featlist.len; i++){

			if (ls->ls_featlist.data[i] == nFeat){

				uint16_t * temp = (uint16_t*)lua.mem.nwnx_malloc((ls->ls_featlist.len-1)*sizeof(uint16_t));
				if (temp){
					
					uint16_t swap = ls->ls_featlist.data[ls->ls_featlist.len - 1];
					ls->ls_featlist.data[ls->ls_featlist.len - 1] = ls->ls_featlist.data[i];
					ls->ls_featlist.data[i] = swap;

					memcpy(temp, ls->ls_featlist.data, (ls->ls_featlist.len - 1)*sizeof(uint16_t));

					lua.mem.nwnx_free(ls->ls_featlist.data);

					ls->ls_featlist.data = temp;
					ls->ls_featlist.len = ls->ls_featlist.len - 1;
					ls->ls_featlist.alloc = ls->ls_featlist.len;

					goto UNNEST;
				}				
			}
		}
	}

	cre->cre_stats->RemoveFeat(nFeat);

	lua_pop(L, lua_gettop(L));
	return 0;
}

int NWNX_GetLevelStat(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	int nLevel = lua_tointeger(L, 2)-1;
	lua_pop(L, lua_gettop(L));

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if (!cre || nLevel < 0 || nLevel >= cre->cre_stats->cs_levelstat.len){

		lua_pushnil(L);
		return 1;
	}

	CNWSStats_Level * ls = (CNWSStats_Level*)cre->cre_stats->cs_levelstat.data[nLevel];

	lua_newtable(L);

	lua_pushstring(L, "ability");
	lua_pushinteger(L, ls->ls_ability );
	lua_settable(L, -3);

	lua_pushstring(L, "class");
	lua_pushinteger(L, ls->ls_class);
	lua_settable(L, -3);

	lua_pushstring(L, "hp");
	lua_pushinteger(L, ls->ls_hp);
	lua_settable(L, -3);

	lua_pushstring(L, "skillpoints");
	lua_pushinteger(L, ls->ls_skillpoints);
	lua_settable(L, -3);
	lua_pushstring(L, "feats");
	lua_newtable(L);
	for (int n = 0; n < ls->ls_featlist.len; n++){

		lua_pushinteger(L, n+1);
		lua_pushinteger(L, ls->ls_featlist.data[n]);
		lua_settable(L, -3);
	}
	lua_settable(L, -3);

	lua_pushstring(L, "skills");
	lua_newtable(L);
	for (int n = 0; n < lua.Skills; n++){

		lua_pushinteger(L, n + 1);
		lua_pushinteger(L, ls->ls_skilllist[n]);
		lua_settable(L, -3);
	}
	lua_settable(L, -3);

	lua_pushstring(L, "spells");
	lua_newtable(L);
	int nSpellTrack = 1;
	for (int n = 0; n < 10; n++){

		if (ls->ls_spells_known[n].len>0){

			lua_pushinteger(L, n);
			lua_newtable(L);
			for (int i = 0; i < ls->ls_spells_known[n].len; i++){

				lua_pushinteger(L, i + 1);
				lua_pushinteger(L, ls->ls_spells_known[n].data[i]);
				lua_settable(L, -3);
			}
			lua_settable(L, -3);
		}
	}
	lua_settable(L, -3);

	return 1;
}

int NWNX_SetGetDomain(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	int nDomain = lua_tointeger(L, 2);

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if (!cre){

		lua_pop(L, lua_gettop(L));
		lua_pushinteger(L, -1);
		return 1;
	}

	CNWSCreatureClass * cls = NULL;

	for (int n = 0; n < cre->cre_stats->cs_classes_len; n++){

		if( cre->cre_stats->cs_classes[n].cl_class == 2 ){

			cls = &cre->cre_stats->cs_classes[n];
			break;
		}
	}

	if (!cls){

		lua_pop(L, lua_gettop(L));
		lua_pushinteger(L, -1);
		return 1;
	}

	if (lua_gettop(L) == 2 || lua_isnil(L, 3)){

		lua_pop(L, lua_gettop(L));
		if (nDomain == 2)
			lua_pushinteger(L, cls->cl_domain_2);
		else
			lua_pushinteger(L, cls->cl_domain_1);
		return 1;
	}

	int newDomain = lua_tointeger(L, 3);

	if (nDomain == 2)
		cls->cl_domain_2 = newDomain;
	else
		cls->cl_domain_1 = newDomain;

	lua_pop(L, lua_gettop(L));
	if (nDomain == 2)
		lua_pushinteger(L, cls->cl_domain_2);
	else
		lua_pushinteger(L, cls->cl_domain_1);
	return 1;
}

int NWNX_SetGetSpecialization(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if (!cre){

		lua_pop(L, lua_gettop(L));
		lua_pushinteger(L, -1);
		return 1;
	}

	CNWSCreatureClass * cls = NULL;

	for (int n = 0; n < cre->cre_stats->cs_classes_len; n++){

		if (cre->cre_stats->cs_classes[n].cl_class == 10){

			cls = &cre->cre_stats->cs_classes[n];
			break;
		}
	}

	if (!cls){

		lua_pop(L, lua_gettop(L));
		lua_pushinteger(L, -1);
		return 1;
	}

	if (lua_gettop(L) == 1 || lua_isnil(L, 2)){

		lua_pop(L, lua_gettop(L));
		lua_pushinteger(L, cls->cl_specialist);
		return 1;
	}

	int newSpec = lua_tointeger(L, 2);

	cls->cl_specialist = newSpec;

	lua_pop(L, lua_gettop(L));
	lua_pushinteger(L, cls->cl_specialist);
	return 1;
}

int NWNX_DeleteTURD(lua_State *L){

	if(!lua_isstring(L, 1)){

		lua_pop(L, lua_gettop(L));
		lua_pushinteger(L, 0);
		return 1;
	}

	nwn_objid_t ID;
	const char *pc = lua_tostring(L, 1);
	sscanf(pc, "%x", &ID);

	CNWSPlayerTURD * turd = NULL;

	CNWSModule * mod = (*NWN_AppManager)->app_server->srv_internal->GetModule();

	CExoLinkedListElement * list = mod->mod_PlayerTURDList.ListHeader->FirstElement;

	while (list){

		turd = (CNWSPlayerTURD *)list->Data;

		if (ID == turd->TURD_PlayerID){

			list->Data = NULL;
			mod->mod_PlayerTURDList.Remove(list);
			((*NWN_AppManager)->app_server)->GetObjectArray()->DEL(turd->TURD_PlayerID);
			turd->ScalarDestructor(1);
			lua.firstnext = NULL;
			lua_pop(L, lua_gettop(L));
			lua_pushinteger(L, 1);
			return 1;
		}

		list = list->NextElement;
	}

	lua_pop(L, lua_gettop(L));
	lua_pushinteger(L, 0);
	return 1;
}

int NWNX_GetFirstNextTURD(lua_State *L){

	if (lua_gettop(L) == 1 && !lua_isnil(L, 1) && lua_tointeger(L, 1) == 1){

		CNWSModule * mod = (*NWN_AppManager)->app_server->srv_internal->GetModule();
		lua.firstnext = mod->mod_PlayerTURDList.ListHeader->FirstElement;
	}
	else if (lua.firstnext && lua.firstnext->NextElement){
		lua.firstnext = lua.firstnext->NextElement;
	}
	else
		lua.firstnext = NULL;

	if (!lua.firstnext || !lua.firstnext->Data){

		lua_pop(L, lua_gettop(L));
		lua_pushnil(L);
		return 1;
	}

	CNWSPlayerTURD * turd = (CNWSPlayerTURD *)lua.firstnext->Data;

	char buf[12];
	sprintf(buf, "%x", turd->TURD_PlayerID);

	lua_pop(L, lua_gettop(L));

	lua_pushstring(L, buf);
	lua_pushstring(L, turd->TURD_CommntyName.text);

	return 2;
}

int NWNX_GetBic(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	CNWSPlayer * ply = (*NWN_AppManager)->app_server->GetClientObjectByObjectId(ID);

	if (!ply){

		lua_pop(L, lua_gettop(L));
		lua_pushstring(L, "");
		return 1;
	}

	char str[17] = { 0 };
	strncpy(str, ply->pl_bicfile, 16);

	lua_pop(L, lua_gettop(L));
	lua_pushstring(L, str);
	return 1;
}

int NWNX_SetRace(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);
	int race = lua_tointeger(L, 2);

	lua_pop(L, lua_gettop(L));

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	if (!cre)
		return 0;

	cre->cre_stats->cs_race = race;

	return 0;
}

int NWNX_ResetNWScriptTMI(lua_State *L){

	(*NWN_VirtualMachine)->field_8 = 0;
	return 0;
}

int NWNX_SetGender(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);
	int sex = lua_tointeger(L, 2);

	lua_pop(L, lua_gettop(L));

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	if (!cre)
		return 0;

	cre->cre_stats->cs_gender = sex;

	lua.UpdateObjectForAllPlayers(ID);

	return 0;
}

int NWNX_GetArcaneFail(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	lua_pop(L, lua_gettop(L));

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	if (!cre){
		lua_pushinteger(L, -1);
		return 1;
	}

	lua_pushinteger(L, cre->cre_stats->cs_combat_info->ArcaneSpellFail);

	return 1;
}

int NWNX_SetTestVisibilityFunction(lua_State *L){

	if (lua_type(L, 1) != LUA_TFUNCTION){
		lua_pop(L,1);
		luaL_error(L, "no valid function given!");
		return 0;
	}

	if (lua.visibilityfunc == -1){
		ApplyVisibilityHook();
	}
	else{
		luaL_unref(L, LUA_REGISTRYINDEX, lua.visibilityfunc);
	}

	lua.visibilityfunc = luaL_ref(L, LUA_REGISTRYINDEX);

	return 0;
}

int NWNX_QueryPerformanceCounter(lua_State *L){

	LARGE_INTEGER frequency; 
	LARGE_INTEGER ticks;  

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&ticks);

	bool asstrings = lua_toboolean(L, 1)>0;
	lua_pop(L, lua_gettop(L));

	if (asstrings){
		
		char str[50];
		sprintf(str, "%lu", ticks.QuadPart);
		lua_pushstring(L,str);
		sprintf(str, "%lu", frequency.QuadPart);
		lua_pushstring(L, str);
	}
	else{
		lua_pushunsigned(L, ticks.QuadPart);
		lua_pushunsigned(L, frequency.QuadPart);
	}

	return 2;
}

int NWNX_GetAllObjects(lua_State *L){

	CGameObjectArray *pGameObjArray = ((*NWN_AppManager)->app_server)->GetObjectArray();
	if (!pGameObjArray){
		lua_pushnil(L);
		return 1;
	}

	CGameObjectArrayElement **pObjects = pGameObjArray->Objects;
	CGameObjectArrayElement *pElement;

	CGenericObject *pObject = NULL;
	int n = 0;
	lua_newtable(L);

	for (int i = 0; i <= 0xFFF; i++) {

		pElement = pObjects[i];
		while (pElement){

			pObject = (CGenericObject *)pElement->Object;

			if (pObject){

				char buf[12];
				sprintf(buf, "%x", pObject->obj_id);
				lua_pushstring(L, buf);
				lua_rawseti(L, -2, ++n);
			}

			pElement = (CGameObjectArrayElement *)pElement->field_8;
		}
	}

	return 1;
}

int NWNX_GetAllAreas(lua_State *L){

	CNWSModule * mod = (*NWN_AppManager)->app_server->srv_internal->GetModule();
	
	lua_newtable(L);
	
	int i = 0;

	for (int n = 0; n < mod->mod_areas_len; n++){
		char buf[12];
		sprintf(buf, "%x", mod->mod_areas[n]);
		lua_pushstring(L, buf);
		lua_rawseti(L, -2, ++i);
	}

	return 1;
}

int NWNX_GetItemParts(lua_State *L){

	nwn_objid_t ID;
	const char *item = luaL_checkstring(L, 1);
	sscanf(item, "%x", &ID);

	lua_pop(L, lua_gettop(L));

	if (ID == OBJECT_INVALID){
		lua_pushnil(L);
		return 1;
	}

	CNWSItem * itm = (*NWN_AppManager)->app_server->srv_internal->GetItemByGameObjectID(ID);
	if (itm == NULL)
	{
		lua_pushnil(L);
		return 1;
	}

	lua_newtable(L);

	int part;
	int i = 0;

	for (int n = 0; n < 19; n++){
		
		part = (int)itm->ArmorParts[n];
		lua_pushinteger(L,part);
		lua_rawseti(L, -2, ++i);
	}

	lua_pushstring(L, "models");
	lua_newtable(L);

	lua_pushinteger(L, 1);
	lua_pushinteger(L, itm->ModelPart1);
	lua_settable(L, -3);

	lua_pushinteger(L, 2);
	lua_pushinteger(L, itm->ModelPart2);
	lua_settable(L, -3);

	lua_pushinteger(L, 3);
	lua_pushinteger(L, itm->ModelPart3);
	lua_settable(L, -3);

	lua_settable(L, -3);

	return 1;
}

int NWNX_SetItemPart(lua_State *L){

	nwn_objid_t ID;
	const char *item = luaL_checkstring(L, 1);
	sscanf(item, "%x", &ID);

	int index = lua_tointeger(L, 2);
	int newpart = lua_tointeger(L, 3);

	if (ID == OBJECT_INVALID){		
		return 0;
	}

	CNWSItem * itm = (*NWN_AppManager)->app_server->srv_internal->GetItemByGameObjectID(ID);
	if (itm == NULL)
	{
		return 0;
	}

	if (index>=1 && index <= 19)
		itm->UArmorParts[index - 1] = newpart;

	return 0;
}

int NWNX_SetItemModel(lua_State *L){

	nwn_objid_t ID;
	const char *item = luaL_checkstring(L, 1);
	sscanf(item, "%x", &ID);

	int index = lua_tointeger(L, 2);
	int newpart = lua_tointeger(L, 3);

	if (ID == OBJECT_INVALID){
		return 0;
	}

	CNWSItem * itm = (*NWN_AppManager)->app_server->srv_internal->GetItemByGameObjectID(ID);
	if (itm == NULL)
	{
		return 0;
	}

	switch (index){
		case 1: itm->ModelPart1 = newpart; break;
		case 2: itm->ModelPart2 = newpart; break;
		case 3: itm->ModelPart3 = newpart; break;
		default:return 0;
	}

	return 0;
}

int NWNX_GetAreasByResRef(lua_State *L){

	char resref[17];
	char comp[17];

	const char * ref = luaL_checkstring(L, 1);
	lua_pop(L, 1);

	strncpy(resref,ref,16);
	resref[16] = 0;
	comp[16] = 0;

	char cur;
	for (int n = 0; n < 16; n++){
		cur = resref[n];
		if (cur == '\0')
			break;
		else
			resref[n] = tolower(cur);
	}

	CNWSModule * mod = (*NWN_AppManager)->app_server->srv_internal->GetModule();
	CNWSArea * are;

	lua_newtable(L);

	int i = 0;

	for (int n = 0; n < mod->mod_areas_len; n++){

		are = mod->GetArea(mod->mod_areas[n]);

		if (are){

			strncpy(comp, (char*)are + 180, 16);

			for (int j = 0; j < 16; j++){
				cur = comp[j];
				if (cur == '\0')
					break;
				else
					comp[j] = tolower(cur);
			}

			if (strcmp(resref, comp) == 0){
				char buf[12];
				sprintf(buf, "%x", mod->mod_areas[n]);
				lua_pushstring(L, buf);
				lua_rawseti(L, -2, ++i);
			}
		}
	}

	return 1;
}

int NWNX_PossessCreature(lua_State *L){

	nwn_objid_t ID;
	nwn_objid_t TRG;
	const char *pc = luaL_checkstring(L, 1);
	const char *trg = luaL_checkstring(L, 2);
	sscanf(pc, "%x", &ID);
	sscanf(trg, "%x", &TRG);

	lua_pop(L, lua_gettop(L));

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	CNWSCreature * target = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(TRG);

	if (!cre || !target || ID == OBJECT_INVALID || TRG == OBJECT_INVALID){
		return 0;
	}

	cre->PossessCreatureDM(TRG,lua_tointeger(L,3));

	return 0;
}

int NWNX_UnPossessCreature(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	lua_pop(L, lua_gettop(L));

	if (!cre || ID == OBJECT_INVALID){
		return 0;
	}

	cre->UnpossessCreatureDM();

	return 0;
}

int NWNX_SetAssociate(lua_State *L){

	nwn_objid_t ID;
	nwn_objid_t TRG;
	const char *pc = luaL_checkstring(L, 1);
	const char *trg = luaL_checkstring(L, 2);
	sscanf(pc, "%x", &ID);
	sscanf(trg, "%x", &TRG);
	int type = lua_tointeger(L,3);

	lua_pop(L, lua_gettop(L));

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	CNWSCreature * target = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(TRG);

	if (!cre || !target || ID == OBJECT_INVALID || TRG == OBJECT_INVALID){
		return 0;
	}

	if (type < 0){
		cre->RemoveToAssociateList(TRG);
		target->SetAssociateType(0);
		cre->RemoveAssociate(TRG);
	}
	else{
		target->SetAssociateType(type);
		cre->AddAssociate(TRG, type);
		cre->AddToAssociateList(TRG);
	}

	return 0;
}

int NWNX_GetFeatUses(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);
	int feat = lua_tointeger(L, 2);

	lua_pop(L, lua_gettop(L));

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if (ID == OBJECT_INVALID || !cre){
		lua_pushinteger(L, -1);
		return 1;
	}

	if (!cre->cre_stats->HasFeat(feat)){
		lua_pushinteger(L, -1);
		return 1;
	}
	
	int uses = cre->cre_stats->GetFeatRemainingUses(feat);
	
	lua_pushinteger(L, uses);

	return 1;
}

int NWNX_GetSetBodyBag(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if (ID == OBJECT_INVALID || !cre){
		lua_pushinteger(L, -1);
		return 1;
	}

	int bodybag = cre->cre_bodybag;
	if (lua_type(L, 2) == LUA_TNUMBER){
		cre->cre_bodybag = lua_tointeger(L, 2);
	}
	
	lua_pop(L, lua_gettop(L));
	lua_pushinteger(L, bodybag);

	return 1;
}

int NWNX_GetAnimalCompanion(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if (ID == OBJECT_INVALID || !cre){
		lua_pushinteger(L, -1);
		lua_pushstring(L, "");
		return 2;
	}

	lua_pushinteger(L, cre->cre_stats->cs_acomp_type);
	
	if (cre->cre_stats->cs_acomp_name.text){
		lua_pushstring(L, cre->cre_stats->cs_acomp_name.text);
	}
	else{
		lua_pushstring(L, "");
	}
	
	return 2;
}

int NWNX_SetAnimalCompanion(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);
	int type = lua_tointeger(L, 2);
	size_t len;
	const char * name = luaL_tolstring(L, 3, &len);

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if (ID == OBJECT_INVALID || !cre){
		return 0;
	}

	cre->cre_stats->cs_acomp_type = type;

	if (cre->cre_stats->cs_acomp_name.text){
		lua.mem.nwnx_free(cre->cre_stats->cs_acomp_name.text);
		cre->cre_stats->cs_acomp_name.text = NULL;
	}
	
	cre->cre_stats->cs_acomp_name.text = (char*)lua.mem.nwnx_malloc(len+1);
	strcpy(cre->cre_stats->cs_acomp_name.text, name);
	cre->cre_stats->cs_acomp_name.len = len;

	return 0;
}

int NWNX_GetFamiliar(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if (ID == OBJECT_INVALID || !cre){
		lua_pushinteger(L, -1);
		lua_pushstring(L, "");
		return 2;
	}

	lua_pushinteger(L, cre->cre_stats->cs_famil_type);

	if (cre->cre_stats->cs_famil_name.text){
		lua_pushstring(L, cre->cre_stats->cs_famil_name.text);
	}
	else{
		lua_pushstring(L, "");
	}

	return 2;
}

int NWNX_SetFamiliar(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);
	int type = lua_tointeger(L, 2);
	size_t len;
	const char * name = luaL_tolstring(L, 3, &len);

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if (ID == OBJECT_INVALID || !cre){
		return 0;
	}

	cre->cre_stats->cs_famil_type = type;

	if (cre->cre_stats->cs_famil_name.text){
		lua.mem.nwnx_free(cre->cre_stats->cs_famil_name.text);
		cre->cre_stats->cs_famil_name.text = NULL;
	}

	cre->cre_stats->cs_famil_name.text = (char*)lua.mem.nwnx_malloc(len + 1);
	strcpy(cre->cre_stats->cs_famil_name.text, name);
	cre->cre_stats->cs_famil_name.len = len;

	return 0;
}

int NWNX_BecomeCreature(lua_State *L){

	nwn_objid_t ID;
	nwn_objid_t TRG;
	const char *pc = luaL_checkstring(L, 1);
	const char *trg = luaL_checkstring(L, 2);
	sscanf(pc, "%x", &ID);
	sscanf(trg, "%x", &TRG);

	lua_pop(L, lua_gettop(L));

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	CNWSCreature * target = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(TRG);

	if (!cre || !target || ID == OBJECT_INVALID || TRG == OBJECT_INVALID){
		return 0;
	}

	cre->PossessCreature(TRG);

	DWORD temppc = cre->cre_is_pc;
	DWORD temp1 = *(DWORD *)(target + 0xC64);
	DWORD temp2 = *(DWORD *)(target + 0x78);

	target->cre_is_pc = cre->cre_is_pc;
	*(DWORD *)(target + 0xC64) = *(DWORD *)(cre + 0xC64);
	*(DWORD *)(target + 0x78) = *(DWORD *)(cre + 0xC64);

	cre->cre_is_pc = temppc;
	*(DWORD *)(cre + 0xC64) = temp1;
	*(DWORD *)(cre + 0xC64) = temp2;

	return 0;
}

int NWNX_GetObjectType(lua_State *L){

	nwn_objid_t TRG;
	const char *trg = luaL_checkstring(L, 1);
	sscanf(trg, "%x", &TRG);
	lua_pop(L, 1);
	CGenericObject * obj = (*NWN_AppManager)->app_server->srv_internal->GetGameObject(TRG);
	if (!obj){
		lua_pushinteger(L, -1);
		return 1;
	}

	lua_pushinteger(L, obj->obj_type2);
	return 1;
}

int NWNX_AddSpellLikeAbility(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	if (!cre || ID == OBJECT_INVALID){
		return 0;
	}

	SpecialAbility sa;

	sa.Spell = lua_tointeger(L, 2);
	sa.Level = (char)lua_tointeger(L, 3);
	sa.Ready = lua_toboolean(L, 4);

	lua_pop(L, lua_gettop(L));

	NWNXSpecialAbilities * Repository = (NWNXSpecialAbilities*)cre->cre_stats->cs_specabil;
	Repository->Insert(sa);

	return 0;
}

int NWNX_GetSpellLikeAbility(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);
	int index = lua_tointeger(L, 2);

	lua_pop(L, lua_gettop(L));

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	if (!cre || ID == OBJECT_INVALID || index < 0){
		lua_pushnil(L);
		lua_pushnil(L);
		lua_pushnil(L);
		return 3;
	}

	NWNXSpecialAbilities * Repository = (NWNXSpecialAbilities*)cre->cre_stats->cs_specabil;

	if (index>=Repository->Len){
		lua_pushnil(L);
		lua_pushnil(L);
		lua_pushnil(L);
	}
	else{
		lua_pushinteger(L, Repository->Array[index].Spell);
		lua_pushinteger(L, Repository->Array[index].Level);
		lua_pushinteger(L, Repository->Array[index].Ready);
	}

	return 3;
}

int NWNX_RemoveSpellLikeAbility(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);
	int index = lua_tointeger(L, 2);

	lua_pop(L, lua_gettop(L));

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);

	if (!cre || ID == OBJECT_INVALID || index < 0){
		return 0;
	}

	NWNXSpecialAbilities * Repository = (NWNXSpecialAbilities*)cre->cre_stats->cs_specabil;

	if (index < Repository->Len){
		Repository->Remove(index);
	}
	
	return 0;
}

int NWNX_GetFileInfo(lua_State *L){

	WIN32_FIND_DATA data;

	if (get_file_information(luaL_checkstring(L, 1), &data)){

		lua_pop(L, 1);

		lua_newtable(L);

		lua_pushstring(L, "isFolder");
		lua_pushboolean(L, data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		lua_settable(L, -3);

		lua_pushstring(L, "Size");
		lua_pushinteger(L, data.nFileSizeLow);
		lua_settable(L, -3);

		lua_pushstring(L, "Creation");
		lua_pushinteger(L, FILETIME_to_time_t(&data.ftCreationTime));
		lua_settable(L, -3);

		lua_pushstring(L, "Access");
		lua_pushinteger(L, FILETIME_to_time_t(&data.ftLastAccessTime));
		lua_settable(L, -3);

		lua_pushstring(L, "Write");
		lua_pushinteger(L, FILETIME_to_time_t(&data.ftLastWriteTime));
		lua_settable(L, -3);
	}
	else{
		lua_pop(L, 1);
		lua_pushnil(L);
	}

	return 1;
}

int NWNX_CopyFile(lua_State *L){

	const char * src = luaL_checkstring(L, 1);
	const char * dst = luaL_checkstring(L, 2);

	BOOL ret = CopyFile(src, dst, lua_toboolean(L, 3));

	lua_pop(L,lua_gettop(L));
	lua_pushboolean(L,ret);
	
	return 1;
}

int NWNX_MoveFile(lua_State *L){

	const char * src = luaL_checkstring(L, 1);
	const char * dst = luaL_checkstring(L, 2);

	BOOL ret = MoveFile(src, dst);

	lua_pop(L, lua_gettop(L));
	lua_pushboolean(L,ret);

	return 1;
}

int NWNX_DeleteFile(lua_State *L){

	const char * src = luaL_checkstring(L, 1);

	BOOL ret = DeleteFile(src);

	lua_pop(L, lua_gettop(L));
	lua_pushboolean(L, ret);

	return 1;
}

int NWNX_CreateDirectory(lua_State *L){

	const char * src = luaL_checkstring(L, 1);

	BOOL ret = CreateDirectory(src,NULL);

	lua_pop(L, lua_gettop(L));
	lua_pushboolean(L, ret);

	return 1;
}

int NWNX_RemoveDirectory(lua_State *L){

	const char * src = luaL_checkstring(L, 1);

	BOOL ret = RemoveDirectory(src);

	lua_pop(L, lua_gettop(L));
	lua_pushboolean(L, ret);

	return 1;
}

int NWNX_Rename(lua_State *L){

	const char * src = luaL_checkstring(L, 1);
	const char * dst = luaL_checkstring(L, 2);

	BOOL ret = rename(src, dst);

	lua_pop(L, lua_gettop(L));
	lua_pushboolean(L, ret);

	return 1;
}

int NWNX_SetCreatureEventHandlerFunction(lua_State *L){

	if (lua_type(L, 1) != LUA_TFUNCTION){
		lua_pop(L, 1);
		luaL_error(L, "no valid function given!");
		return 0;
	}

	if (lua.creaturehandlerfunc == -1){
		ApplyEventHandlerHook();
	}
	else{
		luaL_unref(L, LUA_REGISTRYINDEX, lua.creaturehandlerfunc);
	}

	lua.creaturehandlerfunc = luaL_ref(L, LUA_REGISTRYINDEX);

	return 0;
}

int NWNX_GetEventName(lua_State *L){
	
	int event = lua_tointeger(L, 1);
	lua_pop(L,1);
	lua.PushEventString(event);
	return 1;
}

int NWNX_AddKnownSpell(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	int classpos = lua_tointeger(L, 2);
	if (classpos < 1 || classpos>3){
		luaL_error(L, "Class position must be 1-3");
		return 0;
	}

	int spelllevel = lua_tointeger(L, 3);
	if (spelllevel < 0 || spelllevel>9){
		luaL_error(L, "Spell level position must be 0-9");
		return 0;
	}

	int spellid = lua_tointeger(L, 4);
	lua_pop(L, lua_gettop(L));

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if (!cre || ID == OBJECT_INVALID){	
		return 0;
	}

	if (classpos - 1 >= cre->cre_stats->cs_classes_len)
		return 0;

	CNWSCreatureClass * cla = &cre->cre_stats->cs_classes[classpos - 1];

	CExoArrayList_uint32 * list = (CExoArrayList_uint32*)&cla->cl_spells_known[spelllevel];

	if (CExoArrayList_uint32_contains(list, spellid) == -1)
		CExoArrayList_uint32___Add(list, NULL, spellid);

	return 0;
}

int NWNX_RemoveKnownSpell(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	int classpos = lua_tointeger(L, 2);
	if (classpos < 1 || classpos>3){
		luaL_error(L, "Class position must be 1-3");
		return 0;
	}

	int spelllevel = lua_tointeger(L, 3);
	if (spelllevel < 0 || spelllevel>9){
		luaL_error(L, "Spell level position must be 0-9");
		return 0;
	}

	int spellid = lua_tointeger(L, 4);
	lua_pop(L, lua_gettop(L));

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if (!cre || ID == OBJECT_INVALID){
		return 0;
	}

	if (classpos - 1 >= cre->cre_stats->cs_classes_len)
		return 0;

	CNWSCreatureClass * cla = &cre->cre_stats->cs_classes[classpos - 1];

	CExoArrayList_uint32 * list = (CExoArrayList_uint32*)&cla->cl_spells_known[spelllevel];

	if (CExoArrayList_uint32_contains(list, spellid) == -1)
		CExoArrayList_uint32___Remove(list, NULL, spellid);

	return 0;
}

int NWNX_GetSpellSlots(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	int classpos = lua_tointeger(L, 2);
	if (classpos < 1 || classpos>3){
		luaL_error(L, "Class position must be 1-3");
		return 0;
	}

	int spelllevel = lua_tointeger(L, 3);
	if (spelllevel < 0 || spelllevel>9){
		luaL_error(L, "Spell level position must be 0-9");
		return 0;
	}

	lua_pop(L, lua_gettop(L));

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if (!cre || ID == OBJECT_INVALID){
		lua_pushinteger(L, -1);
		return 1;
	}

	if (classpos - 1 >= cre->cre_stats->cs_classes_len){
		lua_pushinteger(L, -1);
		return 1;
	}

	CNWSCreatureClass * cla = &cre->cre_stats->cs_classes[classpos - 1];

	CExoArrayList_uint32 * list = (CExoArrayList_uint32*)&cla->cl_spells_mem[spelllevel];
	
	lua_newtable(L);

	for (int n = 0; n < list->len; n++){
		
		if (list->data[n])
			lua_pushinteger(L, ((CNWSStats_Spell*)list->data[n])->sp_id);
		else
			lua_pushinteger(L, -1);
		lua_rawseti(L, -2, n+1);
	}

	return 1;
}

int NWNX_SetSpellSlots(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	int classpos = lua_tointeger(L, 2);
	if (classpos < 1 || classpos>3){
		luaL_error(L, "Class position must be 1-3");
		return 0;
	}

	int spelllevel = lua_tointeger(L, 3);
	if (spelllevel < 0 || spelllevel>9){
		luaL_error(L, "Spell level position must be 0-9");
		return 0;
	}

	int slots = lua_tointeger(L, 4);
	lua_pop(L, lua_gettop(L));

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if (!cre || ID == OBJECT_INVALID){
		lua_pushinteger(L, -1);
		return 1;
	}

	if (classpos - 1 >= cre->cre_stats->cs_classes_len){
		lua_pushinteger(L, -1);
		return 1;
	}

	CNWSCreatureClass * cla = &cre->cre_stats->cs_classes[classpos - 1];

	CExoArrayList_uint32 * list = (CExoArrayList_uint32*)&cla->cl_spells_mem[spelllevel];

	if (slots <= 0 && list->data){
		lua.mem.nwnx_free(list->data);
		list->data = NULL;
		list->len = 0;
		list->alloc = 0;
	}
	else if (slots <= list->len){
		list->len = slots;
	}
	else{
		void * temp = lua.mem.nwnx_malloc(slots*sizeof(DWORD));
		if (!temp){
			lua_pushinteger(L, -1);
			return 1;
		}

		if (list->data){
			memset(temp, NULL, slots*sizeof(DWORD));
			memcpy(temp, list->data,list->len*sizeof(DWORD));
			lua.mem.nwnx_free(list->data);
		}

		list->data = (uint32_t*)temp;
		list->alloc = slots;
		list->len = slots;
	}

	return 1;
}

int NWNX_GetSpellInSlot(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	int classpos = lua_tointeger(L, 2);
	if (classpos < 1 || classpos>3){
		luaL_error(L, "Class position must be 1-3");
		return 0;
	}

	int spelllevel = lua_tointeger(L, 3);
	if (spelllevel < 0 || spelllevel>9){
		luaL_error(L, "Spell level position must be 0-9");
		return 0;
	}

	int spellslot = lua_tointeger(L, 4);

	lua_pop(L, lua_gettop(L));

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if (!cre || ID == OBJECT_INVALID){
		lua_pushinteger(L, -1);
		return 1;
	}

	if (classpos - 1 >= cre->cre_stats->cs_classes_len){
		lua_pushinteger(L, -1);
		return 1;
	}

	CNWSCreatureClass * cla = &cre->cre_stats->cs_classes[classpos - 1];

	CExoArrayList_uint32 * list = (CExoArrayList_uint32*)&cla->cl_spells_mem[spelllevel];

	if (spellslot >= list->len || spellslot<0|| !list->data[spellslot]){
		lua_pushnil(L);
	}
	else{

		CNWSStats_Spell * spell = (CNWSStats_Spell *)list->data[spellslot];

		lua_newtable(L);

		lua_pushstring(L, "id");
		lua_pushinteger(L, spell->sp_id);
		lua_settable(L, -3);

		lua_pushstring(L, "ready");
		lua_pushboolean(L, spell->sp_ready>0);
		lua_settable(L, -3);

		lua_pushstring(L, "meta");
		lua_pushinteger(L, spell->sp_meta);
		lua_settable(L, -3);

		lua_pushstring(L, "domain");
		lua_pushinteger(L, spell->sp_domain);
		lua_settable(L, -3);
	}

	return 1;
}

int NWNX_SetSpellInSlot(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	int classpos = lua_tointeger(L, 2);
	if (classpos < 1 || classpos>3){
		luaL_error(L, "Class position must be 1-3");
		return 0;
	}

	int spelllevel = lua_tointeger(L, 3);
	if (spelllevel < 0 || spelllevel>9){
		luaL_error(L, "Spell level position must be 0-9");
		return 0;
	}

	int spellslot = lua_tointeger(L, 4);

	lua_pop(L, 4);

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if (!cre || ID == OBJECT_INVALID){
		return 0;
	}

	if (classpos - 1 >= cre->cre_stats->cs_classes_len){
		return 0;
	}

	CNWSCreatureClass * cla = &cre->cre_stats->cs_classes[classpos - 1];

	CExoArrayList_uint32 * list = (CExoArrayList_uint32*)&cla->cl_spells_mem[spelllevel];

	if (spellslot >= list->len || spellslot<0){
		return 0;
	}
	else if(lua_type(L,1)==LUA_TTABLE){

		CNWSStats_Spell * spell = (CNWSStats_Spell *)lua.mem.nwnx_malloc(sizeof(CNWSStats_Spell));

		lua_pushstring(L, "id");
		lua_gettable(L, -2);

		if (lua_type(L, 1) == LUA_TNUMBER)
			spell->sp_id = lua_tointeger(L, 1);
		else
			spell->sp_id = -1;

		lua_pop(L, 1);

		if (spell->sp_id < 0)
			spell->sp_id = -1;

		lua_pushstring(L, "ready");
		lua_gettable(L, -2);

		if (lua_type(L, 1) == LUA_TBOOLEAN)
			spell->sp_ready = lua_toboolean(L, 1);
		else
			spell->sp_ready = 0;
		lua_pop(L, 1);

		lua_pushstring(L, "meta");
		lua_gettable(L, -2);

		if (lua_type(L, 1) == LUA_TNUMBER)
			spell->sp_meta = lua_tointeger(L, 1);
		else
			spell->sp_meta = 0;
		lua_pop(L, 1);

		lua_pushstring(L, "domain");
		lua_gettable(L, -2);

		if (lua_type(L, 1) == LUA_TNUMBER)
			spell->sp_domain = lua_tointeger(L, 1);
		else
			spell->sp_domain = 0;

		lua_pop(L, 2);

		lua.Log("o Spell %u %i %i %i\n",spell->sp_id,spell->sp_ready,spell->sp_meta,spell->sp_ready);

		if (spell->sp_id >= 0){

			if (list->data[spellslot])
				lua.mem.nwnx_free((void*)list->data[spellslot]);

			list->data[spellslot] = (uint32_t)spell;
		}
		else{
			lua.mem.nwnx_free(spell);
		}
	}
	else{
		if (list->data[spellslot])
			lua.mem.nwnx_free((void*)list->data[spellslot]);
		list->data[spellslot] = NULL;
	}

	return 0;
}

int NWNX_GetSetSpellsPerDay(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	int classpos = lua_tointeger(L, 2);
	if (classpos < 1 || classpos>3){
		luaL_error(L, "Class position must be 1-3");
		return 0;
	}

	int spelllevel = lua_tointeger(L, 3);
	if (spelllevel < 0 || spelllevel>9){
		luaL_error(L, "Spell level position must be 0-9");
		return 0;
	}

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if (!cre || ID == OBJECT_INVALID){
		lua_pop(L, lua_gettop(L));
		return 0;
	}

	if (classpos - 1 >= cre->cre_stats->cs_classes_len){
		lua_pop(L, lua_gettop(L));
		return 0;
	}

	CNWSCreatureClass * cla = &cre->cre_stats->cs_classes[classpos - 1];

	int ret = cla->cl_spells_max[spelllevel];

	if (lua_type(L, 4) == LUA_TNUMBER){
		cla->cl_spells_max[spelllevel] = (uint8_t)lua_tointeger(L,4);
	}

	lua_pop(L, lua_gettop(L));

	lua_pushinteger(L, ret);
	return 1;
}

int NWNX_GetSetSpellsAvailable(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	int classpos = lua_tointeger(L, 2);
	if (classpos < 1 || classpos>3){
		luaL_error(L, "Class position must be 1-3");
		return 0;
	}

	int spelllevel = lua_tointeger(L, 3);
	if (spelllevel < 0 || spelllevel>9){
		luaL_error(L, "Spell level position must be 0-9");
		return 0;
	}

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if (!cre || ID == OBJECT_INVALID){
		lua_pop(L, lua_gettop(L));
		return 0;
	}

	if (classpos - 1 >= cre->cre_stats->cs_classes_len){
		lua_pop(L, lua_gettop(L));
		return 0;
	}

	CNWSCreatureClass * cla = &cre->cre_stats->cs_classes[classpos - 1];

	int ret = cla->cl_spells_left[spelllevel];

	if (lua_type(L, 4) == LUA_TNUMBER){
		cla->cl_spells_left[spelllevel] = (uint8_t)lua_tointeger(L, 4);
	}

	lua_pop(L, lua_gettop(L));

	lua_pushinteger(L, ret);
	return 1;
}

int NWNX_GetSetSpellsBonus(lua_State *L){

	nwn_objid_t ID;
	const char *pc = luaL_checkstring(L, 1);
	sscanf(pc, "%x", &ID);

	int classpos = lua_tointeger(L, 2);
	if (classpos < 1 || classpos>3){
		luaL_error(L, "Class position must be 1-3");
		return 0;
	}

	int spelllevel = lua_tointeger(L, 3);
	if (spelllevel < 0 || spelllevel>9){
		luaL_error(L, "Spell level position must be 0-9");
		return 0;
	}

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID(ID);
	if (!cre || ID == OBJECT_INVALID){
		lua_pop(L, lua_gettop(L));
		return 0;
	}

	if (classpos - 1 >= cre->cre_stats->cs_classes_len){
		lua_pop(L, lua_gettop(L));
		return 0;
	}

	CNWSCreatureClass * cla = &cre->cre_stats->cs_classes[classpos - 1];

	int ret = cla->cl_spells_bonus[spelllevel];

	if (lua_type(L, 4) == LUA_TNUMBER){
		cla->cl_spells_bonus[spelllevel] = (uint8_t)lua_tointeger(L, 4);
	}

	lua_pop(L, lua_gettop(L));

	lua_pushinteger(L, ret);
	return 1;
}

int NWNX_SetCanEquipFunction(lua_State *L){

	if (lua_type(L, 1) != LUA_TFUNCTION){
		lua_pop(L, 1);
		luaL_error(L, "no valid function given!");
		return 0;
	}

	if (lua.canequipfunc == -1){
		ApplyCanEquipHook();
	}
	else{
		luaL_unref(L, LUA_REGISTRYINDEX, lua.canequipfunc);
	}

	lua.canequipfunc = luaL_ref(L, LUA_REGISTRYINDEX);

	return 0;
}

int NWNX_SetGetWeightFunction(lua_State *L){

	if (lua_type(L, 1) != LUA_TFUNCTION){
		lua_pop(L, 1);
		luaL_error(L, "no valid function given!");
		return 0;
	}

	if (lua.weightfunc == -1){
		ApplyWeightHook();
	}
	else{
		luaL_unref(L, LUA_REGISTRYINDEX, lua.weightfunc);
	}

	lua.weightfunc = luaL_ref(L, LUA_REGISTRYINDEX);

	return 0;
}

int NWNX_SetGetCostFunction(lua_State *L){

	if (lua_type(L, 1) != LUA_TFUNCTION){
		lua_pop(L, 1);
		luaL_error(L, "no valid function given!");
		return 0;
	}

	if (lua.costfunc == -1){
		ApplyCostHook();
	}
	else{
		luaL_unref(L, LUA_REGISTRYINDEX, lua.costfunc);
	}

	lua.costfunc = luaL_ref(L, LUA_REGISTRYINDEX);

	return 0;
}

void RegisterNWNLib( lua_State *L ){
	
	char buf[12];
	sprintf(buf, "%x", OBJECT_INVALID);
	lua_pushstring(L, buf);
	lua_setglobal(L, "OBJECT_INVALID");
	
	//Override printNWNX_ExecuteScript
	lua_register( L, "print", NWNX_Print );
	
	lua.Log( "Additional nwn functions: (useage nwn.funcname)\n\n" );

	register_c_function(L, "nwn", "MainLoopHook", NWNX_RegisterMainLoopFunc);
	lua.Log("void MainLoopHook( function ) - hooks nwn's mainloop and runs the function every frame. void func()\n");

	register_c_function(L, "nwn", "SetGetWeightFunction", NWNX_SetGetWeightFunction);
	lua.Log("void SetGetWeightFunction( function ) - Set a function to evaluate the weight on an item. int func(item). Return a number to override or nil for default behavior\n");

	register_c_function(L, "nwn", "SetGetCostFunction", NWNX_SetGetCostFunction);
	lua.Log("void SetGetCostFunction( function ) - Set a function to evaluate the cost on an item. int func(item). Return a number to override or nil for default behavior\n");

	register_c_function(L, "nwn", "SetCanEquipFunction", NWNX_SetCanEquipFunction);
	lua.Log("void SetCanEquipFunction( function ) - Causes the engine to evalue the given lua function to check if an item is equipable. bool func(target,item,slotid); Return nil for standard behavior\n");

	register_c_function(L, "nwn", "SetEventHandler", NWNX_SetCreatureEventHandlerFunction);
	lua.Log("void SetEventHandler( function ) - Causes the event-handler to run this lua function: void func(object_self,target,event, subdata)\n");

	register_c_function(L, "nwn", "GetEventName", NWNX_GetEventName);
	lua.Log("string GetEventName( event ) - Get the name of the event, helper function to the event handler\n");

	register_c_function(L, "nwn", "TestVisibilityHook", NWNX_SetTestVisibilityFunction);
	lua.Log("void TestVisibilityHook( function ) - Adds a function to evaluate visibility. bool func(object, target), if the function returns a non bool normal nwn behavior is retained\n");

	register_c_function(L, "nwn", "GetSetSpellsPerDay", NWNX_GetSetSpellsPerDay);
	lua.Log("int GetSetSpellsPerDay(creature,class,spelllevel,new) - Get the number of spells available, if new is not nil the spells per day is instead set\n");

	register_c_function(L, "nwn", "GetSetSpellsAvailable", NWNX_GetSetSpellsAvailable);
	lua.Log("int GetSetSpellsAvailable(creature,class,spelllevel,new) - Get the number of spells available, if new is not nil the spells ready to cast is instead set\n");

	register_c_function(L, "nwn", "GetSetSpellsBonus", NWNX_GetSetSpellsBonus);
	lua.Log("int GetSetSpellsBonus(creature,class,spelllevel,new) - Get the number of spells available, if new is not nil the spells as bonus is instead set\n");

	register_c_function(L, "nwn", "SetSpellInSlot", NWNX_SetSpellInSlot);
	lua.Log("void SetSpellInSlot(creature,class,spelllevel,slot, table) - Set a spell in a slot, table should contain (id,ready,meta,domain), if table is nil the spellslot is emptied\n");

	register_c_function(L, "nwn", "GetSpellInSlot", NWNX_GetSpellInSlot);
	lua.Log("table GetSpellInSlot(creature,class,spelllevel,slot) - Returns a table containg the spell info from the slot (id,ready,meta,domain) or nil if invalid\n");

	register_c_function(L, "nwn", "SetSpellSlots", NWNX_SetSpellSlots);
	lua.Log("void SetSpellSlots(creature,class,spelllevel,slots) - Sets the amount of spell-slots creature should have\n");

	register_c_function(L, "nwn", "GetSpellSlots", NWNX_GetSpellSlots);
	lua.Log("table GetSpellSlots(creature,class,spelllevel) - Returns a list containing the spells memorized for that spell-level (-1 = empty slot)\n");

	register_c_function(L, "nwn", "RemoveKnownSpell", NWNX_RemoveKnownSpell);
	lua.Log("void RemoveKnownSpell(creature,class,spelllevel,spellid) - Removes a known spell to creature\n");

	register_c_function(L, "nwn", "AddKnownSpell", NWNX_AddKnownSpell);
	lua.Log("void AddKnownSpell(creature,class,spelllevel,spellid) - Adds a known spell to creature\n");

	register_c_function(L, "nwn", "RemoveDirectory", NWNX_RemoveDirectory);
	lua.Log("bool RemoveDirectory(src) - Remove a directory\n");

	register_c_function(L, "nwn", "CreateDirectory", NWNX_CreateDirectory);
	lua.Log("bool CreateDirectory(src) - Create a directory\n");

	register_c_function(L, "nwn", "DeleteFile", NWNX_DeleteFile);
	lua.Log("bool DeleteFile(src) - Delete source\n");

	register_c_function(L, "nwn", "RenameFile", NWNX_Rename);
	lua.Log("bool RenameFile(old,new) - Renames old to new\n");

	register_c_function(L, "nwn", "MoveFile", NWNX_MoveFile);
	lua.Log("bool MoveFile(src,dst) - moves source to destination\n");

	register_c_function(L, "nwn", "CopyFile", NWNX_CopyFile);
	lua.Log("bool CopyFile(src,dst,failifexists) - Copy sorce file to the destination, if failifexists is true it'll fail if the file exists\n");

	register_c_function(L, "nwn", "GetFileInfo", NWNX_GetFileInfo);
	lua.Log("table GetFileInfo(file) - returns file info about the file (or nil) isFolder, Creation, Access, Write, Size\n");

	register_c_function(L, "nwn", "GetObjectType", NWNX_GetObjectType);
	lua.Log("type GetObjectType(object) - returns the objecttype of object\n");

	register_c_function(L, "nwn", "AddSpellLikeAbility", NWNX_AddSpellLikeAbility);
	lua.Log("void AddSpellLikeAbility(creature,spell,level,ready) - Adds a spell-like ability to creature\n");

	register_c_function(L, "nwn", "GetSpellLikeAbility", NWNX_GetSpellLikeAbility);
	lua.Log("spell,level,ready GetSpellLikeAbility(creature,index) - returns a spell-like ability from creature at index, if not valid it returns nil\n");

	register_c_function(L, "nwn", "RemoveSpellLikeAbility", NWNX_RemoveSpellLikeAbility);
	lua.Log("void RemoveSpellLikeAbility(creature,index) - Removes the spell-like ability from creature at index\n");

	register_c_function(L, "nwn", "GetAnimalCompanion", NWNX_GetAnimalCompanion);
	lua.Log("type, name GetAnimalCompanion(creature) - returns the type and name of the familar the creature has\n");

	register_c_function(L, "nwn", "SetAnimalCompanion", NWNX_SetAnimalCompanion);
	lua.Log("void SetAnimalCompanion(creature,type,name) - Sets creatures familar to type with name\n");

	register_c_function(L, "nwn", "GetFamiliar", NWNX_GetFamiliar);
	lua.Log("type,name GetFamiliar(creature) - Returns the type and name of the familiar creature has\n");

	register_c_function(L, "nwn", "SetFamiliar", NWNX_SetFamiliar);
	lua.Log("void SetFamiliar(creature,type,name) - Sets the type and name of creatures familar\n");

	register_c_function(L, "nwn", "GetSetBodyBag", NWNX_GetSetBodyBag);
	lua.Log("number GetSetBodyBag(creature,newid) - Get the bodybag type of creature, if newid supplied it'll set the bodybag type instead\n");

	register_c_function(L, "nwn", "GetFeatUses", NWNX_GetFeatUses);
	lua.Log("number GetFeatUses(creature,feat) - Get the featuses creature has with feat, returns -1 if the creature does not have the feat\n");

	register_c_function(L, "nwn", "SetAssociate", NWNX_SetAssociate);
	lua.Log("void SetAssociate(creature,target,type) - Adds target as an associate to creature as type, if type is -1 the associate is instead removed\n");

	register_c_function(L, "nwn", "BecomeCreature", NWNX_BecomeCreature);
	lua.Log("void BecomeCreature(player,target) - Target becomes the player's new character\n");

	register_c_function(L, "nwn", "PossessCreature", NWNX_PossessCreature);
	lua.Log("void PossessCreature(player,target,type) - cause player to possess the target as type\n");

	register_c_function(L, "nwn", "Unpossess", NWNX_UnPossessCreature);
	lua.Log("void Unpossess(target) - unpossesses a creature, might not work correctly, instead use PossessCreature in reverse\n");

	register_c_function(L, "nwn", "SetItemPart", NWNX_SetItemPart);
	lua.Log("void SetItemPart(item,index,part) - Set an item part, index must be between 1 and 19. Does not update appearance on the client!\n");

	register_c_function(L, "nwn", "SetItemModel", NWNX_SetItemModel);
	lua.Log("void SetItemModel(item,index,part) - Sets the model of an item, index is valid between 1 and 3. Does not update appearance on the client!\n");

	register_c_function(L, "nwn", "GetItemModelData", NWNX_GetItemParts);
	lua.Log("list.models GetItemModelData( item ) - Returns a list containing all parts (1 indexed) and a subtable containing the models\n");

	register_c_function(L, "nwn", "GetAreasByResRef", NWNX_GetAreasByResRef);
	lua.Log("list GetAreasByResRef( resref ) - returns a list of all areas with resref (not case sensetive)\n");

	register_c_function(L, "nwn", "GetAllAreas", NWNX_GetAllAreas);
	lua.Log("list GetAllAreas( ) - returns a list of all areas\n");

	register_c_function(L, "nwn", "GetAllObjects", NWNX_GetAllObjects);
	lua.Log("list GetAllObjects( ) - returns a list containing all objects that exists\n");

	register_c_function(L, "nwn", "QueryPerformanceCounter", NWNX_QueryPerformanceCounter);
	lua.Log("ticks, tickspersecond QueryPerformanceCounter( asstring ) - returns the current ticks and the ticks per second, if asstring is true it'll return strings\n");

	register_c_function(L, "nwn", "ResetNWScriptTMI", NWNX_ResetNWScriptTMI);
	lua.Log("void ResetNWScriptTMI( ) - Resets nwscript's tmi counter to 0\n");

	register_c_function(L, "nwn", "SetLuaTMI", NWNX_SetInstructionCap);
	lua.Log("void SetLuaTMI( new ) - the tmi is handed in 100.000 units. 1 = 100.000 instructions. Calling this also resets the current count value\n");
	
	register_c_function(L, "nwn", "GetLuaTMI", NWNX_GetInstructionCap);
	lua.Log("int GetLuaTMI( ) - Get the TMI, see SetLuaTMI\n");

	register_c_function(L, "nwn", "GetArcaneSpellFailure", NWNX_GetArcaneFail);
	lua.Log("int GetArcaneSpellFailure( creature ) - Get the arcane spellfailure on the creature\n");

	register_c_function(L, "nwn", "GetFirstNextTURD", NWNX_GetFirstNextTURD);
	lua.Log("int GetFirstNextTURD( first ) - Traverse the turds, if first is true it'll reset and start from the top. Returns nil on fail and id and communityname on success\n");

	register_c_function(L, "nwn", "SetRace", NWNX_SetRace);
	lua.Log("void SetRace( creature, racialtype ) - Set the racial type of a creature\n");

	register_c_function(L, "nwn", "GetBic", NWNX_GetBic);
	lua.Log("string GetBic( creature ) - Get the bicfilecurrently in use by creature (player object)\n");

	register_c_function(L, "nwn", "DeleteTURD", NWNX_DeleteTURD);
	lua.Log("int DeleteTURD( pccreature ) - Delete a TURD (object containing info about PCs not online).\n");

	register_c_function(L, "nwn", "SetGender", NWNX_SetGender);
	lua.Log("void SetGender( creature, newgender ) - Set the gender of a creature.\n");

	register_c_function(L, "nwn", "SetGetDomain", NWNX_SetGetDomain);
	lua.Log("int SetGetDomain( creature, domain, newdomain ) - set the domain (1 or 2) to a new domain or return the current if newdomain is nil. Returns -1 for non clerics, does not mod feats.\n");
	
	register_c_function(L, "nwn", "SetGetSpecialization", NWNX_SetGetSpecialization);
	lua.Log("int SetGetSpecialization( creature, newspec ) - set the wizard spec to a new specilization or return the current if newspec is nil. Returns -1 for non wizards.\n");

	register_c_function(L, "nwn", "GetLevelStat", NWNX_GetLevelStat);
	lua.Log("table GetLevelStat( creature, level ) - returns a table filled in with the info of the level that was requested, nil on failure\n");

	register_c_function(L, "nwn", "AddFeat", NWNX_AddFeat);
	lua.Log("int AddFeat( creature, feat, level ) - adds feat to creature, if level is higher then 0 then its also added to that level in the statlist\n");

	register_c_function(L, "nwn", "RemoveFeat", NWNX_RemoveFeat);
	lua.Log("void RemoveFeat( creature, feat ) - removes feat from creature, it'll also remove it from the statlist if any\n");

	register_c_function(L, "nwn", "GetABVs", NWNX_GetABVs);
	lua.Log("int GetABVs( creature, target ) - returns the AB the creature has vs the target\n" );

	register_c_function(L, "nwn", "GetACVs", NWNX_GetACVs);
	lua.Log("int GetACVs( creature, target ) - returns the AC the creature has vs the target\n");

	register_c_function(L, "nwn", "SetSkillScore", NWNX_SetSkillScore);
	lua.Log("void SetSkillScore( creature, type, number ) - Sets the skill of a creature, type corresponds to SKILL_*, if type is negative it'll set the 'stored' skillpoints\n");

	register_c_function(L, "nwn", "GetStoredSkillPoints", NWNX_GetStoredSkillPoints);
	lua.Log("int GetStoredSkillPoints( creature ) - Get the number of stored/unassigned skillpoints the creature has\n");

	register_c_function(L, "nwn", "SetAbilityScore", NWNX_SetAbilityScore);
	lua.Log("void SetAbilityScore( creature, type, number ) - Set the base ability score for a creature. type corresponds to ABILITY_*\n");

	register_c_function(L, "nwn", "SetGetName", NWNX_SetGetName);
	lua.Log("string SetGetName( creature, type, name ) - Set the first or lastname of a creature. type 1 = lastname, type 0 = firstname. If name is nil it will only return a value\n");

	register_c_function(L, "nwn", "SetGetScript", NWNX_SetGetScript);
	lua.Log("string SetGetScript( creature, type, script ) - sets or gets an eventscript on a creature.\nIf script is nil it will return only. \ntype: 1: onattack 2: onblock 3: ondamage 4: ondeath 5: onconvo 6: ondisturbed 7: onendcombat 8: onheartbeat 9: onperception 10: onrested 11: onspawn 12: onspellcastat 13: userdefined\n");

	register_c_function(L, "nwn", "SetGetMovementRate", NWNX_SetGetMovementRate);
	lua.Log("int SetGetMovementRate( creature, movementratetype ) - sets the movementrate for a creature. nil or 0 will return only. type: 0 PC movement, 1: immobile, 2: very slow, 3:slow, 4: normal, 5: fast, 6: very fast, 7: default, 8: dm fast\n");

	register_c_function(L, "nwn", "SetGetVoiceSet", NWNX_SetGetVoiceSet);
	lua.Log("int SetGetVoiceSet( creature, voiceset ) - sets the voiceset of creature, if voiceset is nil or less then 0 it will return the current without changing\n");

	register_c_function(L, "nwn", "SetBaseDesc", NWNX_SetBaseDesc);
	lua.Log("void SetBaseDesc( creature, text ) - Set the base desc of a creature\n");

	register_c_function( L, "nwn", "SendMSGToAll", NWNX_msgall );
	lua.Log( "void SendMSGToAll( text )\n" );

	register_c_function( L, "nwn", "SendMSGToPC", NWNX_msgpc );
	lua.Log( "void SendMSGToPC( PC, text )\n" );

	register_c_function( L, "nwn", "IsValid", NWNX_IsValidObject );
	lua.Log( "int IsValid( object )\n" );

	register_c_function( L, "nwn", "GetFirstPC", NWNX_GetFirstPC );
	lua.Log( "object GetFirstPC( )\n" );

	register_c_function( L, "nwn", "GetNextPC", NWNX_GetNextPC );
	lua.Log( "object GetNextPC( )\n" );

	register_c_function( L, "nwn", "ExecuteScript", NWNX_ExecuteScript );
	lua.Log( "int ExecuteScript( script, objectself )\n" );

	register_c_function( L, "nwn", "SetLocalString", NWNX_SetLocalString );
	lua.Log( "void SetLocalString( object, varname, data )\n" );

	register_c_function( L, "nwn", "GetLocalString", NWNX_GetLocalString );
	lua.Log( "string GetLocalString( object, varname )\n" );

	register_c_function( L, "nwn", "DeleteLocalString", NWNX_DeleteLocalString );
	lua.Log( "void DeleteLocalString( object, varname )\n" );
	
	register_c_function( L, "nwn", "SetLocalObject", NWNX_SetLocalObject );
	lua.Log( "void SetLocalObject( object, varname, target )\n" );

	register_c_function( L, "nwn", "GetLocalObject", NWNX_GetLocalObject );
	lua.Log( "object GetLocalObject( object, varname )\n" );

	register_c_function( L, "nwn", "DeleteLocalObject", NWNX_DeleteLocalObject );
	lua.Log( "void DeleteLocalObject( object, varname )\n" );

	register_c_function( L, "nwn", "SetLocalInt", NWNX_SetLocalInt );
	lua.Log( "void SetLocalInt( object, varname, int )\n" );

	register_c_function( L, "nwn", "GetLocalInt", NWNX_GetLocalInt );
	lua.Log( "int GetLocalInt( object, varname )\n" );

	register_c_function( L, "nwn", "DeleteLocalInt", NWNX_DeleteLocalInt );
	lua.Log( "void DeleteLocalInt( object, varname )\n" );

	register_c_function( L, "nwn", "SendChatMessage", NWNX_SendChatMessage );
	lua.Log( "int SendChatMessage( receiver, sender, message, channel ) channel = 0:talk 1:whisper 2:party 3:tell 4:shout 5:dm 6: dmtalk 7: dmwhisper\n" );

	register_c_function( L, "nwn", "IntToObject", NWNX_IntToObject );
	lua.Log( "object IntToObject( number )\n" );

	register_c_function( L, "nwn", "ObjectToInt", NWNX_ObjectToInt );
	lua.Log( "int IntToObject( object )\n" );

	register_c_function( L, "nwn", "NWScriptFunction", NWNX_CallNWScriptFunction );
	lua.Log( "data NWScriptFunction( proc-address-string, opcode, secondarg, ... ): attempts calling a nwscript function, proc-address-string must the address to the function. Then the function behaves the same as the nwscript version, this can horribly crash everything! Only works with nwscript functions that handle ints, floats, strings and objects!\n" );
	
	register_c_function( L, "nwn", "LuaLog", NWNX_SetLog );
	lua.Log( "void LuaLog( int ) 0 (default) no logging done, 1 log stuff\n" );

	register_c_function( L, "nwn", "GetFirstFile", NWNX_GetFirstFile );
	lua.Log( "file, attributes GetFirstFile( filepathfilter )\n" );
	
	register_c_function( L, "nwn", "GetNextFile", NWNX_GetNextFile );
	lua.Log( "file, attributes GetNextFile( )\n" );

	register_c_function( L, "nwn", "GetCheckDistance", NWNX_GetCheckDistance );
	lua.Log( "spot, listen GetCheckDistance( creature )\n" );

	register_c_function( L, "nwn", "SetCheckDistance", NWNX_SetCheckDistance );
	lua.Log( "oldvalue SetCheckDistance( creature, type, newvalue ) type: 1 combat spot 2 spot 3 combat listen 4 listen\n" );

	register_c_function( L, "nwn", "ClearVisibleList", NWNX_ClearVisibleList );
	lua.Log( "void ClearVisibleList( creature )\n" );

	register_c_function( L, "nwn", "UpdateVisibleList", NWNX_UpdateVisibleList );
	lua.Log( "void UpdateVisibleList( creature )\n" );

	register_c_function( L, "nwn", "AddToVisibleList", NWNX_AddToVisibleList );
	lua.Log( "int AddToVisibleList( creature, target, a1,a2,a3,a4 )\n" );

	register_c_function( L, "nwn", "RemoveFromVisibleList", NWNX_RemoveFromVisibleList );
	lua.Log( "void RemoveFromVisibleList( creature, target )\n" );

	register_c_function( L, "nwn", "SetVisibleListFlag", NWNX_SetVisibleListElement );
	lua.Log( "oldflags SetVisibleListFlag( creature, target, flags )\n" );

	register_c_function( L, "nwn", "GetVisibleListFlag", NWNX_GetVisibleListElement );
	lua.Log( "flags GetVisibleListFlag( creature, target )\n" );

	register_c_function( L, "nwn", "GetInvisible", NWNX_GetInvisible );
	lua.Log( "bool GetInvisible( creature, target, DC )\n" );

	register_c_function( L, "nwn", "GetBlind", NWNX_GetBlind );
	lua.Log( "bool GetBlind( creature )\n" );

	register_c_function(L, "nwn", "SetObject", NWNX_SetObject);
	lua.Log("void SetObject( object ): sets the object which is returned by nwscript GetObject\n");

	lua.Log( "\nAdditional:\nThe object type is a hexadecimal string, same as nwns ObjectToString.\nThe command print is by default redirected to this logfile.\nAll standard lua libraries enabled!\nOBJECT_SELF is available in lua-scripts executed from nwscript and holds the object executing the script.\n\n" );

	ApplyHooks();

	lua.logging=0;

	lua_sethook(L, luadiehook, LUA_MASKCOUNT, 100000 );
}