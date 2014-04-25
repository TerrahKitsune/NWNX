/***************************************************************************
    Created by Robin "Terrah" Karlsson (emailarbra@gmail.com)
	
	This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ***************************************************************************/

#if !defined(__BINARY__TREE__)
#define __BINARY__TREE__
#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

class BinaryTree{

public:

	static const int TRAVERSAL_TYPE_INORDER = 1;
	static const int TRAVERSAL_TYPE_PREORDER = 2;
	static const int TRAVERSAL_TYPE_POSTORDER = 3;
	//static const int TRAVERSAL_TYPE_LEVELORDER = 4;

	struct BTEntry{

		BTEntry * RIGHT;
		BTEntry * LEFT;
		int nID;
		unsigned int dataLen;
		void * data;
	};

	typedef int (_stdcall * EntryFunction)( BTEntry * );

	BinaryTree();
	BinaryTree( EntryFunction func );
	~BinaryTree();
	
	//Add data, not copied
	BTEntry * Add( int ID, unsigned int dataLen, void * dataptr );
	//Add entry, not copied
	BTEntry * Add( BTEntry * entryPtr );
	//Traverses the tree in desired order, poping traversal proc on the tree nodes.
	//Retuns the current node if the traversalproc returns non-zero
	BTEntry * Traverse( int TRAVERSAL_TYPE, EntryFunction traversalproc );
	//Attempts finding a node with ID starting on node, if node is null it'll start on root
	//if ignorestartnode is true it'll only look for the id in the undernodes
	BTEntry * Find( int ID, BTEntry * node, bool ignorestartnode ); 
	//Destroy a node, this will destroy all undernodes too
	//if node is NULL it'll use root
	//Retruns the number of nodes destroyed
	//Runs registered EntryFunction for deallocation if registered
	//if no EntryFunction is registered it attempts standard deallocation
	int DeleteBranch( BTEntry * node );
	//Destroys the node and re-inserts all undernodes back into the tree
	//Runs the registered function on the deletion or deallocates as per default if no function is registered
	//This rebuilds the entire tree and will be resource costly in larger trees
	void DeleteNode( BTEntry * node );

	//Start a first/next stack at start node, if start is null it starts at root
	BTEntry * GetFirst( int TRAVERSAL_TYPE, BinaryTree::BTEntry * start=NULL );
	BTEntry * GetNext( void );
	BTEntry * GetRoot( void );

	//Creates a id based of the text given
	unsigned int TextToID( const char * text, unsigned int nLen );
	unsigned int TextToID( const char * text );

	unsigned int GetSize(void); 

protected:

	BTEntry * ROOT;
	unsigned int CURSOR;
	BTEntry ** STACK;
	unsigned int CurrentStackSize;
	int Traversal;
	unsigned int Size;

	EntryFunction proc;

	bool CreateStack( BinaryTree::BTEntry * node );
	void StackTraverse( BinaryTree::BTEntry * node );
	BTEntry * ProcTraverse( BinaryTree::BTEntry * node, EntryFunction traversalproc );
};

#endif