#include "types.h"
#include "nwn_internals.h"
#include <stdio.h>
#include <ctype.h>

CResRef* (__thiscall *CResRef__CResRef)(CResRef *pTHIS, CExoString const &) = (CResRef* (__thiscall*)(CResRef *pTHIS, CExoString const &))0x00415AD0;

CResRef *CResRef::CResRef(CExoString const &Ref) {
	return CResRef__CResRef(this, Ref);
}

CResRef * CResRef::Set( const char * text ){

	memset( resref, 0, 16 );

	for( int n=0;n<16;n++ ){

		if( text[n]==0 )
			break;
		else
			resref[n]=tolower( text[n] );
	}
}