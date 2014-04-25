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

#if !defined(__LINKED__LIST__)
#define __LINKED__LIST__
#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>




class LinkedList{

public:

	struct LLEntry{

		LLEntry * NEXT;
		LLEntry * PREV;
		int nID;
		unsigned int dataLen;
		void * data;
	};

	typedef int (_stdcall * EntryDeleteFunction)( LLEntry * );

	LinkedList();
	LinkedList( EntryDeleteFunction );
	~LinkedList();
	
	//Add data, not copied
	LLEntry * Add( unsigned int dataLen, void * dataptr );
	//Add entry, not copied
	LLEntry * Add( LLEntry * entryPtr );
	//Sets the current positions data
	//Not copied
	LLEntry * Set( unsigned int dataLen, void * dataptr );
	//Sets the current positions data
	//Not copied
	LLEntry * Set( LLEntry * entryPtr );
	//Inserts a node at the current position
	//Not copied
	LLEntry * Insert( unsigned int dataLen, void * dataptr );
	//Inserts a node at the current position
	//Not copied
	LLEntry * Insert( LLEntry * entryPtr );
	//Delete the current node we're on
	//Advances the node to the next one
	//Runs EntryDeleteFunction if registered, if none it attempts standard memory deallocation
	//Returns 1 on success or if the registed function returned somehting other then 0
	bool Delete( void );

	LLEntry * GetFirst( bool first, bool Reverse );
	LLEntry * GetNext( void );
	LLEntry * GetCursor( void );

	unsigned int GetSize(void);

protected:

	LLEntry * FIRST;
	LLEntry * LAST;
	LLEntry * CURSOR;
	LLEntry * DELETEDNEXT;
	unsigned int size;
	bool ReverseTraversal;

	EntryDeleteFunction func;	
};

#endif