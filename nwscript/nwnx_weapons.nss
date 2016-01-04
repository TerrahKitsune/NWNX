/*

First time this plugin runs it'll create a config file called weaponconfiguration.txt in root.
This file is used to set the default behavior of weapons, see the logfile on formatting!

*/


const int WEAPON_TYPE_FINESSABLE = 0;
const int WEAPON_TYPE_UNARMEDBASEATTACK = 1;
const int WEAPON_TYPE_WEAPONFOCUS = 2;
const int WEAPON_TYPE_EPIC_WEAPONFOCUS = 3;
const int WEAPON_TYPE_SPECIALIZATION = 4;
const int WEAPON_TYPE_EPIC_SPECIALIZATION = 5;
const int WEAPON_TYPE_WEAPON_OF_CHOICE = 6;
const int WEAPON_TYPE_IMPROVED_CRITICAL = 7;
const int WEAPON_TYPE_OVERWHELMING_CRITICAL = 8;
const int WEAPON_TYPE_DEVASTATING_CRITICAL = 9;
const int WEAPON_TYPE_CRITICAL_HIT_MOD = 10;
const int WEAPON_TYPE_CRITICAL_HIT_THREAT = 11;

//Get the current flag or feat required for an itemtype
//nType = WEAPON_TYPE_*
int GetWeaponData( int nBaseItemType, int nType );

//Set/change data for the baseitem type
//nType = WEAPON_TYPE_*
int SetWeaponData( int nBaseItemType, int nType, int nData );

//Returns a list of the feats that work as proficiencies for the item type
//The list is delmitted by spaces
string GetProficiencies( int nBaseItemType );

//Adds nFeat as a profiency feat for the item type
void AddProficiencies( int nBaseItemType, int nFeat );

//Removes nFeat as a profiency for the item type if present
void RemoveProficiencies( int nBaseItemType, int nFeat );

//Sets the behavior of critical hit
//nSet:
//0: Fort vs death (default)
//1: Runs a script nwnx_devcrit (runs for each target in whirlwind attack) (see: GetDevastatingCriticalTarget)
//2: Adds +1 to critical damage mod
//3: Adds +2 to critical threat
void SetDevastatingCriticalBehavior( int nSet );

//Extracts data for the devcrit script
//nType:
//0: the item performing the devcrit
//1: target of the critical hit
//OBJECT_SELF in the devcrit script is the attacker
object GetDevastatingCriticalTarget( int nType );

//Sets the items owner to creature
//this means that oCreature can equip the item even if it doesnt have the required profiencies
//if nBound is true, only that creature cna equip the weapon
void SetItemOwner( object oCreature, object oItem, int nBound );

//Returns the owner of the weapon (as set by SetItemOwner)
object GetItemOwner( object oItem );

//Removes the owner (if any)
//Returns the owner the item had
object RemoveItemOwner( object oItem );

object RemoveItemOwner( object oItem ){

    return GetLocalObject( oItem, "NWNX!WEAPONS!7" );
}

object GetItemOwner( object oItem ){

    return GetLocalObject( oItem, "NWNX!WEAPONS!6" );
}

void SetItemOwner( object oCreature, object oItem, int nBound ){

    SetLocalString( oCreature, "NWNX!WEAPONS!SetOwn", ObjectToString( oItem )+" "+IntToString( nBound ) );
    DeleteLocalString( oCreature, "NWNX!WEAPONS!SetOwn" );
}

object GetDevastatingCriticalTarget( int nType ){

    return GetLocalObject( OBJECT_SELF, "NWNX!WEAPONS!+"+IntToString( nType ) );
}

void SetDevastatingCriticalBehavior( int nSet ){

    SetLocalString( GetModule(), "NWNX!WEAPONS!SetDevCrit", IntToString( nSet ) );
    DeleteLocalString( GetModule(), "NWNX!WEAPONS!SetDevCrit" );
}

void RemoveProficiencies( int nBaseItemType, int nFeat ){

    SetLocalString( GetModule(), "NWNX!WEAPONS!RemoveProficiencies", IntToString( nBaseItemType )+" "+IntToString( nFeat ) );
    DeleteLocalString( GetModule(), "NWNX!WEAPONS!RemoveProficiencies" );
}

void AddProficiencies( int nBaseItemType, int nFeat ){

    SetLocalString( GetModule(), "NWNX!WEAPONS!AddProficiencies", IntToString( nBaseItemType )+" "+IntToString( nFeat ) );
    DeleteLocalString( GetModule(), "NWNX!WEAPONS!AddProficiencies" );
}

string GetProficiencies( int nBaseItemType ){

    SetLocalString( GetModule(), "NWNX!WEAPONS!GetProficiencies", IntToString( nBaseItemType )+"                                                                                                                                                                                    " );
    string nRet = GetLocalString( GetModule(), "NWNX!WEAPONS!GetProficiencies" );
    DeleteLocalString( GetModule(), "NWNX!WEAPONS!GetProficiencies" );

    return nRet;
}

int GetWeaponData( int nBaseItemType, int nType ){

    SetLocalString( GetModule(), "NWNX!WEAPONS!GetData", IntToString( nBaseItemType )+" "+IntToString( nType ) );
    int nRet = StringToInt( GetLocalString( GetModule(), "NWNX!WEAPONS!GetData" ) );
    DeleteLocalString( GetModule(), "NWNX!WEAPONS!GetData" );

    return nRet;
}

void SetWeaponData( int nBaseItemType, int nType, int nData ){

    SetLocalString( GetModule(), "NWNX!WEAPONS!SetData", IntToString( nBaseItemType )+" "+IntToString( nType )+" "+IntToString( nData ) );
    DeleteLocalString( GetModule(), "NWNX!WEAPONS!SetData" );
}
