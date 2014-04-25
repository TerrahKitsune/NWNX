nwn["GetName"] = function (obj) return nwn.NWScriptFunction( "570580", 0, 0, obj, 0 ); end
nwn["GetAge"] = function (obj) return nwn.NWScriptFunction( "5863B0", 0, 0, obj ); end
nwn["GetModule"] = function () return nwn.NWScriptFunction( "578AB0", 0, 0 ); end
nwn["Random"] = function (maximum) return nwn.NWScriptFunction( "567DA0", 0, 0, maximum); end
nwn["PrintString"] = function (str) return nwn.NWScriptFunction( "567E00", 0, 0, str ); end
nwn["SetLocalString"] = function (obj,var,data) return nwn.NWScriptFunction( "56E120", 57, 0, obj,var,data ); end
nwn["SetLocalInt"] = function (obj,var,data) return nwn.NWScriptFunction( "56E120", 55, 0, obj,var,data ); end
nwn["SetLocalObject"] = function (obj,var,data) return nwn.NWScriptFunction( "56E120", 58, 0, obj,var,data ); end
nwn["SetLocalFloat"] = function (obj,var,data) return nwn.NWScriptFunction( "56E120", 56, 0, obj,var,data ); end
nwn["GetLocalString"] = function (obj,var) return nwn.NWScriptFunction( "56DD90", 53, 0, obj,var ); end
nwn["GetLocalInt"] = function (obj,var) return nwn.NWScriptFunction( "56DD90", 51, 0, obj,var ); end
nwn["GetLocalObject"] = function (obj,var) return nwn.NWScriptFunction( "56DD90", 54, 0, obj,var ); end
nwn["GetLocalFloat"] = function (obj,var) return nwn.NWScriptFunction( "56DD90", 52, 0, obj,var ); end
nwn["DeleteLocalString"] = function (obj,var) return nwn.NWScriptFunction( "57AF90", 267, 0, obj,var ); end
nwn["DeleteLocalInt"] = function (obj,var) return nwn.NWScriptFunction( "57AF90", 265, 0, obj,var ); end
nwn["DeleteLocalObject"] = function (obj,var) return nwn.NWScriptFunction( "57AF90",268, 0, obj,var ); end
nwn["DeleteLocalFloat"] = function (obj,var) return nwn.NWScriptFunction( "57AF90",266, 0, obj,var ); end
nwn["GetTag"] = function (obj) return nwn.NWScriptFunction( "574CF0", 0, 0, obj ); end
nwn["GetResref"] = function (obj) return nwn.NWScriptFunction( "589FD0", 0, 0, obj ); end

print( "Loaded: " .. nwn.GetName( nwn.GetModule( ) ) );
for key,value in pairs(nwn) do print( tostring(key) .." " .. tostring(value)) end

nwn.SetLocalInt( nwn.GetModule(), "lua_loaded", 1 );
