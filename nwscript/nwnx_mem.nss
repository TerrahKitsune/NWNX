
//Returns the number of bytes currently allocated by nwn
int MEM_Bytes( );

//Returns the number of allocations
int MEM_Count( );

//Returns the number of bad frees prevented
int MEM_Bad( );

int MEM_Bytes( ){

    SetLocalString( OBJECT_SELF, "NWNX!MEM!MEM", "                              " );
    string sMem = GetLocalString( OBJECT_SELF, "NWNX!MEM!MEM" );
    DeleteLocalString( OBJECT_SELF, "NWNX!MEM!MEM" );
    return StringToInt(sMem);
}

int MEM_Count( ){

    SetLocalString( OBJECT_SELF, "NWNX!MEM!CNT", "                              " );
    string sMem = GetLocalString( OBJECT_SELF, "NWNX!MEM!CNT" );
    DeleteLocalString( OBJECT_SELF, "NWNX!MEM!CNT" );
    return StringToInt(sMem);
}

int MEM_Bad( ){

    SetLocalString( OBJECT_SELF, "NWNX!MEM!BAD", "                              " );
    string sMem = GetLocalString( OBJECT_SELF, "NWNX!MEM!BAD" );
    DeleteLocalString( OBJECT_SELF, "NWNX!MEM!BAD" );
    return StringToInt(sMem);
}
