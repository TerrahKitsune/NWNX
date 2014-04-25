#include "NWNLuaLib.h"
#include "../NWNXdll/IniFile.h"
#include "../NWSERVER/types.h"
#include "../NWNXdll/madCHook.h"
#pragma comment(lib, "madChook.lib")

extern CNWNXLua lua;

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

int (__thiscall * CNWSMessage__SendServerToPlayerChat_Party)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043D790;
int (__thiscall * CNWSMessage__SendServerToPlayerChat_Talk)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043D9A0;
int (__thiscall * CNWSMessage__SendServerToPlayerChat_Tell)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043DC90;
int (__thiscall * CNWSMessage__SendServerToPlayerChat_Shout)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043DF80;
int (__thiscall * CNWSMessage__SendServerToPlayerChat_DM_Silent_Shout)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043E5C0;
int (__thiscall * CNWSMessage__SendServerToPlayerChat_Whisper)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043E4E0;
int (__thiscall * CNWSMessage__SendServerToPlayerChat_DM_Talk)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043DA80;
int (__thiscall * CNWSMessage__SendServerToPlayerChat_DM_Whisper)( CNWSMessage * mess, DWORD PlyID, nwn_objid_t oSender, CExoString send ) = (int (__thiscall *)( CNWSMessage *, DWORD, nwn_objid_t, CExoString ))0x0043E7D0;

int(__fastcall *CServerExoApp__MainLoopNext)(CServerExoApp * pThis, void*);
int __fastcall CServerExoApp__MainLoop(CServerExoApp * pThis, void*){

	if (lua.MainLoopFunc){

		lua_getglobal(lua.L, lua.MainLoopFunc);

		if (lua_isfunction(lua.L, -1)){

			lua_call(lua.L, 0, 0);
		}
	}
	return CServerExoApp__MainLoopNext(pThis, NULL);
}

int NWNX_SetObject(lua_State * L){

	sscanf(lua_tostring(L, 1), "%x", &lua.LastObjid);
	lua_pop(L, 1);

	//lua.Log("o OBJECT %08X\n", lua.LastObjid);

	return 0;
}

void SendMessageToPC( nwn_objid_t obj, const char * msg ){

	CNWSCreature * cre = (*NWN_AppManager)->app_server->srv_internal->GetCreatureByGameObjectID( obj );
	if( !cre )
		return;

	CNWCCMessageData * ccmsg = (CNWCCMessageData *)lua.mem.nwnx_malloc( 0x34 );
	CExoString str;
	memset( ccmsg, 0, 0x34 );
	memset( &str, 0, sizeof( CExoString ) );
	str.CExoStringCpy( msg );
	ccmsg->SetString( 0, str );
	cre->SendFeedbackMessage( 204, ccmsg, 0 );  
	lua.mem.nwnx_free( ccmsg );
}

int NWNX_RegisterMainLoopFunc(lua_State * L){

	if (lua.mainloophooked){

		lua_pop(L, 1);
		return 0;
	}

	size_t size;
	const char * Func = lua_tolstring(L,1, &size);
	if (lua.MainLoopFunc)
		delete []lua.MainLoopFunc;

	lua.MainLoopFunc = new char[size+1];
	strcpy(lua.MainLoopFunc, Func);
	lua_pop(L, 1);

	HookCode((PVOID)0x0042CA10, CServerExoApp__MainLoop, (PVOID*)&CServerExoApp__MainLoopNext);

	int temp = lua.logging;
	lua.logging = 1;
	lua.Log( "o MAINLOOP HOOKED! Lua mainloop func: %s\n", lua.MainLoopFunc );
	lua.logging = temp;
	lua.mainloophooked = true;
	return 0;
}

int NWNX_GetBlind( lua_State * L ){

	nwn_objid_t ID;
	const char *pc = lua_tostring(L, 1); 
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
	const char *pc = lua_tostring(L, 1); 
	sscanf( pc, "%x", &ID );
	pc = lua_tostring(L, 2); 
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
	const char *pc = lua_tostring(L, 1); 
	sscanf( pc, "%x", &ID );
	pc = lua_tostring(L, 2); 
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
	const char *pc = lua_tostring(L, 1); 
	sscanf( pc, "%x", &ID );
	pc = lua_tostring(L, 2); 
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
	const char *pc = lua_tostring(L, 1); 
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
	const char *pc = lua_tostring(L, 1); 
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
	const char *pc = lua_tostring(L, 1); 
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
	const char *pc = lua_tostring(L, 1); 
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
	const char *pc = lua_tostring(L, 1); 
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
	const char *pc = lua_tostring(L, 1); 
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
		lua_pushunsigned( L, lua.FindFileData.dwFileAttributes );
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
		lua_pushunsigned( L, lua.FindFileData.dwFileAttributes );
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

	const char *s = lua_tostring(L, -1); 

	nwn_objid_t oPC = (*NWN_AppManager)->app_server->GetFirstPCObject();

	if( oPC != OBJECT_INVALID ){

		SendMessageToPC( oPC, s );

		oPC = (*NWN_AppManager)->app_server->GetNextPCObject();
	}

	lua_pop( L, 1 );
	return 0;
}

int NWNX_msgpc (lua_State *L){

	nwn_objid_t ID;
	const char *pc = lua_tostring(L, 1); 
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
	return 1;
}

int NWNX_GetNextPC(lua_State *L){
	nwn_objid_t oPC = (*NWN_AppManager)->app_server->GetNextPCObject();
	char buf[12];
	sprintf( buf, "%x", oPC );
	lua_pushstring( L, buf );
	return 1; 
}

int NWNX_ExecuteScript(lua_State *L){

	nwn_objid_t ID;
	const char *pc = lua_tostring(L, 2); 
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

	nwn_objid_t ID;
	const char *pc = lua_tostring(L, -1);
	sscanf( pc, "%X", &ID );

	if( ID == OBJECT_INVALID ){
		lua_pushinteger( L, 0 );
		return 1;
	}

	lua_pop( L, 1 );
	CGenericObject * obj = (*NWN_AppManager)->app_server->GetGameObject( ID );
	if( obj )
		lua_pushinteger( L, 1 );
	else
		lua_pushinteger( L, 0 );
	return 1;
}

int NWNX_SetLocalString(lua_State *L){

	nwn_objid_t ID;
	const char *pc = lua_tostring(L, 1); 
	const char *sVar = lua_tostring(L, 2); 
	const char *sData = lua_tostring(L, 3); 
	sscanf( pc, "%x", &ID );

	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( ID );

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
	const char *pc = lua_tostring(L, 1); 
	const char *sVar = lua_tostring(L, 2); 
	sscanf( pc, "%x", &ID );

	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( ID );

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
	const char *pc = lua_tostring(L, 1); 
	const char *sVar = lua_tostring(L, 2); 
	sscanf( pc, "%x", &ID );

	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( ID );

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
	const char *pc = lua_tostring(L, 1); 
	const char *sMessage = lua_tostring(L, 3); 
	int nChannel = lua_tointeger(L, 4); 
	sscanf( pc, "%x", &ID );
	pc = lua_tostring(L, 2); 
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
	const char *pc = lua_tostring(L, 1); 
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
	const char *pc = lua_tostring(L, 1); 
	const char *sVar = lua_tostring(L, 2); 
	const char *sData = lua_tostring(L, 3); 
	sscanf( pc, "%x", &ID );
	sscanf( sData, "%x", &data );
	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( ID );
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
	const char *pc = lua_tostring(L, 1); 
	const char *sVar = lua_tostring(L, 2); 
	sscanf( pc, "%x", &ID );

	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( ID );

	if( !obj || ID == OBJECT_INVALID ){
		lua_pop( L, 2 );
		lua_pushstring( L, "" );
		return 1;
	}
	CExoString var;

	var.CExoStringCpy( sVar );

	nwn_objid_t data = obj->obj_vartable.GetNWNObject( var );

	CExoString str;
	str.text = (char*)lua.mem.nwnx_malloc( 12 );
	sprintf( str.text, "%x", data );

	lua_pop( L, 2 );

	lua_pushstring( L, str.text );

	return 1;
}

int NWNX_DeleteLocalObject(lua_State *L){

	nwn_objid_t ID;
	const char *pc = lua_tostring(L, 1); 
	const char *sVar = lua_tostring(L, 2); 
	sscanf( pc, "%x", &ID );

	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( ID );

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
	const char *pc = lua_tostring(L, 1); 
	const char *sVar = lua_tostring(L, 2); 

	sscanf( pc, "%x", &ID );
	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( ID );
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
	const char *pc = lua_tostring(L, 1); 
	const char *sVar = lua_tostring(L, 2); 
	sscanf( pc, "%x", &ID );

	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( ID );

	if( !obj || ID == OBJECT_INVALID ){
		lua_pop( L, 2 );
		lua_pushstring( L, "" );
		return 1;
	}
	CExoString var;

	lua_pop( L, 2 );

	lua_pushinteger( L, obj->obj_vartable.GetInt( var ) );

	return 1;
}

int NWNX_DeleteLocalInt(lua_State *L){

	nwn_objid_t ID;
	const char *pc = lua_tostring(L, 1); 
	const char *sVar = lua_tostring(L, 2); 
	sscanf( pc, "%x", &ID );

	CNWSObject * obj = (CNWSObject *)(*NWN_AppManager)->app_server->GetGameObject( ID );

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
	sscanf( lua_tostring( L, 1 ), "%x", &proc );
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

void register_c_function( lua_State *L, const char * tableName, const char * funcName, lua_CFunction funcPointer)
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

int (__fastcall *CVirtualMachine__StackPopIntegerNext)( CVirtualMachine * pThis, void*, int * data );
int (__fastcall *CVirtualMachine__StackPushIntegerNext)( CVirtualMachine * pThis, void*, int data );
int (__fastcall *CVirtualMachine__StackPopFloatNext)( CVirtualMachine * pThis, void*, float * data );
int (__fastcall *CVirtualMachine__StackPushFloatNext)( CVirtualMachine * pThis, void*, float data );
int (__fastcall *CVirtualMachine__StackPopStringNext)( CVirtualMachine * pThis, void*, CExoString * data );
int (__fastcall *CVirtualMachine__StackPushStringNext)( CVirtualMachine * pThis, void*, CExoString &data );
int (__fastcall *CVirtualMachine__StackPopObjectNext)( CVirtualMachine * pThis, void*, nwn_objid_t * data );
int (__fastcall *CVirtualMachine__StackPushObjectNext)( CVirtualMachine * pThis, void*, nwn_objid_t data );
int (__fastcall *CVirtualMachine__StackPopVectorNext)( CVirtualMachine * pThis, void*, Vector * data );
int (__fastcall *CVirtualMachine__StackPushVectorNext)( CVirtualMachine * pThis, void*, Vector data );


void *nwn_alloc (void *ud, void *ptr, size_t osize, size_t nsize) {

	if( nsize==0 ){
		free( ptr );
		return NULL;
	}
	
	return realloc( ptr, nsize );
}

int __fastcall CVirtualMachine__StackPushObject( CVirtualMachine * pThis, void*, nwn_objid_t data ){

	if( lua.L_Current==NULL ){
		return CVirtualMachine__StackPushObjectNext( pThis, NULL, data );
	} 

	char str[12];
	sprintf( str, "%x", data );
	lua_pushstring( lua.L_Current, str );
	
	if( lua.logging >= 2 )
		lua.Log( "o Push Object: %s\n", str );

	lua.L_Pushed++;

	return 1;
}

int __fastcall CVirtualMachine__StackPushString( CVirtualMachine * pThis, void*, CExoString & data ){

	if( lua.L_Current==NULL ){
		return CVirtualMachine__StackPushStringNext( pThis, NULL, data );
	} 

	const char * str = data.CStr();

	lua_pushstring( lua.L_Current, str );

	if( lua.logging >= 2 )
		lua.Log( "o Push String: %s\n", str );

	lua.L_Pushed++;

	return 1;
}

int __fastcall CVirtualMachine__StackPushInteger( CVirtualMachine * pThis, void*, int data ){

	if( lua.L_Current==NULL ){
		return CVirtualMachine__StackPushIntegerNext( pThis, NULL, data );
	} 
	
	int n = data;

	lua_pushinteger( lua.L_Current, n );
	if( lua.logging >= 2 )
		lua.Log( "o Push Int: %d\n", n );

	lua.L_Pushed++;

	return 1;
}

int __fastcall CVirtualMachine__StackPushFloat( CVirtualMachine * pThis, void*, float data ){

	if( lua.L_Current==NULL ){
		return CVirtualMachine__StackPushFloatNext( pThis, NULL, data );
	} 

	lua_Number fl = data;

	lua_pushnumber( lua.L_Current, fl );

	if( lua.logging >= 2 )
		lua.Log( "o Push Float: %f\n", fl );

	lua.L_Pushed++;

	return 1;
}

int __fastcall CVirtualMachine__StackPushVector( CVirtualMachine * pThis, void*, Vector data ){

	if( lua.L_Current==NULL ){
		return CVirtualMachine__StackPushVectorNext( pThis, NULL, data );
	}


	lua.lua_pushvector(lua.L_Current, data);

	//lua_pushlstring( lua.L_Current, (const char*)&data, sizeof( Vector ) );

	if( lua.logging >= 2 )
		lua.Log( "o Push Vector: x %f y %f z %f\n", data.x, data.y, data.z );

	lua.L_Pushed++;

	return 1;
}

int __fastcall CVirtualMachine__StackPopObject( CVirtualMachine * pThis, void*, nwn_objid_t * data ){

	if( lua.L_Current==NULL ){
		return CVirtualMachine__StackPopObjectNext( pThis, NULL, data );
	}

	const char * str = lua_tostring( lua.L_Current, 1 );

	sscanf( str, "%x", data );

	if( lua.logging >= 2 )
		lua.Log( "o Pop Object: %x\n", *data );

	lua_remove( lua.L_Current, 1 );
	return 1;
}

int __fastcall CVirtualMachine__StackPopString( CVirtualMachine * pThis, void*, CExoString * data ){

	if( lua.L_Current==NULL ){
		return CVirtualMachine__StackPopStringNext( pThis, NULL, data );
	} 

	
	if( data->text )
		lua.mem.nwnx_free( data->text );

	size_t len;
	const char * str = lua_tolstring( lua.L_Current, 1, &len );

	if( !str ){
		str = "";
		len = 0;
	}

	data->text = (char*)lua.mem.nwnx_malloc( len+1 );
	strcpy( data->text, str );

	if( lua.logging >= 2 )
		lua.Log( "o Pop String: %s\n", data->text );

	lua_remove( lua.L_Current, 1 );
	return 1;
}

int __fastcall CVirtualMachine__StackPopInteger( CVirtualMachine * pThis, void*, int * data ){

	if( lua.L_Current==NULL ){
		return CVirtualMachine__StackPopIntegerNext( pThis, NULL, data );
	}

	int n = lua_tointeger( lua.L_Current, 1 );
		
	*data = n;

	if( lua.logging >= 2 )
		lua.Log( "o Pop Int: %d\n", n );

	lua_remove( lua.L_Current, 1 );
	return 1;
}

int __fastcall CVirtualMachine__StackPopFloat( CVirtualMachine * pThis, void*, float * data ){

	if( lua.L_Current==NULL ){
		return CVirtualMachine__StackPopFloatNext( pThis, NULL, data );
	} 

	lua_Number num = lua_tonumber( lua.L_Current, 1 );	
	*data = num;
	
	if( lua.logging >= 2 )
		lua.Log( "o Pop Float: %f\n", num );

	lua_remove( lua.L_Current, 1 );
	return 1;
}

int __fastcall CVirtualMachine__StackPopVector( CVirtualMachine * pThis, void*, Vector * data ){

	if( lua.L_Current==NULL ){
		return CVirtualMachine__StackPopVectorNext( pThis, NULL, data );
	} 

	*data = lua.lua_tovector(lua.L_Current, 1);

	if( lua.logging >= 2 )
		lua.Log( "o Pop Vector: x %f y %f z %f\n", data->x, data->y, data->z );

	lua_remove( lua.L_Current, 1 );
	return 1;
}

void RegisterNWNLib( lua_State *L ){
	//Override printNWNX_ExecuteScript
	lua_register( L, "print", NWNX_Print );

	lua.Log( "Additional nwn functions: (useage nwn.funcname)\n\n" );

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

	register_c_function(L, "nwn", "LuaMainLoop", NWNX_RegisterMainLoopFunc);
	lua.Log("void LuaMainLoop( string FuncName ): hooks nwservers mainloop and executes the function with FuncName as its name\n");

	register_c_function(L, "nwn", "SetObject", NWNX_SetObject);
	lua.Log("void SetObject( object ): sets the object which is returned by nwscript GetObject\n");

	lua.Log( "\nAdditional:\nThe object type is a hexadecimal string, same as nwns ObjectToString.\nThe command print is by default redirected to this logfile.\nAll standard lua libraries enabled!\nOBJECT_SELF is available in lua-scripts executed from nwscript and holds the object executing the script.\n\n" );

	int ok = 1;
		
	ok = ok && HookCode( (PVOID)0x005C0A10, CVirtualMachine__StackPushObject, (PVOID*)&CVirtualMachine__StackPushObjectNext );
	ok = ok && HookCode( (PVOID)0x005C0860, CVirtualMachine__StackPushString, (PVOID*)&CVirtualMachine__StackPushStringNext );
	ok = ok && HookCode( (PVOID)0x005C05E0, CVirtualMachine__StackPushInteger, (PVOID*)&CVirtualMachine__StackPushIntegerNext );
	ok = ok && HookCode( (PVOID)0x005C0660, CVirtualMachine__StackPushFloat, (PVOID*)&CVirtualMachine__StackPushFloatNext );
	ok = ok && HookCode( (PVOID)0x005C09C0, CVirtualMachine__StackPopObject, (PVOID*)&CVirtualMachine__StackPopObjectNext );
	ok = ok && HookCode( (PVOID)0x005C0800, CVirtualMachine__StackPopString, (PVOID*)&CVirtualMachine__StackPopStringNext );
	ok = ok && HookCode( (PVOID)0x005C0590, CVirtualMachine__StackPopInteger, (PVOID*)&CVirtualMachine__StackPopIntegerNext );
	ok = ok && HookCode( (PVOID)0x005C0610, CVirtualMachine__StackPopFloat, (PVOID*)&CVirtualMachine__StackPopFloatNext );

	lua.Log( "Hooking VM: %s\n\n", ( ok == 1 ? "ok" : "broken" ) );

	lua.Log("\n\n");

	lua.logging=0;
}