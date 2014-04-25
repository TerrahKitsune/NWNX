//Unpacks a string into special abilities
//The formatting should be "Spell Ready Level\n"
//IE: 0 1 15 (acid cloud, ready, level 15)
int NWNX_Magic_UnpackSpecialAbilityString( object oCreature, string sData );

//Packs the spelllike ability list of oCreature
//The string is formatted as "Spell Ready Level\n"
//And can be directly used with NWNX_Magic_UnpackSpecialAbilityString
string NWNX_Magic_PackSpecialAbilityString( object oCreature );

//Returns the spell-like ability from nIndex
//The string is formatted as "Spell Ready Level"
//Returns an empty string on failure
string NWNX_Magic_GetSpellLikeAbility( object oCreature, int nIndex );

//Get the length of the spell-like ability array
int NWNX_Magic_GetSpellLikeLength( object oCreature );

//Removes the data located in nIndex
//Returns true/false
int NWNX_Magic_RemoveSpellLikeAbility( object oCreature, int nIndex );

//Adds a memorized spell to oCreature
//This will create additional spellslots if neasesary
//Use with caution
//Formatting: "classindex spelllevel spellid ready metamagic domain"
//Class index should be 0, 1 or 2
int NWNX_Magic_AddMemorizedSpell( object oCreature, string sData );

//Modify the amount of spellslots avaiable for oCreature
//This function only covers memorization spell-slots
//If nSpellslots = -1 the function will return the number of spellslots available instead
int NWNX_Magic_SetNumberOfSpellslots( object oCreature, int nClassIndex, int nSpellLevel, int nSpellSlots );

//This modifies spells per day data (relevant for sorcs etc)
//nClassIndex must be 0-2
//nType: 0=bonus spells 1=spells left 2=spells max
//if nSpells is -1 the function will return the current number
int NWNX_Magic_ModifySpellsPerDay( object oCreature, int nClassIndex, int nSpellLevel, int nType, int nSpells );

//Sets a memorized spell to oCreature
//This will create additional spellslots if neasesary
//Use with caution
//Formatting: "classindex spelllevel spellslots spellid ready metamagic domain"
//Class index should be 0, 1 or 2
//If spellid, ready, metamagic, domain is 0 the spellslot will be removed (unmemorized)
int NWNX_Magic_SetMemorizedSpell( object oCreature, string sData );

//Get memorized spelldata from oCreature
//nClass has to be within 0-2 range
//returns "spellid ready metamagic domain"
//returns "0 0 0 0" if the slot is empty
string NWNX_Magic_GetMemorizedSpell( object oCreature, int nClass, int nSpellLevel, int nSlot );

//Packs a spellevel into a string
//nClassIndex must be between 0-2
string NWNX_Magic_PackSpellLevelIntoString( object oCreature, int nClassIndex, int nSpellLevel );

//Unpacks a spellevel from a string unto oCreature
//This does not add spellslots if they're missing
//Returns the number of spells unpacked
//nClassIndex must be between 0-2
int NWNX_Magic_UnPackSpellLevelString( object oCreature, int nClassIndex, int nSpellLevel, string sData );

//Removes all memorized spells from nSpellLevel
//nClassIndex must be between 0-2
//if nSetSpellsUnReady = 1 the spells are kept, but set to un-ready (used up)
void NWNX_Magic_EmptySpellLevel( object oCreature, int nClassIndex, int nSpellLevel, int nSetSpellsUnReady );

//Adds a known spell to oCreature at nSpellevel
//ClassIndex must be between 0-2
void NWNX_Magic_AddKnownSpell( object oCreature, int nClassIndex, int nSpellLevel, int nSpellToAdd );

//Get a known spell from oCreature
//classindex must be between 0-2
//returns -1 if there is no known spell in nIndex (end of the array)
int NWNX_Magic_GetKnownSpell( object oCreature, int nClassIndex, int nSpellLevel, int nIndex );

//Attempts to remove a known spell from oCreature
//classindex must be between 0-2
//Returns true/false depending on success
int NWNX_Magic_RemoveKnownSpell( object oCreature, int nClassIndex, int nSpellLevel, int nSpell );

int NWNX_Magic_RemoveKnownSpell( object oCreature, int nClassIndex, int nSpellLevel, int nSpell ){

    SetLocalString( oCreature, "NWNX!MAGIC!RemoveKnownSpell", IntToString( nClassIndex )+" "+IntToString( nSpellLevel )+" "+IntToString( nSpell ) );
    int nRet = StringToInt( GetLocalString( oCreature, "NWNX!MAGIC!RemoveKnownSpell" ) );
    DeleteLocalString( oCreature, "NWNX!MAGIC!RemoveKnownSpell" );
    return nRet;
}

int NWNX_Magic_GetKnownSpell( object oCreature, int nClassIndex, int nSpellLevel, int nIndex ){

    SetLocalString( oCreature, "NWNX!MAGIC!GetKnownSpell", IntToString( nClassIndex )+" "+IntToString( nSpellLevel )+" "+IntToString( nIndex ) );
    int nRet = StringToInt( GetLocalString( oCreature, "NWNX!MAGIC!GetKnownSpell" ) );
    DeleteLocalString( oCreature, "NWNX!MAGIC!GetKnownSpell" );
    return nRet;
}

void NWNX_Magic_AddKnownSpell( object oCreature, int nClassIndex, int nSpellLevel, int nSpellToAdd ){

    SetLocalString( oCreature, "NWNX!MAGIC!AddKnownSpell", IntToString( nClassIndex )+" "+IntToString( nSpellLevel )+" "+IntToString( nSpellToAdd ) );
    DeleteLocalString( oCreature, "NWNX!MAGIC!AddKnownSpell" );
}

void NWNX_Magic_EmptySpellLevel( object oCreature, int nClassIndex, int nSpellLevel, int nSetSpellsUnReady ){

    SetLocalString( oCreature, "NWNX!MAGIC!EmptySpellBook", IntToString( nClassIndex )+" "+IntToString( nSpellLevel )+" "+IntToString( nSetSpellsUnReady ) );
    DeleteLocalString( oCreature, "NWNX!MAGIC!EmptySpellBook" );
}

int NWNX_Magic_UnPackSpellLevelString( object oCreature, int nClassIndex, int nSpellLevel, string sData ){

    if( GetObjectType( oCreature ) != OBJECT_TYPE_CREATURE )
        return -1;

    SetLocalString( oCreature, "NWNX!MAGIC!UnPackSpellLevelString", IntToString( nClassIndex )+" "+IntToString( nSpellLevel )+" "+sData );
    int nRet = StringToInt( GetLocalString( oCreature, "NWNX!MAGIC!UnPackSpellLevelString" ) );
    DeleteLocalString( oCreature, "NWNX!MAGIC!UnPackSpellLevelString" );
    return nRet;
}

string NWNX_Magic_PackSpellLevelIntoString( object oCreature, int nClassIndex, int nSpellLevel ){

    if( GetObjectType( oCreature ) != OBJECT_TYPE_CREATURE )
        return "";

    //16*32 memoryspace
    string sString;
    int n;
    for( n=0;n<32;n++ ){
        sString+="                ";
    }

    SetLocalString( oCreature, "NWNX!MAGIC!PackSpellLevelIntoString", IntToString( nClassIndex )+" "+IntToString( nSpellLevel )+"~"+sString );
    string nRet = GetLocalString( oCreature, "NWNX!MAGIC!PackSpellLevelIntoString" );
    DeleteLocalString( oCreature, "NWNX!MAGIC!PackSpellLevelIntoString" );
    return nRet;
}

string NWNX_Magic_GetMemorizedSpell( object oCreature, int nClass, int nSpellLevel, int nSlot ){

    if( GetObjectType( oCreature ) != OBJECT_TYPE_CREATURE )
        return "";

    SetLocalString( oCreature, "NWNX!MAGIC!GetMemorizedSpell", IntToString( nClass )+" "+IntToString( nSpellLevel )+" "+IntToString( nSlot )+"~                                  " );
    string nRet = GetLocalString( oCreature, "NWNX!MAGIC!GetMemorizedSpell" );
    DeleteLocalString( oCreature, "NWNX!MAGIC!GetMemorizedSpell" );
    return nRet;
}

int NWNX_Magic_ModifySpellsPerDay( object oCreature, int nClassIndex, int nSpellLevel, int nType, int nSpells ){

    if( GetObjectType( oCreature ) != OBJECT_TYPE_CREATURE )
        return 0;

    SetLocalString( oCreature, "NWNX!MAGIC!ModifySpellsPerDay", IntToString( nClassIndex )+" "+IntToString( nSpellLevel )+" "+IntToString( nType )+" "+IntToString( nSpells ) );
    int nRet = StringToInt( GetLocalString( oCreature, "NWNX!MAGIC!ModifySpellsPerDay" ) );
    DeleteLocalString( oCreature, "NWNX!MAGIC!ModifySpellsPerDay" );
    return nRet;
}

int NWNX_Magic_SetNumberOfSpellslots( object oCreature, int nClassIndex, int nSpellLevel, int nSpellSlots ){

    if( GetObjectType( oCreature ) != OBJECT_TYPE_CREATURE )
        return 0;

    SetLocalString( oCreature, "NWNX!MAGIC!SetNumberOfSpellslots", IntToString( nClassIndex )+" "+IntToString( nSpellLevel )+" "+IntToString( nSpellSlots ) );
    int nRet = StringToInt( GetLocalString( oCreature, "NWNX!MAGIC!SetNumberOfSpellslots" ) );
    DeleteLocalString( oCreature, "NWNX!MAGIC!SetNumberOfSpellslots" );
    return nRet;
}

int NWNX_Magic_SetMemorizedSpell( object oCreature, string sData ){

    if( GetObjectType( oCreature ) != OBJECT_TYPE_CREATURE )
        return 0;

    SetLocalString( oCreature, "NWNX!MAGIC!AddMemorizedSpell", sData );
    int nRet = StringToInt( GetLocalString( oCreature, "NWNX!MAGIC!AddMemorizedSpell" ) );
    DeleteLocalString( oCreature, "NWNX!MAGIC!AddMemorizedSpell" );
    return nRet;
}

int NWNX_Magic_AddMemorizedSpell( object oCreature, string sData ){

    if( GetObjectType( oCreature ) != OBJECT_TYPE_CREATURE )
        return 0;

    SetLocalString( oCreature, "NWNX!MAGIC!AddMemorizedSpell", sData );
    int nRet = StringToInt( GetLocalString( oCreature, "NWNX!MAGIC!AddMemorizedSpell" ) );
    DeleteLocalString( oCreature, "NWNX!MAGIC!AddMemorizedSpell" );
    return nRet;
}

int NWNX_Magic_GetSpellLikeLength( object oCreature ){

    if( GetObjectType( oCreature ) != OBJECT_TYPE_CREATURE )
        return 0;

    SetLocalString( oCreature, "NWNX!MAGIC!GetSpellLikeLength", "~        " );
    int nRet = StringToInt( GetLocalString( oCreature, "NWNX!MAGIC!GetSpellLikeLength" ) );
    DeleteLocalString( oCreature, "NWNX!MAGIC!GetSpellLikeLength" );
    return nRet;
}

int NWNX_Magic_UnpackSpecialAbilityString( object oCreature, string sData ){

    if( GetObjectType( oCreature ) != OBJECT_TYPE_CREATURE )
        return -1;

    SetLocalString( oCreature, "NWNX!MAGIC!AddSpellLikeAbility", sData );
    int nRet = StringToInt( GetLocalString( oCreature, "NWNX!MAGIC!AddSpellLikeAbility" ) );
    DeleteLocalString( oCreature, "NWNX!MAGIC!AddSpellLikeAbility" );
    return nRet;
}

string NWNX_Magic_PackSpecialAbilityString( object oCreature ){

    if( GetObjectType( oCreature ) != OBJECT_TYPE_CREATURE )
        return "";

    //16*32 memoryspace
    string sString;
    int n;
    for( n=0;n<32;n++ ){
        sString+="                ";
    }

    SetLocalString( oCreature, "NWNX!MAGIC!PackSpellLikeAbilities", "~"+sString );
    string nRet = GetLocalString( oCreature, "NWNX!MAGIC!PackSpellLikeAbilities" );
    DeleteLocalString( oCreature, "NWNX!MAGIC!PackSpellLikeAbilities" );
    return nRet;
}

string NWNX_Magic_GetSpellLikeAbility( object oCreature, int nIndex ){

    if( GetObjectType( oCreature ) != OBJECT_TYPE_CREATURE )

        return "";
    //16*2 memoryspace
    string sString;
    int n;
    for( n=0;n<2;n++ ){
        sString+="                ";
    }

    SetLocalString( oCreature, "NWNX!MAGIC!GetSpellLikeAbility", IntToString( nIndex )+"~"+sString );
    string nRet = GetLocalString( oCreature, "NWNX!MAGIC!GetSpellLikeAbility" );
    DeleteLocalString( oCreature, "NWNX!MAGIC!GetSpellLikeAbility" );
    return nRet;
}

int NWNX_Magic_RemoveSpellLikeAbility( object oCreature, int nIndex ){

    if( GetObjectType( oCreature ) != OBJECT_TYPE_CREATURE )
        return -1;

    SetLocalString( oCreature, "NWNX!MAGIC!RemoveSpellLikeAbility", IntToString( nIndex ) );
    int nRet = StringToInt( GetLocalString( oCreature, "NWNX!MAGIC!RemoveSpellLikeAbility" ) );
    DeleteLocalString( oCreature, "NWNX!MAGIC!RemoveSpellLikeAbility" );
    return nRet;
}


