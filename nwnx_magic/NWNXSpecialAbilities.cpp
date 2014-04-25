#include "NWNXSpecialAbilities.h"

#include <stdio.h>
#include <string.h>
#include "nwnx_memory.h"

NWNXSpecialAbilities::NWNXSpecialAbilities( void ){
	
	Alloc=0;
	Len=0;
	Resize( 12 );
}

SpecialAbility * NWNXSpecialAbilities::Resize( unsigned int newSize ){

	CNWNXMemory mem;

	if( Alloc == 0 ){

		Array = (SpecialAbility*)mem.nwnx_calloc( newSize, sizeof( SpecialAbility ) );
		if( Array != NULL )
			Alloc=newSize;
		Len = 0;
		return (SpecialAbility*)Array;
	}
	else if( newSize == 0 ){

		Alloc = 0;
		Len = 0;
		mem.nwnx_free( Array );
		Array = NULL;

		return NULL;
	}
	else if( newSize <= (unsigned int)Alloc )
		return Array;

	SpecialAbility * Temp = (SpecialAbility*)mem.nwnx_calloc( newSize, sizeof( SpecialAbility ) );

	if( Temp == NULL )
		return NULL;
	
	if( Len > 0 )
		memcpy( Temp, Array, Len*sizeof( SpecialAbility ) );
	
	mem.nwnx_free( Array );

	Array = Temp;
	Alloc = newSize;

	return Array;
}

SpecialAbility * NWNXSpecialAbilities::Insert( SpecialAbility Data ){

	if( Len == Alloc )
		Resize( Alloc+1 );

	memcpy( &Array[Len], &Data, sizeof( SpecialAbility ) );
	Len++;

	return &Array[Len-1];
}

SpecialAbility * NWNXSpecialAbilities::GetDataInIndex( unsigned int nIndex ){

	if( nIndex < 0 || nIndex >= (unsigned int)Len ){

		return NULL;
	}

	return &Array[nIndex];
}

SpecialAbility * NWNXSpecialAbilities::SetDataInIndex( unsigned int nIndex, SpecialAbility Data ){

	if( nIndex < 0 || nIndex >= (unsigned int)Len ){

		return NULL;
	}

	memcpy( &Array[nIndex], &Data, sizeof( SpecialAbility ) );

	return &Array[nIndex];
}

bool NWNXSpecialAbilities::Remove( unsigned int nIndex ){

	if( nIndex < 0 || nIndex >= (unsigned int)Len ){

		return false;
	}

	if( nIndex == (Len-1) ){
		memset( &Array[nIndex], 0, sizeof( SpecialAbility ) );
	}
	else{
		memcpy( &Array[nIndex], &Array[nIndex+1], ((Len-1)-nIndex)*sizeof( SpecialAbility ) );
		memset( &Array[Len-1], 0, sizeof( SpecialAbility ) );
	}

	Len--;

	return true;
}

int NWNXSpecialAbilities::PackSpecialAbilitiesToString( char * output ){

	if( Alloc == 0 || Len == 0 || Array == NULL )
		return 0;

	int nReturn=0;
	output[0]='\0';

	for( int n=0;n<Len;n++ ){
	
		nReturn++;
		sprintf( output, "%s%u %u %u\n", output, Array[n].Spell, Array[n].Ready, Array[n].Level );
	}

	return nReturn;
}

int NWNXSpecialAbilities::UnPackSpecialAbilitiesToString( char * input ){

	int nReturn = 0;
	SpecialAbility SA = {0};
	int start=0, loop=0, end=0;

	unsigned long level;

	while( true ){

		if( input[loop]=='\n' || input[loop]=='\0' || input[loop]=='|' ){

			if( input[loop]=='\0' )
				end=1;

			sscanf( &input[start], "%u %u %u", &SA.Spell, &SA.Ready, &level );			
			SA.Level = (unsigned char)level;

			Insert( SA );

			if( end )
				break;

			input[loop]='\n';
			nReturn++;
			start=loop;
		}
		loop++;	
	}

	return nReturn;
}