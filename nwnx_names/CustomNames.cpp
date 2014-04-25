#include "CustomNames.h"
#include "nwnx_memory.h"

DynamicNames::DynamicNames( void ){
	Construct( );
}

void DynamicNames::Construct( void ){

	ObjID=0;
	Array=0;
	Len=0;
	Alloc=0;
}

Name * DynamicNames::Resize( unsigned int newSize ){

	CNWNXMemory mem;

	if( Alloc == 0 ){

		Array = (Name*)mem.nwnx_calloc( newSize, sizeof( Name ) );
		if( Array != NULL )
			Alloc=newSize;
		Len = 0;
		return (Name*)Array;
	}
	else if( newSize == 0 ){

		for( int n=0;n<Len;n++ ){

			if( Array[n].text != NULL )
				mem.nwnx_free( Array[n].text );
		}

		Alloc = 0;
		Len = 0;
		mem.nwnx_free( Array );
		Array = NULL;

		return NULL;
	}
	else if( newSize <= Alloc )
		return Array;

	Name * Temp = (Name*)mem.nwnx_calloc( newSize, sizeof( Name ) );

	if( Temp == NULL )
		return NULL;
	
	if( Len > 0 )
		memcpy( Temp, Array, Len*sizeof( Name ) );
	
	mem.nwnx_free( Array );

	Array = Temp;
	Alloc = newSize;

	return Array;
}

Name * DynamicNames::Add( DWORD ObjID, const char * name ){

	CNWNXMemory mem;

	Name * test = Get( ObjID, false );

	if( test ){

		mem.nwnx_free( test->text );
		test->len = strlen( name );
		test->text = (char*)mem.nwnx_calloc( test->len+1, 1 );
		strcpy( test->text, name );
		return test;
	}

	if( Len == Alloc ){
		if( Resize( Alloc+1 ) == NULL ){
			return NULL;
		}
	}
		
	Array[Len].ObjID = ObjID;
	Array[Len].len = strlen( name );
	Array[Len].text = (char*)mem.nwnx_calloc( Array[Len].len+1, 1 );
	strcpy( Array[Len].text, name );
	Len++;

	return &Array[Len-1];
}

bool DynamicNames::Remove( DWORD ObjID ){

	int nIndex = -1;

	for( int n=0;n<Len;n++ ){

		if( Array[n].ObjID == ObjID ){

			nIndex = n;
			break;
		}
	}

	if( nIndex == -1 )
		return false;

	CNWNXMemory mem;

	mem.nwnx_free( Array[nIndex].text );

	if( nIndex == (Len-1) ){
		memset( &Array[nIndex], 0, sizeof( Name ) );
	}
	else{
		memcpy( &Array[nIndex], &Array[nIndex+1], ((Len-1)-nIndex)*sizeof( Name ) );
		memset( &Array[Len-1], 0, sizeof( Name ) );
	}

	Len--;

	return true;
}

Name * DynamicNames::Get( DWORD ObjID, bool GetDefault ){

	int nIndex = -1;

	for( int n=0;n<Len;n++ ){

		if( Array[n].ObjID == ObjID ){

			nIndex = n;
			break;
		}
	}

	if( nIndex == -1 && GetDefault ){
	
		for( int n=0;n<Len;n++ ){

			if( Array[n].ObjID == OBJECT_INVALID ){

				nIndex = n;
				break;
			}
		}
	}
	
	if( nIndex == -1 )
		return NULL;

	return &Array[nIndex];
}