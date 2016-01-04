//Runs a lua chunk
string ExecuteLuaString( object oObjSelf, string sLua );

//Run a lua function
string ExecuteLuaFunction( object oObjSelf, string sFunc, string sParam );

//Return the object that was previously set in luascript
//Index 0 target 1
//Index 1 target 2
//Index 3 target set by lua SetObject
object LuaGetObject(int index);

//Set a KeyValue in the lua environment
//See SetLuaKeyValueTable
void SetLuaKeyValue(string key, string value);

//Set a KeyValue in the lua environment
//See SetLuaKeyValueTable
string GetLuaKeyValue(string key);

//Set a KeyValue in the lua environment
//this will return a value with a integer key
//See SetLuaKeyValueTable
string GetLuaIndexValue(int index);

//Set a KeyValue in the lua environment
//See SetLuaKeyValueTable
void DeleteLuaKeyValue(string key);

//0 indexed, get the key from index
//Returns "" on error (or there is none in the index)
//See SetLuaKeyValueTable
string GetLuaKeyValueKey(int index);

//Sets the table to point the LuaKeyValue functions towards
//the tables are created (if they don't exist) in a table
//called nwscript in the global environment
//IE, if you wanted to reach the table from lua:
//local tbl = nwscript.table;
//default: localdata
void SetLuaKeyValueTable(string table);

//Returns the table name that
//SetLuaKeyValueTable is set too
//default: localdata
string GetLuaKeyValueTable();

//Runs lua and returns the first return from the snippet
//if it was an error the first character is going to be a "%"
string RunLua(string lua);

string GetLuaIndexValue( int index ){

    return GetLocalString( OBJECT_SELF,"LUA!giv!"+IntToString(index) );
}

string RunLua(string lua){
    return GetLocalString( OBJECT_SELF,"LUA!run!"+lua );
}

string GetLuaKeyValueTable(){
    return GetLocalString( OBJECT_SELF,"LUA!gt!0" );
}

void SetLuaKeyValueTable(string table){
    GetLocalString( OBJECT_SELF,"LUA!st!"+table );
}

string GetLuaKeyValueKey(int index){
    return GetLocalString( OBJECT_SELF,"LUA!lkv!"+IntToString(index) );
}

void DeleteLuaKeyValue( string key ){

    GetLocalString( OBJECT_SELF,"LUA!dkv!"+key );
}

string GetLuaKeyValue( string key ){

    return GetLocalString( OBJECT_SELF,"LUA!gkv!"+key );
}

void SetLuaKeyValue( string key, string value ){

    GetLocalString( OBJECT_SELF,"LUA!skv!"+key+"|"+value );
}

object LuaGetObject(int index){

    return GetLocalObject(OBJECT_SELF,"NWNX!LUA!"+IntToString(index));
}

string ExecuteLuaFunction( object oObjSelf, string sFunc, string sParam ){

    SetLocalString( oObjSelf, "NWNX!LUA!"+sFunc, sParam+"|                                                                                                                                                                                        " );
    string sLua = GetLocalString( oObjSelf, "NWNX!LUA!"+sFunc );
    DeleteLocalString( oObjSelf, "NWNX!LUA!"+sFunc );
    return sLua;
}

string ExecuteLuaString( object oObjSelf, string sLua ){

    SetLocalString( oObjSelf, "NWNX!LUA!sexec", sLua+"|                                                                       " );
    sLua = GetLocalString( oObjSelf, "NWNX!LUA!sexec" );
    DeleteLocalString( oObjSelf, "NWNX!LUA!sexec" );
    return sLua;
}

