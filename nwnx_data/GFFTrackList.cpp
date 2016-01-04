#include "Aurora.h"

GFFTrackList::GFFTrackList(){

	memset(this, 0, sizeof(GFFTrackList));
}

GFFTrackList::~GFFTrackList(){

	if (this->structs)
		free(this->structs);
	if (this->fields)
		free(this->fields);
	if (this->labels)
		free(this->labels);
}

void GFFTrackList::AddStruct(GFFStruct * data){

	for (DWORD n = 0; n < cntStructs; n++){

		if (structs[n] == data){
			return;
		}
	}

	void * temp = realloc(structs, sizeof(GFFStruct*)*(cntStructs + 1));
	if (temp){

		structs = (GFFStruct**)temp;
		structs[cntStructs] = data;
		cntStructs++;
	}
}

void GFFTrackList::AddField(GFFField * data){

	for (DWORD n = 0; n < cntFields; n++){

		if (fields[n] == data){
			return;
		}
	}

	void * temp = realloc(fields, sizeof(GFFField*)*(cntFields + 1));
	if (temp){

		fields = (GFFField**)temp;
		fields[cntFields] = data;
		cntFields++;
	}
}

void GFFTrackList::AddLabel(char * data){

	for (DWORD n = 0; n < cntLabels; n++){

		if (strcmp( labels[n], data ) == 0 ){
			return;
		}
	}

	void * temp = realloc(labels, sizeof(char*)*(cntLabels + 1));
	if (temp){

		labels = (char**)temp;
		labels[cntLabels] = data;
		cntLabels++;
	}
}

GFFHeader GFFTrackList::CreateHeader(){

	GFFHeader GH;
	memset(&GH, 0, sizeof(GFFHeader));

	GH.StructOffset = sizeof(GFFHeader);
	GH.StructCount = cntStructs;
	GH.FieldOffset = GH.StructOffset + (sizeof(GFFStruct)*GH.StructCount);
	GH.FieldCount = cntFields;
	GH.LabelOffset = GH.FieldOffset + (sizeof(GFFField)*GH.FieldCount);
	GH.LabelCount = cntLabels;
	GH.FieldDataOffset = GH.LabelOffset + (16 * GH.LabelCount);
	GH.FieldDataCount = DataFieldSize;
	GH.FieldIndicesOffset = GH.FieldDataOffset + GH.FieldDataCount;
	GH.FieldIndicesCount = FieldIndicesCNT * sizeof(DWORD);
	GH.ListIndicesOffset = GH.FieldIndicesOffset + GH.FieldIndicesCount;
	GH.ListIndicesCount = ListIndicesCNT * sizeof(DWORD);

	return GH;
}