#include "nwnx_lua"

void main(){

    //Add a lua function to the lua environment, this only needs to run once per session.
    //ExecuteLuaString returns "" on success otherwise it returns the error
    ExecuteLuaString( OBJECT_SELF, "function SendTellToPC( oPC, mess ) nwn.SendChatMessage( oPC, oPC, mess, 3 ); end" );

    //Call our function
    object oPC = OBJECT_SELF;
    string sMessage = "This is a tell sent from myself!";
    ExecuteLuaString( oPC, "SendTellToPC( OBJECT_SELF, '"+sMessage+"' );" );

    //Strings in lua has to be encapuled by either single quotes ', double quotes " or [=[my str]=]
    //OBJECT_SELF in lua is the object that you passed to ExecuteLuaFunction, ExecuteLuaString
    //All nwn related functions are in the nwn table, so to call them nwn.Function


    //How to pass data between the environemnts:
    ExecuteLuaString( oPC, "nwn.SetLocalString(OBJECT_SELF, 'mybicfile', nwn.GetBic( OBJECT_SELF ) );" );

    SendMessageToPC( oPC, "My bicfile is: " + GetLocalString( oPC, "mybicfile" ) );

    //Not there is no table nwn. here in the func def, this too only needs to run once, once its defined it'll stay
    ExecuteLuaString( oPC, "function GetFirstName( ) return nwn.SetGetName( OBJECT_SELF, 0 ); end" );

    //ExecuteLuaFunction can also do this, however ExecuteLuaFunction can only execute functions
    //that isnt in a table and returns a string and takes no parameters
    //So we can create a lua function for it

    //We can now call that function
    SendMessageToPC( oPC, "My first name is: " + ExecuteLuaFunction( oPC, "GetFirstName", "" ) );

    //The above is equal to: local bic = GetBic( ); except that it returns the string to nwn
    //nwn objects in lua are their hex id ObjectToString as a string
}
