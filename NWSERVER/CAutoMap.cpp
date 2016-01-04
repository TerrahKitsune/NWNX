#include "types.h"
#include "CAutoMap.h"

CMiniMapData **CCreatureMaps::ResizeMapList(int NewSize) {
	
	CNWNXMemory mem;
	
	CMiniMapData **p = (CMiniMapData **)mem.nwnx_malloc( sizeof( CMiniMapData*)*NewSize );	

	memcpy(p, this->MiniMap, this->MiniMapCount);

	p[this->MiniMapCount] = NULL;

	mem.nwnx_free( this->MiniMap );

	return p;
}

nwn_objid_t *CCreatureMaps::CopyAreas(int NewSize) {
	
	CNWNXMemory mem;
	
	nwn_objid_t *p = (nwn_objid_t *)mem.nwnx_malloc( sizeof( nwn_objid_t ) * NewSize );
	memcpy(p, this->Areas, this->AreaCount * 4);
	mem.nwnx_free( this->Areas );
	return p;
}

CMiniMapData **CCreatureMaps::Copy(CMiniMapData** CopyFrom, unsigned int nSize) {

	CNWNXMemory mem;
	
	CMiniMapData **p = (CMiniMapData **)mem.nwnx_malloc( sizeof( CMiniMapData*)*nSize );	

	CMiniMapData *d = NULL;
	for (unsigned int i=0; i<nSize; i++) {
		d = (CMiniMapData*)mem.nwnx_malloc( sizeof( CMiniMapData ) );
		memcpy(d, CopyFrom[i], 0x80);
		p[i] = d;
	}
	return p;
}

void CCreatureMaps::DeleteMiniMaps() {
	
	CNWNXMemory mem;
	
	if (MiniMap) {
		if (MiniMapCount > 0) {
			for (unsigned int i=0; i<MiniMapCount; i++) {
				mem.nwnx_free( MiniMap[i] );
				MiniMap[i] = NULL;
			}
			mem.nwnx_free( MiniMap );
			MiniMap = NULL;
		}
	}
	MiniMapCount = 0;
}

void CCreatureMaps::DeleteAreas() {
	CNWNXMemory mem;
	
	if (Areas) {
		mem.nwnx_free( Areas );
		Areas = NULL;
	}
	AreaAlloc = 0;
	AreaCount = 0;
}

int CCreatureMaps::IndexOf(nwn_objid_t area){
	return CExoArrayList_uint32_contains((CExoArrayList_uint32*)&Areas, area);
}

bool CCreatureMaps::Exists(nwn_objid_t areaid){
	int iIndex = CExoArrayList_uint32_contains((CExoArrayList_uint32*)&Areas, areaid);
	return iIndex != -1;
}

void CCreatureMaps::AddArea(unsigned areaid) {

	MiniMap = ResizeMapList(MiniMapCount+1);
	
	CNWNXMemory mem;

	MiniMap[MiniMapCount] = (CMiniMapData*)mem.nwnx_malloc( sizeof( CMiniMapData ) );
	memset( MiniMap[MiniMapCount], 0, sizeof( CMiniMapData ) );
	++MiniMapCount;

	//CExoArrayList_uint32_add((CExoArrayList_uint32*)&Areas, areaid);
	CExoArrayList_uint32___Add( &Areas, NULL, areaid );
}


void CCreatureMaps::RemoveArea(nwn_objid_t area, int index) {

	memcpy(&Areas[index], &Areas[index + 1], (AreaCount - index - 1) * 4);
	AreaCount--;
}

CMiniMapData **CTURDMaps::Copy(CMiniMapData** CopyFrom, unsigned int nSize) {
	
	CNWNXMemory mem;

	CMiniMapData **p = (CMiniMapData **)mem.nwnx_malloc( sizeof(CMiniMapData*) * nSize );
	CMiniMapData *d = NULL;
	for (unsigned int i=0; i<nSize; i++) {
		d = (CMiniMapData*)mem.nwnx_malloc( sizeof( CMiniMapData ) );
		memcpy(d, CopyFrom[i], 0x80);
		p[i] = d;
	}
	return p;
}

CMiniMapData **CTURDMaps::ResizeMapList(int NewSize) {
	
	CNWNXMemory mem;

	CMiniMapData **p = (CMiniMapData **)mem.nwnx_malloc( sizeof(CMiniMapData*) * NewSize );
	for (unsigned int i=0; i<this->AreaCount; i++) {
		p[i] = this->MiniMap[i];
	}
	mem.nwnx_free( this->MiniMap );

	this->MiniMap = p;

	return p;
}

nwn_objid_t *CTURDMaps::CopyAreas(int NewSize) {
	
	CNWNXMemory mem;

	nwn_objid_t *p = (nwn_objid_t*)mem.nwnx_malloc( sizeof( nwn_objid_t ) * NewSize);
	memcpy(p, this->Areas, this->AreaCount * 4);
	mem.nwnx_free( this->Areas );
	this->Areas = p;
	this->AreaCount++;
	return p;
}


void CTURDMaps::AddArea(unsigned areaid) {

	MiniMap = ResizeMapList(AreaCount+1);

	CNWNXMemory mem;

	MiniMap[AreaCount] = (CMiniMapData*)mem.nwnx_malloc(sizeof(CMiniMapData));
	memset(MiniMap[AreaCount], 0, sizeof(CMiniMapData));
	++AreaCount;

	//CExoArrayList_uint32_add((CExoArrayList_uint32*)&Areas, areaid);
	CExoArrayList_uint32___Add(&Areas, NULL, areaid);
}

void CTURDMaps::RemoveArea(unsigned areaid) {
	if( !&Areas )return;
	int iIndex = CExoArrayList_uint32_contains((CExoArrayList_uint32*)&Areas, areaid);

	if (iIndex != -1) {

		CNWNXMemory mem;
		CMiniMapData **Map = (CMiniMapData**)mem.nwnx_malloc( sizeof( unsigned ) * (AreaCount-1) );

		int j=0;
		for (unsigned int i=0; i<AreaCount; i++) {
			if (i != iIndex) {
				Map[j++] = MiniMap[i];
			}
			else {
				mem.nwnx_free( MiniMap[i] );
			}
		}
		mem.nwnx_free( MiniMap );
		MiniMap = Map;
		

		nwn_objid_t *areas = (nwn_objid_t*)mem.nwnx_malloc( sizeof( nwn_objid_t )*(AreaCount-1) );
		j=0;
		for (unsigned int i=0; i<AreaCount; i++) {
			if (i != iIndex) {
				areas[j++] = Areas[i];
			}
		}
		mem.nwnx_free( Areas );
		Areas = areas;
		AreaCount--;
	}
}