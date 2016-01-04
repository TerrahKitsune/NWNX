#include "NWNXSpellbook.h"
#include "nwnx_memory.h"
#include <string.h>
#include <stdio.h>

NWNXSpellBook::NWNXSpellBook( void ){

	Len=0;
	Alloc=0;
	Matrix=NULL;
	Resize( 16 );
}

NWNXSpellBook::~NWNXSpellBook( ){

	Resize( 0 );
}

Spell ** NWNXSpellBook::Resize( unsigned int newSize ){

	CNWNXMemory mem;

	if( Matrix == 0 ){

		Matrix = (Spell**)mem.nwnx_calloc( newSize, sizeof( Spell* ) );
		if( Matrix != NULL )
			Alloc=newSize;
		Len = 0;
		return Matrix;
	}
	else if( newSize == 0 ){

		CNWNXMemory mem;

		for( unsigned int n=0;n<Alloc;n++ ){

			if( Matrix[n] != NULL )
				mem.nwnx_free( Matrix[n] );
		}

		Alloc = 0;
		Len = 0;
		mem.nwnx_free( Matrix );
		Matrix = NULL;

		return NULL;
	}
	else if( newSize <= Alloc )
		return Matrix;

	Spell ** Temp = (Spell**)mem.nwnx_calloc( newSize, sizeof( Spell* ) );

	if( Temp == NULL )
		return NULL;
	
	if( Len > 0 )
		memcpy( Temp, Matrix, Len*sizeof( Spell* ) );
	
	mem.nwnx_free( Matrix );

	Matrix = Temp;
	Alloc = newSize;

	return Matrix;
}

Spell * NWNXSpellBook::SetSpell( Spell Addition, int nSpellslot, int DoNotCreateExtraSpellslots ){

	CNWNXMemory mem;

	if( nSpellslot < 0 ){

		for( unsigned int n=0; n<Len;n++ ){

			if( Matrix[n] == NULL ){
				nSpellslot = n;
				break;
			}
		}
	}

	if( nSpellslot < 0 ){

		if( DoNotCreateExtraSpellslots )
			return NULL;

		if( Len == Alloc )
			Resize( Alloc+1 );

		if( Matrix[Len] == NULL )
			Matrix[Len] = (Spell*)mem.nwnx_calloc( 1, sizeof( Spell ) );

		memcpy( Matrix[Len], &Addition, sizeof( Spell ) );
		Len++;
	}
	else{

		if( nSpellslot < 0 || (unsigned int)nSpellslot >= Len ){

			return NULL;
		}

		if( Matrix[nSpellslot] == NULL )
			Matrix[nSpellslot] = (Spell*)mem.nwnx_calloc( 1, sizeof( Spell ) );

		memcpy( Matrix[nSpellslot], &Addition, sizeof( Spell ) );
	}

	return Matrix[Len];
}

bool NWNXSpellBook::RemoveSpell( int nSpellslot ){

	CNWNXMemory mem;

	if( nSpellslot < 0 || (unsigned int)nSpellslot >= Len ){

		return false;
	}

	if( Matrix[nSpellslot] == NULL )
		return true;

	mem.nwnx_free( Matrix[nSpellslot] );
	Matrix[nSpellslot]=NULL;

	return true;
}

Spell * NWNXSpellBook::GetSpell( int nSpellslot ){

	if( nSpellslot < 0 || (unsigned int)nSpellslot >= Len ){

		return NULL;
	}

	return Matrix[nSpellslot];
}

int NWNXSpellBook::GetSpellsMemorized( void ){

	int nNum = 0;

	for( unsigned int n=0;n<Alloc;n++ ){

		if( Matrix[n]!=NULL )
			nNum++;
	}

	return nNum;
}

void NWNXSpellBook::EmptySpellbook( int EatCharges ){

	if( EatCharges > 0 ){

		for( unsigned int n=0;n<Len;n++ ){

			if( Matrix[n]!=NULL ){
				Matrix[n]->Ready = 0;	
			}
		}	
		return;
	}

	for( unsigned int n=0;n<Len;n++ ){

		if( Matrix[n]!=NULL ){
			RemoveSpell( n );
		}
	}
}

int NWNXSpellBook::PackSpellbookToString( char * output ){

	if( Alloc == 0 || Len == 0 || Matrix == NULL )
		return 0;

	int nReturn=0;
	output[0]='\0';

	for( unsigned int n=0;n<Len;n++ ){
	
		if( Matrix[n] != NULL ){

			nReturn++;
			sprintf( output, "%s%i %u %u %u|", output, Matrix[n]->SpellID, Matrix[n]->Ready, Matrix[n]->MetaMagic ,Matrix[n]->Domain );
		}
	}

	unsigned long outLen = strlen(output);

	if( outLen > 0 )
		output[outLen-1]='\0';

	return nReturn;
}

int NWNXSpellBook::UnPackSpellbookToString( char * input, int DoNotCreateExtraSpellslots ){

	int nReturn = 0;
	Spell SA = {0};
	int start=0, loop=0;
	char current;

	EmptySpellbook( 0 );

	while( true ){

		if( input[loop]=='|' || input[loop]=='\n' || input[loop]=='\0' ){

			current=input[loop];
			input[loop]='\0';

			sscanf( &input[start], "%i %u %u %u", &SA.SpellID, &SA.Ready, &SA.MetaMagic, &SA.Domain );			
			SetSpell( SA, -1, DoNotCreateExtraSpellslots ); 

			input[loop]=current;

			if( input[loop]=='\0' )
				break;

			nReturn++;
			start=loop;
			if( input[start]=='|' )
				start++;
		}
		loop++;	
	}

	return nReturn;
}
