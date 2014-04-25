#ifndef _CEXORESMAN_H_
#define _CEXORESMAN_H_

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

struct CExoResMan_s {
  /* 0x0/0 */ unsigned long field_0;
  /* 0x4/4 */ unsigned long TotalResourceMemory;
  /* 0x8/8 */ unsigned long FreeMemory;
  /* 0xC/12 */ unsigned long field_C;
  /* 0x10/16 */ void *KeyTable1;
  /* 0x14/20 */ void *KeyTable2;
  /* 0x18/24 */ void *KeyTable3;
  /* 0x1C/28 */ void *KeyTable4;
  /* 0x20/32 */ void *ToBeFreedList;
  /* 0x24/36 */ void *ResList2;
  /* 0x28/40 */ unsigned long field_28;
  /* 0x2C/44 */ unsigned long field_2C;
  /* 0x30/48 */ unsigned long SuspendServicing;
  /* 0x34/52 */ unsigned long field_34;
  /* 0x38/56 */ unsigned long field_38;
  /* 0x3C/60 */ unsigned long field_3C;
  /* 0x44/68 */ char rsvd1[4];
  /* 0x44/68 */ unsigned long field_44;

	int Exists(CResRef *ResRef, unsigned short a2, unsigned long *a3);
	int FreeChunk();
	int ServiceFromResFile(void *a2, int a3);
	void * GetTable( CRes * res );
};

#endif