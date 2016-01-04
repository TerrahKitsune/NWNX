#include "Aurora.h"

GFFCExoString::GFFCExoString(){

	memset(this, 0, sizeof(GFFCExoString));
}

GFFCExoString::~GFFCExoString(){

	if (this->str)
		delete[]this->str;

}

bool GFFCExoString::Unpack(const char * buffer, DWORD offset){

	if (this->str)
		delete[]this->str;

	memcpy(&this->Size, &buffer[offset], sizeof(DWORD));
	this->str = new char[this->Size+1];
	if (!this->str)
		return false;
	memcpy(this->str, &buffer[offset + sizeof(DWORD)], this->Size);
	this->str[this->Size] = '\0';

	return true;
}