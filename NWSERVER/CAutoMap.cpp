#include "types.h"
#include "CAutoMap.h"

CMiniMapData **CCreatureMaps::ResizeMapList(int NewSize) {
	
	CNWNXMemory mem;
	
	CMiniMapData **p = (CMiniMapData **)mem.nwnx_malloc( sizeof( CMiniMapData*)*NewSize );	

	for (unsigned int i=0; i<this->MiniMapCount; i++) {
		p[i] = this->MiniMap[i];
	}
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

void CCreatureMaps::AddArea(unsigned areaid) {
	MiniMap = ResizeMapList(MiniMapCount+1);

	CNWNXMemory mem;

	MiniMap[MiniMapCount] = (CMiniMapData*)mem.nwnx_malloc( sizeof( CMiniMapData ) );
	memset( MiniMap[MiniMapCount], 0, sizeof( CMiniMapData ) );
	++MiniMapCount;

	//CExoArrayList_uint32_add((CExoArrayList_uint32*)&Areas, areaid);
	CExoArrayList_uint32___Add( &Areas, NULL, areaid );
}


void CCreatureMaps::RemoveArea(unsigned areaid) {

	int iIndex = CExoArrayList_uint32_contains((CExoArrayList_uint32*)&Areas, areaid);
	if (iIndex != -1) {

		CNWNXMemory mem;	
		CMiniMapData **Map = (CMiniMapData**)mem.nwnx_malloc( sizeof( unsigned ) * (MiniMapCount-1) );
		int j=0;
		for (unsigned int i=0; i<MiniMapCount; i++) {
			if (i != iIndex) {
				Map[j++] = MiniMap[i];
			}
			else {
				mem.nwnx_free( MiniMap[i] );
			}
		}
		mem.nwnx_free( MiniMap );
		MiniMap = Map;
		--MiniMapCount;

		CExoArrayList_uint32___Remove(&Areas, NULL, areaid);
	}
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

	return p;
}

nwn_objid_t *CTURDMaps::CopyAreas(int NewSize) {
	
	CNWNXMemory mem;

	nwn_objid_t *p = (nwn_objid_t*)mem.nwnx_malloc( sizeof( nwn_objid_t ) * NewSize);
	memcpy(p, this->Areas, this->AreaCount * 4);
	mem.nwnx_free( this->Areas );
	return p;
}


void CTURDMaps::AddArea(unsigned areaid) {
	
	CNWNXMemory mem;

	MiniMap = ResizeMapList(AreaCount+1);
	MiniMap[AreaCount] = (CMiniMapData*)mem.nwnx_malloc(sizeof(CMiniMapData));

	Areas = CopyAreas(AreaCount+1);
	Areas[AreaCount] = areaid;

	++AreaCount;
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
	}
}