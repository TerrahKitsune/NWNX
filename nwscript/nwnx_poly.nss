#include "nwnx_magic"

//Returns the shape that is being polymorphed out of or into (POLYMORPH_TYPE_*)
int GetPolymorphType( );
//Returns true if the script was fired when shifting
//false if unshifting
int GetIsShifting( );

void SaveSpellState( object oPC );
void RestoreSpellState( object oPC );

void main( ){

    object oPC = OBJECT_SELF;
    int nPoly = GetPolymorphType( );

    if( GetIsShifting( ) ){
        SaveSpellState( oPC );
    }
    else{
        DelayCommand( 0.5, RestoreSpellState( oPC ) );
    }
}

void SaveSpellState( object oPC ){

    int nClass,n,i;
    string sSpells;

    for( n=0;n<3;n++ ){

        nClass = GetClassByPosition( n+1, oPC );
        if( nClass == CLASS_TYPE_DRUID ||
            nClass == CLASS_TYPE_CLERIC ||
            nClass == CLASS_TYPE_RANGER ||
            nClass == CLASS_TYPE_PALADIN ||
            nClass == CLASS_TYPE_WIZARD ){

            for( i=0;i<10;i++ ){

                sSpells = NWNX_Magic_PackSpellLevelIntoString( oPC, n, i );

                if( sSpells != "" )
                    SetLocalString( oPC, "spells_"+IntToString( n )+"_"+IntToString( i ), sSpells );
            }
        }
        else if( nClass == CLASS_TYPE_SORCERER ||
                nClass == CLASS_TYPE_BARD ){

            for( i=0;i<10;i++ ){

                SetLocalInt( oPC, "left_"+IntToString( n )+"_"+IntToString( i ), NWNX_Magic_ModifySpellsPerDay( oPC, n, i, 1, -1 ) );
            }

        }
        else if( nClass == CLASS_TYPE_INVALID )
            return;
    }
}

void RestoreSpellState( object oPC ){

    int nClass,n,i;
    string sSpells;
    int nSpells, nMax;

    for( n=0;n<3;n++ ){

        nClass = GetClassByPosition( n+1, oPC );
        if( nClass == CLASS_TYPE_DRUID ||
            nClass == CLASS_TYPE_CLERIC ||
            nClass == CLASS_TYPE_RANGER ||
            nClass == CLASS_TYPE_PALADIN ||
            nClass == CLASS_TYPE_WIZARD ){

            for( i=0;i<10;i++ ){

                sSpells = GetLocalString( oPC, "spells_"+IntToString( n )+"_"+IntToString( i ) );

                if( sSpells != "" )
                    NWNX_Magic_UnPackSpellLevelString( oPC, n, i, sSpells );
            }
        }
        else if( nClass == CLASS_TYPE_SORCERER ||
                nClass == CLASS_TYPE_BARD ){

            for( i=0;i<10;i++ ){

                nSpells = GetLocalInt( oPC, "left_"+IntToString( n )+"_"+IntToString( i ) );
                nMax = NWNX_Magic_ModifySpellsPerDay( oPC, n, i, 2, -1 );

                if( nSpells > nMax )
                    nSpells = nMax;

                if( nSpells > 0 )
                    NWNX_Magic_ModifySpellsPerDay( oPC, n, i, 1, nSpells );
            }

        }
        else if( nClass == CLASS_TYPE_INVALID )
            return;
    }
}

int GetPolymorphType( ){

    SetLocalString( OBJECT_SELF, "NWNX!POLYMORPH!GETPOLY", "        " );
    int nRet = StringToInt( GetLocalString( OBJECT_SELF, "NWNX!POLYMORPH!GETPOLY" ) );
    DeleteLocalString( OBJECT_SELF, "NWNX!POLYMORPH!GETPOLY" );
    return nRet;
}

int GetIsShifting( ){

    SetLocalString( OBJECT_SELF, "NWNX!POLYMORPH!GETTYPE", "        " );
    int nRet = StringToInt( GetLocalString( OBJECT_SELF, "NWNX!POLYMORPH!GETTYPE" ) );
    DeleteLocalString( OBJECT_SELF, "NWNX!POLYMORPH!GETTYPE" );
    return nRet;
}

