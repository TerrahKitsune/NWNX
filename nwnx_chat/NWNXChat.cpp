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

#include "NWNXChat.h"
#include "../NWNXdll/IniFile.h"

#include "../NWNXdll/madCHook.h"
#include "../NWNXdll/IniFile.h"
#pragma comment(lib, "madChook.lib")

extern CNWNXChat chat;

/*----------------
--Hook Proccies :)
----------------*/

int(__fastcall *CNWSMessage__SendServerToPlayerChatMessageNext)(CNWSMessage *pThis, void*, unsigned char channel, unsigned long sender, CExoString msg, unsigned long recv, CExoString const & txt);

int __fastcall CNWSMessage__SendServerToPlayerChatMessage(CNWSMessage *pThis, void*, unsigned char channel, unsigned long sender, CExoString msg, unsigned long recv, CExoString const & txt){
	
	if (!msg.text || ( chat.ignore_silent && (channel == 0xD || channel == 0xE)))
		return CNWSMessage__SendServerToPlayerChatMessageNext(pThis, NULL, channel, sender, msg, recv, txt);

	if (chat.processnpc || chat.GetID(sender) != -1){

		chat.Log("o onChat: %08X %s\n", sender, msg.text);

		chat.SetLastMSG(msg.text, channel, recv);
		chat.InScript = true;
		int nRet = (*NWN_VirtualMachine)->Runscript(chat.Script, sender);
		chat.InScript = false;
	}

	if (chat.supress){
		chat.supress = false;
		return 1;
	}

	return CNWSMessage__SendServerToPlayerChatMessageNext(pThis, NULL, channel,sender, msg, recv, txt);
}

/*----------------
--nwnx class :>
----------------*/

BOOL CNWNXChat::OnRelease(){

	if (Script){

		if (Script->text)
			mem.nwnx_free(Script->text);
		mem.nwnx_free(Script);
		Script = NULL;
	}

	if (lastmsg){
		mem.nwnx_free(lastmsg);
		lastmsg = NULL;
	}

	return CNWNXBase::OnRelease();
}

BOOL CNWNXChat::OnCreate(const char* LogDir){

	char log[MAX_PATH];
	char path[MAX_PATH];

	sprintf(log, "%s\\nwnx_chat.txt", LogDir);
	sprintf(path, "%s\\1\\nwnx_chat.txt", LogDir);

	MoveFile(log, path);

	// call the base class function
	if (!CNWNXBase::OnCreate(log))
		return false;

	WriteLogHeader();

	CIniFile INI("NWNX.ini");

	processnpc = INI.ReadBool("CHAT", "processnpc", false);
	ignore_silent = INI.ReadBool("CHAT", "ignore_silent", false);

	Script = (CExoString*)mem.nwnx_malloc(sizeof(CExoString));
	
	Script->text = (char*)mem.nwnx_malloc(17);

	INI.ReadString("CHAT", "chat_script", Script->text, 16, "chat_script");

	int lglvl = INI.ReadInteger("CHAT", "LOG", 1);

	LOG = 1;
	Log("\nchat_script = %s\n", Script->text);
	Log("\processnpc = %d\n", processnpc);
	Log("\ignore_silent = %d\n", ignore_silent);
	Log("\LOG = %d\n", lglvl);

	supress = false;
	lastmsg = NULL;

	Log("Hook: %s\n\n", ( HookCode((PVOID)0x0043CA00, (PVOID)CNWSMessage__SendServerToPlayerChatMessage, (PVOID*)&CNWSMessage__SendServerToPlayerChatMessageNext) == -1 ? "Meowtastic" : "Hella broken" ) );

	LOG = lglvl;
	
	return true;
}

void CNWNXChat::WriteLogHeader(){

	fprintf(m_fFile, "NWNX Chat Plugin V.2 Terrah, previous work by:\n");
	fprintf(m_fFile, "(c) 2005-2006 by dumbo (dumbo@nm.ru)\n");
	fprintf(m_fFile, "(c) 2006-2008 by virusman (virusman@virusman.ru)\n");
	fflush(m_fFile);
}

void CNWNXChat::Log(const char * formatting, ...){

	if (LOG <= 0)
		return;

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

unsigned long CNWNXChat::OnRequestObject(char *gameObject, char* Request){

	return OBJECT_INVALID;
}

char* CNWNXChat::OnRequest(char *gameObject, char* Request, char* Parameters){

	Log("o %s(%s)\n", Request, Parameters);

	if (strcmp(Request, "GETID") == 0){
		nwn_objid_t objid;
		sscanf(Parameters, "%x", &objid);
		int cli_id = GetID(objid);
		sprintf(Parameters, "%d", cli_id);
		return NULL;
	}
	else if (strcmp(Request, "LOGNPC")==0){
		processnpc = atoi(Parameters) > 0;
		return NULL;
	}
	else if (strcmp(Request, "IGNORESILENT") == 0){
		ignore_silent = atoi(Parameters) > 0;
		return NULL;
	}
	else if (strcmp(Request, "SPEAK") == 0){
		sprintf(Parameters, "%d", SendChat(Parameters));
		return NULL;
	}

	if (!InScript)
		return NULL;

	if (strcmp(Request, "TEXT") == 0){
		strncpy(Parameters, lastmsg, strlen(Parameters));
		return NULL;
	}
	else if (strcmp(Request, "LOG") == 0){		
		int logPrev = LOG;
		LOG = 1;
		Log("%s",Parameters);
		LOG = logPrev;
		return NULL;
	}
	else if (strcmp(Request, "SUPRESS") == 0){
		supress = atoi(Parameters) > 0;
		return NULL;
	}

	return NULL;
}

/*----------------
--Helpers ^_^
----------------*/

void CNWNXChat::SetLastMSG(char * msg, int channel, int to){

	if (lastmsg)
		mem.nwnx_free(lastmsg);

	lastmsg = (char*)mem.nwnx_malloc(strlen(msg) + 32);
	if (lastmsg){

		sprintf(lastmsg, "%02d%10d", channel, to);
		strcat(lastmsg, msg);
	}
}

int CNWNXChat::SendChat(char * Parameters){
	
	Log("o SPEAK: %s\n", Parameters);

	int oSender, oRecipient, nChannel;
	int nParamLen = strlen(Parameters);
	char *nLastDelimiter = strrchr(Parameters, '¬');
	if (!nLastDelimiter || (nLastDelimiter - Parameters)<0)
	{
		
		Log("o nLastDelimiter error\n");

		return 0;
	}
	int nMessageLen = nParamLen - (nLastDelimiter - Parameters) + 1;
	char *sMessage = (char*)mem.nwnx_malloc(nMessageLen);//new char[nMessageLen];
	if (sscanf(Parameters, "%x¬%x¬%d¬", &oSender, &oRecipient, &nChannel)<3)
	{
		
		Log("o sscanf error\n");

		mem.nwnx_free(sMessage);
		sMessage = NULL;
		return 0;
	}
	strncpy(sMessage, nLastDelimiter + 1, nMessageLen - 1);
	int nRecipientID = GetID(oRecipient);
	if ((nChannel == 4 || nChannel == 20) && oRecipient == -1)
	{
		
		Log("o oRecipient is not a PC\n");

		mem.nwnx_free(sMessage);
		sMessage = NULL;
		return 0;
	}
	if (nChannel != 4 && nChannel != 20) nRecipientID = -1;
	
	Log("o SendMsg(%d, %08lX, '%s', %d)\n", nChannel, oSender, sMessage, nRecipientID);

	int nResult = SendMsg(nChannel, oSender, sMessage, nRecipientID);
	
	Log("o Return value: %d\n", nResult); //return value for full message delivery acknowledgement

	mem.nwnx_free(sMessage);
	sMessage = NULL;

	if (nResult) return 1;
	else return 0;
}

int CNWNXChat::GetID(nwn_objid_t obj_id){
	CNWSPlayer * cli = (*NWN_AppManager)->app_server->GetClientObjectByObjectId(obj_id);
	if (!cli)
		return -1;
	return cli->pl_id;
}

int CNWNXChat::SendMsg(int nChannel, int oSender, char * sMessage, int nRecipientID){

	CNWSMessage * mess = (*NWN_AppManager)->app_server->GetNWSMessage();

	CExoString * msg = (CExoString*)mem.nwnx_malloc(sizeof(CExoString));

	msg->CExoStringCpy(sMessage);

	int ret = mess->SendServerToPlayerChatMessage(nChannel, oSender, *msg, nRecipientID);

	if (msg->text)
		mem.nwnx_free(msg->text);
	mem.nwnx_free(msg);

	return ret;
}

