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

#if !defined(NWNXDATA_H_)
#define NWNXDATA_H_
#define _CRT_SECURE_NO_WARNINGS

#include "../NWNXdll/NWNXBase.h"
#include "CSQLite.h"

#define OBJECT_INVALID 0x7F000000

struct DataObject{

	char * name;
	int nType;
	void * object;
};

class CNWNXData : public CNWNXBase  
{
public:


	~CNWNXData(){OnRelease();}
	BOOL OnCreate(const char* LogDir);
	char* OnRequest(char *gameObject, char* Request, char* Parameters);
	unsigned long OnRequestObject (char *gameObject, char* Request);
	BOOL OnRelease();
	void Log( const char * formatting, ... );
	void WriteLogHeader( );

	int nLog;

	DataObject * DOArray;
	int DOArrayLen;
	DataObject * LastObject;

	DataObject * AddDataObject( DataObject * obj );	
	DataObject * GetDataObject( const char * name, int nType );
	bool CloseObject( const char * name, int nType );
	void CloseAll();

	bool OpenSQLLiteDatabase( char * input );
	bool SQLiteExec( char * input, int nFetch );
	bool SQLiteStep( char * input );
	bool SQLiteGet( char * input );

protected:

private:

};

#endif