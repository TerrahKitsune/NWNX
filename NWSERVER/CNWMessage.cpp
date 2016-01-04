#include "types.h"
#include "nwn_internals.h"

int (__thiscall *CNWMessage__ReadBOOL)(void *pTHIS) = (int(__thiscall*)(void *pTHIS))0x005078C0;
int (__thiscall *CNWMessage__ReadCResRef)(void *pTHIS, CResRef *a2, signed int a3) = (int (__thiscall*)(void *pTHIS, CResRef *a2, signed int a3))0x00507BC0;
int (__thiscall *CNWMessage__MessageReadOverflow)(void *pTHIS, int a2) = (int (__thiscall*)(void *pTHIS, int a2))0x00507DD0;
int (__thiscall *CNWMessage__CreateWriteMessage)( void * pThis, DWORD length, DWORD recipient, int flag ) = (int (__thiscall*)(void*, DWORD, DWORD, int))0x00507E30;
int (__thiscall *CNWMessage__GetWriteMessage)( void * pThis, char ** data, DWORD * length ) = (int (__thiscall*)(void*,char**,DWORD*))0x00508B80;
CNWMessage * (__thiscall * CNWMessage__CNWSMessage)( void * pThis ) = (CNWMessage * (__thiscall *)( void * ) )0x00507030;
int (__thiscall * CNWMessage__WriteINT)( void *pTHIS, int n, int flag)= (int (__thiscall *)( void *, int , int))0x005084F0;
int (__thiscall * CNWMessage__WriteCExoString)( void *pTHIS, CExoString str, int flag)= (int (__thiscall *)( void *, CExoString , int))0x00508900;
int (__thiscall * CNWMessage___vector_deleting_destructor_)( CNWMessage * pTHIS, uint8_t Flag) = (int (__thiscall * )( CNWMessage * , uint8_t ))0x005070A0;
int (__thiscall * CNWMessage__WriteBYTE)( void *pTHIS, BYTE b, int flag ) = (int (__thiscall *)( void *, BYTE , int))0x00507FE0;
int (__thiscall * CNWMessage__WriteDWORD)( void *pTHIS, DWORD dw, int flag ) = (int (__thiscall *)( void *, DWORD , int))0x00508450;
int (__thiscall * CNWMessage__WriteVOIDPtr)( void *pTHIS, void * ptr, DWORD size) = (int (__thiscall *)( void *, void* , DWORD))0x00508AD0;

int(__stdcall *CNWMessage__WriteBOOL)(int a1) = (int(__stdcall *)(int))0x00507FC0;

int	CNWMessage_s::WriteBool(int nbool){
	return CNWMessage__WriteBOOL(nbool);
}

CNWMessage * NWNX_CreateMessage( int nLen ){

	CNWNXMemory mem;
	CNWMessage * ret = (CNWMessage*)mem.nwnx_malloc( sizeof( CNWMessage_s ) );
	if( !ret )
		return NULL;

	int n = 0;

	CNWMessage__CNWSMessage( ret );

	CNWMessage__CreateWriteMessage( ret, nLen, -1, n);

	return ret;
}

void NWNX_DestroyMessage( CNWMessage * mess ){

	if( !mess )
		return;

	CNWNXMemory mem;
	CNWMessage___vector_deleting_destructor_( mess, 1 );
	mem.nwnx_free( mess );
}

int	CNWMessage_s::WriteBYTE( BYTE b, int flag ){
	return CNWMessage__WriteBYTE(this,b,flag);
}
int	CNWMessage_s::WriteDWORD( DWORD dw, int flag ){
	return CNWMessage__WriteDWORD(this,dw,flag);
}
int	CNWMessage_s::WriteVOIDPtr( void * ptr, DWORD size ){
	return CNWMessage__WriteVOIDPtr(this,ptr,size);
}

int	CNWMessage_s::WriteINT( int i, int flag ){
	return CNWMessage__WriteINT( this, i, flag );
}

int	CNWMessage_s::WriteCExoString( CExoString str, int flag ){
	return CNWMessage__WriteCExoString( this, str, flag );
}

int CNWMessage_s::ReadCResRef(CResRef *ResRef, int a3) {
	return CNWMessage__ReadCResRef(this, ResRef, a3);
}

int CNWMessage_s::MessageReadOverflow(int a2) {
	if (this->field_30 <= this->field_2C)
		return this->field_38 < this->field_3C;
	return 1;
	//return CNWMessage__MessageReadOverflow(this, a2);
}
	
int CNWMessage_s::CreateWriteMessage( DWORD length, DWORD recipient, int flag ) {
	return CNWMessage__CreateWriteMessage(this, length, recipient, flag);
}

int CNWMessage_s::GetWriteMessage( char ** data, DWORD * length ){
	return CNWMessage__GetWriteMessage(this, data, length );
}

int CNWMessage_s::ReadBOOL() {
	return CNWMessage__ReadBOOL(this);
}