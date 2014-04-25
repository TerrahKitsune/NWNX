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

#if !defined(NWNXMESSAGES_H_)
#define NWNXMESSAGES_H_
#define _CRT_SECURE_NO_WARNINGS

#include "../NWSERVER/types.h"
#include "../NWNXdll/NWNXBase.h"
#include "../NWSERVER/nwnx_memory.h"
#include "LetterScramble.h"

#define OBJECT_INVALID 0x7F000000


class CNWNXMessages : public CNWNXBase  
{
public:

	~CNWNXMessages();

	BOOL OnCreate(const char* LogDir);
	char* OnRequest(char *gameObject, char* Request, char* Parameters);
	unsigned long OnRequestObject (char *gameObject, char* Request);
	BOOL OnRelease();
	void Log( const char * formatting, ... );
	void WriteLogHeader( );
	int SendhakList( CNWSPlayer * ply );
	int SendMessageToPlayer( CNWSObject * ply, char * text, int nType );
	int SendCCMessage( CNWSObject * ply, char * text );
	int ScrambleMessage( char * text );
	int LoadLanguages( const char * path );
	int GetLanguageID( char * text );
	int GetLanguageLevel( char * text );
	int GetLanguageName( char * text );
	int GetLanguageIDInIndex( char * text );
	CNWNXMemory mem;

	CExoString * TLK;
	CExoString * HAK;
	CExoString * LASTSTR;
	int HAKLen;

	ScrambleSet * SS;
	DWORD SSLen;

	int LOGLEVEL;

protected:

private:

};

#endif