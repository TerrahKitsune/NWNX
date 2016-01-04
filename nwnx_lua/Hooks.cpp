#include "Hooks.h"
#include "../NWNXdll/madCHook.h"
#pragma comment(lib, "madChook.lib")

void * (__fastcall *CNWSScriptVarTable__GetStringNext)(CNWSScriptVarTable * pThis, void*, void *a2, CExoString *str);
void * __fastcall CNWSScriptVarTable__GetString(CNWSScriptVarTable * pThis, void*, void *a2, CExoString *str){
	
	if (!str || !str->text){

		CExoString * str = (CExoString*)a2;
		if (!str){
			str = (CExoString*)lua.mem.nwnx_malloc(sizeof(CExoString));
		}

		memset(str, 0, sizeof(CExoString));

		return str;
	}
	else if (strncmp(str->text, "LUA!", 4) == 0){
		
		char * request = strstr(str->text, "!")+1;
		char * params = strstr(request, "!");

		*params = '\0';
		params++;

		char * back = lua.OnGetString(request, params);
		
		CExoString * str = (CExoString*)a2;// lua.mem.nwnx_malloc(sizeof(CExoString));
		if (!str){
			str = (CExoString*)lua.mem.nwnx_malloc(sizeof(CExoString));			
		}

		memset(str, 0, sizeof(CExoString));

		if (back){
			str->text = back;
			str->len = strlen(back);
		}

		//lua.Log("o %08X = %s\n", str, str->text);

		return str;
	}

	return CNWSScriptVarTable__GetStringNext(pThis, NULL, a2, str);
}

int(__fastcall *CVirtualMachine__StackPopIntegerNext)(CVirtualMachine * pThis, void*, int * data);
int(__fastcall *CVirtualMachine__StackPushIntegerNext)(CVirtualMachine * pThis, void*, int data);
int(__fastcall *CVirtualMachine__StackPopFloatNext)(CVirtualMachine * pThis, void*, float * data);
int(__fastcall *CVirtualMachine__StackPushFloatNext)(CVirtualMachine * pThis, void*, float data);
int(__fastcall *CVirtualMachine__StackPopStringNext)(CVirtualMachine * pThis, void*, CExoString * data);
int(__fastcall *CVirtualMachine__StackPushStringNext)(CVirtualMachine * pThis, void*, CExoString &data);
int(__fastcall *CVirtualMachine__StackPopObjectNext)(CVirtualMachine * pThis, void*, nwn_objid_t * data);
int(__fastcall *CVirtualMachine__StackPushObjectNext)(CVirtualMachine * pThis, void*, nwn_objid_t data);
int(__fastcall *CVirtualMachine__StackPopVectorNext)(CVirtualMachine * pThis, void*, Vector * data);
int(__fastcall *CVirtualMachine__StackPushVectorNext)(CVirtualMachine * pThis, void*, Vector data);

void *nwn_alloc(void *ud, void *ptr, size_t osize, size_t nsize) {

	if (nsize == 0){
		free(ptr);
		return NULL;
	}

	return realloc(ptr, nsize);
}

int __fastcall CVirtualMachine__StackPushObject(CVirtualMachine * pThis, void*, nwn_objid_t data){

	if (lua.L_Current == NULL){
		return CVirtualMachine__StackPushObjectNext(pThis, NULL, data);
	}

	char str[12];
	sprintf(str, "%x", data);
	lua_pushstring(lua.L_Current, str);

	if (lua.logging >= 2)
		lua.Log("o Push Object: %s\n", str);

	lua.L_Pushed++;

	return 1;
}

int __fastcall CVirtualMachine__StackPushString(CVirtualMachine * pThis, void*, CExoString & data){

	if (lua.L_Current == NULL){
		return CVirtualMachine__StackPushStringNext(pThis, NULL, data);
	}

	const char * str = data.CStr();

	lua_pushstring(lua.L_Current, str);

	if (lua.logging >= 2)
		lua.Log("o Push String: %s\n", str);

	lua.L_Pushed++;

	return 1;
}

int __fastcall CVirtualMachine__StackPushInteger(CVirtualMachine * pThis, void*, int data){

	if (lua.L_Current == NULL){
		return CVirtualMachine__StackPushIntegerNext(pThis, NULL, data);
	}

	int n = data;

	lua_pushinteger(lua.L_Current, n);
	if (lua.logging >= 2)
		lua.Log("o Push Int: %d\n", n);

	lua.L_Pushed++;

	return 1;
}

int __fastcall CVirtualMachine__StackPushFloat(CVirtualMachine * pThis, void*, float data){

	if (lua.L_Current == NULL){
		return CVirtualMachine__StackPushFloatNext(pThis, NULL, data);
	}

	lua_Number fl = data;

	lua_pushnumber(lua.L_Current, fl);

	if (lua.logging >= 2)
		lua.Log("o Push Float: %f\n", fl);

	lua.L_Pushed++;

	return 1;
}

int __fastcall CVirtualMachine__StackPushVector(CVirtualMachine * pThis, void*, Vector data){

	if (lua.L_Current == NULL){
		return CVirtualMachine__StackPushVectorNext(pThis, NULL, data);
	}


	lua.lua_pushvector(lua.L_Current, data);

	//lua_pushlstring( lua.L_Current, (const char*)&data, sizeof( Vector ) );

	if (lua.logging >= 2)
		lua.Log("o Push Vector: x %f y %f z %f\n", data.x, data.y, data.z);

	lua.L_Pushed++;

	return 1;
}

int __fastcall CVirtualMachine__StackPopObject(CVirtualMachine * pThis, void*, nwn_objid_t * data){

	if (lua.L_Current == NULL){
		return CVirtualMachine__StackPopObjectNext(pThis, NULL, data);
	}

	const char * str = lua_tostring(lua.L_Current, 1);

	sscanf(str, "%x", data);

	if (lua.logging >= 2)
		lua.Log("o Pop Object: %x\n", *data);

	lua_remove(lua.L_Current, 1);
	return 1;
}

int __fastcall CVirtualMachine__StackPopString(CVirtualMachine * pThis, void*, CExoString * data){

	if (lua.L_Current == NULL){
		return CVirtualMachine__StackPopStringNext(pThis, NULL, data);
	}

	if (data->text)
		lua.mem.nwnx_free(data->text);

	size_t len;
	const char * str = lua_tolstring(lua.L_Current, 1, &len);

	if (!str){
		str = "";
		len = 0;
	}

	data->text = (char*)lua.mem.nwnx_malloc(len + 1);
	strcpy(data->text, str);

	if (lua.logging >= 2)
		lua.Log("o Pop String: %s\n", data->text);

	lua_remove(lua.L_Current, 1);
	return 1;
}

int __fastcall CVirtualMachine__StackPopInteger(CVirtualMachine * pThis, void*, int * data){

	if (lua.L_Current == NULL){
		return CVirtualMachine__StackPopIntegerNext(pThis, NULL, data);
	}

	int n = lua_tointeger(lua.L_Current, 1);

	*data = n;

	if (lua.logging >= 2)
		lua.Log("o Pop Int: %d\n", n);

	lua_remove(lua.L_Current, 1);
	return 1;
}

int __fastcall CVirtualMachine__StackPopFloat(CVirtualMachine * pThis, void*, float * data){

	if (lua.L_Current == NULL){
		return CVirtualMachine__StackPopFloatNext(pThis, NULL, data);
	}

	lua_Number num = lua_tonumber(lua.L_Current, 1);
	*data = num;

	if (lua.logging >= 2)
		lua.Log("o Pop Float: %f\n", num);

	lua_remove(lua.L_Current, 1);
	return 1;
}

int __fastcall CVirtualMachine__StackPopVector(CVirtualMachine * pThis, void*, Vector * data){

	if (lua.L_Current == NULL){
		return CVirtualMachine__StackPopVectorNext(pThis, NULL, data);
	}

	*data = lua.lua_tovector(lua.L_Current, 1);

	if (lua.logging >= 2)
		lua.Log("o Pop Vector: x %f y %f z %f\n", data->x, data->y, data->z);

	lua_remove(lua.L_Current, 1);
	return 1;
}

int exception(unsigned int code, struct _EXCEPTION_POINTERS *ep) {

	char errorlog[1024];

	if (code == STATUS_ACCESS_VIOLATION)
		sprintf(errorlog, "STATUS_ACCESS_VIOLATION (%u) %08X: %08X ", code, ep->ExceptionRecord->ExceptionCode, ep->ExceptionRecord->ExceptionAddress);
	else if (code == STATUS_BREAKPOINT)
		sprintf(errorlog, "STATUS_BREAKPOINT (%u) %08X: %08X ", code, ep->ExceptionRecord->ExceptionCode, ep->ExceptionRecord->ExceptionAddress);
	else if (code == STATUS_DATATYPE_MISALIGNMENT)
		sprintf(errorlog, "STATUS_DATATYPE_MISALIGNMENT (%u) %08X: %08X ", code, ep->ExceptionRecord->ExceptionCode, ep->ExceptionRecord->ExceptionAddress);
	else if (code == STATUS_FLOAT_DIVIDE_BY_ZERO)
		sprintf(errorlog, "STATUS_FLOAT_DIVIDE_BY_ZERO (%u) %08X: %08X ", code, ep->ExceptionRecord->ExceptionCode, ep->ExceptionRecord->ExceptionAddress);
	else if (code == STATUS_FLOAT_OVERFLOW)
		sprintf(errorlog, "STATUS_FLOAT_OVERFLOW (%u) %08X: %08X ", code, ep->ExceptionRecord->ExceptionCode, ep->ExceptionRecord->ExceptionAddress);
	else if (code == STATUS_FLOAT_UNDERFLOW)
		sprintf(errorlog, "STATUS_FLOAT_UNDERFLOW (%u) %08X: %08X ", code, ep->ExceptionRecord->ExceptionCode, ep->ExceptionRecord->ExceptionAddress);
	else if (code == STATUS_ILLEGAL_INSTRUCTION)
		sprintf(errorlog, "STATUS_ILLEGAL_INSTRUCTION (%u) %08X: %08X ", code, ep->ExceptionRecord->ExceptionCode, ep->ExceptionRecord->ExceptionAddress);
	else if (code == STATUS_PRIVILEGED_INSTRUCTION)
		sprintf(errorlog, "STATUS_PRIVILEGED_INSTRUCTION (%u) %08X: %08X ", code, ep->ExceptionRecord->ExceptionCode, ep->ExceptionRecord->ExceptionAddress);
	else if (code == STATUS_INTEGER_DIVIDE_BY_ZERO)
		sprintf(errorlog, "STATUS_INTEGER_DIVIDE_BY_ZERO (%u) %08X: %08X ", code, ep->ExceptionRecord->ExceptionCode, ep->ExceptionRecord->ExceptionAddress);
	else if (code == STATUS_SINGLE_STEP)
		sprintf(errorlog, "STATUS_SINGLE_STEP (%u) %08X: %08X ", code, ep->ExceptionRecord->ExceptionCode, ep->ExceptionRecord->ExceptionAddress);
	else
		sprintf(errorlog, "EXCEPTION (%u) %08X: %08X ", code, ep->ExceptionRecord->ExceptionCode, ep->ExceptionRecord->ExceptionAddress);


	int current = lua.logging;
	lua.logging = 1;
	lua.Log("%d %s\n", lua.lives, errorlog);
	lua.logging = current;

	if (--lua.lives <= 0){
		return EXCEPTION_EXECUTE_HANDLER;
	}

	/*nwn_objid_t oPC = (*NWN_AppManager)->app_server->GetFirstPCObject();

	while (oPC != OBJECT_INVALID){

		SendMessageToPC(oPC, errorlog);

		oPC = (*NWN_AppManager)->app_server->GetNextPCObject();
	}*/

	return EXCEPTION_CONTINUE_EXECUTION;
}

int(__fastcall *CServerExoApp__MainLoopNext)(CServerExoApp * pThis, void*);
int __fastcall CServerExoApp__MainLoop(CServerExoApp * pThis, void*){

	int ret = 0;

	__try{

		lua_rawgeti(lua.L, LUA_REGISTRYINDEX, lua.mainloop);

		if (lua_pcall(lua.L, 0, 0, 0)){

			int temp = lua.logging;
			lua.logging = 1;
			lua.Log("! MainLoop: %s\n", lua_tostring(lua.L, -1));
			lua.logging = temp;
			lua_pop(lua.L, 1);
		}
		else
			lua_pop(lua.L, lua_gettop(lua.L));

		ret = CServerExoApp__MainLoopNext(pThis, NULL);

	}
	__except (exception(GetExceptionCode(), GetExceptionInformation())){
		lua.logging = 1;
		lua.Log("Killing nwserver; too many hardware exceptions");
		exit(0);
	}

	return ret;
}

bool(__fastcall *CNWSMessage__TestObjectVisibleNext)(CNWSMessage * pThis, void*, CNWSObject * ori, CNWSObject * trg);
bool __fastcall CNWSMessage__TestObjectVisible(CNWSMessage * pThis, void*, CNWSObject * ori, CNWSObject * trg){

	if (!ori || !trg || ori == trg || lua.isinhook)
		return CNWSMessage__TestObjectVisibleNext(pThis, NULL, ori, trg);

	lua_pop(lua.L, lua_gettop(lua.L));

	lua_rawgeti(lua.L, LUA_REGISTRYINDEX, lua.visibilityfunc);

	char buf[12];
	sprintf(buf, "%x", trg->obj_generic.obj_id);
	lua_pushstring(lua.L, buf);

	sprintf(buf, "%x", ori->obj_generic.obj_id);
	lua_pushstring(lua.L, buf);
	lua.isinhook = true;
	if (lua_pcall(lua.L, 2, 1, 0)){
		lua.isinhook = false;
		int temp = lua.logging;
		lua.logging = 1;
		lua.Log("! TestObjectVisible (%08X, %08X): %s\n", trg->obj_generic.obj_id, ori->obj_generic.obj_id, lua_tostring(lua.L, -1));
		lua.logging = temp;
		lua_pop(lua.L, 1);
	}
	else if (lua_isboolean(lua.L, -1)){
		lua.isinhook = false;
		int visible = lua_toboolean(lua.L, -1);
		lua_pop(lua.L, 1);

		//lua.logging = 1;
		//lua.Log("o TestObjectVisible (%08X, %08X): %d\n", trg->obj_generic.obj_id, ori->obj_generic.obj_id, visible);

		return visible>0;
	}
	lua.isinhook = true;
	lua_pop(lua.L, 1);
	return CNWSMessage__TestObjectVisibleNext(pThis,NULL,ori,trg);
}

void(__fastcall *CNWSCreature__EventHandlerNext)(CNWSCreature *pThis, void*, int event1, nwn_objid_t target, DWORD *inout, int a5, int a6);
void __fastcall CNWSCreature__EventHandler(CNWSCreature *pThis, void*, int event, nwn_objid_t target, DWORD *inout, int a5, int a6){

	if (pThis)
		lua.EventHandler(pThis->obj.obj_generic.obj_id, event, target, inout);

	CNWSCreature__EventHandlerNext(pThis, NULL, event, target,inout,a5,a6);
}

void(__fastcall *CNWSStore__EventHandlerNext)(CNWSStore *pThis, void*, int event1, nwn_objid_t target, DWORD *inout, int a5, int a6);
void __fastcall CNWSStore__EventHandler(CNWSStore *pThis, void*, int event, nwn_objid_t target, DWORD *inout, int a5, int a6){

	if (pThis)
		lua.EventHandler(pThis->obj.obj_generic.obj_id, event, target, inout);

	CNWSStore__EventHandlerNext(pThis, NULL, event, target, inout, a5, a6);
}

void(__fastcall *CNWSModule__EventHandlerNext)(CNWSModule *pThis, void*, int event1, nwn_objid_t target, DWORD *inout, int a5, int a6);
void __fastcall CNWSModule__EventHandler(CNWSModule *pThis, void*, int event, nwn_objid_t target, DWORD *inout, int a5, int a6){

	if (pThis)
		lua.EventHandler(pThis->obj_id, event, target, inout);

	CNWSModule__EventHandlerNext(pThis, NULL, event, target, inout, a5, a6);
}

void(__fastcall *CNWSItem__EventHandlerNext)(CNWSItem *pThis, void*, int event1, nwn_objid_t target, DWORD *inout, int a5, int a6);
void __fastcall CNWSItem__EventHandler(CNWSItem *pThis, void*, int event, nwn_objid_t target, DWORD *inout, int a5, int a6){

	if (pThis)
		lua.EventHandler(pThis->obj.obj_generic.obj_id, event, target, inout);

	CNWSItem__EventHandlerNext(pThis, NULL, event, target, inout, a5, a6);
}

void(__fastcall *CNWSArea__EventHandlerNext)(CNWSArea *pThis, void*, int event1, nwn_objid_t target, DWORD *inout, int a5, int a6);
void __fastcall CNWSArea__EventHandler(CNWSArea *pThis, void*, int event, nwn_objid_t target, DWORD *inout, int a5, int a6){

	if (pThis)
		lua.EventHandler(pThis->obj_id, event, target, inout);

	CNWSArea__EventHandlerNext(pThis, NULL, event, target, inout, a5, a6);
}

void(__fastcall *CNWSPlaceable__EventHandlerNext)(CNWSPlaceable *pThis, void*, int event1, nwn_objid_t target, DWORD *inout, int a5, int a6);
void __fastcall CNWSPlaceable__EventHandler(CNWSPlaceable *pThis, void*, int event, nwn_objid_t target, DWORD *inout, int a5, int a6){

	if (pThis)
		lua.EventHandler(pThis->obj.obj_generic.obj_id, event, target, inout);

	CNWSPlaceable__EventHandlerNext(pThis, NULL, event, target, inout, a5, a6);
}

void(__fastcall *CNWSDoor__EventHandlerNext)(CNWSDoor *pThis, void*, int event1, nwn_objid_t target, DWORD *inout, int a5, int a6);
void __fastcall CNWSDoor__EventHandler(CNWSDoor *pThis, void*, int event, nwn_objid_t target, DWORD *inout, int a5, int a6){

	if (pThis)
		lua.EventHandler(pThis->obj.obj_generic.obj_id, event, target, inout);

	CNWSDoor__EventHandlerNext(pThis, NULL, event, target, inout, a5, a6);
}

void(__fastcall *CNWSAreaOfEffectObject__EventHandlerNext)(CNWSAreaOfEffectObject *pThis, void*, int event1, nwn_objid_t target, DWORD *inout, int a5, int a6);
void __fastcall CNWSAreaOfEffectObject__EventHandler(CNWSAreaOfEffectObject *pThis, void*, int event, nwn_objid_t target, DWORD *inout, int a5, int a6){

	if (pThis)
		lua.EventHandler(pThis->obj.obj_generic.obj_id, event, target, inout);

	CNWSAreaOfEffectObject__EventHandlerNext(pThis, NULL, event, target, inout, a5, a6);
}

void(__fastcall *CNWSTrigger__EventHandlerNext)(CNWSTrigger *pThis, void*, int event1, nwn_objid_t target, DWORD *inout, int a5, int a6);
void __fastcall CNWSTrigger__EventHandler(CNWSTrigger *pThis, void*, int event, nwn_objid_t target, DWORD *inout, int a5, int a6){

	if (pThis)
		lua.EventHandler(pThis->obj.obj_generic.obj_id, event, target, inout);

	CNWSTrigger__EventHandlerNext(pThis, NULL, event, target, inout, a5, a6);
}

void(__fastcall *CNWSEncounter__EventHandlerNext)(CNWSEncounter *pThis, void*, int event1, nwn_objid_t target, DWORD *inout, int a5, int a6);
void __fastcall CNWSEncounter__EventHandler(CNWSEncounter *pThis, void*, int event, nwn_objid_t target, DWORD *inout, int a5, int a6){

	if (pThis)
		lua.EventHandler(pThis->obj.obj_generic.obj_id, event, target, inout);

	CNWSEncounter__EventHandlerNext(pThis, NULL, event, target, inout, a5, a6);
}

void(__fastcall *CNWSWaypoint__EventHandlerNext)(CNWSWaypoint *pThis, void*, int event1, nwn_objid_t target, DWORD *inout, int a5, int a6);
void __fastcall CNWSWaypoint__EventHandler(CNWSWaypoint *pThis, void*, int event, nwn_objid_t target, DWORD *inout, int a5, int a6){

	if (pThis)
		lua.EventHandler(((CNWSObject*)pThis)->obj_generic.obj_id, event, target, inout);

	CNWSWaypoint__EventHandlerNext(pThis, NULL, event, target, inout, a5, a6);
}

void(__fastcall *CNWSPlayerTURD__EventHandlerNext)(CNWSPlayerTURD *pThis, void*, int event1, nwn_objid_t target, DWORD *inout, int a5, int a6);
void __fastcall CNWSPlayerTURD__EventHandler(CNWSPlayerTURD *pThis, void*, int event, nwn_objid_t target, DWORD *inout, int a5, int a6){

	if (pThis)
		lua.EventHandler(((CNWSObject*)pThis)->obj_generic.obj_id, event, target,inout);

	CNWSPlayerTURD__EventHandlerNext(pThis, NULL, event, target, inout, a5, a6);
}

char(__fastcall *CNWSCreature__CanEquipWeaponNext)(CNWSCreature *pThis, void*, CNWSItem * itm, DWORD * arg0, int arg1, int arg2, CNWSPlayer * ply);
char  __fastcall CNWSCreature__CanEquipWeapon(CNWSCreature *pThis, void*, CNWSItem * itm, DWORD * arg0, int arg1, int arg2, CNWSPlayer * ply){

	if (!pThis || !itm || !arg0 || lua.isinhook)
		return CNWSCreature__CanEquipWeaponNext(pThis, NULL, itm, arg0, arg1, arg2,ply);

	lua_pop(lua.L, lua_gettop(lua.L));

	lua_rawgeti(lua.L, LUA_REGISTRYINDEX, lua.canequipfunc);

	char buf[12];
	sprintf(buf, "%x", pThis->obj.obj_generic.obj_id);
	lua_pushstring(lua.L, buf);

	sprintf(buf, "%x", itm->obj.obj_generic.obj_id);
	lua_pushstring(lua.L, buf);

	DWORD test = *arg0;
	int slot = -1;
	for (int n = 0; n < 32; n++){

		if (test >> n & 0x1){
			slot = n;
			break;
		}
	}

	lua_pushinteger(lua.L, slot);
	lua.isinhook = true;
	if (lua_pcall(lua.L, 3, 1, 0)){
		lua.isinhook = false;
		int temp = lua.logging;
		lua.logging = 1;
		lua.Log("! Canequip (%08X, %08X): %s\n", pThis->obj.obj_generic.obj_id, itm->obj.obj_generic.obj_id, lua_tostring(lua.L, -1));
		lua.logging = temp;
		lua_pop(lua.L, 1);
	}
	else if (lua_isboolean(lua.L, -1)){
		lua.isinhook = false;
		int ok = lua_toboolean(lua.L, -1);
		lua_pop(lua.L, 1);

		return ok>0;
	}
	lua.isinhook = false;
	lua_pop(lua.L, 1);

	return CNWSCreature__CanEquipWeaponNext(pThis, NULL, itm, arg0, arg1, arg2, ply);
}

char(__fastcall *CNWSCreature__CanEquipShieldNext)(CNWSCreature *pThis, void*, CNWSItem * itm, int arg1, int arg2);
char  __fastcall CNWSCreature__CanEquipShield(CNWSCreature *pThis, void*, CNWSItem * itm, int arg1, int arg2){

	if (!pThis || !itm || lua.isinhook)
		return CNWSCreature__CanEquipShieldNext(pThis, NULL, itm, arg1, arg2);

	lua_pop(lua.L, lua_gettop(lua.L));

	lua_rawgeti(lua.L, LUA_REGISTRYINDEX, lua.canequipfunc);

	char buf[12];
	sprintf(buf, "%x", pThis->obj.obj_generic.obj_id);
	lua_pushstring(lua.L, buf);

	sprintf(buf, "%x", itm->obj.obj_generic.obj_id);
	lua_pushstring(lua.L, buf);

	lua_pushinteger(lua.L, 5);
	lua.isinhook = true;
	if (lua_pcall(lua.L, 3, 1, 0)){
		lua.isinhook = false;
		int temp = lua.logging;
		lua.logging = 1;
		lua.Log("! Canequip (%08X, %08X): %s\n", pThis->obj.obj_generic.obj_id, itm->obj.obj_generic.obj_id, lua_tostring(lua.L, -1));
		lua.logging = temp;
		lua_pop(lua.L, 1);
	}
	else if (lua_isboolean(lua.L, -1)){
		lua.isinhook = false;
		int ok = lua_toboolean(lua.L, -1);
		lua_pop(lua.L, 1);

		return ok>0;
	}
	lua.isinhook = false;
	lua_pop(lua.L, 1);

	return CNWSCreature__CanEquipShieldNext(pThis, NULL, itm, arg1, arg2);
}

char(__fastcall *CNWSCreature__CanEquipMiscellaneousNext)(CNWSCreature *pThis, void*, CNWSItem * itm, DWORD * arg0, int arg1, int arg2);
char  __fastcall CNWSCreature__CanEquipMiscellaneous(CNWSCreature *pThis, void*, CNWSItem * itm, DWORD * arg0, int arg1, int arg2){

	if (!pThis || !itm || !arg0 || lua.isinhook)
		return CNWSCreature__CanEquipMiscellaneousNext(pThis, NULL, itm, arg0,arg1, arg2);

	lua_pop(lua.L, lua_gettop(lua.L));

	lua_rawgeti(lua.L, LUA_REGISTRYINDEX, lua.canequipfunc);

	char buf[12];
	sprintf(buf, "%x", pThis->obj.obj_generic.obj_id);
	lua_pushstring(lua.L, buf);

	sprintf(buf, "%x", itm->obj.obj_generic.obj_id);
	lua_pushstring(lua.L, buf);

	DWORD test = *arg0;
	int slot = -1;
	for (int n = 0; n < 32; n++){

		if (test >> n & 0x1){
			slot = n;
			break;
		}
	}

	lua_pushinteger(lua.L, slot);
	lua.isinhook = true;
	if (lua_pcall(lua.L, 3, 1, 0)){
		lua.isinhook = false;
		int temp = lua.logging;
		lua.logging = 1;
		lua.Log("! Canequip (%08X, %08X): %s\n", pThis->obj.obj_generic.obj_id, itm->obj.obj_generic.obj_id, lua_tostring(lua.L, -1));
		lua.logging = temp;
		lua_pop(lua.L, 1);
	}
	else if (lua_isboolean(lua.L, -1)){
		lua.isinhook = false;
		int ok = lua_toboolean(lua.L, -1);
		lua_pop(lua.L, 1);

		return ok>0;
	}
	lua.isinhook = false;
	lua_pop(lua.L, 1);

	return CNWSCreature__CanEquipMiscellaneousNext(pThis, NULL, itm, arg0, arg1, arg2);
}

int(__fastcall *CNWSItem__GetCostNext)(CNWSItem *pThis, void*, int arg1, int arg2, int arg3, int arg4);
int  __fastcall CNWSItem__GetCost(CNWSItem *pThis, void*, int arg1, int arg2, int arg3, int arg4){

	if (!pThis || lua.isinhook)
		return CNWSItem__GetCostNext(pThis, NULL, arg1, arg2, arg3, arg4);

	lua_pop(lua.L, lua_gettop(lua.L));

	lua_rawgeti(lua.L, LUA_REGISTRYINDEX, lua.costfunc);

	char buf[12];
	sprintf(buf, "%x", pThis->obj.obj_generic.obj_id);
	lua_pushstring(lua.L, buf);

	lua.isinhook = true;
	if (lua_pcall(lua.L, 1, 1, 0)){
		lua.isinhook = false;
		int temp = lua.logging;
		lua.logging = 1;
		lua.Log("! GetCost (%08X): %s\n", pThis->obj.obj_generic.obj_id, lua_tostring(lua.L, -1));
		lua.logging = temp;
		lua_pop(lua.L, 1);
	}
	else if (lua_isnumber(lua.L, -1)){
		lua.isinhook = false;
		int ok = lua_tointeger(lua.L, -1);
		lua_pop(lua.L, 1);

		return ok;
	}

	lua.isinhook = false;

	lua_pop(lua.L, 1);

	return CNWSItem__GetCostNext(pThis, NULL, arg1, arg2, arg3, arg4);
}

int(__fastcall *CNWSItem__GetWeightNext)(CNWSItem *pThis, void*);
int  __fastcall CNWSItem__GetWeight(CNWSItem *pThis, void*){

	if (!pThis || lua.isinhook)
		return CNWSItem__GetWeightNext(pThis, NULL);

	lua_pop(lua.L, lua_gettop(lua.L));

	lua_rawgeti(lua.L, LUA_REGISTRYINDEX, lua.weightfunc);

	char buf[12];
	sprintf(buf, "%x", pThis->obj.obj_generic.obj_id);
	lua_pushstring(lua.L, buf);

	lua.isinhook = true;
	if (lua_pcall(lua.L, 1, 1, 0)){
		lua.isinhook = false;
		int temp = lua.logging;
		lua.logging = 1;
		lua.Log("! GetWeight (%08X): %s\n", pThis->obj.obj_generic.obj_id, lua_tostring(lua.L, -1));
		lua.logging = temp;
		lua_pop(lua.L, 1);
	}
	else if (lua_isnumber(lua.L, -1)){
		lua.isinhook = false;
		int ok = lua_tointeger(lua.L, -1);
		lua_pop(lua.L, 1);

		return ok;
	}

	lua.isinhook = false;

	lua_pop(lua.L, 1);

	return CNWSItem__GetWeightNext(pThis, NULL);
}

void ApplyWeightHook(){
	HookCode((PVOID)0x00501630, CNWSItem__GetWeight, (PVOID*)&CNWSItem__GetWeightNext);
}

void ApplyCostHook(){
	HookCode((PVOID)0x00500EE0, CNWSItem__GetCost, (PVOID*)&CNWSItem__GetCostNext);
}

void ApplyCanEquipHook(){

	HookCode((PVOID)0x004BF4E0, CNWSCreature__CanEquipWeapon, (PVOID*)&CNWSCreature__CanEquipWeaponNext);
	HookCode((PVOID)0x004BFF30, CNWSCreature__CanEquipShield, (PVOID*)&CNWSCreature__CanEquipShieldNext);
	HookCode((PVOID)0x004C0040, CNWSCreature__CanEquipMiscellaneous, (PVOID*)&CNWSCreature__CanEquipMiscellaneousNext);
}

void ApplyEventHandlerHook(){
	HookCode((PVOID)0x00496CE0, CNWSCreature__EventHandler, (PVOID*)&CNWSCreature__EventHandlerNext);
	HookCode((PVOID)0x004656D0, CNWSStore__EventHandler, (PVOID*)&CNWSStore__EventHandlerNext);
	HookCode((PVOID)0x004D1E90, CNWSModule__EventHandler, (PVOID*)&CNWSModule__EventHandlerNext);
	HookCode((PVOID)0x004FB810, CNWSItem__EventHandler, (PVOID*)&CNWSItem__EventHandlerNext);
	HookCode((PVOID)0x0050AB70, CNWSArea__EventHandler, (PVOID*)&CNWSArea__EventHandlerNext);
	HookCode((PVOID)0x00520CB0, CNWSPlaceable__EventHandler, (PVOID*)&CNWSPlaceable__EventHandlerNext);
	HookCode((PVOID)0x00523DC0, CNWSDoor__EventHandler, (PVOID*)&CNWSDoor__EventHandlerNext);
	HookCode((PVOID)0x005A3CA0, CNWSAreaOfEffectObject__EventHandler, (PVOID*)&CNWSAreaOfEffectObject__EventHandlerNext);
	HookCode((PVOID)0x005A6E00, CNWSTrigger__EventHandler, (PVOID*)&CNWSTrigger__EventHandlerNext);
	HookCode((PVOID)0x005AA230, CNWSEncounter__EventHandler, (PVOID*)&CNWSEncounter__EventHandlerNext);
	HookCode((PVOID)0x005B53C0, CNWSWaypoint__EventHandler, (PVOID*)&CNWSWaypoint__EventHandlerNext);
	HookCode((PVOID)0x005B5C30, CNWSPlayerTURD__EventHandler, (PVOID*)&CNWSPlayerTURD__EventHandlerNext);
}

void ApplyVisibilityHook(){
	HookCode((PVOID)0x004409F0, CNWSMessage__TestObjectVisible, (PVOID*)&CNWSMessage__TestObjectVisibleNext);
}

void ApplyMainLoopHook(){
	HookCode((PVOID)0x0042CA10, CServerExoApp__MainLoop, (PVOID*)&CServerExoApp__MainLoopNext);
}

void ApplyHooks(){

	int ok = 1;

	ok = ok && HookCode((PVOID)0x005C0A10, CVirtualMachine__StackPushObject, (PVOID*)&CVirtualMachine__StackPushObjectNext);
	ok = ok && HookCode((PVOID)0x005C0860, CVirtualMachine__StackPushString, (PVOID*)&CVirtualMachine__StackPushStringNext);
	ok = ok && HookCode((PVOID)0x005C05E0, CVirtualMachine__StackPushInteger, (PVOID*)&CVirtualMachine__StackPushIntegerNext);
	ok = ok && HookCode((PVOID)0x005C0660, CVirtualMachine__StackPushFloat, (PVOID*)&CVirtualMachine__StackPushFloatNext);
	ok = ok && HookCode((PVOID)0x005C09C0, CVirtualMachine__StackPopObject, (PVOID*)&CVirtualMachine__StackPopObjectNext);
	ok = ok && HookCode((PVOID)0x005C0800, CVirtualMachine__StackPopString, (PVOID*)&CVirtualMachine__StackPopStringNext);
	ok = ok && HookCode((PVOID)0x005C0590, CVirtualMachine__StackPopInteger, (PVOID*)&CVirtualMachine__StackPopIntegerNext);
	ok = ok && HookCode((PVOID)0x005C0610, CVirtualMachine__StackPopFloat, (PVOID*)&CVirtualMachine__StackPopFloatNext);

	ok = ok && HookCode((PVOID)0x005A10F0, CNWSScriptVarTable__GetString, (PVOID*)&CNWSScriptVarTable__GetStringNext);

	lua.Log("Hooking VM: %s\n\n", (ok == 1 ? "ok" : "broken"));

	lua.Log("\n\n");
}