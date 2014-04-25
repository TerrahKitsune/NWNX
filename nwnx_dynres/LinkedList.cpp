#include "LinkedList.h"

LinkedList::LinkedList(){

	memset( this, 0, sizeof( LinkedList ) );
}

LinkedList::LinkedList( EntryDeleteFunction function ){

	memset( this, 0, sizeof( LinkedList ) );
	func = function;
}

LinkedList::~LinkedList(){

	CURSOR = FIRST;
	while( Delete( ) );
}

LinkedList::LLEntry * LinkedList::Add( unsigned int dataLen, void * dataptr ){

	LLEntry * node = (LLEntry*)calloc( 1, sizeof( LLEntry ) ); 

	if( !node )return NULL;

	if( FIRST ){

		node->PREV = LAST;
		node->NEXT = NULL;

		LAST->NEXT = node;
		LAST = node;
	}
	else{

		node->PREV = NULL;
		node->NEXT = NULL;

		FIRST = node;
		LAST = node;
	}
	
	node->data = dataptr;
	node->dataLen = dataLen;

	size++;

	return node;
}

LinkedList::LLEntry * LinkedList::Add( LinkedList::LLEntry * entryPtr ){

	LLEntry * node = entryPtr;

	if( !node )return NULL;

	if( FIRST ){

		node->PREV = LAST;
		node->NEXT = NULL;

		LAST->NEXT = node;
		LAST = node;
	}
	else{

		node->PREV = NULL;
		node->NEXT = NULL;

		FIRST = node;
		LAST = node;
	}
	
	size++;

	return node;
}

LinkedList::LLEntry * LinkedList::GetCursor( void ){

	return CURSOR;
}

bool LinkedList::Delete( void ){

	if( !CURSOR )return false;

	if( CURSOR == FIRST ){
		FIRST = CURSOR->NEXT;
	}

	if( CURSOR == LAST ){
		LAST = CURSOR->PREV;
	}

	if( CURSOR->PREV )
		CURSOR->PREV->NEXT = CURSOR->NEXT;		
	
	if( CURSOR->NEXT ) 
		CURSOR->NEXT->PREV = CURSOR->PREV;
	
	if( ReverseTraversal )
		DELETEDNEXT = CURSOR->PREV;
	else
		DELETEDNEXT = CURSOR->NEXT;

	if( func ){
	
		int nReturn = func( CURSOR );

		if( nReturn > 0 ){
			CURSOR = DELETEDNEXT;
			size--;
			return true;
		}
	}
	else{
		if(CURSOR->data)free( CURSOR->data );
		free( CURSOR );
	}

	CURSOR = DELETEDNEXT;
	size--;

	return true;
}

LinkedList::LLEntry * LinkedList::GetFirst( bool first, bool Reverse ){

	if( Reverse )
		CURSOR = LAST;
	else
		CURSOR = FIRST;

	ReverseTraversal=Reverse;

	return CURSOR;
}

LinkedList::LLEntry * LinkedList::GetNext( ){
	
	if( DELETEDNEXT ){

		CURSOR = DELETEDNEXT;
		DELETEDNEXT = NULL;
	}
	else if( !CURSOR )return NULL;
	else if( ReverseTraversal )
		CURSOR = CURSOR->PREV;
	else
		CURSOR = CURSOR->NEXT;

	return CURSOR;
}

unsigned int LinkedList::GetSize(void){
	return size;
}

LinkedList::LLEntry * LinkedList::Set( unsigned int dataLen, void * dataptr ){

	if( !CURSOR )return NULL;

	CURSOR->data = dataptr;
	CURSOR->dataLen = dataLen;

	return CURSOR;
}

LinkedList::LLEntry * LinkedList::Set( LinkedList::LLEntry * entryPtr ){

	if( !CURSOR )return NULL;

	CURSOR->data = entryPtr->data;
	CURSOR->dataLen = entryPtr->dataLen;

	return CURSOR;
}

LinkedList::LLEntry * LinkedList::Insert( unsigned int dataLen, void * dataptr ){

	if( DELETEDNEXT ){

		CURSOR=DELETEDNEXT;
		DELETEDNEXT=NULL;
	}

	if( !CURSOR )return NULL;

	LLEntry * node = (LLEntry*)calloc( 1, sizeof( LLEntry ) ); 
	if( !node )return NULL;

	LLEntry * next = NULL;
	LLEntry * prev = next;

	node->data=dataptr;
	node->dataLen=dataLen;

	//CURSOR->PREV -> <reverse> CURSOR <forward> -> CURSOR->NEXT

	if( ReverseTraversal ){

		if( CURSOR->PREV )
			CURSOR->PREV->NEXT=node;

		node->PREV = CURSOR->PREV;
		node->NEXT = CURSOR;

		CURSOR->PREV = node;
	}
	else{

		if( CURSOR->NEXT )
			CURSOR->NEXT->PREV=node;

		node->NEXT = CURSOR->NEXT;
		node->PREV = CURSOR;

		CURSOR->NEXT = node;
	}

	CURSOR = node;
	size++;

	return CURSOR;
}

LinkedList::LLEntry * LinkedList::Insert( LLEntry * entryPtr ){

	if( DELETEDNEXT ){

		CURSOR=DELETEDNEXT;
		DELETEDNEXT=NULL;
	}

	if( !CURSOR )return NULL;

	LLEntry * node = entryPtr; 
	if( !node )return NULL;

	LLEntry * next = NULL;
	LLEntry * prev = next;

	//CURSOR->PREV -> <reverse> CURSOR <forward> -> CURSOR->NEXT

	if( ReverseTraversal ){

		if( CURSOR->PREV )
			CURSOR->PREV->NEXT=node;

		node->PREV = CURSOR->PREV;
		node->NEXT = CURSOR;

		CURSOR->PREV = node;
	}
	else{

		if( CURSOR->NEXT )
			CURSOR->NEXT->PREV=node;

		node->NEXT = CURSOR->NEXT;
		node->PREV = CURSOR;

		CURSOR->NEXT = node;
	}

	CURSOR = node;
	size++;

	return CURSOR;
}