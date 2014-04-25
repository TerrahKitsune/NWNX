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

#if !defined(NWNLUALIB__)
#define NWNLUALIB__

#include "NWNXLua.h"

void *nwn_alloc (void *ud, void *ptr, size_t osize, size_t nsize);
static int NWNX_Print (lua_State *L) ;

#define OBJECT_INVALID 0x7F000000

void RegisterNWNLib( lua_State *L );
void stackDump(lua_State *L);

#endif