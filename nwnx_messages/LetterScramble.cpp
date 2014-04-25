#include "LetterScramble.h"

void ScrambleSet::Set( char letter, char * substitution ){

	char sym = toupper(letter);
	letter = sym -'A';

	if( letter > 26 || letter < 0 )
		return;

	if( Scramble[letter].Scramble )
		delete []Scramble[letter].Scramble;

	Scramble[letter].Scramble = (char*)new char[strlen(substitution)+1];
	if(Scramble[letter].Scramble){
		strcpy( Scramble[letter].Scramble, substitution );
		Scramble[letter].Symbol=sym;
	}
}

const char * ScrambleSet::Get( char letter ){
	
	letter = letter-'A';

	if( letter > 26 || letter < 0 )
		return NULL;

	return Scramble[letter].Scramble;
}

bool ScrambleSet::OpenFromFile( const char * file, int nVersion ){

	/*unsigned int nStart=0;
	unsigned int nEnd=0;
	unsigned int nLen = strlen(file);
	for( unsigned int n=0;n<nLen;n++ ){

		if( file[n]=='\\' || file[n]=='/' ){
			nStart = n+1;
		}
		else if( file[n]=='.' ){
			nEnd = n;
		}
	}

	if( nEnd - nStart <= 0 )
		return false;

	char * Language = new char[(nEnd - nStart)+1];
	Language[nEnd - nStart]=0;
	strncpy( Language, &file[nStart], nEnd - nStart );*/

	char * filecontents=NULL;
	FILE * fFile = fopen( file, "r" );
	if( !fFile ){

		return false;
	}

	fseek( fFile, 0, SEEK_END );
	unsigned int fLen = ftell( fFile );
	rewind( fFile );

	filecontents = new char[fLen+1];
	if( !filecontents ){
		fclose( fFile );
		return false;
	}

	fread( filecontents, 1, fLen, fFile );
	fclose( fFile );

	if( strncmp( filecontents, "SCRAMBLESET", 11 ) > 0 ){
		delete []filecontents;
		return false;
	}

	char * Substr = strstr( filecontents, "VERSION: " );
	if( !Substr ){
		delete []filecontents;		
		return false;
	}
	else{
		
		Substr = strstr( Substr, " " )+1;
		int nActualVer;
		sscanf( Substr, "%i", &nActualVer );
		if( nActualVer != nVersion ){
			delete []filecontents;
			return false;
		}
	}

	Substr = strstr( filecontents, "LEVEL: " );
	if( !Substr ){
		delete []filecontents;		
		return false;
	}
	else{
		
		Substr = strstr( Substr, " " )+1;
		sscanf( Substr, "%i", &this->LEVEL );
	}

	Substr = strstr( filecontents, "ID: " );
	if( !Substr ){
		delete []filecontents;
		return false;
	}
	else{
		
		Substr = strstr( Substr, " " )+1;
		sscanf( Substr, "%i", &this->ID );
	}

	Substr = strstr( filecontents, "LANGUAGE: " );
	if( !Substr ){
		delete []filecontents;
		return false;
	}
	else{
		
		Substr = strstr( Substr, " " )+1;
		char * temp = strstr( Substr, "\n" );
		if( !temp ){
			delete []filecontents;
			return false;
		}

		*temp='\0';
		unsigned int langLen = strlen(Substr);
		this->Language = new char[langLen+1];
		for( unsigned int n=0;n<langLen;n++ ){

			if( isalpha( Substr[n] ) && isupper( Substr[n] ) )
				this->Language[n]=tolower( Substr[n] );			
			else
				this->Language[n]=Substr[n];	
		}
		this->Language[langLen]='\0';
		*temp='\n';
	}

	Substr = strstr( filecontents, "LIST:" );
	if( !Substr ){
		delete []filecontents;
		return false;
	}
	else{
		
		Substr = strstr( Substr, "\n" );
		if( !Substr ){
			delete []filecontents;
			return false;
		}
		else
			Substr++;

		char * cursor;
		char line[255]={0};
		while( true ){

			for( int n=0;true;n++ ){

				if( Substr[n] == '\n' || Substr[n] == '\0' ){

					line[n]='\0';
					break;
				}

				line[n]=Substr[n];
			}
			
			cursor = strstr( line, " " );
			if( cursor ){
				cursor++;
				this->Set( line[0], cursor );	
			}

			Substr = strstr( Substr, "\n" );
			if( Substr )
				Substr++;
			else 
				break;
		}

	}

	if(filecontents)delete []filecontents;

	return true;
}

bool ScrambleSet::ScrambleText( char * text, int max, const char * escapesq, bool reversesq, const char * head, const char * tail ){

	char * copy = new char[max+1];
	char * sub;
	unsigned int nCnt = max;
	unsigned int len = strlen(text);
	bool isUp;
	bool isok=true;
	unsigned int sublen;
	char filter[300] = {0};
	char * curfilter;
	int filterindex=0;
	strcpy( copy, text );
	memset( text, 0, max );
	bool on=true;
	char tailcls=0;

	if( tail )
		tailcls=tail[0];

	if( escapesq && reversesq ){

		if( tail )
			strcpy( filter, tail );

		filterindex = strlen(filter);
		filter[filterindex]='0';

		if( head ){
			strcat( filter, head );
		}

		sublen = strlen(head);

		if( sublen >= nCnt ){
			isok=false;
			return isok;
		}
		else
			nCnt-=sublen;

		strcat( text, head );

		on = false;
	}
	else{

		if( head )
			strcpy( filter, head );

		filterindex = strlen(filter);
		filter[filterindex]='0';

		if( tail ){
			strcat( filter, tail );
		}
	}


	for( unsigned int n=0;n<len;n++ ){
	
		if( isalpha( copy[n] ) ){

			isUp = isupper( copy[n] )>0;
			if( on )
				sub = Scramble[toupper(copy[n])-'A'].Scramble;
			else
				sub = NULL;

			if( sub ){
				
				if( isUp )
					sub[0]=toupper(sub[0]);
				else
					sub[0]=tolower(sub[0]);
				
				sublen = strlen( sub );

				if( sublen >= nCnt ){
					isok=false;
					break;
				}
				else
					nCnt-=sublen;

				strcat( text, sub );
			}
			else{

				if( 1 >= nCnt ){
					isok=false;
					break;
				}
				else
					nCnt-=1;

				curfilter=&filter[filterindex];
				filter[filterindex]=copy[n];
				filter[filterindex+1]=0;
				strcat( text, curfilter );

			}
		}
		else{

			filter[filterindex+1]=0;

			if( escapesq && escapesq[0] == copy[n] && strcmp( escapesq, &copy[n] ) <= 0 ){
			
				if( reversesq ){

					if( on ){
					
						filter[filterindex+1]=tailcls;
						curfilter=&filter[filterindex];								
					}
					else{

						curfilter=&filter[0];	
					}

				}

				else{

					if( on ){
					
						curfilter=&filter[0];				
					}
					else{
						filter[filterindex+1]=tailcls;
						curfilter=&filter[filterindex];
					}

				}
				on = !on;
			}
			else
				curfilter=&filter[filterindex];

			filter[filterindex]=copy[n];

			sublen = strlen(curfilter);

			if( sublen >= nCnt ){
				isok=false;
				break;
			}
			else
				nCnt-=sublen;

			strcat( text, curfilter );
			filter[filterindex+1]=0;
		}
	}

	if( !on ){

		sublen = strlen(tail);

		if( sublen >= nCnt ){
			isok=false;
			return isok;
		}
		else
			nCnt-=sublen;

		strcat( text, tail );

	}

	return isok;
}