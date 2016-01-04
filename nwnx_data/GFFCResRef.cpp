#include "Aurora.h"

GFFCResRef::GFFCResRef(){

	memset(this, 0, sizeof(GFFCResRef));
}

GFFCResRef::GFFCResRef(const char * str){

	this->Size = strlen(str);
	if (this->Size > RESREF_MAX)
		this->Size = RESREF_MAX;

	this->str = new char[this->Size + 1];

	if (this->str){

		strncpy(this->str, str, RESREF_MAX);
		this->str[RESREF_MAX] = '\0';
	}
	else
		this->Size = 0;
}

GFFCResRef::~GFFCResRef(){

	if (this->str)
		delete[]this->str;

}

bool GFFCResRef::Unpack(const char * buffer, DWORD offset){

	if (this->str)
		delete[]this->str;

	memcpy(&this->Size, &buffer[offset], sizeof(BYTE));

	if (this->Size > RESREF_MAX)
		this->Size = RESREF_MAX;

	this->str = new char[this->Size+1];
	if (!this->str)
		return false;
	memcpy(this->str, &buffer[offset + sizeof(BYTE)], this->Size);
	this->str[this->Size] = '\0';

	return true;
}

GFFCResRef * GFFCResRef::operator = (const char * str){

	if (this->str)
		delete[]this->str;

	this->Size = strlen(str);
	if (this->Size > RESREF_MAX)
		this->Size = RESREF_MAX;

	this->str = new char[this->Size + 1];

	if (this->str){

		strncpy(this->str, str, RESREF_MAX);
		this->str[RESREF_MAX] = '\0';
	}
	else
		this->Size = 0;

	return this;
}

const char * GFFCResRef::c_str(){
	return this->str;
}