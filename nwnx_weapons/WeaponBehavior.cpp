#include "WeaponBehavior.h"

WeaponBehavior::WeaponBehavior( void ){

	WEArray=NULL;
	Len=0;
	Size=0;
	WeaponFinesseFeat = 42;
	memset( HasListPopulated, 0, 12 );
	LoadConfiguration( NULL );
}

WeaponBehavior::~WeaponBehavior( void ){

	Resize( 0 );
}

WeaponBehavior::WeaponEntry * WeaponBehavior::Resize( int newSize ){

	if( Size == 0 ){

		WEArray = (WeaponEntry *)calloc( newSize, sizeof( WeaponEntry ) );
		if( WEArray != NULL ){
			Size=newSize;
			for( int n=0;n<newSize;n++ ){
				WEArray[n].BASE_TYPE=0xFFFFFFFF;
			}
		}
		return WEArray;
	}
	else if( newSize == 0 ){

		for( int n=0;n<Size;n++ ){

			if( WEArray[n].Proficiencies != NULL )
				free( WEArray[n].Proficiencies );
		}

		if( WEArray != NULL )
			free( WEArray );
		WEArray=NULL;
		Len=0;
		Size=0;
	}
	else if( newSize <= Size )
		return WEArray;

	WeaponEntry * Temp = (WeaponEntry*)calloc( newSize, sizeof( WeaponEntry ) );

	if( Temp == NULL )
		return NULL;
	
	if( Len > 0 )
		memcpy( Temp, WEArray, Len*sizeof( WeaponEntry ) );
	
	for( int n=Len;n<newSize;n++ ){
		Temp[n].BASE_TYPE=0xFFFFFFFF;
	}

	free( WEArray );

	WEArray = Temp;
	Size = newSize;

	return WEArray;
}

WeaponBehavior::WeaponEntry * WeaponBehavior::GetWeaponType( DWORD BASE_TYPE ){

	for( int n=0;n<Len;n++ ){

		if( WEArray[n].BASE_TYPE == BASE_TYPE )
			return &WEArray[n];
	}

	return NULL;
}

WeaponBehavior::WeaponEntry * WeaponBehavior::AddWeaponType( DWORD BASE_TYPE ){

	WeaponEntry * Weapon = GetWeaponType( BASE_TYPE );

	if( Weapon != NULL )
		return Weapon;

	if( Len == Size )
		Resize( Size+1 );

	WEArray[Len].BASE_TYPE=BASE_TYPE;
	Len++;

	return &WEArray[Len-1];
}

WeaponBehavior::WeaponEntry * WeaponBehavior::GetFirstNext( bool first ){

	if( first )
		FirstNext=0;
	else
		FirstNext++;

	if( FirstNext>=Len )
		return NULL;

	return &WEArray[FirstNext];
}

WeaponBehavior::WeaponEntry * WeaponBehavior::RemoveWeaponType( DWORD BASE_TYPE ){

	int nIndex = -1;

	for( int n=0;n<Len;n++ ){

		if( WEArray[n].BASE_TYPE == BASE_TYPE ){

			nIndex=n;
			break;
		}
	}
	
	if( nIndex < 0 || nIndex >= Len ){

		return NULL;
	}

	free( WEArray[nIndex].Proficiencies );

	if( nIndex == Len-1 ){

		memset( &WEArray[nIndex], 0, sizeof( WeaponEntry ) );
		WEArray[nIndex].BASE_TYPE=0xFFFFFFFF;
	}
	else{

		memcpy( &WEArray[nIndex], &WEArray[nIndex+1], ((Len-1)-nIndex)*sizeof( WeaponEntry ) );
		memset( &WEArray[Len-1], 0, sizeof( WeaponEntry ) );
		WEArray[Len-1].BASE_TYPE=0xFFFFFFFF;
	}
	Len--;

	return &WEArray[nIndex];
}

BYTE WeaponBehavior::GetWeaponFinessable( DWORD BASE_TYPE ){

	WeaponEntry * WE = GetWeaponType( BASE_TYPE );
	
	if( WE == NULL )
		return 0;

	return WE->FINESSABLE;
}

BYTE WeaponBehavior::GetWeaponUsesUnarmedBaseAttackBonus( DWORD BASE_TYPE ){

	WeaponEntry * WE = GetWeaponType( BASE_TYPE );
	
	if( WE == NULL )
		return 0;

	return WE->UNARMED_BASEATTACK;
}

WORD WeaponBehavior::GetWeaponFocusFeat( DWORD BASE_TYPE ){

	WeaponEntry * WE = GetWeaponType( BASE_TYPE );
	
	if( WE == NULL )
		return 0;

	return WE->WEAPON_FOCUS_FEAT;
}

WORD WeaponBehavior::GetEpicWeaponFocusFeat( DWORD BASE_TYPE ){

	WeaponEntry * WE = GetWeaponType( BASE_TYPE );
	
	if( WE == NULL )
		return 0;

	return WE->EPIC_WEAPON_FOCUS_FEAT;
}
	
WORD WeaponBehavior::GetWeaponSpecializationFeat( DWORD BASE_TYPE ){

	WeaponEntry * WE = GetWeaponType( BASE_TYPE );
	
	if( WE == NULL )
		return 0;

	return WE->WEAPON_SPECIALIZATION_FEAT;
}

WORD WeaponBehavior::GetEpicWeaponSpecializationFeat( DWORD BASE_TYPE ){

	WeaponEntry * WE = GetWeaponType( BASE_TYPE );
	
	if( WE == NULL )
		return 0;

	return WE->EPIC_WEAPON_SPECIALIZATION_FEAT;
}

WORD WeaponBehavior::GetWeaponOfChoiceFeat( DWORD BASE_TYPE ){

	WeaponEntry * WE = GetWeaponType( BASE_TYPE );
	
	if( WE == NULL )
		return 0;

	return WE->WEAPON_OF_CHOICE_FEAT;
}
	
WORD WeaponBehavior::GetImprovedCriticalFeat( DWORD BASE_TYPE ){

	WeaponEntry * WE = GetWeaponType( BASE_TYPE );
	
	if( WE == NULL )
		return 0;

	return WE->IMPROVED_CIRITCAL_FEAT;
}
	
WORD WeaponBehavior::GetOverwhelmingCirticalHitFeat( DWORD BASE_TYPE ){

	WeaponEntry * WE = GetWeaponType( BASE_TYPE );
	
	if( WE == NULL )
		return 0;

	return WE->OVERWHELMING_CRITICAL_FEAT;
}
	
WORD WeaponBehavior::GetDevastatingCirticalHitFeat( DWORD BASE_TYPE ){

	WeaponEntry * WE = GetWeaponType( BASE_TYPE );
	
	if( WE == NULL )
		return 0;

	return WE->DEVASTATING_CRITICAL_FEAT;
}

void WeaponBehavior::ParseList( WORD Feat, const char * ItemList ){

	int nLoop=0,Find=0;
	char cursor = ItemList[nLoop];
	DWORD BaseItem;
	while( cursor != '\0' ){

		if( cursor != ':' && Find == 0 ){
			
			Find=1;
			BaseItem = atoi( &ItemList[nLoop] ); 
			AddProficiencies( BaseItem, Feat );
		}
		else if( cursor == ':' )
			Find=0;

		cursor = ItemList[++nLoop];
	}
}

void WeaponBehavior::AddProficiencies( DWORD BASE_TYPE, WORD Feat ){

	WeaponEntry * WE = GetWeaponType( BASE_TYPE );
	if( WE == NULL )
		return;

	if( WE->Proficiencies == NULL ){

		WE->Proficiencies = (WORD*)calloc( 1, sizeof( WORD ) );
		WE->Proficiencies[0]=Feat;
		WE->numProficiencies=1;
	}
	else{

		WORD * temp = (WORD*)calloc( WE->numProficiencies+1, sizeof( WORD ) );
		memcpy( temp, WE->Proficiencies, WE->numProficiencies*sizeof( WORD ) );
		free( WE->Proficiencies );
		WE->Proficiencies=temp;
		WE->Proficiencies[WE->numProficiencies]=Feat;
		WE->numProficiencies++;
	}
}

void WeaponBehavior::RemoveProficiencies( DWORD BASE_TYPE, WORD Feat ){

	WeaponEntry * WE = GetWeaponType( BASE_TYPE );
	int nIndex = -1;

	if( WE == NULL )
		return;

	else if( WE->Proficiencies == NULL )
		return;

	for( int n=0;n<WE->numProficiencies;n++ ){

		if( WE->Proficiencies[n]==Feat ){

			nIndex = n;
			break;
		}
	}

	if( nIndex == -1 )
		return;

	if( nIndex == (WE->numProficiencies-1) ){
		memset( &WE->Proficiencies[nIndex], 0, sizeof( WORD ) );
	}
	else{
		memcpy( &WE->Proficiencies[nIndex], &WE->Proficiencies[nIndex+1], ((WE->numProficiencies-1)-nIndex)*sizeof( WORD ) );
		memset( &WE->Proficiencies[WE->numProficiencies-1], 0, sizeof( WORD ) );
	}

	WE->numProficiencies--;

	if( WE->numProficiencies == 0 ){

		free( WE->Proficiencies );
		WE->Proficiencies = NULL;
	}
}

BYTE WeaponBehavior::GetCriticalHitDamageMod( DWORD BASE_TYPE ){
	
	WeaponEntry * WE = GetWeaponType( BASE_TYPE );

	if( WE == NULL )
		return 0;

	return WE->CriticalDamageMod;
}

BYTE WeaponBehavior::GetCriticalHitThreat( DWORD BASE_TYPE ){
	
	WeaponEntry * WE = GetWeaponType( BASE_TYPE );

	if( WE == NULL )
		return 0;

	return WE->CriticalHitThreat;
}

WeaponBehavior::WeaponEntry * WeaponBehavior::LoadConfiguration( const char * filename ){

	char * data;
	long len;
	int lines=0;
	int n=0;
	int i=0;
	int j=0;
	char cursor;
	char substring[100];
	char name[100];

	char nwndefault[]=	"SHORTSWORD 0 1 103 633 141 671 925 65 723 509 2 2\n\
LONGSWORD 1 0 106 636 144 674 928 68 726 512 2 2\n\
BATTLEAXE 2 0 110 640 148 678 931 72 730 516 3 1\n\
BASTARDSWORD 3 0 123 652 161 690 940 85 742 528 2 2\n\
LIGHTFLAIL 4 0 114 644 152 682 935 76 734 520 2 1\n\
WARHAMMER 5 0 115 645 153 683 936 77 735 521 3 1\n\
HEAVYCROSSBOW 6 0 92 622 130 660 0 54 712 498 2 2\n\
LIGHTCROSSBOW 7 0 93 623 131 661 0 55 713 499 2 2\n\
LONGBOW 8 0 101 631 139 669 0 63 721 507 3 1\n\
LIGHTMACE 9 1 94 624 132 662 921 56 714 500 2 1\n\
HALBERD 10 0 112 642 150 680 933 74 732 518 3 1\n\
SHORTBOW 11 0 102 632 140 670 0 64 722 508 3 1\n\
TWOBLADEDSWORD 12 0 127 655 165 693 943 89 745 531 2 2\n\
GREATSWORD 13 0 107 637 145 675 929 69 727 513 2 2\n\
SMALLSHIELD 14 0 0 0 0 0 0 0 0 0 0 0\n\
TORCH 15 0 1 0 0 0 0 0 0 0 0 0\n\
ARMOR 16 0 0 0 0 0 0 0 0 0 0 0\n\
HELMET 17 0 0 0 0 0 0 0 0 0 0 0\n\
GREATAXE 18 0 111 641 149 679 932 73 731 517 3 1\n\
AMULET 19 0 0 0 0 0 0 0 0 0 0 0\n\
ARROW 20 0 0 0 0 0 0 0 0 0 0 0\n\
BELT 21 0 0 0 0 0 0 0 0 0 0 0\n\
DAGGER 22 1 90 620 128 658 920 52 710 496 2 2\n\
MISCSMALL 24 0 0 0 0 0 0 0 0 0 0 0\n\
BOLT 25 0 0 0 0 0 0 0 0 0 0 0\n\
BOOTS 26 0 0 0 0 0 0 0 0 0 0 0\n\
BULLET 27 0 0 0 0 0 0 0 0 0 0 0\n\
CLUB 28 0 43 619 47 657 919 15 709 495 2 1\n\
MISCMEDIUM 29 0 0 0 0 0 0 0 0 0 0 0 0\n\
DART 31 0 91 621 129 659 0 53 711 497 2 1\n\
DIREMACE 32 1 125 653 163 691 941 87 743 529 2 1\n\
DOUBLEAXE 33 0 126 654 164 692 942 88 744 530 3 1\n\
MISCLARGE 34 0 0 0 0 0 0 0 0 0 0 0\n\
HEAVYFLAIL 35 0 116 646 154 684 937 78 736 522 2 1\n\
GLOVES 36 1 100 630 138 668 0 62 720 506 2 1\n\
LIGHTHAMMER 37 1 113 643 151 681 934 75 733 519 3 1\n\
HANDAXE 38 1 108 638 146 676 930 70 728 514 3 1\n\
HEALERSKIT 39 0 0 0 0 0 0 0 0 0 0 0\n\
KAMA 40 1 117 647 155 685 880 79 737 523 2 1\n\
KATANA 41 0 122 651 160 689 939 84 741 527 2 2\n\
KUKRI 42 1 118 648 156 686 881 80 738 524 2 3\n\
MISCTALL 43 0 0 0 0 0 0 0 0 0 0 0\n\
MAGICROD 44 0 0 0 0 0 0 0 0 0 0 0\n\
MAGICSTAFF 45 0 0 0 0 0 0 0 0 0 0 0\n\
MAGICWAND 46 0 0 0 0 0 0 0 0 0 0 0\n\
MORNINGSTAR 47 0 95 625 133 663 922 57 715 501 2 1\n\
POTIONS 49 0 0 0 0 0 0 0 0 0 0 0\n\
QUARTERSTAFF 50 0 96 626 134 664 923 58 716 502 2 1\n\
RAPIER 51 1 104 634 142 672 926 66 724 510 2 3\n\
RING 52 0 0 0 0 0 0 0 0 0 0 0\n\
SCIMITAR 53 0 105 635 143 673 927 67 725 511 2 3\n\
SCROLL 54 0 0 0 0 0 0 0 0 0 0 0\n\
SCYTHE 55 0 121 650 159 688 938 83 740 526 4 1\n\
LARGESHIELD 56 0 0 0 0 0 0 0 0 0 0 0\n\
TOWERSHIELD 57 0 0 0 0 0 0 0 0 0 0 0\n\
SHORTSPEAR 58 0 97 627 135 665 924 59 717 503 3 1\n\
SHURIKEN 59 0 120 649 158 687 0 82 739 525 2 1\n\
SICKLE 60 1 98 628 136 666 879 60 718 504 2 1\n\
SLING 61 0 99 629 137 667 0 61 719 505 2 1\n\
THIEVESTOOLS 62 0 0 0 0 0 0 0 0 0 0 0\n\
THROWINGAXE 63 0 109 639 147 677 0 71 729 515 2 1\n\
TRAPKIT 64 0 0 0 0 0 0 0 0 0 0 0\n\
KEY 65 0 0 0 0 0 0 0 0 0 0 0\n\
LARGEBOX 66 0 0 0 0 0 0 0 0 0 0 0\n\
MISCWIDE 68 0 0 0 0 0 0 0 0 0 0 0\n\
CSLASHWEAPON 69 0 291 656 290 694 0 292 746 532 2 1\n\
CPIERCWEAPON 70 0 291 656 290 694 0 292 746 532 2 1\n\
CBLUDGWEAPON 71 0 291 656 290 694 0 292 746 532 2 1\n\
CSLSHPRCWEAP 72 0 291 656 290 694 0 292 746 532 2 1\n\
CREATUREITEM 73 0 291 656 290 694 0 292 746 532 2 1\n\
BOOK 74 0 0 0 0 0 0 0 0 0 0 0\n\
SPELLSCROLL 75 0 0 0 0 0 0 0 0 0 0 0\n\
GOLD 76 0 0 0 0 0 0 0 0 0 0 0\n\
GEM 77 0 0 0 0 0 0 0 0 0 0 0\n\
BRACER 78 1 100 630 138 668 0 62 720 506 2 1\n\
MISCTHIN 79 0 0 0 0 0 0 0 0 0 0 0\n\
CLOAK 80 0 0 0 0 0 0 0 0 0 0 0\n\
GRENADE 81 0 0 0 0 0 0 0 0 0 0 0\n\
TRIDENT 95 0 1072 1076 1073 1077 1079 1074 1078 1075 2 1\n\
BLANK_POTION 101 0 0 0 0 0 0 0 0 0 0 0\n\
BLANK_SCROLL 102 0 0 0 0 0 0 0 0 0 0 0\n\
BLANK_WAND 103 0 0 0 0 0 0 0 0 0 0 0\n\
ENCHANTED_POTION 104 0 0 0 0 0 0 0 0 0 0 0\n\
ENCHANTED_SCROLL 105 0 0 0 0 0 0 0 0 0 0 0\n\
ENCHANTED_WAND 106 0 0 0 0 0 0 0 0 0 0 0\n\
DWARVENWARAXE 108 0 952 956 953 957 959 954 958 955 3 1\n\
CRAFTMATERIALMED 109 0 0 0 0 0 0 0 0 0 0 0\n\
CRAFTMATERIALSML 110 0 0 0 0 0 0 0 0 0 0 0\n\
WHIP 111 1 993 997 994 998 1000 995 999 996 2 1\n\
INVALID 256 1 100 630 138 668 0 62 720 506 2 1\n\n\
SIMPLE 46 28:22:9:60:58:47:50:7:6:31:61\n\
DRUID 48 28:22:31:50:53:60:61:58:95\n\
ROGUE 50 28:22:31:38:7:6:50:9:0:51:11:47:61\n\
WIZARD 51 28:22:7:6:50\n\
ELF 256 1:8:51:11\n\
MONK 49 28:22:31:6:7:50:59:40:61\n\
MARTIAL 45 2:13:18:10:38:35:4:5:37:11:8:1:51:53:0:95:63\n\
EXOTIC 44 3:32:108:33:40:41:42:55:59:111:12\n\
CREATURE 289 69:70:71:72:73\n\
SMALLSHIELD 32 14:56:57";

	char defaultFileName[] = "weaponconfiguration.txt";
	FILE * file;

	if( filename == NULL ){

		filename = defaultFileName;

		file = fopen( filename, "r" );
		if( file == NULL ){
			
			file = fopen( filename, "w" );
			fprintf( file, nwndefault );
			fflush( file );
			fclose( file );
		}
		else
			fclose( file );
	}
		
	file = fopen( filename, "r" );
		
	if( file==NULL ){

		data = nwndefault;
		len = 3968;
		lines = 90;

	}
	else{

		fseek( file, 0, SEEK_END );
		len = ftell( file );
		rewind( file );

		data = new char[len+1];

		while( !feof( file ) ){

			cursor = fgetc( file );
			data[n++] = cursor;
			if( cursor == '\n' )
				lines++;
		}
		data[n]='\0';
		fclose( file );
	}
	
	if( Resize( lines+1 ) == NULL ){

		return NULL;
	}

	int phase = 0;

	for( n=0;n<len; n++ ){

		if( data[n] == '\n' || data[n] == '\0' ){

			substring[i]='\0';

			if( phase == 0 ){

				sscanf( substring, "%s %u %i %i %i %i %i %i %i %i %i %i %i", name,&WEArray[j].BASE_TYPE,&WEArray[j].FINESSABLE,&WEArray[j].WEAPON_FOCUS_FEAT,&WEArray[j].EPIC_WEAPON_FOCUS_FEAT,&WEArray[j].WEAPON_SPECIALIZATION_FEAT,&WEArray[j].EPIC_WEAPON_SPECIALIZATION_FEAT,&WEArray[j].WEAPON_OF_CHOICE_FEAT,&WEArray[j].IMPROVED_CIRITCAL_FEAT,&WEArray[j].OVERWHELMING_CRITICAL_FEAT,&WEArray[j].DEVASTATING_CRITICAL_FEAT, &WEArray[j].CriticalDamageMod, &WEArray[j].CriticalHitThreat );

				if( WEArray[j].FINESSABLE > 0 )
					HasListPopulated[0]=1;
				if( WEArray[j].WEAPON_FOCUS_FEAT > 0 )
					HasListPopulated[2]=1;
				if( WEArray[j].EPIC_WEAPON_FOCUS_FEAT > 0 )
					HasListPopulated[3]=1;
				if( WEArray[j].WEAPON_SPECIALIZATION_FEAT > 0 )
					HasListPopulated[4]=1;
				if( WEArray[j].EPIC_WEAPON_SPECIALIZATION_FEAT > 0 )
					HasListPopulated[5]=1;
				if( WEArray[j].WEAPON_OF_CHOICE_FEAT > 0 )
					HasListPopulated[6]=1;
				if( WEArray[j].IMPROVED_CIRITCAL_FEAT > 0 )
					HasListPopulated[7]=1;
				if( WEArray[j].OVERWHELMING_CRITICAL_FEAT > 0 )
					HasListPopulated[8]=1;
				if( WEArray[j].DEVASTATING_CRITICAL_FEAT > 0 )
					HasListPopulated[9]=1;
				if( WEArray[j].CriticalDamageMod > 0 )
					HasListPopulated[11]=1;
				if( WEArray[j].CriticalHitThreat > 0 )
					HasListPopulated[12]=1;
				Len++;
			}
			else{

				char ItemList[255];
				DWORD Feat;
				sscanf( substring, "%s %u %s", name, &Feat, ItemList );
				ParseList( Feat, ItemList );
				HasListPopulated[10]=1;
			}

			j++;
			i=0;
			
			if( data[n] == '\0' )
				break;
			else if( data[n] == '\n' && data[n+1] == '\n' ){

				n++;
				phase++;
			}
		}
		else{
			substring[i++]=data[n];
		}
	
	}

	return WEArray;
}