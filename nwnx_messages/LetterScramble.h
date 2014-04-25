#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct ScrambleSetEntry{

	char Symbol;
	char * Scramble;
};

class ScrambleSet{

public:

	ScrambleSet(){memset(this,0,sizeof( ScrambleSet ) );}
	~ScrambleSet(){
		for(int n=0;n<26;n++ ){
			if(Scramble[n].Scramble)
				delete []Scramble[n].Scramble;
		}
	}
	int ID;
	int LEVEL;
	char * Language;
	ScrambleSetEntry Scramble[26];

	void Set( char letter, char * substitution );
	const char * Get( char letter );
	bool OpenFromFile( const char * file, int nVersion );
	bool ScrambleText( char * text, int max, const char * escapesq, bool reversesq, const char * head, const char * tail );
};