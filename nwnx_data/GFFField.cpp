#include "Aurora.h"

GFFField::GFFField(){
	
	memset(this, 0, sizeof( GFFField ));
}

GFFField::~GFFField(){

	if (this->Label)
		delete []this->Label;
	
	DeleteData();
}

BYTE GFFField::AsByte(){

	return (BYTE)this->Data;
}
char GFFField::AsChar(){

	return (char)this->Data;
}
WORD GFFField::AsWord(){

	return (WORD)this->Data;
}
short GFFField::AsShort(){

	return (short)this->Data;
}
DWORD GFFField::AsDword(){
	
	return (DWORD)this->Data;
}
int GFFField::AsInt(){

	return (int)this->Data;
}
DWORD64 GFFField::AsDword64(){

	if (this->Type != 6)
		return NULL;

	return *(DWORD64*)this->Data;
}
INT64 GFFField::AsInt64(){

	if (this->Type != 7)
		return NULL;

	return *(INT64*)this->Data;
}
float GFFField::AsFloat(){

	return *(float*)&this->Data;
}
double GFFField::AsDouble(){

	if (this->Type != 9)
		return NULL;

	return *(double*)this->Data;
}
GFFCExoString * GFFField::AsExoString(){

	if (this->Type != 10)
		return NULL;

	return (GFFCExoString*)this->Data;
}
GFFCResRef * GFFField::AsResRef(){

	if (this->Type != 11)
		return NULL;

	return (GFFCResRef*)this->Data;
}
GFFCExoLocString * GFFField::AsExoLocString(){

	if (this->Type != 12)
		return NULL;

	return (GFFCExoLocString*)this->Data;
}
GFFCVoid * GFFField::AsVoid(){

	if (this->Type != 13)
		return NULL;

	return (GFFCVoid*)this->Data;
}
GFFStruct * GFFField::AsStruct(){

	if (this->Type != 14)
		return NULL;

	return (GFFStruct*)this->Data;
}
GFFCList * GFFField::AsList(){

	if (this->Type != 15)
		return NULL;

	return (GFFCList*)this->Data;
}

const char * GFFField::tostring(char * buffer, int nMax){

	return tostring(buffer,nMax,0);
}

const char * GFFField::tostring(char * buffer, int nMax, int nIndex){

	switch (this->Type){

		case 0:sprintf(buffer, "%u", AsByte()); break;
		case 1:sprintf(buffer, "%d", AsChar()); break;
		case 2:sprintf(buffer, "%u", AsWord()); break;
		case 3:sprintf(buffer, "%d", AsShort()); break;
		case 4:sprintf(buffer, "%u", AsDword()); break;
		case 5:sprintf(buffer, "%d", AsInt()); break;
		case 6:sprintf(buffer, "%llu", AsDword64()); break;
		case 7:sprintf(buffer, "%lld", AsInt64()); break;
		case 8:sprintf(buffer, "%f", AsFloat()); break;
		case 9:sprintf(buffer, "%Lf", AsDouble()); break;
		case 10:strncpy(buffer, AsExoString()->str, nMax); break;
		case 11:strncpy(buffer, AsResRef()->str, nMax); break;
		case 12:
			
			if (nIndex >= (int)AsExoLocString()->StringCount)
				buffer[0] = '\0';
			else				
				strncpy(buffer, AsExoLocString()->SubString[nIndex].str, nMax); 
			break;
		case 13:sprintf(buffer, "<binary %u bytes>", AsVoid()->Size); break;
		case 14:sprintf(buffer, "struct ID: %d", AsStruct()->Type ); break;
		case 15:sprintf(buffer, "list %i entries", AsList()->Size); break;
		default:buffer[0] = '\0'; break;
	}

	return buffer;
}

void GFFField::DeleteData(){

	if (this->Type == 6 || this->Type == 7 || (this->Type >= 9 && this->Type <= 15)){

		if (this->Data){

			switch (this->Type){

				case 6: delete((DWORD64*)this->Data); break;
				case 7: delete((INT64*)this->Data); break;
				case 9: delete((double*)this->Data); break;
				case 10: delete((GFFCExoString*)this->Data); break;
				case 11: delete((GFFCResRef*)this->Data); break;
				case 12: delete((GFFCExoLocString*)this->Data); break;
				case 13: delete((GFFCVoid*)this->Data); break;
				case 14: delete((GFFStruct*)this->Data); break;
				case 15: delete((GFFCList*)this->Data); break;
			}
		}
	}

	this->Data = NULL;
}

const char * GFFField::GetTypeString(){

	switch (this->Type){

	case 0: return "Byte";
	case 1: return "Char";
	case 2: return "Word";
	case 3: return "Short";
	case 4: return "Dword";
	case 5: return "Int";
	case 6: return "Dword64";
	case 7: return "Int64";
	case 8: return "Float";
	case 9: return "Double";
	case 10: return "GFFCExoString";
	case 11: return "ResRef";
	case 12: return "GFFCExoLocString";
	case 13: return "Void";
	case 14: return "Struct";
	case 15: return "List";
	default:break;
	}

	return "Invalid";
}

bool GFFField::Unpack(const char * buffer, DWORD offset){

	if (this->Label)
		delete[]this->Label;
	DeleteData();

	if (offset >= ((GFFHeader*)buffer)->LabelOffset){

		memset(this, 0, sizeof(GFFField));
		return false;
	}

	memcpy(this, &buffer[offset], sizeof(GFFField));
	
	DWORD LabelIndex = (DWORD)this->Label;

	this->Label = new char[LABEL_MAX+1];
	if (!this->Label)
		return false;

	GFFHeader * GH = (GFFHeader *)buffer;
	memcpy(this->Label, &buffer[GH->LabelOffset + (LabelIndex*LABEL_MAX)], LABEL_MAX);
	this->Label[LABEL_MAX] = '\0';

	//printf("%s: %s\n", this->GetTypeString(), this->Label);

	if ( this->Type == 6 || this->Type == 7 || ( this->Type >= 9 && this->Type <= 15 ) ){

		DWORD Offset = (DWORD)this->Data;
		this->Data = NULL;
		
		if (this->Type == 6){

			this->Data = new DWORD64;
			if (!this->Data)
				return false;
			
			memcpy(this->Data, &buffer[GH->FieldDataOffset+Offset], sizeof(DWORD64));
		}
		else if (this->Type == 7){

			this->Data = new INT64;
			if (!this->Data)
				return false;

			memcpy(this->Data, &buffer[GH->FieldDataOffset + Offset], sizeof(INT64));
		}
		else if (this->Type == 9){

			this->Data = new double;
			if (!this->Data)
				return false;

			memcpy(this->Data, &buffer[GH->FieldDataOffset + Offset], sizeof(double));
		}
		else if (this->Type == 10){

			this->Data = new GFFCExoString;
			if (!this->Data)
				return false;

			if (!((GFFCExoString*)this->Data)->Unpack(buffer, GH->FieldDataOffset + Offset)){

				return false;
			}
		}
		else if (this->Type == 11){

			this->Data = new GFFCResRef;
			if (!this->Data)
				return false;

			if (!((GFFCResRef*)this->Data)->Unpack(buffer, GH->FieldDataOffset + Offset)){

				return false;
			}
		}
		else if (this->Type == 12){

			this->Data = new GFFCExoLocString;
			if (!this->Data)
				return false;

			if (!((GFFCExoLocString*)this->Data)->Unpack(buffer, GH->FieldDataOffset + Offset)){

				return false;
			}
		}
		else if (this->Type == 13){

			this->Data = new GFFCVoid;
			if (!this->Data)
				return false;

			if (!((GFFCVoid*)this->Data)->Unpack(buffer, GH->FieldDataOffset + Offset)){

				return false;
			}
		}
		else if (this->Type == 14){

			this->Data = new GFFStruct;
			if (!this->Data)
				return false;

			if (!((GFFStruct*)this->Data)->Unpack(buffer, GH->StructOffset + (Offset*sizeof(GFFStruct)))){

				return false;
			}
		}
		else if (this->Type == 15){

			this->Data = new GFFCList;
			if (!this->Data)
				return false;

			if (!((GFFCList*)this->Data)->Unpack(buffer, GH->ListIndicesOffset+Offset ) ){

				return false;
			}
		}

	}

	return true;
}

DWORD GFFField::GetDataSize(){

	DWORD size = 0;

	switch (this->Type){

	case 6:size = sizeof(DWORD64); break;
	case 7:size = sizeof(INT64); break;
	case 9:size = sizeof(double); break;
	case 10:size = this->AsExoString()->Size + sizeof(DWORD); break;
	case 11:size = this->AsResRef()->Size + sizeof(BYTE); break;
	case 12:size = this->AsExoLocString()->TotalSize + sizeof(DWORD); break;
	case 13:size = this->AsVoid()->Size + sizeof(DWORD); break;

	}

	return size;
}