#ifndef _NX_NWN_STRUCT_CNWMESSAGE_
#define _NX_NWN_STRUCT_CNWMESSAGE_

struct CNWMessage_s {
	void *vftable;
	void *field_04;
	unsigned __int32 field_08;
	unsigned __int32 field_0C;
	void *field_10;
	unsigned __int32 field_14;
	unsigned __int32 field_18;
	unsigned __int32 field_1C;
	unsigned __int8 field_20;
	unsigned __int8 field_21;
	unsigned __int8 field_22;
	unsigned __int8 field_23;
	unsigned __int32 field_24;
	unsigned __int32 field_28;
	unsigned __int32 field_2C;
	unsigned __int32 field_30;
	unsigned __int32 field_34;
	unsigned __int32 field_38;
	unsigned __int32 field_3C;
	unsigned __int32 field_40;
	unsigned __int8 field_44;
	char field_45;

	int 		ReadBOOL();
	int			ReadCResRef(CResRef *ResRef, int a3);
	int 		MessageReadOverflow(int a2);
	int			CreateWriteMessage( DWORD length, DWORD recipient, int flag );
	int			GetWriteMessage( char ** data, DWORD * length );
	int			WriteINT( int i, int flag );
	int			WriteCExoString( CExoString str, int flag );
	int			WriteBYTE( BYTE b, int flag );	
	int			WriteDWORD( DWORD dw, int flag );
	int			WriteVOIDPtr( void * ptr, DWORD size );
	int			WriteBool(int nbool);
};

void NWNX_DestroyMessage( CNWMessage * mess );
CNWMessage * NWNX_CreateMessage( int nLen );

#endif