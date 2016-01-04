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

#include "NWNXMem.h"
#include "../NWNXdll/madCHook.h"
#include "../NWNXdll/IniFile.h"
#include <math.h>
#pragma comment(lib, "madChook.lib")

extern CNWNXMem cmem;


/*int CNWSModule__AIUpdate(void *a1)
{
	int ret = CNWSModule__AIUpdate_R(a1);
	if (ret == 1 && !runLock) {
		runLock = 1;
		int ret2 = CNWSModule__AIUpdate(a1);
		runLock = 0;
		fixes.Log(3, "CNWSModule__AIUpdate: %d\n", ret2);
		return ret2;
	}
	return ret;
}*/

unsigned __int64 (__fastcall *CExoTimersInternal__GetHighResolutionTimerNext)(void * pThis);
unsigned __int64 __fastcall CExoTimersInternal__GetHighResolutionTimer(void * pThis){

	LARGE_INTEGER frequency;
	LARGE_INTEGER ticks;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&ticks);

	unsigned __int64 result = ticks.QuadPart / frequency.QuadPart;
	unsigned __int64 result2 = CExoTimersInternal__GetHighResolutionTimerNext(pThis);

	cmem.Log("%lu -> %lu\n", result, result2);

	return result2;
}

int(__fastcall *CNWSModule__AIUpdateNext)(CNWSModule * mod);
int runLock;

int __fastcall CNWSModule__AIUpdate(CNWSModule * mod){

	int ret = CNWSModule__AIUpdateNext(mod);
	if (ret == 1 && !runLock) {
		runLock = 1;
		int ret2 = CNWSModule__AIUpdate(mod);
		runLock = 0;
		//cmem.Log("! AIUpdate lock: %d\n", ret2);
		return ret2;
	}
	return ret;
}


//---Memory hooks

void * (__cdecl *MallocExNext)(size_t mem, DWORD _newmode);
void * __cdecl MallocEx(size_t mem, DWORD _newmode){

	if (cmem.IsInside)
		return MallocExNext(mem, _newmode);

	cmem.IsInside = true;

	void * alloc = MallocExNext(mem, _newmode);

	if (alloc){
		try{
			cmem.memory[alloc] = mem;
		}
		catch (std::bad_alloc){
			cmem.Log("! unable to allocate memory for map (malloc)\n");
		}
	}

	cmem.IsInside = false;

	return alloc;
}

void(__cdecl *FreeExNext)(void * mem);
void __cdecl FreeEx(void * mem){

	if (!mem)
		return;
	else if (cmem.IsInside){
		FreeExNext(mem);
		return;
	}

	cmem.IsInside = true;

	if (cmem.memory.find(mem) != cmem.memory.end()){
		cmem.memory.erase(mem);
		FreeExNext(mem);
	}
	else{
		if (time(NULL) > cmem.started){
			cmem.Log("! Invalid Free: %08X\n", mem);
			cmem.badfree++;
		}
		else
			FreeExNext(mem);
	}
	
	cmem.IsInside = false;
}

void * (__cdecl *ReallocExNext)(void * mem, size_t size);
void * __cdecl ReallocEx(void * mem, size_t size){

	if (cmem.IsInside)
		return ReallocExNext(mem, size);

	cmem.IsInside = true;

	void * alloc = ReallocExNext(mem, size);

	if (alloc){

		if (mem != NULL){

			if (cmem.memory.find(mem) != cmem.memory.end()){
				cmem.memory.erase(mem);
				FreeExNext(mem);
			}
			else{
				if (time(NULL) > cmem.started){
					cmem.Log("! Invalid Realloc: %08X\n", mem);
					cmem.badfree++;
				}
				else
					FreeExNext(mem);
			}
		}

		try{
			cmem.memory[alloc] = size;
		}
		catch (std::bad_alloc){
			cmem.Log("! unable to allocate memory for map (realloc)\n");
		}		
	}

	cmem.IsInside = false;

	return alloc; 
}

void * (__cdecl *CallocExNext)(size_t num, size_t size);
void * __cdecl CallocEx(size_t num, size_t size){

	if (cmem.IsInside)
		return CallocExNext(num, size);

	cmem.IsInside = true;

	void * alloc = CallocExNext(num,size);

	if (alloc){
		try{
			cmem.memory[alloc] = num*size;
		}
		catch (std::bad_alloc){
			cmem.Log("! unable to allocate memory for map (calloc)\n");
		}
	}

	cmem.IsInside = false;

	return alloc;
}

//---END

size_t CNWNXMem::GetTotalSize(){

	cmem.IsInside = true;

	size_t total = 0;
	
	for (std::map<void*, size_t>::iterator it = cmem.memory.begin(); it != cmem.memory.end(); ++it)
		total += it->second;

	cmem.IsInside = false;

	return total;
}

size_t CNWNXMem::GetTotalCount(){

	cmem.IsInside = true;

	size_t total = 0;

	for (std::map<void*, size_t>::iterator it = cmem.memory.begin(); it != cmem.memory.end(); ++it)
		total++;

	cmem.IsInside = false;

	return total;
}

BOOL CNWNXMem::OnRelease(){


	return CNWNXBase::OnRelease();
}

BOOL CNWNXMem::OnCreate(const char* LogDir){

	runLock = 0;
	cmem.IsInside = true;
	badfree = 0;
	char log[MAX_PATH];
	char logmove[MAX_PATH];
	sprintf(log, "%s\\nwnx_mem.txt", LogDir);
	sprintf(logmove, "%s\\1\\nwnx_mem.txt", LogDir);

	MoveFile(log, logmove);

	// call the base class function
	if (!CNWNXBase::OnCreate(log))
		return false;

	WriteLogHeader();
	
	BOOL ok = HookCode((PVOID)0x004D1D00, CNWSModule__AIUpdate, (PVOID*)&CNWSModule__AIUpdateNext);
	//ok = ok && HookCode((PVOID)0x00411F20, CExoTimersInternal__GetHighResolutionTimer, (PVOID*)&CExoTimersInternal__GetHighResolutionTimerNext);

	ok = ok && HookCode((PVOID)0x00602669, MallocEx, (PVOID*)&MallocExNext);
	ok = ok && HookCode((PVOID)0x0060256E, FreeEx, (PVOID*)&FreeExNext);
	ok = ok && HookCode((PVOID)0x006052EC, ReallocEx, (PVOID*)&ReallocExNext);
	ok = ok && HookCode((PVOID)0x00603F18, CallocEx, (PVOID*)&CallocExNext);

	if (ok)
		Log("Memory hooks ok!\n\n");
	else
		Log("Memory hooks failed!\n\n");

	started = time(NULL)+3;

	cmem.IsInside = false;

	return true;
}

void CNWNXMem::WriteLogHeader(){

	fprintf(m_fFile, "NWNXMem v0.1 created by Terra_777\n");
	fflush(m_fFile);
}

void CNWNXMem::Log(const char * formatting, ...){

	va_list argList;
	char *pos;

	if (m_fFile){

		if (ftell(m_fFile) > m_maxLogSizeKB){

			fclose(m_fFile);
			m_fFile = fopen(m_LogFile, "w");
			WriteLogHeader();
			fprintf(m_fFile, "o Logfile hit maximum size limit, starting again.\n");
		}

		// build up the string
		va_start(argList, formatting);
		vsprintf(acBuffer, formatting, argList);
		va_end(argList);

		// replace any percent signs
		pos = strchr(acBuffer, '%');
		while (pos){

			*pos = '~';
			pos = strchr(acBuffer, '%');
		}

		fprintf(m_fFile, acBuffer);
		fflush(m_fFile);
	}
}

/*----------------
--Script interface
----------------*/

unsigned long CNWNXMem::OnRequestObject(char *gameObject, char* Request){

	//Log("o OnRequestObject: 0x%08X: %s( )", gameObject, Request);

	return OBJECT_INVALID;
}

char* CNWNXMem::OnRequest(char *gameObject, char* Request, char* Parameters){

	//Log("o OnRequest: 0x%08X: %s( %s )", gameObject, Request, Parameters);

	if (strcmp(Request, "MEM") == 0){
		sprintf(Parameters, "%lu", cmem.GetTotalSize());
	}
	else if (strcmp(Request, "CNT") == 0){
		sprintf(Parameters, "%lu", cmem.GetTotalCount());
	}
	else if (strcmp(Request, "BAD") == 0){
		sprintf(Parameters, "%lu", cmem.badfree);
	}

	return NULL;
}

/*----------------
--Patch functions
----------------*/