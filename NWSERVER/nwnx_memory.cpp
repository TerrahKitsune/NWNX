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

	nwnx_realloc = (void * (__cdecl *)(void *, unsigned int))0x006052EC;

	nwnx_calloc = (void * (__cdecl *)(unsigned int, unsigned int))0x00603F18;
}