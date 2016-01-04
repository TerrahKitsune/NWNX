#include "Aurora.h"

GFFCExoLocSubString::GFFCExoLocSubString(){

	memset(this, 0, sizeof(GFFCExoLocSubString));
}

GFFCExoLocSubString::GFFCExoLocSubString(const char * str){
	
	this->StringLength = strlen(str);
	this->str = new char[this->StringLength + 1];
	if (this->str){
		strcpy(this->str, str);
	}
	else
		this->StringLength = 0;
}

GFFCExoLocSubString::~GFFCExoLocSubString(){

	if (this->str)
		delete[]this->str;
}

bool GFFCExoLocSubString::Unpack(const char * buffer, DWORD offset){

	if (this->str)
		delete[]this->str;

	memcpy(this, &buffer[offset], sizeof(DWORD)*2);

	if (this->StringLength < 0)
		return false;

	this->str = new char[this->StringLength+1];
	if (!this->str)
		return false;
	memcpy(this->str, &buffer[offset + (sizeof(DWORD)*2)], this->StringLength);
	this->str[this->StringLength] = '\0';

	return true;
}

GFFCExoLocSubString * GFFCExoLocSubString::operator = (const char * str){

	if (this->str)
		delete[]this->str;

	this->StringLength = strlen(str);
	this->str = new char[this->StringLength + 1];
	if (this->str){
		strcpy(this->str, str);
	}
	else
		this->StringLength = 0;

	return this;
}

GFFCExoLocSubString * GFFCExoLocSubString::operator += (const char * str){

	if (this->str){

		void * temp = realloc(this->str, this->StringLength + strlen(str) + 1);
		if (temp){
			
			this->str = (char*)temp;
			strcpy(&this->str[this->StringLength], str);
			this->StringLength += strlen(str);
		}
	}
	else{
		
		this->StringLength = strlen(str);
		this->str = new char[this->StringLength + 1];
		if (this->str){
			strcpy(this->str, str);
		}
		else
			this->StringLength = 0;
	}

	return this;
}

const char * GFFCExoLocSubString::c_str(){
	return this->str;
}