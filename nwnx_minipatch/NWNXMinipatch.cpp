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

#include "NWNXMinipatch.h"
#include "../NWNXdll/madCHook.h"

#pragma comment(lib, "madChook.lib")

extern CNWNXMinipatch minipatch;

void (__fastcall *CNWSObject__ClearAllPartyInvalidActionsNext)( CNWSObject * obj, void *);
void __fastcall CNWSObject__ClearAllPartyInvalidActions( CNWSObject * obj, void *){
	
	if( obj != NULL && obj->obj_generic.obj_type2 == CGameObject__OBJECT_TYPE_CREATURE )
		return;

	CNWSObject__ClearAllPartyInvalidActionsNext( obj, NULL );
}

int (__fastcall * CNWSItem__MergeItemNext)( CNWSItem * pThis, void*, CNWSItem * itm );
int __fastcall CNWSItem__MergeItem( CNWSItem * pThis, void*, CNWSItem * itm ){

	DWORD (__thiscall *CNWBaseItemArray__GetBaseItem)(void *pTHIS, DWORD arg1) = (DWORD (__thiscall*)(void *pTHIS, DWORD arg1))0x00512AF0;

	DWORD * g_pRules = (DWORD*)0x0066C060;

	DWORD baseItem = CNWBaseItemArray__GetBaseItem( (void*)(*(DWORD *)(*g_pRules + 0x18)), pThis->it_baseitemtype );

	WORD StackSizeMax = *(WORD *)(baseItem + 36);

	if( StackSizeMax <= 1 )
		return 0;
	else
		StackSizeMax = 0xFFFF;

	if( !pThis->CompareItem( itm ) ){
		return 0;
	}

	DWORD PrimaryStack = pThis->it_stacksize;
	DWORD NewSize = itm->it_stacksize + PrimaryStack;

	if( NewSize > StackSizeMax ){

		pThis->it_stacksize = StackSizeMax;
		itm->it_stacksize += PrimaryStack - StackSizeMax;
		return 0;
	}

	pThis->it_stacksize = NewSize;
	return 1;
}

BOOL CNWNXMinipatch::OnRelease( ){

	return CNWNXBase::OnRelease();
}

BOOL CNWNXMinipatch::OnCreate(const char* LogDir){

	char log[MAX_PATH];
	
	sprintf( log, "%s\\nwnx_minipatch.txt", LogDir);

	// call the base class function
	if( !CNWNXBase::OnCreate( log ) )
		return false;

	WriteLogHeader( );
	RemoveCopyItemAndModifyColorCap( );
	if( HookCode( (PVOID)0x004DFA60, CNWSObject__ClearAllPartyInvalidActions, (PVOID*)&CNWSObject__ClearAllPartyInvalidActionsNext ) )
		Log( "o Hooked ClearPartyInvalidActions!\n" );
	else
		Log( "! Failed to hook ClearPartyInvalidActions!\n" );

	if( HookCode( (PVOID)0x004FEC40, CNWSItem__MergeItem, (PVOID*)&CNWSItem__MergeItemNext ) )
		Log( "o Hooked MergeItem!\n" );
	else
		Log( "! Failed to hook MergeItem!\n" );

	RemoveChatListenRestrictions( );
	RemoveSetStacksizeCap( );

	return true;
}

void CNWNXMinipatch::WriteLogHeader( ){

	fprintf( m_fFile, "NWNXMinipatch v0.5 created by Terra_777\n\n" );
	fflush( m_fFile );	
}

void CNWNXMinipatch::Log( const char * formatting, ... ){

	va_list argList;
	char *pos;

	if( m_fFile ){  

		if( ftell( m_fFile ) > m_maxLogSizeKB ){

			fclose( m_fFile );
			m_fFile = fopen( m_LogFile, "w" );
			WriteLogHeader( );
			fprintf( m_fFile, "o Logfile hit maximum size limit, starting again.\n" );
		}

		// build up the string
		va_start( argList, formatting );
		vsprintf( acBuffer, formatting, argList );
		va_end( argList );

		// replace any percent signs
		pos = strchr( acBuffer, '%' );
		while( pos ){

			*pos = '~';
			pos = strchr( acBuffer, '%' );
		}

		fprintf( m_fFile, acBuffer );
		fflush( m_fFile );
	}
}

void CNWNXMinipatch::RemoveCopyItemAndModifyColorCap( void ){

	//Absorlute address
	DWORD * CopyItemAndModifyCap = (DWORD*)0x0058A872;
	DWORD DefaultPrivs;
	unsigned char * func = (unsigned char*)CopyItemAndModifyCap;

	//Make the memoryspace writeable
	VirtualProtect( CopyItemAndModifyCap, 1, PAGE_EXECUTE_READWRITE, &DefaultPrivs );
	
	//0x0058A871: BA [04] 00 00 00: mov edx 0x04
	func[0]=0xFF; //<- set cap to 255

	//Restore the privs back to default
	VirtualProtect( CopyItemAndModifyCap, 1, DefaultPrivs, NULL );

	Log( "o Removed CopyItemAndModify weapon color cap!\n" );
}

void CNWNXMinipatch::RemoveSetStacksizeCap( void ){

	DWORD privs;
	unsigned char * func = (unsigned char*)0x0058E1CA;

	VirtualProtect( func, 4, PAGE_EXECUTE_READWRITE, &privs ); 
	//0058E1C9      66:8B48 24    MOV CX,WORD PTR DS:[EAX+24]
	//0058E1C9      66:B9 FFFF    MOV CX,7FFF
	func[0]=0xB9;
	func[1]=0xFF;
	func[2]=0xFF;

	VirtualProtect( func, 4, privs, NULL );

	Log( "o Removed SetStackSize cap!\n" );
}

void CNWNXMinipatch::RemoveChatListenRestrictions( ){

	DWORD privs;
	unsigned char * func = (unsigned char*)0x0043CD01;

	VirtualProtect( func, 7, PAGE_EXECUTE_READWRITE, &privs ); 

	func[0]=0xEB;
	func[1]=0x2A;
	func[2]=0x90;
	func[3]=0x90;
	func[4]=0x90;
	func[5]=0x90;
	func[6]=0x90;

	VirtualProtect( func, 7, privs, NULL );

	func = (unsigned char*)0x0043CED1;

	VirtualProtect( func, 7, PAGE_EXECUTE_READWRITE, &privs ); 

	func[0]=0xEB;
	func[1]=0x2A;
	func[2]=0x90;
	func[3]=0x90;
	func[4]=0x90;
	func[5]=0x90;
	func[6]=0x90;

	VirtualProtect( func, 7, privs, NULL );

	Log( "o RemoveChatListenRestrictions: visible checks removed from chat!\n" );
}
/*----------------
--Script interface
----------------*/

unsigned long CNWNXMinipatch::OnRequestObject (char *gameObject, char* Request){

	Log( "o OnRequestObject: 0x08X: %s( )", gameObject, Request );

	return OBJECT_INVALID;
}

char* CNWNXMinipatch::OnRequest(char *gameObject, char* Request, char* Parameters){

	Log( "o OnRequest: 0x08X: %s( %s )", gameObject, Request, Parameters );
	
	return NULL;
}

/*----------------
--Patch functions
----------------*/