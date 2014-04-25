//NWScript lua function for lua52.dll and nwnx_lua.dll

//Executes a lua string, see the lua log in log0 for more info
string ExecuteLuaString( object oObjSelf, string sLua );

string ExecuteLuaString( object oObjSelf, string sLua ){

    SetLocalString( oObjSelf, "NWNX!LUA!sexec", sLua );
    sLua = GetLocalString( oObjSelf, "NWNX!LUA!sexec" );
    DeleteLocalString( oObjSelf, "NWNX!LUA!sexec" );
    return sLua;
}
