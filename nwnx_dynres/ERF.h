/***************************************************************************
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ***************************************************************************/

#if !defined(_ERF_H_)
#define _ERF_H_
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>

//Taken from ResMan
enum NwnResType{

	NwnResType_RES		= 0x0000,
	NwnResType_BMP		= 0x0001,
	NwnResType_MVE		= 0x0002,
	NwnResType_TGA		= 0x0003,
	NwnResType_WAV		= 0x0004,
	NwnResType_PLT		= 0x0006,
	NwnResType_INI		= 0x0007,
	NwnResType_BMU		= 0x0008,
	NwnResType_MPG		= 0x0009,
	NwnResType_TXT		= 0x000A,
	NwnResType_PLH		= 0x07D0,
	NwnResType_TEX		= 0x07D1,
	NwnResType_MDL		= 0x07D2,
	NwnResType_THG		= 0x07D3,
	NwnResType_FNT		= 0x07D5,
	NwnResType_LUA		= 0x07D7,
	NwnResType_SLT		= 0x07D8,
	NwnResType_NSS		= 0x07D9,
	NwnResType_NCS		= 0x07DA,
	NwnResType_MOD		= 0x07DB,
	NwnResType_ARE		= 0x07DC,
	NwnResType_SET		= 0x07DD,
	NwnResType_IFO		= 0x07DE,
	NwnResType_BIC		= 0x07DF,
	NwnResType_WOK		= 0x07E0,
	NwnResType_2DA		= 0x07E1,
	NwnResType_TLK		= 0x07E2,
	NwnResType_TXI		= 0x07E6,
	NwnResType_GIT		= 0x07E7,
	NwnResType_BTI		= 0x07E8,
	NwnResType_UTI		= 0x07E9,
	NwnResType_BTC		= 0x07EA,
	NwnResType_UTC		= 0x07EB,
	NwnResType_DLG		= 0x07ED,
	NwnResType_ITP		= 0x07EE,
	NwnResType_BTT		= 0x07EF,
	NwnResType_UTT		= 0x07F0,
	NwnResType_DDS		= 0x07F1,
	NwnResType_UTS		= 0x07F3,
	NwnResType_LTR		= 0x07F4,
	NwnResType_GFF		= 0x07F5,
	NwnResType_FAC		= 0x07F6,
	NwnResType_BTE		= 0x07F7,
	NwnResType_UTE		= 0x07F8,
	NwnResType_BTD		= 0x07F9,
	NwnResType_UTD		= 0x07FA,
	NwnResType_BTP		= 0x07FB,
	NwnResType_UTP		= 0x07FC,
	NwnResType_DTF		= 0x07FD,
	NwnResType_GIC		= 0x07FE,
	NwnResType_GUI		= 0x07FF,
	NwnResType_CSS		= 0x0800,
	NwnResType_CCS		= 0x0801,
	NwnResType_BTM		= 0x0802,
	NwnResType_UTM		= 0x0803,
	NwnResType_DWK		= 0x0804,
	NwnResType_PWK		= 0x0805,
	NwnResType_BTG		= 0x0806,
	NwnResType_UTG		= 0x0807,
	NwnResType_JRL		= 0x0808,
	NwnResType_SAV		= 0x0809,
	NwnResType_UTW		= 0x080A,
	NwnResType_4PC		= 0x080B,
	NwnResType_SSF		= 0x080C,
	NwnResType_HAK		= 0x080D,
	NwnResType_NWM		= 0x080E,
	NwnResType_BIK		= 0x080F,
	NwnResType_PTM		= 0x0811,
	NwnResType_PTT		= 0x0812,
	NwnResType_ERF		= 0x270D,
	NwnResType_BIF		= 0x270E,
	NwnResType_KEY		= 0x270F,

	//
	// The following are not Bioware IDS but used internally
	// in this software
	//

	NwnResType_Unknown	= 0xFFFFFFFF,
};

struct ERFHeader{
	char FileType[4];
	char Version[4];
	DWORD LanguageCount;
	DWORD LocalizedStringSize;
	DWORD EntryCount;
	DWORD OffsetToLocalizedString;
	DWORD OffsetToKeyList;
	DWORD OffsetToResourceList;
	DWORD BuildYear;
	DWORD BuildDay;
	DWORD DescriptionStrRef;
	BYTE Reserved[116];
};

class LocalizedString{

public:
	DWORD LanguageID;
	DWORD StringSize;
	char * String;

	LocalizedString();
	~LocalizedString();
};

struct KeyEntry{

	char ResRef[16];
	DWORD ResID;
	WORD ResType;
	WORD Unused;
};

struct ResEntry{

	DWORD OffsetToResource;
	DWORD ResourceSize;
};

class ERF{

public:

	ERFHeader head;

	ERF( const char * file );
	~ERF();
	bool ERFRead( void * dest, DWORD unit_size, DWORD count, DWORD Offset );
	LocalizedString * GetString( DWORD nIndex, LocalizedString * dest );
	KeyEntry * GetKey( DWORD nIndex, KeyEntry * dest );
	ResEntry GetResEntry( KeyEntry * key );
	void * GetResource( KeyEntry * key );
	bool ExtractResource( KeyEntry * key, const char * folder );
	const char * NwnGetResTypeExtension( WORD nType );

private:

	FILE * fFile;
};

#endif