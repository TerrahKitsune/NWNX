if nwn == nil then

	nwn = {}
	nwn.NWScriptFunction = print;

end

SQL = SQL or {};
SQL.nwn =SQL.nwn or {};

function SQL.nwn.Set(obj,var,data) return nwn.NWScriptFunction( "56E120", 57, 0, obj,var,data ); end
function SQL.nwn.Get(obj,var) return nwn.NWScriptFunction( "56DD90", 53, 0, obj,var ); end
function SQL.nwn.Del(obj,var) return nwn.NWScriptFunction( "57AF90", 267, 0, obj,var ); end
function SQL.nwn.Mod() return nwn.NWScriptFunction( "578AB0", 0, 0 ); end

function SQL.EscapeSQL(sql)

	local str = string.gsub( sql, "'", "''");

	return str;
end

function SQL.ExecDirect( sql )

	SQL.nwn.Set( SQL.nwn.Mod(), "NWNX!ODBC!EXEC", sql );
end

function SQL.Fetch()

	local sRow;
	local oModule = SQL.nwn.Mod();
	SQL.nwn.Set(oModule, "NWNX!ODBC!FETCH", SQL.nwn.Get(oModule, "NWNX!ODBC!SPACER"));
	sRow = SQL.nwn.Get(oModule, "NWNX!ODBC!FETCH");
	if sRow ~= nil and string.len(sRow) > 0 then

		SQL.nwn.Set(oModule, "NWNX_ODBC_CurrentRow", sRow);
        return true;
	end

	SQL.nwn.Set(oModule, "NWNX_ODBC_CurrentRow", "");
	return false;
end

function SQL.GetData(iCol)

	local sResultSet = SQL.nwn.Get(SQL.nwn.Mod(), "NWNX_ODBC_CurrentRow");
	local iPos = string.find( sResultSet, "¬", 0, true );
	local iCount = 0;
	local sColValue = "";

	if iPos == nil and iCol == 1 then
		sColValue = sResultSet;
	elseif iPos == nil then
		sColValue = "";
	else

		while iCount ~= iCol do

			iCount = iCount + 1;
			if iCount == iCol then
				sColValue = string.sub(sResultSet, 0, iPos-1);
			else

				sResultSet = string.sub(sResultSet, ( string.len(sResultSet) - iPos ) * -1);
                iPos = string.find( sResultSet, "¬", 0, true );
			end

			if iPos == nil then
				iPos = string.len(sResultSet)+1;
			end

		end
	end

	return sColValue;
end
