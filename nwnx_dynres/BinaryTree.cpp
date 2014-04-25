#include "BinaryTree.h"

BinaryTree::BinaryTree(){

	ROOT=NULL;
	CURSOR=NULL;
	STACK=NULL;
	Size=0;
	proc = NULL;
}

BinaryTree::BinaryTree( EntryFunction func ){

	ROOT=NULL;
	CURSOR=NULL;
	STACK=NULL;
	Size=0;
	proc = func;
}
	
BinaryTree::~BinaryTree(){

	DeleteBranch( NULL );
	if( STACK )
		free( STACK );
}


BinaryTree::BTEntry * BinaryTree::Add( int ID, unsigned int dataLen, void * dataptr ){

	BTEntry * Node = (BTEntry*)calloc( 1, sizeof( BTEntry ) ); 
	if( !Node )
		return NULL;

	Node->data = dataptr;
	Node->dataLen = dataLen;
	Node->nID = ID;

	return Add( Node );
}

BinaryTree::BTEntry * BinaryTree::Add( BinaryTree::BTEntry * entryPtr ){

	BTEntry ** Node = NULL;

	if( !ROOT ){
		Node = &ROOT;
	}
	else{
		
		BTEntry * Trv = ROOT;
		while( Trv ){

			if( entryPtr->nID > Trv->nID ){
				Node = &Trv->RIGHT;
			}
			else{
				Node = &Trv->LEFT;
			}

			if( *Node ){
				Trv = * Node;
			}
			else{
				break;
			}
		}
	}

	if( !Node )
		return NULL;

	entryPtr->LEFT=NULL;
	entryPtr->RIGHT=NULL;

	*Node = entryPtr;

	Size++;

	return *Node;
}

BinaryTree::BTEntry * BinaryTree::GetRoot( void ){

	return ROOT;
}

unsigned int BinaryTree::TextToID( const char * text ){

	return TextToID( text, strlen( text ) );
}

unsigned int BinaryTree::TextToID( const char * text, unsigned int nLen ){

	unsigned int id = 0x123;

	for( unsigned int n = 0;n<nLen;n++ ){

		id += ((unsigned int)text[n]*n);
	}
	
	return id;
}

BinaryTree::BTEntry * BinaryTree::GetFirst( int TRAVERSAL_TYPE, BinaryTree::BTEntry * start ){

	if( !start )
		start = ROOT;

	Traversal = TRAVERSAL_TYPE;
	if( !CreateStack( start ) )return NULL;

	CURSOR=1;

	return STACK[0];
}

BinaryTree::BTEntry * BinaryTree::GetNext( void ){

	if( !STACK )return NULL; 
	else if( CURSOR >= CurrentStackSize ){

		free( STACK );
		STACK = NULL;
		return NULL;
	}

	return STACK[CURSOR++];
}

unsigned int BinaryTree::GetSize(void){

	return Size;
}

BinaryTree::BTEntry * BinaryTree::Traverse( int TRAVERSAL_TYPE, EntryFunction traversalproc ){

	Traversal = TRAVERSAL_TYPE;
	return ProcTraverse( ROOT, traversalproc );
}

int BinaryTree::DeleteBranch( BinaryTree::BTEntry * node ){

	if( !node )
		node = ROOT;

	BTEntry * find = GetFirst( TRAVERSAL_TYPE_POSTORDER, node );
	if( CurrentStackSize <= 0 )return -1;
	BTEntry ** List = (BTEntry ** )calloc( CurrentStackSize, sizeof( BTEntry * ) );
	if( !List )return -1;

	int nAdd = 0;

	while( find ){

		List[nAdd++]=find;

		find = GetNext( );
	}

	find = GetFirst( TRAVERSAL_TYPE_POSTORDER );
	ROOT=NULL;
	Size=0;
	bool ok;
	while( find ){

		ok = node!=ROOT;

		if( ok ){

			for( int n=0;n<nAdd;n++ ){

				if( List[n]==find ){

					ok=false;
					break;
				}
			}
		}
		if( ok )
			Add( find );

		find = GetNext( );
	}

	for( int n=0;n<nAdd;n++ ){

		if( proc ){
			proc( List[n] );
		}
		else{
			if( List[n]->data )free( List[n]->data );
			free( List[n] );
		}
	}

	free( List );

	return nAdd;
}

void BinaryTree::DeleteNode( BinaryTree::BTEntry * node ){

	BTEntry * find = GetFirst( TRAVERSAL_TYPE_POSTORDER );
	ROOT = NULL;
	Size=0;
	while( find ){

		if( find != node ){
			Add( find );	
		}

		find = GetNext( );
	}

	if( proc ){
		proc( node );
	}
	else{
		if( node->data )free( node->data );
		free( node );
	}
}

BinaryTree::BTEntry * BinaryTree::Find( int ID, BinaryTree::BTEntry * node, bool ignorestartnode ){

	if( !node ){
		node = ROOT;
	}

	if( !node )
		return NULL;

	if( node->nID == ID && !ignorestartnode ){
		return node;
	}

	BTEntry * find = node;
	while( find ){

		if( ID > find->nID && find->RIGHT )
			find = find->RIGHT;
		else if( find->LEFT )
			find = find->LEFT;
		else
			return NULL;

		if( find->nID == ID )
			return find;
	}

	return NULL;
}

BinaryTree::BTEntry * BinaryTree::ProcTraverse( BinaryTree::BTEntry * node, EntryFunction traversalproc ){

	if( !node )
		return NULL;

	BTEntry * retrn;

	if( Traversal == TRAVERSAL_TYPE_INORDER ){

		retrn = ProcTraverse( node->LEFT, traversalproc );
		if( retrn ) return retrn;

		if( traversalproc( node ) > 0 )
			return node;

		retrn = ProcTraverse( node->RIGHT, traversalproc );
		if( retrn ) return retrn;
	}
	else if( Traversal == TRAVERSAL_TYPE_PREORDER ){
		
		if( traversalproc( node ) > 0 )
			return node;
		
		retrn = ProcTraverse( node->LEFT, traversalproc );
		if( retrn ) return retrn;

		retrn = ProcTraverse( node->RIGHT, traversalproc );
		if( retrn ) return retrn;
	}
	else if( Traversal == TRAVERSAL_TYPE_POSTORDER ){
		
		retrn = ProcTraverse( node->LEFT, traversalproc );
		if( retrn ) return retrn;

		retrn = ProcTraverse( node->RIGHT, traversalproc );
		if( retrn ) return retrn;

		if( traversalproc( node ) > 0 )
			return node;
	}
}

void BinaryTree::StackTraverse( BinaryTree::BTEntry * node ){

	if( !node )
		return;

	if( Traversal == TRAVERSAL_TYPE_INORDER ){

		StackTraverse( node->LEFT );

		STACK[CurrentStackSize++]=node;

		StackTraverse( node->RIGHT );
	}
	else if( Traversal == TRAVERSAL_TYPE_PREORDER ){
		
		STACK[CurrentStackSize++]=node;
		
		StackTraverse( node->LEFT );

		StackTraverse( node->RIGHT );
	}
	else if( Traversal == TRAVERSAL_TYPE_POSTORDER ){
		
		StackTraverse( node->LEFT );

		StackTraverse( node->RIGHT );

		STACK[CurrentStackSize++]=node;
	}	
}

bool BinaryTree::CreateStack( BinaryTree::BTEntry * node ){

	if( STACK ){
		free( STACK );
	}

	if( Size <= 0 )return false;
	

	CurrentStackSize=0;
	STACK = (BTEntry**)calloc( Size, sizeof( BTEntry* ) ); 
	if( !STACK )return false;

	StackTraverse( node );
	return true;
}