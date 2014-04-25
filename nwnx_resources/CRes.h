#ifndef __CNW_RES__
#define __CNW_RES__

#include "windows.h"

typedef struct GFFHeader{

	BYTE FileType[4];
	BYTE FileVersion[4];
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
}GFFHeader;

class CRes{

public:
	CRes( );		
	~CRes( );

	void * GetData( );
	unsigned short CancelRequest( );
	unsigned int Demand( );
	unsigned int Dump( );
	signed int Release( );
	signed int Request( );
	signed int OnResourceServiced( );
	signed int GetFixedResourceSize( );
	unsigned long LoadResource( const char * ResRef );
	void * LoadFile( const char * ResRef, unsigned long * uLen, const char * AlternativeFile );
	void SetGameEngienPointers( void * newData, unsigned int size );

	/*
	*(_DWORD *)this = &CRes___vftable_;
	*(_WORD *)(this + 4) = 0;
	*(_DWORD *)(this + 8) = -1;
	*(_WORD *)(this + 6) = 0;
	*(_DWORD *)(this + 24) = 0;
	*(_DWORD *)(this + 16) = 0;
	*(_DWORD *)(this + 20) = 0;
	*(_DWORD *)(this + 28) = 0;
	*(_DWORD *)(this + 32) = 0;
	*(_DWORD *)(this + 36) = 0;
	*(_DWORD *)(this + 40) = 0;
	*(_DWORD *)(this + 12) = 0;
	*/

	unsigned int CRes_vftable; //0x0
	unsigned short bHasContents; //0x4
	unsigned short word0x6; //0x6
	unsigned int ResPackedType; //0x8
	unsigned int fileType; //0xC
	void * fileContents; //0x10
	char * dString; //0x14
	unsigned int fileCLen; //0x18
	unsigned int bIsReady; //0x1C
	unsigned int dword0x20; //0x20
	unsigned int dword0x24; //0x24
	unsigned int dword0x28; //0x28
	unsigned int bIsReady2;	// 0x2C
	unsigned int resSize2;	// 0x30
	void * pResData2;		// 0x34
};	

#endif