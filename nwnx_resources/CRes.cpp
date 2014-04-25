#define _CRT_SECURE_NO_WARNINGS
#include "CRes.h"
#include "NWNXResources.h"
#include "string.h"
extern CNWNXResources Resources;

int (__thiscall *dtorCRes)(void * pThis) = (int (__thiscall *)(void *))0x00416160;
int (__thiscall *ctorCRes)(void * pThis) = (int (__thiscall *)(void *))0x00416030;
unsigned int (__thiscall *CRes__CancelRequest)(void * pThis) = (unsigned int (__thiscall *)(void *))0x00416240;
unsigned int (__thiscall *CRes__Demand)(void * pThis) = (unsigned int (__thiscall *)(void *))0x00416250;
unsigned int (__thiscall *CRes__Dump)(void * pThis, int a2) = (unsigned int (__thiscall *)(void *,int))0x00416260;
signed int (__thiscall *CRes__Release)(void * pThis) = (signed int (__thiscall *)(void *))0x004162A0;
signed int (__thiscall *CRes__Request)(void * pThis) = (signed int (__thiscall *)(void *))0x004162B0;

CRes::CRes( ){
	
	ctorCRes( this );
}

CRes::~CRes( ){

	dtorCRes( this );
}

void * CRes::LoadFile( const char * ResRef, unsigned long * uLen, const char * AlternativeFile ){

	if( AlternativeFile == NULL ){
		
		const char * BasePath = Resources.GetPathFromType( Resources.LastType );

		//Makesure the path will fit
		if( BasePath == NULL || ( strlen( BasePath ) + strlen( ResRef ) ) >= 260 )
			return NULL;

		//Format the path
		sprintf( Resources.LastFile, "%s%s", BasePath, ResRef );

	}
	else
		strcpy( Resources.LastFile, AlternativeFile );

	//Open the file
	FILE * pFile = fopen( Resources.LastFile, "rb" );
	
	//Failed to open
	if( pFile == NULL ){
		*uLen=1;
		return NULL;
	}

	//Check length
	fseek( pFile, 0, SEEK_END );
	unsigned long nSize = ftell( pFile );
	rewind( pFile );

	//Close if the file is empty
	if( nSize <= 0 ){
		
		fclose( pFile );
		return NULL;
	}

	//New memory and fill it with the file
	void * vNew = Resources.nwnx_malloc( nSize );

	//Bail
	if( vNew == NULL ){

		fclose( pFile );
		return NULL;
	}

	if( fread( vNew, 1, nSize, pFile ) < nSize ){

		Resources.nwnx_free( vNew );
		fclose( pFile );
		return NULL;
	}

	//Close
	fclose( pFile );

	*uLen = nSize;
	return vNew;
}

unsigned long CRes::LoadResource( const char * ResRef ){

	//Look for a behavior for this unique resource
	NWRes * ResE = Resources.GetBehaviorForRes( ResRef );
	unsigned char Behavior = 0;
	bool blog = true;
	if( ResE != NULL )
		Behavior = ResE->Behavior;
	else if( Resources.Behavior==1 ){
		return 0;
	}

	void * vNew = NULL;
	unsigned long nSize = 0;

	//This is cache
	if( Behavior == 1 ){

		//Load the data if it wasnt loaded
		if( ResE->Data <= 0 ){

			ResE->vPtr = LoadFile( ResRef, &nSize, NULL );

			if( nSize <= 1 ){
				nSize = 0;
				Resources.Log( 1, "o Unable to cache: file not found: %s\n", Resources.LastFile );
			}
			else
				Resources.Log( 1, "o Cached External File: %s\n", Resources.LastFile, nSize );

			ResE->Data = nSize;
		}
		else
			Resources.Log( 1, "o Loaded Cached Resource: %s: %lu bytes\n", ResE->cResRef, ResE->Data );

		vNew = Resources.nwnx_malloc( ResE->Data );
		memcpy( vNew, ResE->vPtr, ResE->Data );
		nSize = ResE->Data;
	}
	//Ignore, don't load
	else if( Behavior == 2 ){

		//Return 1 without setting anything will cause the game to ignore it
		this->fileContents = NULL;
		this->fileCLen = 0;
		this->bHasContents = 1;
		this->fileType = 4;
		this->bIsReady = 1;
		this->resSize2 = 0;
		this->pResData2 = NULL;
		this->bIsReady2 = 1;

		Resources.Log( 1, "o Ignored: %s\n", ResE->cResRef );

		return 1;
	}
	//Load default
	else if( Behavior == 3 ){

		Resources.Log( 1, "o Forced Internal Loading: %s\n", ResE->cResRef );
		return 0;
	}
	//Load from a completely different file
	else if( Behavior == 4 ){

		vNew = LoadFile( ResRef, &nSize, (const char*)ResE->vPtr );

		if( nSize <= 1 ){
		
			if( nSize == 0 )
				Resources.Log( 1, "o Unable to load: %s\n", (const char*)ResE->vPtr );
			else if( nSize == 1 )
				Resources.Log( 1, "o Remote file not found: %s\n", (const char*)ResE->vPtr );

			this->fileContents = NULL;
			this->fileCLen = 0;
			this->bHasContents = 1;
			this->fileType = 4;
			this->bIsReady = 1;
			this->resSize2 = 0;
			this->pResData2 = NULL;
			this->bIsReady2 = 1;

			return 1;
		}

	}
	//Default, load external file
	else{

		vNew = LoadFile( ResRef, &nSize, NULL );

		if( nSize <= 1 ){
		
			Resources.Log( 2, "o Loaded Internal File: %s\n", ResRef );
		}
		else{
			Resources.Log( 2, "o Loaded External File: %s\n", Resources.LastFile );		
		}
	}

	//Set all the game-engien pointers for us
	SetGameEngienPointers( vNew, nSize );

	return nSize;
}

void CRes::SetGameEngienPointers( void * newData, unsigned int size ){

	//Don't do this if len is 1 or less
	if( size <= 1 )
		return;

	//Free the old memory if any
	if( this->fileContents != NULL )
		Resources.nwnx_free( this->fileContents );

	this->fileContents = newData;
	this->fileCLen = size;
	this->bHasContents = 1;
	this->fileType |= 0x10;
	this->bIsReady = 1;

	this->resSize2 = size;
	this->pResData2 = newData;
	this->bIsReady2 = 1;
	
	//Set the res object
	//SetResObject( this, Resources.ResRef,  Resources.LastType );	

	//call the respective OnServiced events
	int (__fastcall *CRes_OnServiced)(CRes * cRes) = (int (__fastcall *)(CRes* cRes))(DWORD*)(*(DWORD*)(this->CRes_vftable+0x10));
	this->fileType = this->fileType & 0xFFFFFFFB | (CRes_OnServiced( this ) != 0 ? 4 : 0);
	this->fileType &= 0xFFFFFFEF;
}

void * CRes::GetData( ){

	return fileContents;
}

unsigned short CRes::CancelRequest( ){

	return CRes__CancelRequest( this );
}

unsigned int CRes::Demand( ){

	return CRes__Demand( this );
}

unsigned int CRes::Dump( ){

	return CRes__Dump( this, 0 );
}

signed int CRes::Release( ){

	return CRes__Release( this );
}

signed int CRes::Request( ){

	return CRes__Request( this );
}

signed int CRes::OnResourceServiced( ){

	return 1;
}

signed int CRes::GetFixedResourceSize( ){

	return -1;
}