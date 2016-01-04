#include "Aurora.h"

GFFCVoid::GFFCVoid(){

	memset(this, 0, sizeof(GFFCVoid));
}

GFFCVoid::GFFCVoid(void * data, DWORD size){

	memset(this, 0, sizeof(GFFCVoid));
	this->CopyTo(data, size);
}

GFFCVoid::~GFFCVoid(){

	if (this->data)
		delete[]this->data;
}

bool GFFCVoid::Unpack(const char * buffer, DWORD offset){

	if (this->data)
		delete[]this->data;

	memcpy(&this->Size, &buffer[offset], sizeof(DWORD));
	this->data = new BYTE[this->Size];
	if (!this->data)
		return false;
	memcpy(this->data, &buffer[offset + sizeof(DWORD)], this->Size);

	return true;
}

void GFFCVoid::Set(void * data, DWORD size){

	this->data = (BYTE*)data;
	this->Size = size;
}

void GFFCVoid::CopyTo(void* data, DWORD size){

	if (this->Size > size)
		return;

	if (this->data)
		delete[]this->data;

	this->data = new BYTE[size];
	if (this->data){
		memcpy(this->data, data,this->Size);
		this->Size = size;
	}
	else
		this->Size = 0;
}