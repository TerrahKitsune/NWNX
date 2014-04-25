/***************************************************************************
    Created by Robin "Terrah" Karlsson (emailarbra@gmail.com)
	
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

#if !defined(CFILEMANAGER_H_)
#define CFILEMANAGER_H_
#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <time.h>

extern "C"{
	#include "sha256.h"
}

//The filemanager object
//see FileManager.h for more
class CFileManager;

//Type def for functions that the CFileManager object calls upon.
//CfileManager * in the call will contain the this pointer to enable the function to access object members.
//the const char * will contain the full path to the file currently pointed at.
//The fuction is required to return an int where 0 is carry on.
//If non-zero RunForEach and RunForAll will stop and return the number that was passed.
//While CopyFiles and DestroyFolder will stop if the function returns 2 or higher, skip the delete/copy if 1 and carry on if 0 or less.
typedef int (_stdcall * FileOPFunc)( CFileManager *, const char * );

class CFileManager{

public:
	CFileManager();
	CFileManager( const char * path );
	virtual ~CFileManager();

	//Formats a path to a absorlute path
	//MAX_PATH (260) is respected
	//returns 1 on success
	virtual int FormatPath( char * path );
	//fills output with the subfolder (difference between path and the basepath set in the object)
	virtual int FormatSubfolder( char * path, char * output );
	//Returns 0 if the function was not interupted
	//if the caller function returned something higher then 0 that number is returned
	//Loops though the desired filter and runs a function on them
	virtual int RunForeach( FileOPFunc function, const char * fileFilter, int nRecursive );
	//Loops though all files in the system and runs a function on each of them
	//Returns 0 if the function was not interupted
	//if the caller function returned something higher then 0 that number is returned
	virtual int RunForAll( FileOPFunc function );
	//Loops though a fileFilter copying files
	//function is run before the file is copied, if the function returns 1 the file isnt copied
	//if the function returns a value higher then 1 the function is terminated
	//returns the number of files copied, will copy entire folder trees if nRecursive is 1
	//failed is incremented by one for each file that fails to copy
	//Function can be null if no functioncall is desired
	virtual int CopyFiles( FileOPFunc function, const char * fileFilter, const char * destination, int failIfExists, int nRecursive, int * failed );
	//Loop though the available drive letters
	//Returns a uppercase character correpsonding to drive
	virtual char GetFirstNextDriveLetter( bool first );
	//Returns a pointer of the data of the file currently pointed at by the system
	//used in RunForAll RunForEach
	//Don't free/deallocate!
	virtual WIN32_FIND_DATA * GetCurrentFileData( void );
	//Loop though the files with the filefilter
	//Returns a pointed to a find data structure
	//Don't free/deallocate!
	virtual WIN32_FIND_DATA * GetFirstNextFile( const char * fileFilter, bool first );
	//Returns 0 if the folder cannot be created or found
	//Returns 1 if the folder exists or was created	
	//Returns 2 if the path isnt a folder but it does exist
	//Returns 3 if the path isnt valid (if creation was attempted, 0 otherwise)
	//This will create subfolders if required
	virtual int EnsureFolderExistance( const char * folderpath, int CreateIfMissing );
	//Set custom data in the filemanager object (this function does not allocate or deallocate memory)
	//this is useful for passing data along to the stdcall in foreach/forall
	//the data can be accessed though void * CustomData and int CustomDataLength in the filemanager object
	//the deconstructor will free newPtr if its non-zero
	//If this points to more intricate structures of data it should be handled before the object is destroyed
	//free and set to zero if the data is on longer valid!
	//Returns the data pointed at, if nLen is less then 0 the current pointer is returned
	virtual void * SetCustomData( void * newPtr, int nLen );
	//Destroys a folder its subfolders and all files, !!USE WITH CAUTION!!
	//if OnlyDestroyIfEmpty is 0 all files and subfolders will be deleted too
	//Returns the number of files deleted, 0 if the target isnt a folder or it wasnt possible to destroy
	//failed should be the address to a intiger which will hold the number of failes/folders that failed to delete
	//Function will run before deletion, if function returns 1 the deletion is skipped, if 2 the operation is cancelled
	//Function can be null if the functioncall is undesired
	virtual int DestroyFolder( FileOPFunc function, const char * folderpath, int OnlyDestroyIfEmpty, int * failed );
	//Returns the number of files in a folder, includes subfolders if includeSubfolders > 0
	virtual int GetNumberOfFilesInFolder( const char * folderpath, int includeSubfolders );
	//Converts the filetime into unixtime
	virtual time_t FileTimeToUnixTime( FILETIME * ft );
	//Fills in input and returns the size of the file
	virtual size_t GetSHA256FromFile( const char * filename, unsigned char * input );

	//Pointer to custom data, this is maintained by the program rahter then the object
	//The deconstructor will attempt to deallocate this if non-zero
	void * CustomData;

	//Length/size of the memoryblock allocated in customdata, this is maintained by the program rather then the object
	int CustomDataLength;

	//Holds the destination file in a copy procedture, used in CopyFiles
	char SubDestination[MAX_PATH];

protected:
	DWORD DriveLetterMask;
	DWORD FirstNext;
	char BasePath[MAX_PATH];
	unsigned int FILES_COUNTED;
	HANDLE FirstNextHandle;
	WIN32_FIND_DATA LAST_FILE_DATA;
	WIN32_FIND_DATA LAST_FIRSTNEXT_DATA;	
	
};

#endif