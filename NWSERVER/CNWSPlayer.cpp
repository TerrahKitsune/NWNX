#include "types.h"
#include "nwn_internals.h"

int 			(__thiscall *CNWSPlayer__HasExpansionPack)(CNWSPlayer *pTHIS, unsigned char a2, int a3) = (int (__thiscall*)(CNWSPlayer *pTHIS, unsigned char a2, int a3))0x00436720;
int 			(__thiscall *CNWSPlayer__AddArea)(CNWSPlayer* pTHIS, uint32_t areaid) = (int (__thiscall*)(CNWSPlayer* pTHIS, uint32_t areaid))0x00436560;
CGenericObject* (__thiscall *CNWSPlayer__GetGameObject)(CNWSPlayer *pTHIS) = (CGenericObject* (__thiscall*)(CNWSPlayer *pTHIS))0x004364E0;
void 			(__thiscall *CNWSPlayer__vec_dtor)(CNWSPlayer *pTHIS, char Flag) = (void (__thiscall*)(CNWSPlayer *pTHIS, char Flag))0x0042EEA0;
CLastUpdateObject*(__thiscall *CNWSPlayer__GetLastUpdateObject)(CNWSPlayer *pTHIS) = (CLastUpdateObject* (__thiscall*)(CNWSPlayer *pTHIS))0x0042FA30;
int				(__thiscall *CNWSPlayer__SetGameObject)(CNWSPlayer *pTHIS, CNWSObject * obj) = (int (__thiscall*)(CNWSPlayer *pTHIS, CNWSObject * obj))0x00436280;
CExoString*		(__thiscall *CNWSPlayer__GetPlayerName)(CNWSPlayer * pThis, CExoString * str) = (CExoString*(__thiscall*)(CNWSPlayer *, CExoString*))0x004365A0;


CExoString * CNWSPlayer_s::GetPlayerName( CExoString * str ){

	return CNWSPlayer__GetPlayerName( this, str );
}

int CNWSPlayer_s::SetGameObject( CNWSObject * obj ){
	return CNWSPlayer__SetGameObject(this,obj); 
}

CLastUpdateObject* CNWSPlayer_s::GetLastUpdateObject() {
	return CNWSPlayer__GetLastUpdateObject(this);
}

int CNWSPlayer_s::AddArea(uint32_t areaid) {
	return CNWSPlayer__AddArea(this, areaid);
}

CGenericObject *CNWSPlayer_s::GetGameObject() {
	return CNWSPlayer__GetGameObject(this);
}

int CNWSPlayer_s::HasExpansionPack(unsigned char a2, int a3) {
	return CNWSPlayer__HasExpansionPack(this, a2, a3);
}

void CNWSPlayer_s::vec_dtor(char Flag) {
	CNWSPlayer__vec_dtor(this, Flag);
}

