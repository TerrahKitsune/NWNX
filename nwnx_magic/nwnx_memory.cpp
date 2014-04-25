#include "nwnx_memory.h"
#include <string.h>

CNWNXMemory::CNWNXMemory( void ){

	SetFunctionPointers( );
}

CNWNXMemory::~CNWNXMemory( void ){

	//Nothing here yet
	return;
}
	
void CNWNXMemory::SetFunctionPointers( void ){

	//Set our nwnx memory funcs
	nwnx_malloc = (void *(__cdecl *)(unsigned int))0x0040D550;
	//nwns free routine
	nwnx_free = (void (__cdecl *)(void *))0x0040D560;
}

void * CNWNXMemory::nwnx_calloc( unsigned int num, unsigned int size ){

	void * pArray = nwnx_malloc( num*size );

	if( pArray == NULL )
		return NULL;

	memset( pArray, NULL, num*size );

	return pArray;
}

void * CNWNXMemory::nwnx_realloc( void * ptr, unsigned int size ){

	//If size is 0, just free and return null
	if( size == 0 ){

		nwnx_free( ptr );
		return NULL;
	}
	//ptr is null, behave like malloc
	else if( ptr == NULL ){

		return nwnx_malloc( size );
	}

	//Create new memory and grab the old size
	void * temp = nwnx_malloc( size );
	unsigned long oldSize = sizeof( ptr );

	//It succeeded, proceed to copy all or as much of the old memory as possible
	//And free the old memory
	if( temp != NULL ){

		memcpy( temp, ptr, oldSize > size ? size : oldSize );	
		nwnx_free( ptr );
		ptr=temp;
	}
	
	//Return the pointer holding the memory
	return ptr;
}