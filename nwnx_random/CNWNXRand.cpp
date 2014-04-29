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

#include "CNWNXRand.h"
#include "../NWNXdll/madCHook.h"
#include "../NWNXdll/IniFile.h"
#pragma comment(lib, "madChook.lib")

extern CNWNXRand crand;

int (__cdecl * OldRand)();
int __cdecl NewRand(){

	if (crand.isInHook)
		return rand();

	crand.isInHook = true;

	int n = crand.RO->Random();

	if (crand.LogRandRolls)
		crand.Log("rand: %d\n",n);

	crand.isInHook = false;

	return n;
}

int(__fastcall * CExoRandInternal__Rand)(void * pThis);
int __fastcall CExoRandInternal__NewRand(void * pThis){

	if (crand.isInHook)
		return rand();

	crand.isInHook = true;

	int n = crand.RO->Random();

	if (crand.LogRandRolls)
		crand.Log("CExoRandInternal::Rand: %d\n", n);
	
	crand.isInHook = false;

	return n;

	//return CExoRandInternal__Rand(pThis);
}

BOOL CNWNXRand::OnRelease(){

	delete RO;

	return CNWNXBase::OnRelease();
}

BOOL CNWNXRand::OnCreate(const char* LogDir){

	char log[MAX_PATH];

	sprintf(log, "%s\\nwnx_random.txt", LogDir);

	// call the base class function
	if (!CNWNXBase::OnCreate(log))
		return false;

	CIniFile ini("nwnx.ini");
	ini.ReadString("RANDOM", "Cache", log, MAX_PATH, "");
	LogRandRolls = ini.ReadBool("RANDOM", "Log", false);

	isInHook = false;

	RO = new RandomOrg(log);
	RO->NoDownload = ini.ReadBool("RANDOM", "NoDownload", true);
	RO->PreferStock = ini.ReadBool("RANDOM", "PreferStock", false);
	RO->NoReuse = ini.ReadBool("RANDOM", "NoReuse", false);
	
	int nMaxCache = ini.ReadInteger("RANDOM", "MaxCache", 0);

	ini.ReadString("RANDOM", "DownloadUrl", log, MAX_PATH, "");

	if (log && log[0] != '\0'){

		if (RO->GetURL)
			delete[]RO->GetURL;

		RO->GetURL = new char[strlen(log) + 1];
		strcpy(RO->GetURL, log);
	}

	ini.ReadString("RANDOM", "QuotaUrl", log, MAX_PATH, "");

	if (log && log[0] != '\0'){

		if (RO->QuotaURL)
			delete[]RO->QuotaURL;

		RO->QuotaURL = new char[strlen(log) + 1];
		strcpy(RO->QuotaURL, log);
	}

	WriteLogHeader();

	RO->LOG = this->m_fFile;

	if (nMaxCache>0)
		RO->TrimRNDFiles(nMaxCache);
	
	Log("\n");
	if (HookCode((PVOID)0x00604482, NewRand, (PVOID*)&OldRand))
		Log("o Hooked rand\n");
	else
		Log("! Failed to hook rand\n");

	if (HookCode((PVOID)0x00411870, CExoRandInternal__NewRand, (PVOID*)&CExoRandInternal__Rand))
		Log("o Hooked internal rand\n");
	else
		Log("! Failed to hook internal rand\n");

	Log("\n");

	return true;
}

void CNWNXRand::WriteLogHeader(){

	fprintf(m_fFile, "NWNXRandom v1.0 created by Terra_777\nPowered by random.org\n\n");
	fflush(m_fFile);
}

void CNWNXRand::Log(const char * formatting, ...){

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

unsigned long CNWNXRand::OnRequestObject(char *gameObject, char* Request){

	return OBJECT_INVALID;
}

char* CNWNXRand::OnRequest(char *gameObject, char* Request, char* Parameters){

	if (strcmp(Request, "NoDL") == 0){

		Log("NoDownload = %s\n", atoi(Parameters) > 0 ? "true" : "false");
		RO->NoDownload = atoi(Parameters) > 0;
	}
	else if (strcmp(Request, "NoRU") == 0){

		Log("NoReuse = %s\n", atoi(Parameters) > 0 ? "true" : "false");
		RO->NoReuse = atoi(Parameters) > 0;
	}
	else if (strcmp(Request, "QUT") == 0){

		if (atoi(Parameters) == 1){

			Log("Quota Flag Reset\n");
			RO->outofquota = false;
		}
		else{

			int nQuote = RO->GetQuota();
			sprintf(Parameters, "%d", nQuote);
			Log("Get Quota: %d\n", nQuote);
		}
	}
	return NULL;
}

/*----------------
--Patch functions
----------------*/