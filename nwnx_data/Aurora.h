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

#if !defined(__AURORA__H__)
#define __AURORA__H__
#define _CRT_SECURE_NO_WARNINGS

#define LABEL_MAX 16
#define RESREF_MAX 16

#include <Windows.h>
#include <stdio.h>

class GFFField;
class GFFCExoLocSubString;
class GFFCResRef;
class GFFCVoid;
class GFFCExoString;
class GFFStruct;
class GFFCList;
class GFFCExoLocString;
class GFF;
class GFFField;

typedef struct GFFHeader{

	char FileType[4];
	char FileVersion[4];
	DWORD StructOffset;
	DWORD StructCount;
	DWORD FieldOffset;
	DWORD FieldCount;
	DWORD LabelOffset;
	DWORD LabelCount;
	DWORD FieldDataOffset;
	DWORD FieldDataCount;
	DWORD FieldIndicesOffset;
	DWORD FieldIndicesCount;
	DWORD ListIndicesOffset;
	DWORD ListIndicesCount;
} GFFHeader;

class GFFPack{

public:

	GFFPack( GFF * gPack );
	~GFFPack();

	DWORD cntStructs;
	DWORD cntFields;
	DWORD cntLabels;
	DWORD cntData;
	DWORD cntFieldIndices;
	DWORD cntListIndices;
	DWORD TotalSize;
	char * RAW;
	GFF * UNPACKED;

	GFFHeader * Header;
	GFFStruct * StructArray;
	GFFField * FieldArray;
	char * LabelArray;
	BYTE * FieldDataBlock;
	DWORD * FieldIndicesArray;
	DWORD * ListIndicesArray;

protected:
	int AddStruct(GFFStruct * data);
	int AddField(GFFField * data);
	int AddLabel(char * label);
};

class GFFTrackList{

public:

	GFFTrackList();
	~GFFTrackList();

	DWORD cntStructs;
	DWORD cntFields;
	DWORD cntLabels;
	DWORD DataFieldSize;
	DWORD FieldIndicesCNT;
	DWORD ListIndicesCNT;

	GFFStruct ** structs;
	GFFField ** fields;
	char ** labels;

	void AddStruct(GFFStruct * data);
	void AddField(GFFField * data);
	void AddLabel(char * data);

	GFFHeader CreateHeader();
};

class GFFCExoLocSubString{

public:

	GFFCExoLocSubString();
	GFFCExoLocSubString( const char * str );
	~GFFCExoLocSubString();

	int StringID;
	int StringLength;
	char * str;

	bool Unpack(const char * buffer, DWORD offset);
	GFFCExoLocSubString * operator = (const char * str);
	GFFCExoLocSubString * operator += (const char * str);
	const char * c_str();
};

class GFFCResRef{

public:

	GFFCResRef();
	GFFCResRef(const char * str);
	~GFFCResRef();

	BYTE Size;
	char * str;

	bool Unpack(const char * buffer, DWORD offset);
	GFFCResRef * operator = (const char * str);
	const char * c_str();
};

class GFFCVoid{

public:

	GFFCVoid();
	GFFCVoid(void * data, DWORD size);
	~GFFCVoid();

	DWORD Size;
	BYTE * data;

	bool Unpack(const char * buffer, DWORD offset);
	void Set(void * data, DWORD size);
	void CopyTo(void* data, DWORD size);
};

class GFFCExoString{

public:

	GFFCExoString();
	~GFFCExoString();

	DWORD Size;
	char * str;

	bool Unpack(const char * buffer, DWORD offset);
};

class GFFStruct{

public:

	GFFStruct();
	~GFFStruct();

	int Type;
	GFFField * Fields;
	DWORD FieldCount;

	bool Unpack(const char * buffer, DWORD offset);
	GFFTrackList * CalculateHeader(GFFTrackList * current);
};

class GFFCList{

public:

	GFFCList();
	~GFFCList();

	DWORD Size;
	GFFStruct * list;

	bool Unpack(const char * buffer, DWORD offset);
};

class GFFCExoLocString{

public:

	GFFCExoLocString();
	GFFCExoLocString( const char * str );
	~GFFCExoLocString();

	DWORD TotalSize;
	DWORD StringRef;
	DWORD StringCount;
	GFFCExoLocSubString * SubString;

	bool Unpack(const char * buffer, DWORD offset);
};

class GFF{

public:

	GFF();
	~GFF();

	char Type[4];
	char Ver[4];

	GFFStruct * TopStruct;

	bool LoadGFFFromFile(const char * file);
};

class GFFField{

public:

	GFFField();
	~GFFField();

	int Type;
	char * Label;
	void * Data;

	bool Unpack(const char * buffer, DWORD offset);

	void DeleteData();
	const char * GetTypeString();

	BYTE AsByte();
	char AsChar();
	WORD AsWord();
	short AsShort();
	DWORD AsDword();
	int AsInt();
	DWORD64 AsDword64();
	INT64 AsInt64();
	float AsFloat();
	double AsDouble();
	GFFCExoString * AsExoString();
	GFFCResRef * AsResRef();
	GFFCExoLocString * AsExoLocString();
	GFFCVoid * AsVoid();
	GFFStruct * AsStruct();
	GFFCList * AsList();

	const char * tostring(char * buffer, int nMax);
	const char * tostring(char * buffer, int nMax, int nIndex);

	DWORD GetDataSize();
};




#endif