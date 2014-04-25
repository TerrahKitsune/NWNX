//void main(){}

//Sets a dynamic display name for oPC on oNamer
//if oPC is the module oNamers name will be set as its default name
//default name affects all display name functions!
//Returns true/false depending on success or not
int NWNX_Names_SetName( object oPC, object oNamer, string sNewName );

//Gets the display name oPC currently sees oNamer as
//if oPC is the module it will return the set default dynamic name
//Returns an empty string if there is no dynamic name set
string NWNX_Names_GetName( object oPC, object oNamer );

//Removes the dynamic display name oPC sees oNamer as
//if oPC is the module it will remove the set default dynamic name
//Returns true/false depending or success or not
int NWNX_Names_RemoveName( object oPC, object oNamer );

//Sets the name of oCreature to sNewName, this will update the object for all PCs
//This also works persistantly on playerobjects
//if nLastName = 1 the lastname will be set instead of the firstname
//Returns true false depending on success or not
int NWNX_Names_SetPermanentName( object oCreature, string sNewName, int nLastName );

//Get the first or lastname of oCreature, returns an empty string on failure
string NWNX_Names_GetPermanentName( object oCreature, int nLastName );

//This will update oObject for all logged in pcs
//Only works on creature objects
void NWNX_Names_UpdateObject( object oObject );

//Set the name of oMasters familiar or (nType=1)animal companion
//This can be done eventhough oMaster has no animal companion set
//This will not update on summoned familiar or companions!
void NWNX_Names_SetFamiliarCompanionName( object oMaster, string sNewName, int nType );

//Get the name of oMasters familiar or (nType=1)animal companion
//Returns an empty string if there are no name set
string NWNX_Names_GetFamiliarCompanionName( object oMaster, string sNewName, int nType );

//Sends a message from oSender to oReceiver
//This uses the talk channel but only the receiver will see sMessage
//oReceiver must be a player controlled object and oSender must be a creature
//This works cross areas and reguardless of vision between the two unlike normal talk
//Returns false if the message wasnt sent
int NWNX_Names_SendMessage( object oSender, object oReceiver, string sMessage );

int NWNX_Names_SendMessage( object oSender, object oReceiver, string sMessage ){

    SetLocalString( oReceiver, "NWNX!NAMES!Text", ObjectToString( oSender )+" "+sMessage );
    int nRet = StringToInt( GetLocalString( oReceiver, "NWNX!NAMES!Text" ) );
    DeleteLocalString( oReceiver, "NWNX!NAMES!Text" );
    return nRet;
}

string NWNX_Names_GetFamiliarCompanionName( object oMaster, string sNewName, int nType ){

    SetLocalString( GetModule(), "NWNX!NAMES!GetFamilarCompanionName", ObjectToString( oMaster )+" "+IntToString( nType )+" ~                                                                                                                                        " );
    string nReturn = GetLocalString( GetModule(), "NWNX!NAMES!GetFamilarCompanionName" );
    DeleteLocalString( GetModule(), "NWNX!NAMES!GetFamilarCompanionName" );

    return nReturn;
}

void NWNX_Names_SetFamiliarCompanionName( object oMaster, string sNewName, int nType ){

    SetLocalString( GetModule(), "NWNX!NAMES!SetFamilarCompanionName", ObjectToString( oMaster )+" "+IntToString( nType )+" "+sNewName );
    DeleteLocalString( GetModule(), "NWNX!NAMES!SetFamilarCompanionName" );
}

void NWNX_Names_UpdateObject( object oObject ){

    SetLocalString( GetModule(), "NWNX!NAMES!UpdateObject", ObjectToString( oObject ) );
    DeleteLocalString( GetModule(), "NWNX!NAMES!UpdateObject" );
}

string NWNX_Names_GetPermanentName( object oCreature, int nLastName ){

    SetLocalString( oCreature, "NWNX!NAMES!GetPermanentName", IntToString( nLastName )+" ~                                                                                           " );
    string nReturn = GetLocalString( oCreature, "NWNX!NAMES!GetPermanentName" );
    DeleteLocalString( oCreature, "NWNX!NAMES!GetPermanentName" );

    return nReturn;
}

int NWNX_Names_SetPermanentName( object oCreature, string sNewName, int nLastName ){

    SetLocalString( oCreature, "NWNX!NAMES!SetPermanentName", IntToString( nLastName )+" "+sNewName );
    int nReturn = StringToInt( GetLocalString( oCreature, "NWNX!NAMES!SetPermanentName" ) );
    DeleteLocalString( oCreature, "NWNX!NAMES!SetPermanentName" );

    return nReturn;
}

int NWNX_Names_RemoveName( object oPC, object oNamer ){

    SetLocalString( oPC, "NWNX!NAMES!RemoveName", ObjectToString( oNamer ) );
    int nReturn = StringToInt( GetLocalString( oPC, "NWNX!NAMES!RemoveName" ) );
    DeleteLocalString( oPC, "NWNX!NAMES!RemoveName" );

    return nReturn;
}

string NWNX_Names_GetName( object oPC, object oNamer ){

    SetLocalString( oPC, "NWNX!NAMES!GetName", ObjectToString( oNamer )+" ~                                                                                                          " );
    string nReturn = GetLocalString( oPC, "NWNX!NAMES!GetName" );
    DeleteLocalString( oPC, "NWNX!NAMES!GetName" );

    return nReturn;
}

int NWNX_Names_SetName( object oPC, object oNamer, string sNewName ){

    SetLocalString( oPC, "NWNX!NAMES!SetName", ObjectToString( oNamer )+" "+sNewName );
    int nReturn = StringToInt( GetLocalString( oPC, "NWNX!NAMES!SetName" ) );
    DeleteLocalString( oPC, "NWNX!NAMES!SetName" );

    return nReturn;
}

