#include "Aurora.h"

GFFPack::GFFPack(GFF * gPack){

	memset(this, 0, sizeof(GFF));

	if (!gPack->TopStruct)
		return;

	this->UNPACKED = gPack;

	GFFTrackList * node = new GFFTrackList;
	node = this->UNPACKED->TopStruct->CalculateHeader(node);
	GFFHeader GH = node->CreateHeader();
	delete node;

	memcpy(&GH.FileType, this->UNPACKED->Type, 4);
	memcpy(&GH.FileVersion, this->UNPACKED->Ver, 4);

	TotalSize = GH.ListIndicesOffset + GH.ListIndicesCount;

	this->RAW = new char[TotalSize];

	if (!this->RAW){

		return;
	}

	memset(this->RAW, 0, GH.ListIndicesOffset + GH.ListIndicesCount);
	memcpy(this->RAW, &GH, sizeof(GFFHeader));

	this->Header = (GFFHeader*)this->RAW;

	GH = *this->Header;
	this->StructArray = (GFFStruct*)&this->RAW[GH.StructOffset];
	this->FieldArray = (GFFField*)&this->RAW[GH.FieldOffset];
	this->LabelArray = (char*)&this->RAW[GH.LabelOffset];
	this->FieldDataBlock = (BYTE*)&this->RAW[GH.FieldDataOffset];
	this->FieldIndicesArray = (DWORD*)&this->RAW[GH.FieldIndicesOffset];
	this->ListIndicesArray = (DWORD*)&this->RAW[GH.ListIndicesOffset];

	/*printf("\nFileType: %c%c%c%c\n", GH.FileType[0], GH.FileType[1], GH.FileType[2], GH.FileType[3]);
	printf("FileVersion: %c%c%c%c\n", GH.FileVersion[0], GH.FileVersion[1], GH.FileVersion[2], GH.FileVersion[3]);
	printf("StructOffset: %u\n", GH.StructOffset);
	printf("StructCount: %u\n", GH.StructCount);
	printf("FieldOffset: %u\n", GH.FieldOffset);
	printf("FieldCount: %u\n", GH.FieldCount);
	printf("LabelOffset: %u\n", GH.LabelOffset);
	printf("LabelCount: %u\n", GH.LabelCount);
	printf("FieldDataOffset: %u\n", GH.FieldDataOffset);
	printf("FieldDataCount: %u\n", GH.FieldDataCount);
	printf("FieldIndicesOffset: %u\n", GH.FieldIndicesOffset);
	printf("FieldIndicesCount: %u\n", GH.FieldIndicesCount);
	printf("ListIndicesOffset: %u\n", GH.ListIndicesOffset);
	printf("ListIndicesCount: %u\n", GH.ListIndicesCount);
	printf("Size: %u\n\n", GH.ListIndicesOffset + GH.ListIndicesCount);*/

	if (this->AddStruct(UNPACKED->TopStruct) == -1){

		delete[]RAW;
		memset(this, 0, sizeof(GFFPack));
	}
}

GFFPack::~GFFPack(){

	if (this->RAW)
		delete[]this->RAW;
}

int GFFPack::AddStruct(GFFStruct * data){

	if (!data->Fields || this->cntStructs >= this->Header->StructCount)
		return -1;

	int ret = cntStructs;
	GFFStruct * cur = &StructArray[cntStructs++];

	if (!data->Fields)
		return ret;

	memcpy(cur, data, sizeof(GFFStruct));

	if (cur->FieldCount == 1){

		cur->Fields = (GFFField*)AddField(data->Fields);
	}
	else{

		DWORD current = cntFieldIndices;
		cur->Fields = (GFFField*)(cntFieldIndices*sizeof(DWORD));
		cntFieldIndices += data->FieldCount;
		for (DWORD n = 0; n < data->FieldCount; n++){

			FieldIndicesArray[current + n] = (DWORD)this->AddField(&data->Fields[n]);

			//if (FieldIndicesArray[((int)cur->Fields) + n] == -1){

			//	return -1;
			//}
		}
	}

	return ret;
}

int GFFPack::AddField(GFFField * data){

	if (!data->Label || data->Type < 0 || data->Type > 15 || this->cntFields >= this->Header->FieldCount)
		return -1;

	int ret = (int)cntFields;
	GFFField * cur = &FieldArray[cntFields++];

	memcpy(cur, data, sizeof(GFFField));

	cur->Label = (char*)AddLabel(data->Label);

	DWORD size;
	GFFCExoLocString * str;
	GFFCExoLocSubString * subcr;

	switch (data->Type){

	case 6:
		size = sizeof(DWORD64);
		cur->Data = (void*)cntData;
		memcpy(&FieldDataBlock[cntData], data->Data, size);
		cntData += size;
		break;

	case 7:
		size = sizeof(INT64);
		cur->Data = (void*)cntData;
		memcpy(&FieldDataBlock[cntData], data->Data, size);
		cntData += size;
		break;

	case 9:
		size = sizeof(double);
		cur->Data = (void*)cntData;
		memcpy(&FieldDataBlock[cntData], data->Data, size);
		cntData += size;
		break;
	case 10:
		size = data->AsExoString()->Size;
		cur->Data = (void*)cntData;
		memcpy(&FieldDataBlock[cntData], data->Data, sizeof(DWORD));
		cntData += sizeof(DWORD);
		memcpy(&FieldDataBlock[cntData], data->AsExoString()->str, size);
		cntData += size;
		break;
	case 11:
		size = data->AsResRef()->Size;
		cur->Data = (void*)cntData;
		memcpy(&FieldDataBlock[cntData], data->Data, sizeof(BYTE));
		cntData += sizeof(BYTE);
		memcpy(&FieldDataBlock[cntData], data->AsResRef()->str, size);
		cntData += size;
		break;
	case 12:

		cur->Data = (void*)cntData;
		str = data->AsExoLocString();

		memcpy(&FieldDataBlock[cntData], &str->TotalSize, sizeof(DWORD));
		cntData += sizeof(DWORD);

		memcpy(&FieldDataBlock[cntData], &str->StringRef, sizeof(DWORD));
		cntData += sizeof(DWORD);

		memcpy(&FieldDataBlock[cntData], &str->StringCount, sizeof(DWORD));
		cntData += sizeof(DWORD);

		for (DWORD n = 0; n < str->StringCount; n++){

			subcr = &str->SubString[n];

			memcpy(&FieldDataBlock[cntData], &subcr->StringID, sizeof(DWORD));
			cntData += sizeof(DWORD);

			memcpy(&FieldDataBlock[cntData], &subcr->StringLength, sizeof(DWORD));
			cntData += sizeof(DWORD);

			memcpy(&FieldDataBlock[cntData], subcr->str, subcr->StringLength);
			cntData += subcr->StringLength;

		}

		break;
	case 13:size = data->AsVoid()->Size;
		cur->Data = (void*)cntData;
		memcpy(&FieldDataBlock[cntData], data->Data, sizeof(DWORD));
		cntData += sizeof(DWORD);
		memcpy(&FieldDataBlock[cntData], data->AsVoid()->data, size);
		cntData += size;
		break;
	case 14:
		cur->Data = (char*)this->AddStruct(data->AsStruct());
		if ((int)cur->Data == -1)
			return -1;
		break;
	case 15:
		int current = cntListIndices;
		cur->Data = (char*)(cntListIndices*sizeof(DWORD));
		ListIndicesArray[cntListIndices] = data->AsList()->Size;
		cntListIndices += ListIndicesArray[cntListIndices] + 1;
		for (DWORD n = 0; n < data->AsList()->Size; n++){

			ListIndicesArray[current + n + 1] = this->AddStruct(&data->AsList()->list[n]);

			//if (ListIndicesArray[((int)cur->Data) + n + 1] == -1)
			//	return -1;
		}
		break;
	}

	return ret;
}

int GFFPack::AddLabel(char * label){

	if (this->cntLabels > this->Header->LabelCount)
		return -1;

	char l[16] = { 0 };
	strncpy(l, label, 16);

	char * current;

	for (DWORD n = 0; n < cntLabels; n++){

		current = (LabelArray + (n * 16));

		if (strncmp(current, l, 16) == 0){

			return (int)n;
		}
	}

	current = (LabelArray + (cntLabels * 16));
	memcpy(current, l, 16);
	cntLabels++;
	return cntLabels - 1;
}