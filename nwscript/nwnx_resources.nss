//void main(){}

//Sets sResref to be loaded defaulty
//IE: Loaded externally if found, internally if not found
//ResRef should be the resref and the extension
//IE: Creature.utc
void RES_SetResToDefault( string sResRef );

//Sets sResref to be loaded and then kept loaded
//ResRef should be the resref and the extension
//IE: Creature.utc
void RES_SetResToCache( string sResRef );

//Sets sResref to not be loaded at all, internally or externally
//ResRef should be the resref and the extension
//IE: Creature.utc
void RES_SetResToNotLoad( string sResRef );

//Sets sResref to be loaded by nwserver's default
//ResRef should be the resref and the extension
//IE: Creature.utc
void RES_SetResToInternalLoad( string sResRef );

//Sets sResref to be loaded from another file
//sFile should be absorlute path
//IE: "C:/Stuff/Creature.utc"
//ResRef should be the resref and the extension
//IE: Creature.utc
void RES_SetResToLoadFromFile( string sResRef, string sFile );

void RES_SetResToDefault( string sResRef ){

    string sFormat = "0|";
    sFormat = sFormat+sResRef + "|";
    sFormat = sFormat+IntToString( 1 ) + "|";
    sFormat = sFormat+"0";

    SetLocalString( GetModule( ), "NWNX!RESOURCES!0", sFormat );
    DeleteLocalString( GetModule( ), "NWNX!RESOURCES!0" );

}

void RES_SetResToCache( string sResRef ){

    string sFormat = "1|";
    sFormat = sFormat+sResRef + "|";
    sFormat = sFormat+IntToString( 1 ) + "|";
    sFormat = sFormat+"0";

    SetLocalString( GetModule( ), "NWNX!RESOURCES!1", sFormat );
    DeleteLocalString( GetModule( ), "NWNX!RESOURCES!1" );

}

void RES_SetResToNotLoad( string sResRef ){

    string sFormat = "2|";
    sFormat = sFormat+sResRef + "|";
    sFormat = sFormat+IntToString( 1 ) + "|";
    sFormat = sFormat+"0";

    SetLocalString( GetModule( ), "NWNX!RESOURCES!2", sFormat );
    DeleteLocalString( GetModule( ), "NWNX!RESOURCES!2" );

}

void RES_SetResToInternalLoad( string sResRef ){

    string sFormat = "3|";
    sFormat = sFormat+sResRef + "|";
    sFormat = sFormat+IntToString( 1 ) + "|";
    sFormat = sFormat+"0";

    SetLocalString( GetModule( ), "NWNX!RESOURCES!3", sFormat );
    DeleteLocalString( GetModule( ), "NWNX!RESOURCES!3" );

}

void RES_SetResToLoadFromFile( string sResRef, string sFile ){

    string sFormat = "4|";
    sFormat = sFormat+sResRef + "|";
    sFormat = sFormat+IntToString( GetStringLength( sFile ) ) + "|";
    sFormat = sFormat+sFile;

    SetLocalString( GetModule( ), "NWNX!RESOURCES!4", sFormat );
    DeleteLocalString( GetModule( ), "NWNX!RESOURCES!4" );
}
