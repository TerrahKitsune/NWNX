//Create an area based on sResRef
object AREAS_CreateArea( string sResRef );

//Destroy an area, returns true/false
//Areas which has players in them cannot be destroyed
int AREAS_DestroyArea( object oArea );

//Set the name of oArea
void AREAS_SetAreaName( object oArea, string sNewName );

//Returns corresponding area to nArea where nArea is the index into the area list
//Returns object_invalid if nArea is invalid or there are no more areas
object AREAS_GetArea( int nArea );

//Returns the ground height at location
float AREAS_GetHeight( location lLoc );

//Get the number of players in the area
int AREAS_GetPlayers( object oArea );

//Forces the server to send area updates to DMs
//as well as fixing minimap data on existing creatures
void AREAS_Update( );

void AREAS_Update( ){
    SetLocalString( OBJECT_SELF, "NWNX!AREAS!COMMIT", ".........." );
    DeleteLocalString( OBJECT_SELF, "NWNX!AREAS!COMMIT" );
}

int AREAS_GetPlayers( object oArea ){

    SetLocalString( oArea, "NWNX!AREAS!GET_PLAY", ".........." );
    int nRet = StringToInt( GetLocalString( oArea, "NWNX!AREAS!GET_PLAY" ) );
    DeleteLocalString( oArea, "NWNX!AREAS!GET_PLAY" );
    return nRet;
}

float AREAS_GetHeight( location lLoc ){

    vector v = GetPositionFromLocation( lLoc );
    object oArea = GetAreaFromLocation( lLoc );

    SetLocalString( oArea, "NWNX!AREAS!GET_HEIGHT", FloatToString( v.x )+" "+FloatToString( v.y )+" "+FloatToString( v.z ) );
    float fData = StringToFloat( GetLocalString( oArea, "NWNX!AREAS!GET_HEIGHT" ) );
    DeleteLocalString( oArea, "NWNX!AREAS!GET_HEIGHT" );
    return fData;
}

object AREAS_GetArea( int nArea ){

    SetLocalString( OBJECT_SELF, "NWNX!AREAS!GET_AREA", IntToString( nArea ) );
    DeleteLocalString( OBJECT_SELF, "NWNX!AREAS!GET_AREA" );
    return GetLocalObject( OBJECT_SELF, "NWNX!AREAS!GET_LAST_AREA_ID" );
}

void AREAS_SetAreaName( object oArea, string sNewName ){

    SetLocalString( oArea, "NWNX!AREAS!SET_AREA_NAME", sNewName );
    DeleteLocalString( oArea, "NWNX!AREAS!SET_AREA_NAME" );
}

object AREAS_CreateArea( string sResRef ){

    SetLocalString( OBJECT_SELF, "NWNX!AREAS!CREATE_AREA", sResRef );
    DeleteLocalString( OBJECT_SELF, "NWNX!AREAS!CREATE_AREA" );
    return GetLocalObject( OBJECT_SELF, "NWNX!AREAS!GET_LAST_AREA_ID" );
}

int AREAS_DestroyArea( object oArea ){

    SetLocalString( OBJECT_SELF, "NWNX!AREAS!DESTROY_AREA", ObjectToString( oArea ) );
    int n = StringToInt( GetLocalString( OBJECT_SELF, "NWNX!AREAS!DESTROY_AREA" ) );
    DeleteLocalString( OBJECT_SELF, "NWNX!AREAS!DESTROY_AREA" );
    return n;
}

