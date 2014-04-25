//-----------------------------------------------------------------------------
// header
//-----------------------------------------------------------------------------
//script:  nwnx_td_events
//description: This is the event script for the nwnx_events.dll plugin
//used as: Event script
//date:    2013/05/04
//author:  Terra

//-------------------------------------------------------------------------------
// includes
//-------------------------------------------------------------------------------
#include "nwnx_weapons"
#include "amia_include"

//-------------------------------------------------------------------------------
// changelog
//-------------------------------------------------------------------------------

// 2013/05/04 Terra ported over the devcrit routines from nwnx_events

//-------------------------------------------------------------------------------
// constants
//-------------------------------------------------------------------------------
const int WEAPON_DEV_RANK_1 = 1;
const int WEAPON_DEV_RANK_2 = 2;
const int WEAPON_DEV_RANK_3 = 3;
const int WEAPON_DEV_RANK_4 = 4;

//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------

//Subfunction to DoCheckDevastatingCritical
int GetWeaponDevastatingRoundMod( object oItem, int nSize );

void main( ){

    //The wielder, the attacking creature
    object oPC = OBJECT_SELF;
    //The item that is currently performing the devastating critical hit
    object oItem = GetDevastatingCriticalTarget( 0 );
    //The victim, if there are several victims (IE: whirlwind attack)
    //the script will fire for each victim.
    object oTarget = GetDevastatingCriticalTarget( 1 );

    //Target cannot be devcritted
    if( GetLocalInt( oTarget, "dev_immune" ) > 0 )
        return;

    //Tag and bag
    else if( !FortitudeSave( oTarget, 10 + GetAbilityModifier( ABILITY_STRENGTH, oPC ) + ( GetHitDice( oPC ) / 2 ), SAVING_THROW_TYPE_NONE, oPC ) ){

        int nDur = GetWeaponDevastatingRoundMod( oItem, GetCreatureSize( oPC ) );

        ApplyEffectAtLocation( DURATION_TYPE_INSTANT, EffectVisualEffect( VFX_IMP_DUST_EXPLOSION ), GetLocation( oTarget ) );

        if( GetIsBlocked( oTarget, "dev_block" ) <= 0 ){

            FloatingTextStringOnCreature( "Devastating Critical Hit!", oPC );

            SetBlockTime( oTarget, 0, FloatToInt( RoundsToSeconds( nDur ) ), "dev_block" );
            effect eKD  = SupernaturalEffect( EffectKnockdown( ) );
            effect eBad = EffectLinkEffects( EffectBlindness( ), EffectSilence( ) );
                   eBad = EffectLinkEffects( EffectDeaf( ), eBad );
                   eBad = EffectLinkEffects( EffectVisualEffect( VFX_DUR_MIND_AFFECTING_NEGATIVE ), eBad );
                   eBad = SupernaturalEffect( eBad );

            ApplyEffectToObject( DURATION_TYPE_TEMPORARY, eKD, oTarget, RoundsToSeconds( nDur ) );
            ApplyEffectToObject( DURATION_TYPE_TEMPORARY, eBad, oTarget, TurnsToSeconds( nDur ) );
        }
    }
}

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

int GetWeaponDevastatingRoundMod( object oItem, int nSize ){


    if( nSize <= CREATURE_SIZE_SMALL ){

        switch( GetBaseItemType( oItem ) ){


            case BASE_ITEM_BRACER: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_GLOVES: return WEAPON_DEV_RANK_3;

            case BASE_ITEM_CBLUDGWEAPON: return WEAPON_DEV_RANK_1;
            case BASE_ITEM_CPIERCWEAPON: return WEAPON_DEV_RANK_1;
            case BASE_ITEM_CREATUREITEM: return WEAPON_DEV_RANK_1;
            case BASE_ITEM_CSLASHWEAPON: return WEAPON_DEV_RANK_1;
            case BASE_ITEM_CSLSHPRCWEAP: return WEAPON_DEV_RANK_1;

            case BASE_ITEM_CLUB: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_DAGGER: return WEAPON_DEV_RANK_2;
            case BASE_ITEM_DART: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_DIREMACE: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_DOUBLEAXE: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_DWARVENWARAXE: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_BASTARDSWORD: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_BATTLEAXE: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_GREATAXE: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_GREATSWORD: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_HALBERD: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_HANDAXE: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_HEAVYCROSSBOW: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_HEAVYFLAIL: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_KAMA: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_KATANA: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_KUKRI: return WEAPON_DEV_RANK_1;
            case BASE_ITEM_LIGHTCROSSBOW: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_LIGHTFLAIL: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_LIGHTHAMMER: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_LIGHTMACE: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_LONGBOW: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_LONGSWORD: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_MORNINGSTAR: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_QUARTERSTAFF: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_RAPIER: return WEAPON_DEV_RANK_2;
            case BASE_ITEM_SCIMITAR: return WEAPON_DEV_RANK_2;
            case BASE_ITEM_SCYTHE: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_SHORTBOW: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_SHORTSPEAR: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_SHORTSWORD: return WEAPON_DEV_RANK_2;
            case BASE_ITEM_SHURIKEN: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_SICKLE: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_SLING: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_THROWINGAXE: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_TRIDENT: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_TWOBLADEDSWORD: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_WARHAMMER: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_WHIP: return WEAPON_DEV_RANK_3;

            default: return WEAPON_DEV_RANK_3;
        }
    }
    else{

        switch( GetBaseItemType( oItem ) ){


            case BASE_ITEM_BRACER: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_GLOVES: return WEAPON_DEV_RANK_3;

            case BASE_ITEM_CBLUDGWEAPON: return WEAPON_DEV_RANK_1;
            case BASE_ITEM_CPIERCWEAPON: return WEAPON_DEV_RANK_1;
            case BASE_ITEM_CREATUREITEM: return WEAPON_DEV_RANK_1;
            case BASE_ITEM_CSLASHWEAPON: return WEAPON_DEV_RANK_1;
            case BASE_ITEM_CSLSHPRCWEAP: return WEAPON_DEV_RANK_1;

            case BASE_ITEM_CLUB: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_DAGGER: return WEAPON_DEV_RANK_2;
            case BASE_ITEM_DART: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_DIREMACE: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_DOUBLEAXE: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_DWARVENWARAXE: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_BASTARDSWORD: return WEAPON_DEV_RANK_2;
            case BASE_ITEM_BATTLEAXE: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_GREATAXE: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_GREATSWORD: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_HALBERD: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_HANDAXE: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_HEAVYCROSSBOW: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_HEAVYFLAIL: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_KAMA: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_KATANA: return WEAPON_DEV_RANK_2;
            case BASE_ITEM_KUKRI: return WEAPON_DEV_RANK_1;
            case BASE_ITEM_LIGHTCROSSBOW: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_LIGHTFLAIL: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_LIGHTHAMMER: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_LIGHTMACE: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_LONGBOW: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_LONGSWORD: return WEAPON_DEV_RANK_2;
            case BASE_ITEM_MORNINGSTAR: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_QUARTERSTAFF: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_RAPIER: return WEAPON_DEV_RANK_1;
            case BASE_ITEM_SCIMITAR: return WEAPON_DEV_RANK_1;
            case BASE_ITEM_SCYTHE: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_SHORTBOW: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_SHORTSPEAR: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_SHORTSWORD: return WEAPON_DEV_RANK_2;
            case BASE_ITEM_SHURIKEN: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_SICKLE: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_SLING: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_THROWINGAXE: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_TRIDENT: return WEAPON_DEV_RANK_4;
            case BASE_ITEM_TWOBLADEDSWORD: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_WARHAMMER: return WEAPON_DEV_RANK_3;
            case BASE_ITEM_WHIP: return WEAPON_DEV_RANK_3;

            default: return WEAPON_DEV_RANK_3;
        }
    }
    return WEAPON_DEV_RANK_1;
}
