#include "ERF.h"

LocalizedString::LocalizedString(){
	
	memset( this, 0, sizeof( LocalizedString ) );
}

LocalizedString::~LocalizedString(){
	
	if( String )
		delete []String;
}

LocalizedString * ERF::GetString( DWORD nIndex, LocalizedString * dest ){

	if( nIndex >= head.LanguageCount )
		return NULL;

	bool Created=false;

	if( !dest ){
		dest = new LocalizedString;
		Created=true;
	}

	DWORD Offset = head.OffsetToLocalizedString;

	for( DWORD n=0;n<head.LanguageCount;n++ ){


		if( !ERFRead( dest, sizeof( LocalizedString ), 1, Offset ) ){

			if( Created )
				delete dest;
			return NULL;
		}

		dest->String = new char[dest->StringSize+1];
		if( !dest->String ){

			if( Created )
				delete dest;
			return NULL;
		}

		memset( dest->String, 0, dest->StringSize+1 );

		if( !ERFRead( dest->String, sizeof( char ), dest->StringSize, Offset+(sizeof(DWORD)*2) ) ){

			if( Created )
				delete dest;
			return NULL;
		}

		Offset = Offset+dest->StringSize+(sizeof(DWORD)*2);

		if( n==nIndex )
			break;	
	}

	return dest;
}

KeyEntry * ERF::GetKey( DWORD nIndex, KeyEntry * dest ){

	if( nIndex >= head.EntryCount )
		return NULL;
	
	bool Created=false;

	if( !dest ){
		dest = new KeyEntry;
		Created=true;
	}

	if( !ERFRead( dest, sizeof( KeyEntry ), 1, head.OffsetToKeyList+(sizeof( KeyEntry )*nIndex) ) ){

		if( Created )
			delete dest;
		return NULL;
	}

	return dest;
}

ERF::ERF( const char * file ){

	fFile = fopen( file, "rb" );
	if( !fFile ){
		memset( &head, 0, sizeof( ERFHeader ) );
		return;
	}

	if( !ERFRead( &head, sizeof( ERFHeader ), 1, 0 ) ){
		fclose( fFile );
		fFile=NULL;
		memset( &head, 0, sizeof( ERFHeader ) );
	}

}

ERF::~ERF( ){

	if( fFile )
		fclose( fFile );
}

bool ERF::ERFRead( void * dest, DWORD unit_size, DWORD count, DWORD Offset ){

	if( !fFile )
		return false;

	fseek( fFile, Offset, SEEK_SET );
	return fread( dest, unit_size, count, fFile )==count;
}

ResEntry ERF::GetResEntry( KeyEntry * key ){

	ResEntry ret;

	if( !key ){
		ret.OffsetToResource=0;
		ret.ResourceSize=0;
		return ret;
	}

	if( !ERFRead( &ret, sizeof( ResEntry ), 1, head.OffsetToResourceList+(sizeof(ResEntry)*key->ResID) ) ){

		ret.OffsetToResource=0;
		ret.ResourceSize=0;
		return ret;
	}

	return ret;
}

void * ERF::GetResource( KeyEntry * key ){

	ResEntry ret = GetResEntry( key );
	if( ret.OffsetToResource == 0 || ret.ResourceSize == 0 )
		return NULL;
	BYTE * mem = new BYTE[ret.ResourceSize];
	if( !mem )
		return NULL;

	if( !ERFRead( mem, sizeof(BYTE), ret.ResourceSize, ret.OffsetToResource ) ){

		delete []mem;
		return NULL;
	}

	return mem;
}

bool ERF::ExtractResource( KeyEntry * key, const char * folder ){

	ResEntry ret = GetResEntry( key );
	if( ret.OffsetToResource == 0 || ret.ResourceSize == 0 )
		return false;

	char ResRef[30]={0};
	char File[MAX_PATH];
	DWORD fLen = strlen( folder );

	for( int n=0;n<16;n++ ){
		if( key->ResRef[n] == '\0' )
			break;
		ResRef[n]=key->ResRef[n];
	}
	strcat( ResRef, NwnGetResTypeExtension( key->ResType ) );

	if( strlen( ResRef )+fLen >= MAX_PATH )
		return false;

	if( folder[fLen-1] == '/' || folder[fLen-1] == '\\' )
		sprintf( File, "%s%s", folder, ResRef );
	else
		sprintf( File, "%s/%s", folder, ResRef );

	FILE * fOut = fopen( File, "wb" );
	if( !fOut )
		return false;

	char Buffer[65536];
	DWORD left=ret.ResourceSize;
	DWORD last=0;
	fseek( fFile, ret.OffsetToResource, SEEK_SET );
	while( left > 0 ){

		if( left > 65536 )
			last = 65536;
		else
			last = left;
		
		fread( Buffer, 1, last, fFile );
		fwrite( Buffer, 1, last, fOut );
		left=left-last;
	}

	fclose( fOut );

	return true;
}

//Taken from ResMan
const char * ERF::NwnGetResTypeExtension( WORD nType )
{
	switch (nType)
	{
		case NwnResType_RES: return ".res";
		case NwnResType_BMP: return ".bmp";
		case NwnResType_MVE: return ".mve";
		case NwnResType_TGA: return ".tga";
		case NwnResType_WAV: return ".wav";
		case NwnResType_PLT: return ".plt";
		case NwnResType_INI: return ".ini";
		case NwnResType_BMU: return ".bmu";
		case NwnResType_MPG: return ".mpg";
		case NwnResType_TXT: return ".txt";
		case NwnResType_PLH: return ".plh";
		case NwnResType_TEX: return ".tex";
		case NwnResType_MDL: return ".mdl";
		case NwnResType_THG: return ".thg";
		case NwnResType_FNT: return ".fnt";
		case NwnResType_LUA: return ".lua";
		case NwnResType_SLT: return ".slt";
		case NwnResType_NSS: return ".nss";
		case NwnResType_NCS: return ".ncs";
		case NwnResType_MOD: return ".mod";
		case NwnResType_ARE: return ".are";
		case NwnResType_SET: return ".set";
		case NwnResType_IFO: return ".ifo";
		case NwnResType_BIC: return ".bic";
		case NwnResType_WOK: return ".wok";
		case NwnResType_2DA: return ".2da";
		case NwnResType_TLK: return ".tlk";
		case NwnResType_TXI: return ".txi";
		case NwnResType_GIT: return ".git";
		case NwnResType_BTI: return ".bti";
		case NwnResType_UTI: return ".uti";
		case NwnResType_BTC: return ".btc";
		case NwnResType_UTC: return ".utc";
		case NwnResType_DLG: return ".dlg";
		case NwnResType_ITP: return ".itp";
		case NwnResType_BTT: return ".btt";
		case NwnResType_UTT: return ".utt";
		case NwnResType_DDS: return ".dds";
		case NwnResType_UTS: return ".uts";
		case NwnResType_LTR: return ".ltr";
		case NwnResType_GFF: return ".gff";
		case NwnResType_FAC: return ".fac";
		case NwnResType_BTE: return ".bte";
		case NwnResType_UTE: return ".ute";
		case NwnResType_BTD: return ".btd";
		case NwnResType_UTD: return ".utd";
		case NwnResType_BTP: return ".btp";
		case NwnResType_UTP: return ".utp";
		case NwnResType_DTF: return ".dtf";
		case NwnResType_GIC: return ".gic";
		case NwnResType_GUI: return ".gui";
		case NwnResType_CSS: return ".css";
		case NwnResType_CCS: return ".ccs";
		case NwnResType_BTM: return ".btm";
		case NwnResType_UTM: return ".utm";
		case NwnResType_DWK: return ".dwk";
		case NwnResType_PWK: return ".pwk";
		case NwnResType_BTG: return ".btg";
		case NwnResType_UTG: return ".utg";
		case NwnResType_JRL: return ".jrl";
		case NwnResType_SAV: return ".sav";
		case NwnResType_UTW: return ".utw";
		case NwnResType_4PC: return ".4pc";
		case NwnResType_SSF: return ".ssf";
		case NwnResType_HAK: return ".hak";
		case NwnResType_NWM: return ".nwm";
		case NwnResType_BIK: return ".bik";
		case NwnResType_ERF: return ".erf";
		case NwnResType_BIF: return ".bif";
		case NwnResType_KEY: return ".key";
		case NwnResType_PTM: return ".ptm";
		case NwnResType_PTT: return ".ptt";
		default: return NULL;
	}
}