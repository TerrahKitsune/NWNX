#include "Aurora.h"

GFFCExoLocString::GFFCExoLocString(){

	memset(this, 0, sizeof(GFFCExoLocString));
}

GFFCExoLocString::~GFFCExoLocString(){

	if (this->StringCount == 1 && this->SubString)
		delete this->SubString;
	else if (this->StringCount > 1 && this->SubString)
		delete[]this->SubString;
}

GFFCExoLocString::GFFCExoLocString(const char * str){
	memset(this, 0, sizeof(GFFCExoLocString));

	this->SubString = new GFFCExoLocSubString( str );
	if (this->SubString){
		this->StringCount = 1;
		this->TotalSize = this->SubString->StringLength;
	}

	this->TotalSize += sizeof(DWORD)*2;
}

bool GFFCExoLocString::Unpack(const char * buffer, DWORD offset){

	if (this->StringCount == 1 && this->SubString)
		delete this->SubString;
	else if (this->StringCount > 1 && this->SubString)
		delete[]this->SubString;

	memcpy(this, &buffer[offset], sizeof(DWORD)* 3);

	if (StringCount == 0){
		this->SubString = NULL;
		return true;
	}
	else if (this->StringCount == 1)
		this->SubString = new GFFCExoLocSubString;
	else
		this->SubString = new GFFCExoLocSubString[this->StringCount];

	if (!this->SubString)
		return false;

	DWORD subStrOffset = offset + (sizeof(DWORD)* 3);

	for (DWORD n = 0; n < this->StringCount; n++){

		if (!this->SubString[n].Unpack(buffer, subStrOffset)){
		
			if (this->StringCount == 1 && this->SubString)
				delete this->SubString;
			else if (this->StringCount > 1 && this->SubString)
				delete[]this->SubString;

			this->SubString = NULL;
			
			return false;
		}
		else{
			
			subStrOffset += this->SubString[n].StringLength + (sizeof(DWORD)*2);
		}

		//printf("ExoLoc (%u): %s\n", n, this->SubString[n].str);
	}

	return true;
}