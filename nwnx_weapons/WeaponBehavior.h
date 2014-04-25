#ifndef __NWNX_WEAPON_BEHAVIOR__
#define __NWNX_WEAPON_BEHAVIOR__

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class WeaponBehavior{

public:

	struct WeaponEntry{

		DWORD BASE_TYPE;
		BYTE FINESSABLE;
		BYTE UNARMED_BASEATTACK;
		WORD WEAPON_FOCUS_FEAT;
		WORD EPIC_WEAPON_FOCUS_FEAT;
		WORD WEAPON_SPECIALIZATION_FEAT;
		WORD EPIC_WEAPON_SPECIALIZATION_FEAT;
		WORD WEAPON_OF_CHOICE_FEAT;
		WORD IMPROVED_CIRITCAL_FEAT;
		WORD OVERWHELMING_CRITICAL_FEAT;
		WORD DEVASTATING_CRITICAL_FEAT;
		WORD * Proficiencies;
		BYTE numProficiencies;
		BYTE CriticalDamageMod;
		BYTE CriticalHitThreat;
	};

	WeaponBehavior( void );
	~WeaponBehavior( void );
	WeaponEntry * Resize( int newSize );
	WeaponEntry * AddWeaponType( DWORD BASE_TYPE );
	WeaponEntry * GetWeaponType( DWORD BASE_TYPE );
	WeaponEntry * GetFirstNext( bool first );
	WeaponEntry * RemoveWeaponType( DWORD BASE_TYPE );
	WeaponEntry * LoadConfiguration( const char * filename );

	void AddProficiencies( DWORD BASE_TYPE, WORD Feat );
	void RemoveProficiencies( DWORD BASE_TYPE, WORD Feat );
	void ParseList( WORD Feat, const char * ItemList );

	BYTE GetWeaponFinessable( DWORD BASE_TYPE );
	BYTE GetWeaponUsesUnarmedBaseAttackBonus( DWORD BASE_TYPE );
	WORD GetWeaponFocusFeat( DWORD BASE_TYPE );
	WORD GetEpicWeaponFocusFeat( DWORD BASE_TYPE );
	WORD GetWeaponSpecializationFeat( DWORD BASE_TYPE );
	WORD GetEpicWeaponSpecializationFeat( DWORD BASE_TYPE ); 
	WORD GetWeaponOfChoiceFeat( DWORD BASE_TYPE );
	WORD GetImprovedCriticalFeat( DWORD BASE_TYPE );
	WORD GetOverwhelmingCirticalHitFeat( DWORD BASE_TYPE );
	WORD GetDevastatingCirticalHitFeat( DWORD BASE_TYPE );
	BYTE GetCriticalHitDamageMod( DWORD BASE_TYPE );
	BYTE GetCriticalHitThreat( DWORD BASE_TYPE );

	WeaponEntry * WEArray;
	int Len;
	int Size;
	int FirstNext;
	
	BYTE HasListPopulated[12];

	WORD WeaponFinesseFeat;
};

#endif