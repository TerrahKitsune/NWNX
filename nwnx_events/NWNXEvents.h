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

#if !defined(NWNXMPATCH_H_)
#define NWNXMPATCH_H_
#define _CRT_SECURE_NO_WARNINGS

#include "../NWSERVER/types.h"
#include "../NWNXdll/NWNXBase.h"

#define OBJECT_INVALID 0x7F000000

#define EVENT_SPELLCAST 0
#define EVENT_ITEMCAST 1
#define EVENT_FEATUSE 2
#define EVENT_SKILLUSE 3
#define EVENT_POLYMORPH 4
#define EVENT_UNPOLYMORPH 5
#define EVENT_ATTACK 6
#define EVENT_TOGGLEMODE 7
#define EVENT_DEVCRIT 8
#define EVENT_VALIDATE 9
#define EVENT_REMOVEPC 10
#define NUM_EVENTS 11

struct EventParams{

	int EVENT;
	int BYPASS;
	nwn_objid_t oTarget[2];
	Vector lTarget[2];
	int nData[5];
	CExoString sData[2];
};

class CNWNXEvents : public CNWNXBase  
{
public:


	~CNWNXEvents(){OnRelease();}
	BOOL OnCreate(const char* LogDir);
	char* OnRequest(char *gameObject, char* Request, char* Parameters);
	unsigned long OnRequestObject (char *gameObject, char* Request);
	BOOL OnRelease();
	void Log( const char * formatting, ... );
	void WriteLogHeader( );
	int FireEvent( nwn_objid_t OBJECT_SELF );

	CNWNXMemory mem;
	CExoString * Script;

	EventParams * EP;

protected:

private:
	
	bool InScript;
	int LOG;
};

#endif