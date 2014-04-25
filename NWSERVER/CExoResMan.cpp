#include "types.h"
#include "nwn_internals.h"

int (__thiscall *CExoResMan__Exists)(CExoResMan *pThis, CResRef *, unsigned short, unsigned long *) = (int (__thiscall*)(CExoResMan *pThis, CResRef *, unsigned short, unsigned long *))0x00416BE0;
int (__thiscall *CExoResMan__FreeChunk)(CExoResMan *pTHIS) = (int (__thiscall*)(CExoResMan *pTHIS))0x00416D10;
int (__thiscall *CExoResMan__ServiceFromResFile)(CExoResMan *pTHIS, void *a2, int a3) = (int(__thiscall*)(CExoResMan *pTHIS, void *a2, int a3))0x00417B30;
void * (__thiscall *CExoResMan__GetTable)( CExoResMan * pThis, CRes * res ) = (void * (__thiscall *)( CExoResMan *, CRes * ))0x00417350;

int CExoResMan_s::Exists(CResRef *ResRef, unsigned short a2, unsigned long *a3) {
	return CExoResMan__Exists(this, ResRef, a2, a3);
}

int CExoResMan_s::FreeChunk() {
	return CExoResMan__FreeChunk(this);
}

int CExoResMan_s::ServiceFromResFile(void *a2, int a3)  {
	return CExoResMan__ServiceFromResFile(this, a2, a3);
}

void * CExoResMan_s::GetTable( CRes * res ){
	return CExoResMan__GetTable( this, res );
}


int (__thiscall *dtorCRes)(void * pThis) = (int (__thiscall *)(void *))0x00416160;
int (__thiscall *ctorCRes)(void * pThis) = (int (__thiscall *)(void *))0x00416030;
unsigned int (__thiscall *CRes__CancelRequest)(void * pThis) = (unsigned int (__thiscall *)(void *))0x00416240;
unsigned int (__thiscall *CRes__Demand)(void * pThis) = (unsigned int (__thiscall *)(void *))0x00416250;
unsigned int (__thiscall *CRes__Dump)(void * pThis, int a2) = (unsigned int (__thiscall *)(void *,int))0x00416260;
signed int (__thiscall *CRes__Release)(void * pThis) = (signed int (__thiscall *)(void *))0x004162A0;
signed int (__thiscall *CRes__Request)(void * pThis) = (signed int (__thiscall *)(void *))0x004162B0;

CRes::CRes( ){
	
	ctorCRes( this );
}

CRes::~CRes( ){

	dtorCRes( this );
}

void * CRes::GetData( ){

	return fileContents;
}

unsigned short CRes::CancelRequest( ){

	return CRes__CancelRequest( this );
}

unsigned int CRes::Demand( ){

	return CRes__Demand( this );
}

unsigned int CRes::Dump( ){

	return CRes__Dump( this, 0 );
}

signed int CRes::Release( ){

	return CRes__Release( this );
}

signed int CRes::Request( ){

	return CRes__Request( this );
}

signed int CRes::OnResourceServiced( ){

	return 1;
}

signed int CRes::GetFixedResourceSize( ){

	return -1;
}