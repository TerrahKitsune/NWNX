/*
	nwnx_memory.h by Terra_777

	This is a class that handles memory the way nwserver does.
	Using these functions when handling memory from nwserver
	will prevent the server from crashing when working with 
	the customly allocated memory.

	The constructor or SetFunctionPointers must run first in order to set the function pointers

	The functions then behave like the C standard (malloc, calloc, realloc, free)
	but don't mix them up with the standard functions. IE using the standard free to dealloc a nwnx_malloc'd 
	block of memory.
*/

#ifndef __NWNX_MEMORY__
#define __NWNX_MEMORY__

class CNWNXMemory{

public:

	CNWNXMemory( void );
	~CNWNXMemory( void );
	
	//Sets the function pointers
	//This has to be done before these functions work
	//Its also done in the ctor
	void SetFunctionPointers( void );

	//Allocates size bytes and returns a pointer to its start location
	void * (__cdecl *nwnx_malloc)( unsigned int size );
	//Frees memory blocks allocated by nwnx_malloc/calloc/realloc
	void (__cdecl *nwnx_free)( void * ptr  );
	//Allocates num*size bytes and sets all the bytes to zero
	void * nwnx_calloc( unsigned int num, unsigned int size );
	//Reallocates ptr into a new block of memory with size and returns a new pointer
	//if ptr is null it behaves like malloc, if size is 0 it behaves like free
	void * nwnx_realloc( void * ptr, unsigned int size );
};

#endif