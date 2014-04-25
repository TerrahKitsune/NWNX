//Hook handler header

#ifndef __RES_HOOKS__
#define __RES_HOOKS__

#include "CRes.h"

#include "NWNXResources.h"
#include <string.h>
#include <stdio.h>

void HookCNWRES( );
CRes * GetResObject( char * ResRef, unsigned short uType );
void SetResObject( CRes * nwRes, char * ResRef, unsigned short uType );

#endif