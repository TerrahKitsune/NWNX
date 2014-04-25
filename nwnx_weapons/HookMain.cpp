#include "NWNXWeapons.h"

extern CNWNXWeapons Weapons;

int (__fastcall * GetWeaponFinesseOriginal)( CNWSCreatureStats *, void*, CNWSItem * );
int __fastcall GetWeaponFinesse( CNWSCreatureStats * pThis, void*, CNWSItem * item ){

	if( pThis == NULL )
		return 0;

	unsigned int uiBaseItemType;

	if( !pThis->HasFeat( 42 ) )
		return 0;

	if( item == NULL )
		uiBaseItemType = 256; //Invalid
	else{

		uiBaseItemType = item->it_baseitemtype;
		
		if( pThis->cs_original->GetRelativeWeaponSize( item ) > 0 )
			return 0;
	}

	return Weapons.WB->GetWeaponFinessable( uiBaseItemType );
}

int (__fastcall * GetCanUseMonkAttackTableOriginal)( CNWSCreatureStats *, void*, int );
int __fastcall GetCanUseMonkAttackTable( CNWSCreatureStats * pThis, void*, int arg1 ){

	if( pThis == NULL )
		return 0;

	if( pThis->cs_original->GetUseMonkAbilities( ) ){

		CNWSItem * itmRightHand = pThis->cs_original->cre_equipment->GetItemInSlot( 16 );

		//Handle unarmed-only d20
		if( arg1 && !itmRightHand )
			return 1;

		CNWSItem * itmLeftHand = pThis->cs_original->cre_equipment->GetItemInSlot( 32 );

		DWORD baseItemR = itmRightHand == NULL ? 256 : itmRightHand->it_baseitemtype;
		DWORD baseItemL = itmLeftHand == NULL ? 256 : itmLeftHand->it_baseitemtype;

		return ( Weapons.WB->GetWeaponUsesUnarmedBaseAttackBonus( baseItemR ) == 1 && Weapons.WB->GetWeaponUsesUnarmedBaseAttackBonus( baseItemL ) == 1 );
	}

	return 0;
}

int (__fastcall * GetCanUseMonkAbilitiesOriginal)( CNWSCreature *, void * );
int __fastcall GetCanUseMonkAbilities( CNWSCreature * pThis, void * ){

	if( pThis == NULL || pThis->cre_stats->GetNumLevelsOfClass( 5 ) <= 0 )
		return 0;

	CNWSItem * itmChest = pThis->cre_equipment->GetItemInSlot( 2 );

	int armorAC = itmChest == NULL ? 0 : itmChest->ComputeArmorClass( );

	if( armorAC > 0 )
		return 0;

	CNWSItem * itmHand = pThis->cre_equipment->GetItemInSlot( 32 );
	DWORD baseItem = itmHand == NULL ? 256 : itmHand->it_baseitemtype;

	return Weapons.WB->GetWeaponUsesUnarmedBaseAttackBonus( baseItem );
}

void (__fastcall * SetCombatModeOriginal)( CNWSCreature *, void*, unsigned char, int );
void __fastcall SetCombatMode( CNWSCreature * pThis, void*, unsigned char a1, int a2 ){

	if( a1 == 5 ){

		if( pThis->cre_stats->GetUseMonkAttackTables( 0 ) ){

			CNWSItem * itmRightHand = pThis->cre_equipment->GetItemInSlot( 16 );
			CNWSItem * itmLeftHand = pThis->cre_equipment->GetItemInSlot( 32 );

			DWORD baseItemR = itmRightHand == NULL ? 256 : itmRightHand->it_baseitemtype;
			DWORD baseItemL = itmLeftHand == NULL ? 256 : itmLeftHand->it_baseitemtype;
	
			if( baseItemR != 256 )
				itmRightHand->it_baseitemtype = 40;
			if( baseItemL != 256 )
				itmLeftHand->it_baseitemtype = 40;
			
			SetCombatModeOriginal( pThis, NULL, a1, a2 );

			if( baseItemR != 256 )
				itmRightHand->it_baseitemtype = baseItemR;
			if( baseItemL != 256 )
				itmLeftHand->it_baseitemtype = baseItemL;
		}
	}
	else
		SetCombatModeOriginal( pThis, NULL, a1, a2 );
}

int (__fastcall * GetWeaponFocusOriginal)( CNWSCreatureStats *, void*, CNWSItem * );
int __fastcall GetWeaponFocus( CNWSCreatureStats * pThis, void*, CNWSItem * itm ){

	DWORD baseType = itm == NULL ? 256 : itm->it_baseitemtype;
	WORD Feat = Weapons.WB->GetWeaponFocusFeat( baseType );

	if( Feat == 0 )
		return 0;

	return pThis->HasFeat( Feat );
}

int (__fastcall * GetEpicWeaponFocusOriginal)( CNWSCreatureStats *, void*, CNWSItem * );
int __fastcall GetEpicWeaponFocus( CNWSCreatureStats * pThis, void*, CNWSItem * itm ){

	DWORD baseType = itm == NULL ? 256 : itm->it_baseitemtype;
	WORD Feat = Weapons.WB->GetEpicWeaponFocusFeat( baseType );

	if( Feat == 0 )
		return 0;

	return pThis->HasFeat( Feat );
}

int (__fastcall * GetWeaponSpecializationOriginal)( CNWSCreatureStats *, void*, CNWSItem * );
int __fastcall GetWeaponSpecialization( CNWSCreatureStats * pThis, void*, CNWSItem * itm ){

	DWORD baseType = itm == NULL ? 256 : itm->it_baseitemtype;
	WORD Feat = Weapons.WB->GetWeaponSpecializationFeat( baseType );

	if( Feat == 0 )
		return 0;

	return pThis->HasFeat( Feat );
}

int (__fastcall * GetEpicWeaponSpecializationOriginal)( CNWSCreatureStats *, void*, CNWSItem * );
int __fastcall GetEpicWeaponSpecialization( CNWSCreatureStats * pThis, void*, CNWSItem * itm ){

	DWORD baseType = itm == NULL ? 256 : itm->it_baseitemtype;
	WORD Feat = Weapons.WB->GetEpicWeaponSpecializationFeat( baseType );

	if( Feat == 0 )
		return 0;

	return pThis->HasFeat( Feat );
}

int (__fastcall * GetWeaponImprovedCriticalOriginal)( CNWSCreatureStats *, void*, CNWSItem * );
int __fastcall GetWeaponImprovedCritical( CNWSCreatureStats * pThis, void*, CNWSItem * itm ){

	DWORD baseType = itm == NULL ? 256 : itm->it_baseitemtype;
	WORD Feat = Weapons.WB->GetImprovedCriticalFeat( baseType );

	if( Feat == 0 )
		return 0;

	return pThis->HasFeat( Feat );
}

int (__fastcall * GetEpicWeaponDevastatingCriticalOriginal)( CNWSCreatureStats *, void*, CNWSItem * );
int __fastcall GetEpicWeaponDevastatingCritical( CNWSCreatureStats * pThis, void*, CNWSItem * itm ){

	DWORD baseType = itm == NULL ? 256 : itm->it_baseitemtype;
	WORD Feat = Weapons.WB->GetDevastatingCirticalHitFeat( baseType );

	if( Feat == 0 )
		return 0;

	if( Weapons.DevCritBehavior == 1 && pThis->HasFeat( Feat ) ){

		Weapons.ResetArguments( );

		Weapons.OBJ_IDS[0]=itm->obj.obj_generic.obj_id;
		Weapons.OBJ_IDS[1]=pThis->cs_original->cre_attack_target;

		(*NWN_VirtualMachine)->Runscript( &CExoString( Weapons.DevCritScript ), pThis->cs_original->obj.obj_generic.obj_id );

		return 0;
	}
	else if( Weapons.DevCritBehavior == 2 || Weapons.DevCritBehavior == 3 )
		return 0;

	return pThis->HasFeat( Feat );
}

int (__fastcall * GetEpicWeaponOverwhelmingCriticalOriginal)( CNWSCreatureStats *, void*, CNWSItem * );
int __fastcall GetEpicWeaponOverwhelmingCritical( CNWSCreatureStats * pThis, void*, CNWSItem * itm ){

	DWORD baseType = itm == NULL ? 256 : itm->it_baseitemtype;
	WORD Feat = Weapons.WB->GetOverwhelmingCirticalHitFeat( baseType );

	if( Feat == 0 )
		return 0;

	return pThis->HasFeat( Feat );
}

int (__fastcall * GetIsWeaponOfChoiceOriginal)( CNWSCreatureStats *, void*, DWORD );
int __fastcall GetIsWeaponOfChoice( CNWSCreatureStats * pThis, void*, DWORD baseType ){

	WORD Feat = Weapons.WB->GetWeaponOfChoiceFeat( baseType );

	if( Feat == 0 )
		return 0;

	return pThis->HasFeat( Feat );
}

int (__fastcall * CheckProficienciesOriginal)( CNWSCreature *, void*, CNWSItem *, DWORD );
int __fastcall CheckProficiencies( CNWSCreature * pThis, void*, CNWSItem * itm, DWORD arg1 ){

	if( arg1 != 16 && arg1 != 32 ){

		return CheckProficienciesOriginal( pThis, NULL, itm, arg1 );
	}

	if( itm == NULL )
		return 1;

	WeaponBehavior::WeaponEntry * wData = Weapons.WB->GetWeaponType( itm->it_baseitemtype );
	
	Ownership * OWN = Weapons.GetOwnership( itm->obj.obj_generic.obj_id );

	if( OWN ){

		if( OWN->bound && OWN->owner != pThis->obj.obj_generic.obj_id )
			return 0;
		else if( OWN->owner == pThis->obj.obj_generic.obj_id )
			return 1;
	}

	if( !wData || wData->Proficiencies == NULL )
		return 1;

	for( int n=0;n<wData->numProficiencies;n++ ){

		if( pThis->cre_stats->HasFeat( wData->Proficiencies[n] ) )
			return 1;
	}

	return 0;
}

int (__fastcall * GetCriticalHitMultiplierOriginal)( CNWSCreatureStats *, void*, int );
int __fastcall GetCriticalHitMultiplier( CNWSCreatureStats * pThis, void*, int arg1 ){

	int nBase = 2;
	int nAlt;
	DWORD BaseItemType = 256;

	CNWSItem * itm = NULL;

	if( arg1 != 1 ){

		itm = pThis->cs_original->cre_equipment->GetItemInSlot( 16 );
	}
	else{

		itm = pThis->cs_original->cre_equipment->GetItemInSlot( 32 );
	}

	if( itm != NULL ){

		BaseItemType = itm->it_baseitemtype;
	}
	
	nAlt = Weapons.WB->GetCriticalHitDamageMod( BaseItemType ); 

	if( nAlt > 0 )
		nBase = nAlt;
	else{

		DWORD * BasePTR = Weapons.GetDefaultBaseItemTypeInfo( BaseItemType, 29 );
		if( BasePTR )
			nBase = (int)*(BYTE*)BasePTR;
	}

	if( pThis->HasFeat( 883 ) && pThis->GetIsWeaponOfChoice( BaseItemType ) )
		nBase++;

	if( Weapons.DevCritBehavior == 2 ){

		WORD devFeat = Weapons.WB->GetDevastatingCirticalHitFeat( BaseItemType );
		
		if( devFeat > 0 && pThis->HasFeat( devFeat ) )
			nBase++;
	}

	return nBase;
}

int (__fastcall * GetCriticalHitRollOriginal)( CNWSCreatureStats *, void*, int );
int __fastcall GetCriticalHitRoll( CNWSCreatureStats * pThis, void*, int arg1 ){

	CNWSItem * itm = NULL;

	BYTE nBase = 0;
	BYTE nRange;

	if( arg1 == 1 ){

		itm = pThis->cs_original->cre_equipment->GetItemInSlot( 32 );
	}
	else{

		itm = pThis->cs_original->cre_equipment->GetItemInSlot( 16 );
	}

	if( itm == NULL )
		itm = pThis->cs_original->cre_equipment->GetItemInSlot( 8 );

	DWORD BaseItemType = itm == NULL ? 256 : itm->it_baseitemtype;
	CNWSItemProperty * itmprop;
	nRange = Weapons.WB->GetCriticalHitThreat( BaseItemType );

	if( nRange <= 0 ){

		DWORD * basePTR = Weapons.GetDefaultBaseItemTypeInfo( BaseItemType, 28 );

		if( basePTR )
			nRange = *(BYTE*)basePTR;
	}

	if( itm != NULL && itm->GetPropertyByType( &itmprop, 43, 0 ) ){

		nBase = nRange;
	}

	if( pThis->HasFeat( 885 ) && pThis->GetIsWeaponOfChoice( BaseItemType ) ){

		nBase += 2;
	}

	if( Weapons.DevCritBehavior == 3 ){

		WORD devFeat = Weapons.WB->GetDevastatingCirticalHitFeat( BaseItemType );
		
		if( devFeat > 0 && pThis->HasFeat( devFeat ) )
			nBase += 2;
	}

	if( GetWeaponImprovedCritical( pThis, NULL, itm ) )
		nBase += nRange;

	return 21 - nBase - nRange;
}

void ApplyHooks( void ){
	
	int HasConfig = 0;

	for( int n=0; n<12; n++ ){

		if( Weapons.WB->HasListPopulated[n] == 1 )
			HasConfig = 1;
	}

	if( HasConfig == 0 )
		return;

	if( Weapons.WB->HasListPopulated[0] )
		Weapons.OverwriteFunction( 0x00481210, GetWeaponFinesse, (PVOID*)&GetWeaponFinesseOriginal );
	
	if( Weapons.WB->HasListPopulated[2] )
		Weapons.OverwriteFunction( 0x004812E0, GetWeaponFocus, (PVOID*)&GetWeaponFocusOriginal );
	
	if( Weapons.WB->HasListPopulated[3] )
		Weapons.OverwriteFunction( 0x00481510, GetEpicWeaponFocus, (PVOID*)&GetEpicWeaponFocusOriginal );
	
	if( Weapons.WB->HasListPopulated[4] )
		Weapons.OverwriteFunction( 0x004817E0, GetWeaponSpecialization, (PVOID*)&GetWeaponSpecializationOriginal );
	
	if( Weapons.WB->HasListPopulated[5] )	
		Weapons.OverwriteFunction( 0x00481AA0, GetEpicWeaponSpecialization, (PVOID*)&GetEpicWeaponSpecializationOriginal );
	
	if( Weapons.WB->HasListPopulated[7] )
		Weapons.OverwriteFunction( 0x00481D70, GetWeaponImprovedCritical, (PVOID*)&GetWeaponImprovedCriticalOriginal );
	
	if( Weapons.WB->HasListPopulated[8] )
		Weapons.OverwriteFunction( 0x00481FB0, GetEpicWeaponOverwhelmingCritical, (PVOID*)&GetEpicWeaponOverwhelmingCriticalOriginal );
	
	if( Weapons.WB->HasListPopulated[9] )
		Weapons.OverwriteFunction( 0x00482280, GetEpicWeaponDevastatingCritical, (PVOID*)&GetEpicWeaponDevastatingCriticalOriginal );
	
	if( Weapons.WB->HasListPopulated[6] )
		Weapons.OverwriteFunction( 0x0048CB10, GetIsWeaponOfChoice, (PVOID*)&GetIsWeaponOfChoiceOriginal );
	
	if( Weapons.WB->HasListPopulated[10] )
		Weapons.OverwriteFunction( 0x004C0440, CheckProficiencies, (PVOID*)&CheckProficienciesOriginal );

	if( Weapons.WB->HasListPopulated[11] )
		Weapons.OverwriteFunction( 0x00477A20, GetCriticalHitMultiplier, (PVOID*)&GetCriticalHitMultiplierOriginal );
	
	if( Weapons.WB->HasListPopulated[12] )
		Weapons.OverwriteFunction( 0x004778B0, GetCriticalHitRoll, (PVOID*)&GetCriticalHitRollOriginal );

	/*DWORD memDefault;
	unsigned char * ASM;
	
	if( Weapons.WB->HasListPopulated[1] ){
	
		Weapons.OverwriteFunction( 0x0049E370, GetCanUseMonkAbilities, (PVOID*)&GetCanUseMonkAbilitiesOriginal );
		Weapons.OverwriteFunction( 0x00470730, GetCanUseMonkAttackTable, (PVOID*)&GetCanUseMonkAttackTableOriginal );
		Weapons.OverwriteFunction( 0x004BB4D0, SetCombatMode, (PVOID*)&SetCombatModeOriginal );

		ASM = (unsigned char*)Weapons.FindAddress( 0x0052FCC4 );

		if( ASM == 0 ){

			Weapons.Log( "! Flurry of Blows (0x0052FCC4): NULL address!\n" );
		}
		else{

			VirtualProtect( ASM, 1, PAGE_EXECUTE_READWRITE, &memDefault );
			ASM[0]=0x0F;
			VirtualProtect( ASM, 1, memDefault, NULL );
		}

		ASM = (unsigned char*)Weapons.FindAddress( 0x00530F74 );
		
		if( ASM == 0 ){

			Weapons.Log( "! Flurry of Blows (0x00530F74): NULL address!\n" );
		}
		else{
			VirtualProtect( ASM, 2, PAGE_EXECUTE_READWRITE, &memDefault );
			ASM[0]=0xEB;
			ASM[1]=0x56;
			VirtualProtect( ASM, 2, memDefault, NULL );
		}

		ASM = (unsigned char*)Weapons.FindAddress( 0x00530FC4 );
		
		if( ASM == 0 ){

			Weapons.Log( "! Flurry of Blows (0x00530FC4): NULL address!\n" );
		}
		else{
			VirtualProtect( ASM, 8, PAGE_EXECUTE_READWRITE, &memDefault );
			ASM[0]=0xB8;
			ASM[1]=0x00;
			ASM[2]=0x40;
			ASM[3]=0x00;
			ASM[4]=0x00;
			ASM[5]=0x90;
			ASM[6]=0x90;
			ASM[7]=0x90;
			VirtualProtect( ASM, 8, memDefault, NULL );
		}
	}*/
}