#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <urlmon.h>
#include <windows.h>
#include <iostream>
#include <time.h>
#pragma comment(lib,"Urlmon.lib")



class RandomOrg
{
public:
	RandomOrg(const char * CacheFolder);
	~RandomOrg();

	//Random int, can be anything
	int Random();

	//Downloads a new random buffer
	bool DownloadRandom();
	//Get an old random buffer without connecting to random.org
	bool LoadoldRandom();
	//Fill random with rand()
	void StockRandom();
	//Empties out the random buffer
	void EmptyRandom();
	//Removes all excess files from the cache and keeps nKeep number of files
	//Removes olders files first
	//returns the number of files deleted
	int TrimRNDFiles(int nKeep);

	//Get your remaining quota from random.org
	//If this is negative the application will not recive more new random and will reuse what it has
	int GetQuota();

	char * LastOldFile;
	char * GetURL;
	char * QuotaURL;
	long ** RandomArray;
	long size;
	long rows;
	long cursorX;
	long cursorY;
	char * cache;
	bool outofquota;
	bool NeedsNewNumbers;
	bool NoDownload;
	bool PreferStock;
	bool NoReuse;
	bool NoSave;
	FILE * LOG;

private:
	bool full;
	time_t FileTimeToUnixTime(FILETIME * ft);
};

