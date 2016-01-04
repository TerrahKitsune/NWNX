#include "Aurora.h"

GFFStruct::GFFStruct(){

	memset(this, 0, sizeof(GFFStruct));
}

GFFStruct::~GFFStruct(){

	if (Fields){

		if (FieldCount==1)
			delete Fields;
		else
			delete[]Fields;
	}
}

bool GFFStruct::Unpack(const char * buffer, DWORD offset){

	if (Fields){

		if (FieldCount == 1)
			delete Fields;
		else
			delete[]Fields;
	}

	memcpy(this, &buffer[offset], sizeof(GFFStruct));
	DWORD fieldOffset = (DWORD)this->Fields;
	
	if (this->FieldCount == 1)
		this->Fields = new GFFField;
	else
		this->Fields = new GFFField[this->FieldCount];

	if (!this->Fields)
		return false;

	GFFHeader * GH = (GFFHeader *)buffer;

	if (this->FieldCount == 1 ){

		if (!this->Fields->Unpack(buffer, GH->FieldOffset + (fieldOffset*sizeof(GFFField)))){

			delete this->Fields;
			this->Fields = NULL;
			return false;
		}
	}
	else{

		DWORD * Index = (DWORD*)&buffer[GH->FieldIndicesOffset + fieldOffset];

		for (DWORD n = 0; n < this->FieldCount; n++){

			if (!this->Fields[n].Unpack(buffer, GH->FieldOffset + (Index[n] * sizeof(GFFField)))){

				if (Fields){

					if (FieldCount == 1)
						delete Fields;
					else
						delete[]Fields;
				}
				this->Fields = NULL;
				return false;
			}
		}
	}

	return true;
}


GFFTrackList * GFFStruct::CalculateHeader(GFFTrackList * current){
	
	if (this->FieldCount == 1){

		current->AddField(this->Fields);
		current->AddLabel(this->Fields->Label);
		current->DataFieldSize += this->Fields->GetDataSize();
	}
	else{

		current->FieldIndicesCNT += this->FieldCount;

		for (DWORD n = 0; n<this->FieldCount; n++){

			if (this->Fields && this->Fields[n].Type >= 0 && this->Fields[n].Type <= 15 && this->Fields[n].Label){

				current->AddField(&this->Fields[n]);
				current->AddLabel(this->Fields[n].Label);
				current->DataFieldSize += this->Fields[n].GetDataSize();
				
				if (this->Fields[n].Type == 14){

					current = this->Fields[n].AsStruct()->CalculateHeader(current);
				}
				else if (this->Fields[n].Type == 15){

					GFFCList * l = this->Fields[n].AsList();
					current->ListIndicesCNT += (l->Size+1);

					if (l->Size == 1){
						current = l->list->CalculateHeader(current);
					}
					else{
						for (DWORD m = 0; m < l->Size; m++){

							current = l->list[m].CalculateHeader(current);
						}
					}
				}
			}		
		}
	}

	current->AddStruct(this);

	return current;
}