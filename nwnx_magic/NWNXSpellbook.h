/***************************************************************************
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

#if !defined(NWNXSSPELLS_H_)
#define NWNXSSPELLS_H_
#define _CRT_SECURE_NO_WARNINGS

struct Spell{

	int SpellID;
	unsigned int Ready;
	unsigned int MetaMagic;
	unsigned int Domain;
};

class NWNXSpellBook{

public:
	Spell ** Matrix;
	unsigned int Len;
	unsigned int Alloc;

	NWNXSpellBook( void );
	~NWNXSpellBook( );
	Spell ** Resize( unsigned int newSize );

	//Add spell to spellbook/level
	//nSpellslot will overrwrite an existing spellslot, if -1 it'll seach for the first slot
	//if DoNotCreateExtraSpellslots = 1 it will not create a spellslot if there are no available
	Spell * SetSpell( Spell Addition, int nSpellslot=-1, int DoNotCreateExtraSpellslots=0 );
	//Remove/unmemorize the spellslot
	bool RemoveSpell( int nSpellslot );
	//Get the contents in a spellslot
	Spell * GetSpell( int nSpellslot );
	//Returns the number of spells memorized
	int GetSpellsMemorized( void );
	//Pack book into a string
	int PackSpellbookToString( char * output );
	//Unpack a string to the spellbook
	//if DoNotCreateExtraSpellslots = 1 it will not create a spellslot if there are no available
	int UnPackSpellbookToString( char * input, int DoNotCreateExtraSpellslots );
	//Sets all slots to 0, doesnt alter the spellslot count
	void EmptySpellbook( int EatCharges );
};

#endif