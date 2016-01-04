#include "Aurora.h"

GFFCList::GFFCList(){

	memset(this, 0, sizeof(GFFCList));
}

GFFCList::~GFFCList(){

	if (this->list)
		delete[]this->list;
}

bool GFFCList::Unpack(const char * buffer, DWORD offset){
	
	if (this->list)
		delete[]this->list;

	memcpy(this, &buffer[offset], sizeof(DWORD));

	this->list = new GFFStruct[this->Size];
	if (!this->list)
		return false;

	GFFHeader * GH = (GFFHeader*)buffer;

	DWORD * Index = (DWORD*)&buffer[offset+sizeof(DWORD)];

	for (DWORD n = 0; n < this->Size; n++){

		if (!this->list[n].Unpack( buffer, GH->StructOffset + (Index[n]*sizeof(GFFStruct)))){

			delete[]this->list;
			this->list = NULL;
			return false;
		}
	}

	return true;
}