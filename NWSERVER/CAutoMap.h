#pragma once

#include "nwnx_memory.h"
#include <string.h>

class CMiniMapData {
	char MAP[0x80];
public:
	CMiniMapData() {memset(MAP, 0, 0x80);}
};

class CCreatureMaps {
public:
	uint32_t MiniMapCount;
	CMiniMapData **MiniMap;
	nwn_objid_t *Areas;
	uint32_t AreaCount;
	uint32_t AreaAlloc;
	uint32_t AreaCount2;

	bool Exists(nwn_objid_t areaid);
	void AddArea(unsigned areid);
	void RemoveArea(nwn_objid_t area, int index);
	CMiniMapData **ResizeMapList(int NewSize);
	CMiniMapData **Copy(CMiniMapData** CopyFrom, unsigned int nSize);
	void DeleteMiniMaps();
	void DeleteAreas();
	nwn_objid_t *CopyAreas(int NewSize);
	int IndexOf(nwn_objid_t area);
};

class CTURDMaps {
public:
	uint32_t AreaCount;
	nwn_objid_t *Areas;
	CMiniMapData **MiniMap;

	void AddArea(unsigned areid);
	void RemoveArea(unsigned areaid);
	CMiniMapData **ResizeMapList(int NewSize);
	CMiniMapData **Copy(CMiniMapData** CopyFrom, unsigned int nSize);
	nwn_objid_t *CopyAreas(int NewSize);
};