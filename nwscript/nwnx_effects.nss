//void main( ){}

//Set spell ID on eEffect and return new effect
effect SetEffectSpellID( effect eEffect, int nSpellID );

//Set a script that is to be fired when eEffect expires
//Use GetLastEffect( ) inside the effect script to get the effect
//Object_self is the effect owner
effect SetEffectScript( effect eEffect, string sEffectScript );

//Set the CL for eEffect
effect SetEffectCasterLevel( effect eEffect, int nCL );

//Get the unique effect ID
//this has two parts, index 0 and index 1 since its a 64bit unsigned intiger
int GetEffectID( effect eEffect, int nIndex=0 );

//Set eEffect's creator to o Creator
effect SetEffectCreator( effect eEffect, object oCreator );

//Get the casterlevel of the effect
int GetEffectCasterLevel( effect eEffect );

//Get the last effect
//Used in SetEffectScript scripts
effect GetLastEffect( );

//Creates an AC modifying effect
//nAC can be negative or positive
//This doesnt count as any type of AC
//it also stacks with itself and doesnt respect any caps
effect EffectAC( int nAC );

//Creates an AB modifying effect
//nAB can be negative or positive
//it also stacks with itself and doesnt respect any caps
effect EffectAB( int nAB );

//Return integer data from eEffect
//returns -1 if not valid
int GetEffectInteger( effect eEffect, int nIndex );

//Return the unconditional AC mod (as created by EffectAC)
int GetUnconditionalAC( object oPC );

//Return the unconditional AB mod (as created by EffectAB)
int GetUnconditionalAB( object oPC );

int GetUnconditionalAB( object oPC ){

    SetLocalString( oPC, "NWNX!EFFECTS!GetUAB", "000000000000" );
    string sRet = GetLocalString( oPC, "NWNX!EFFECTS!GetUAB" );
    DeleteLocalString( oPC, "NWNX!EFFECTS!GetUAB" );
    return StringToInt( sRet );
}

int GetUnconditionalAC( object oPC ){

    SetLocalString( oPC, "NWNX!EFFECTS!GetUAC", "000000000000" );
    string sRet = GetLocalString( oPC, "NWNX!EFFECTS!GetUAC" );
    DeleteLocalString( oPC, "NWNX!EFFECTS!GetUAC" );
    return StringToInt( sRet );
}

int GetEffectInteger( effect eEffect, int nIndex ){

    SetLocalString( OBJECT_SELF, "NWNX!EFFECTS!GetInt", IntToString( nIndex ) );
    DeleteLocalString( OBJECT_SELF, "NWNX!EFFECTS!GetInt" );
    return GetEffectDurationType( eEffect );
}

effect EffectAB( int nAB ){

    if( nAB == 0 )
        return EffectHeal(-1);

    effect eEffect;

    if( nAB < 0 ){

        SetLocalString( OBJECT_SELF, "NWNX!EFFECTS!UnconditionalAB", IntToString( nAB*-1 ) );
        eEffect =  MagicalEffect( EffectAttackDecrease( 1 ) );
    }
    else{

        SetLocalString( OBJECT_SELF, "NWNX!EFFECTS!UnconditionalAB", IntToString( nAB ) );
        eEffect =  MagicalEffect( EffectAttackIncrease( 1 ) );
    }

    DeleteLocalString( OBJECT_SELF, "NWNX!EFFECTS!UnconditionalAB" );
    return eEffect;
}

effect EffectAC( int nAC ){

    if( nAC == 0 )
        return EffectHeal(-1);

    effect eEffect;

    if( nAC < 0 ){

        SetLocalString( OBJECT_SELF, "NWNX!EFFECTS!UnconditionalAC", IntToString( nAC*-1 ) );
        eEffect =  MagicalEffect( EffectACDecrease( 1 ) );
    }
    else{

        SetLocalString( OBJECT_SELF, "NWNX!EFFECTS!UnconditionalAC", IntToString( nAC ) );
        eEffect =  MagicalEffect( EffectACIncrease( 1 ) );
    }

    DeleteLocalString( OBJECT_SELF, "NWNX!EFFECTS!UnconditionalAC" );
    return eEffect;
}

effect GetLastEffect( ){

    SetLocalString( OBJECT_SELF, "NWNX!EFFECTS!GetLastEffect", "0" );
    DeleteLocalString( OBJECT_SELF, "NWNX!EFFECTS!GetLastEffect" );
    int nID = GetEffectDurationType( EffectSleep( ) );

    effect eEffect = GetFirstEffect( OBJECT_SELF );
    while( GetIsEffectValid( eEffect ) ){

        if( nID = GetEffectID( eEffect ) )
            break;

        eEffect = GetNextEffect( OBJECT_SELF );
    }

    return eEffect;
}

int GetEffectCasterLevel( effect eEffect ){

    SetLocalString( OBJECT_SELF, "NWNX!EFFECTS!GetEffectCasterLevel", "0" );
    DeleteLocalString( OBJECT_SELF, "NWNX!EFFECTS!GetEffectCasterLevel" );
    return GetEffectDurationType( eEffect );
}

effect SetEffectCreator( effect eEffect, object oCreator ){

    SetLocalString( OBJECT_SELF, "NWNX!EFFECTS!SetEffectCreator", ObjectToString( oCreator ) );
    DeleteLocalString( OBJECT_SELF, "NWNX!EFFECTS!SetEffectCreator" );
    return MagicalEffect( eEffect );
}

int GetEffectID( effect eEffect, int nIndex ){

    SetLocalString( OBJECT_SELF, "NWNX!EFFECTS!GetEffectID", IntToString( nIndex ) );
    DeleteLocalString( OBJECT_SELF, "NWNX!EFFECTS!GetEffectID" );
    return GetEffectDurationType( eEffect );
}

effect SetEffectCasterLevel( effect eEffect, int nCL ){

    SetLocalString( OBJECT_SELF, "NWNX!EFFECTS!SetEffectCasterLevel", IntToString( nCL ) );
    DeleteLocalString( OBJECT_SELF, "NWNX!EFFECTS!SetEffectCasterLevel" );
    return MagicalEffect( eEffect );
}

effect SetEffectScript( effect eEffect, string sEffectScript ){

    SetLocalString( OBJECT_SELF, "NWNX!EFFECTS!SetEffectScript", sEffectScript );
    DeleteLocalString( OBJECT_SELF, "NWNX!EFFECTS!SetEffectScript" );
    return MagicalEffect( eEffect );
}

effect SetEffectSpellID( effect eEffect, int nSpellID ){

    SetLocalString( OBJECT_SELF, "NWNX!EFFECTS!SetEffectSpellID", IntToString( nSpellID ) );
    DeleteLocalString( OBJECT_SELF, "NWNX!EFFECTS!SetEffectSpellID" );
    return MagicalEffect( eEffect );
}

