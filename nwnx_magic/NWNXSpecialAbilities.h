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

#if !defined(NWNXSPECIALABILITIES_H_)
#define NWNXSPECIALABILITIES_H_
#define _CRT_SECURE_NO_WARNINGS

struct SpecialAbility{

	unsigned long Spell;
	unsigned long Ready;
	unsigned char Level;
};

class NWNXSpecialAbilities{

public:

	NWNXSpecialAbilities( void );

	SpecialAbility * Array;
	int Len;
	int Alloc;

	SpecialAbility * Resize( unsigned int newSize );
	SpecialAbility * Insert( SpecialAbility Data );
	SpecialAbility * GetDataInIndex( unsigned int nIndex );
	SpecialAbility * SetDataInIndex( unsigned int nIndex, SpecialAbility Data );
	bool Remove( unsigned int nIndex );
	int PackSpecialAbilitiesToString( char * output );
	int UnPackSpecialAbilitiesToString( char * input );
};


#endif