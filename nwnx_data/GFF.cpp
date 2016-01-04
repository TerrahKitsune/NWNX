#include "Aurora.h"

GFF::GFF(){

	memset(this, 0, sizeof(GFF));
}

GFF::~GFF(){

	if (TopStruct){
		delete TopStruct;
	}
}

bool GFF::LoadGFFFromFile(const char * file){

	if (TopStruct){
		delete TopStruct;
		TopStruct = NULL;
	}

	FILE * fFile = fopen(file, "rb");
	if (!fFile){
		return false;
	}

	fseek(fFile, 0, SEEK_END);
	DWORD Size = ftell(fFile);
	rewind(fFile);

	if (Size < 100){
		fclose(fFile);
		return false;
	}
		

	char * filecontents = new char[Size];

	if (!filecontents){

		fclose(fFile);
		return false;
	}

	GFFHeader GH;
	if (fread(filecontents, 1, Size, fFile) != Size){

		delete[]filecontents;
		fclose(fFile);
		return false;
	}
	else{
		fclose(fFile);
		memcpy(&GH, filecontents, sizeof(GFFHeader));
	}

	/*printf("FileType: %c%c%c%c\n", GH.FileType[0], GH.FileType[1], GH.FileType[2], GH.FileType[3]);
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
	printf("Size: %u\n\n", GH.ListIndicesOffset + GH.ListIndicesCount );
	printf("Actual Size: %u\n\n", Size );*/

	TopStruct = new GFFStruct;

	if (!TopStruct){

		delete[]filecontents;
		return false;
	}

	TopStruct->Unpack(filecontents, GH.StructOffset);

	memcpy(Ver, GH.FileVersion, 4);
	memcpy(Type, GH.FileType, 4);

	delete[]filecontents;
	return true;
}