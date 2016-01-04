#pragma once
#include "NWNLuaLib.h"
#include "../NWNXdll/IniFile.h"
#include "../NWSERVER/types.h"

extern CNWNXLua lua;

void ApplyWeightHook();
void ApplyCostHook();
void ApplyCanEquipHook();
void ApplyEventHandlerHook();
void ApplyMainLoopHook();
void ApplyHooks();
void ApplyVisibilityHook();