#include "RandomOrg.h"


RandomOrg::RandomOrg(const char * CacheFolder){

	ZeroMemory(this, sizeof(RandomOrg));
	srand(time(NULL));
	cache = new char[strlen(CacheFolder) + 1];
	strcpy(cache, CacheFolder);

	char ParseURL[1000];
	sprintf(ParseURL, "https://www.random.org/integers/?num=10000&min=0&max=%u&col=1&base=10&format=plain&rnd=new", RAND_MAX);

	GetURL = new char[strlen(ParseURL)+1];
	strcpy(GetURL, ParseURL);

	strcpy(ParseURL,"http://www.random.org/quota/?format=plain");
	QuotaURL = new char[strlen(ParseURL) + 1];
	strcpy(QuotaURL, ParseURL);

	NeedsNewNumbers = true;
}


RandomOrg::~RandomOrg(){

	if (RandomArray){

		for (int n = 0; n < 100; n++){

			if (RandomArray[n])
				delete[]RandomArray[n];
		}

		delete[]RandomArray;
	}
	delete[]QuotaURL;
	delete[]GetURL; 
	if (LastOldFile)
		delete[]LastOldFile;
}

int RandomOrg::GetQuota(){

	char tmp[L_tmpnam];
	tmpnam(tmp);

	if (URLDownloadToFile(NULL, QuotaURL, tmp, NULL, NULL) == S_OK){

		FILE * f = fopen(tmp, "rb");
		if (!f){

			return -1;
		}

		fseek(f, 0, SEEK_END);
		long fsize = ftell(f);
		rewind(f);

		char * str = new char[fsize + 1];
		
		if (!str){
			fclose(f);
			remove(tmp);
			return -1;
		}
		str[fsize] = 0;
		fread(str, 1, fsize, f);
		fclose(f);
		remove(tmp);

		int qot = atoi(str);

		if (this->LOG){
			fprintf(this->LOG, "Quota: %d\n", qot);
			fflush(this->LOG);
		}

		return qot;
	}

	if (this->LOG){
		fprintf(this->LOG, "Quota: 0 (timeout)\n");
		fflush(this->LOG);
	}

	return -1;
}

bool RandomOrg::DownloadRandom(){

	if (NoDownload){
		return LoadoldRandom();
	}

	if (!NeedsNewNumbers)
		return false;

	if (outofquota)
		return false;
	else if (GetQuota() < 0){
		outofquota = true;
		return false;
	}

	if (NoReuse && RandomArray){

		for (int n = 0; n < 100; n++){

			if (RandomArray[n])
				delete[]RandomArray[n];
		}

		delete[]RandomArray;
		RandomArray = NULL;
	}

	char tmp[L_tmpnam];
	tmpnam(tmp);
	
	if (this->LOG)
		fprintf(this->LOG, "Downloading random: %s -> ", this->GetURL);

	if (URLDownloadToFile(NULL, GetURL, tmp, NULL, NULL) == S_OK){
		FILE * f = fopen(tmp, "r");
		if (!f){

			if (this->LOG){
				fprintf(this->LOG, "No file\n");
				fflush(this->LOG);
			}

			return false;
		}

		fseek(f, 0, SEEK_END);
		size = ftell(f);
		rewind(f);

		if (size < 50000){
			fprintf(this->LOG, "invalid data (out of quota?)\n");
			fflush(this->LOG);
			fclose(f);
			remove(tmp);
			return false;
		}

		char * random = new char[size + 1];

		if (!random){
			fclose(f);
			remove(tmp);

			if (this->LOG){
				fprintf(this->LOG, "No memory\n");
				fflush(this->LOG);
			}

			return false;
		}
		else
			random[size] = 0;

		fread(random, 1, size, f);
		fclose(f);
		remove(tmp);

		if (!NoSave && cache && cache[0] != 0 ){

			char fileName[100];
			time_t cTime;
			time(&cTime);
			strftime(fileName, 100, "%Y-%m-%d-%H-%M-%S.rnd", localtime(&cTime));
			char newFile[MAX_PATH];
			sprintf(newFile, "%s%s", cache, fileName);

			f = fopen(newFile, "w");
			if (f){
				if (random){
					fwrite(random, 1, size, f);
				}

				fclose(f);
			}
		}

		if (!RandomArray){
			RandomArray = new long*[100];
			ZeroMemory(RandomArray, sizeof(long*)* 100);
			rows = 0;
		}
		else if (++rows > 99){
			rows = 0;
			full = true;
		}

		if (RandomArray[rows]){
			delete[]RandomArray[rows];
		}

		RandomArray[rows] = new long[10000];

		if (!RandomArray){
			if (this->LOG){
				fprintf(this->LOG, "No memory\n");
				fflush(this->LOG);
			}
			delete[]random;
			return false;
		}

		int n = 0;
		char * token = strtok(random, "\n");
		while (token){

			RandomArray[rows][n++] = atoi(token);

			if (n >= 10000)
				break;

			token = strtok(NULL, "\n");
		}

		delete[]random;
	}
	else{
		if (this->LOG){
			fprintf(this->LOG, "Timeout\n");
			fflush(this->LOG);
		}
		return false;
	}

	long * temp = RandomArray[rows];
	RandomArray[rows] = RandomArray[0];
	RandomArray[0] = temp;
	cursorY = 0;
	cursorX = 0;
	NeedsNewNumbers = false;

	if (LastOldFile){
		delete[]LastOldFile;
		LastOldFile = NULL;
	}

	if (this->LOG){
		fprintf(this->LOG, "OK\n");
		fflush(this->LOG);
	}

	return true;
}

int RandomOrg::TrimRNDFiles(int nKeep){

	if (!cache || cache[0] == 0)
		return 0;

	char fFile[MAX_PATH];
	sprintf(fFile, "%s*.rnd", cache);
	int nCnt = 0;

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(fFile, &FindFileData);
	while (hFind != INVALID_HANDLE_VALUE)
	{


		nCnt++;

		if (!FindNextFile(hFind, &FindFileData))
		{
			FindClose(hFind);
			hFind = INVALID_HANDLE_VALUE;
		}
	}

	if (nCnt <= nKeep)
		return 0;

	char dieFile[MAX_PATH];
	time_t low;
	time_t cur;
	int nKilled=0;
	while (nCnt > nKeep){

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = FindFirstFile(fFile, &FindFileData);
		low = FileTimeToUnixTime(&FindFileData.ftCreationTime);
		sprintf(dieFile, "%s%s", cache, FindFileData.cFileName);
		while (hFind != INVALID_HANDLE_VALUE)
		{

			cur = FileTimeToUnixTime(&FindFileData.ftCreationTime);

			if (cur < low){
				low = cur;
				sprintf(dieFile, "%s%s", cache, FindFileData.cFileName);
			}
			
			if (!FindNextFile(hFind, &FindFileData))
			{
				FindClose(hFind);
				hFind = INVALID_HANDLE_VALUE;
			}
		}

		remove(dieFile);
		nKilled++;
		if (this->LOG){

			fprintf(this->LOG, "Deleted: %s\n", dieFile);
			fflush(this->LOG);
		}
		nCnt--;
	}

	return nKilled;
}

bool RandomOrg::LoadoldRandom(){

	if (PreferStock){

		StockRandom();
		return true;
	}

	if (!NeedsNewNumbers)
		return false;

	if (!cache || cache[0] == 0)
		return false;

	if (NoReuse && RandomArray){

		for (int n = 0; n < 100; n++){

			if (RandomArray[n])
				delete[]RandomArray[n];
		}

		delete[]RandomArray;
		RandomArray = NULL;
	}

	char fFile[MAX_PATH];
	sprintf(fFile,"%s*.rnd",cache);
	int nCnt = 0;
	
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(fFile, &FindFileData);
	while (hFind != INVALID_HANDLE_VALUE)
	{
		
		
		nCnt++;

		if (!FindNextFile(hFind, &FindFileData))
		{
			FindClose(hFind);
			hFind = INVALID_HANDLE_VALUE;
		}
	}

	if (nCnt <= 0){

		if (this->LOG){
			fprintf(this->LOG, "Loading from cache: failed no files found: %s\n", fFile);
			fflush(this->LOG);
		}

		return false;
	}

	int nRand = rand()%nCnt;
	int nNumb = nCnt;
	nCnt = 0;

	hFind = FindFirstFile(fFile, &FindFileData);
	sprintf(fFile, "%s%s", cache, FindFileData.cFileName);

	while (hFind != INVALID_HANDLE_VALUE)
	{

		if (nRand == nCnt++){

			if (LastOldFile && strcmp(LastOldFile, FindFileData.cFileName) == 0){

				if (nNumb > 1)
					return LoadoldRandom();
				else
					StockRandom();
				
				return true;
			}
			else{
				sprintf(fFile, "%s%s", cache, FindFileData.cFileName);
				if (LastOldFile)
					delete[]LastOldFile;
				LastOldFile = new char[strlen(FindFileData.cFileName) + 1];
				strcpy(LastOldFile, FindFileData.cFileName);
			}
			FindClose(hFind);
			hFind = INVALID_HANDLE_VALUE;
			break;
		}

		if (!FindNextFile(hFind, &FindFileData))
		{
			FindClose(hFind);
			hFind = INVALID_HANDLE_VALUE;
		}
	}

	if (this->LOG){
		fprintf(this->LOG, "Loading from cache: %s\n", fFile);
		fflush(this->LOG);
	}

	FILE * f = fopen(fFile, "r");
	if (!f){
		if (this->LOG){
			fprintf(this->LOG, "Loading from cache: unable to open %s\n", fFile);
			fflush(this->LOG);
		}
		return false;
	}

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	rewind(f);

	char * random = new char[size + 1];

	if (!random){
		fclose(f);
		return false;
	}
	else
		random[size] = 0;

	fread(random, 1, size, f);
	fclose(f);

	if (!RandomArray){
		RandomArray = new long*[100];
		ZeroMemory(RandomArray, sizeof(long*)* 100);
		rows = 0;
	}
	else if (++rows > 99){
		rows = 0;
		full = true;
	}

	if (RandomArray[rows]){
		delete[]RandomArray[rows];
	}

	RandomArray[rows] = new long[10000];

	if (!RandomArray[rows]){
		delete[]random;
		return false;
	}

	int n = 0;
	char * token = strtok(random, "\n");
	while (token){

		RandomArray[rows][n++] = atoi(token);

		if (n >= 10000)
			break;

		token = strtok(NULL, "\n");
	}

	delete[]random;

	long * temp = RandomArray[rows];
	RandomArray[rows] = RandomArray[0];
	RandomArray[0] = temp;
	cursorY = 0;
	cursorX = 0;
	NeedsNewNumbers = false;
	return true;
}

void RandomOrg::StockRandom(){
	
	time_t ssEeed = time(NULL);

	if (this->LOG){
		fprintf(this->LOG, "Generating stock random using seed: %lu\n", ssEeed);
		fflush(this->LOG);
	}

	if (NoReuse && RandomArray){

		for (int n = 0; n < 100; n++){

			if (RandomArray[n])
				delete[]RandomArray[n];
		}

		delete[]RandomArray;
		RandomArray = NULL;
	}

	if (!NeedsNewNumbers)
		return;
	else
		NeedsNewNumbers = false;

	if (!RandomArray){
		RandomArray = new long*[100];
		ZeroMemory(RandomArray, sizeof(long*)* 100);
		rows = 0;
	}
	else if (++rows > 99){
		rows = 0;
		full = true;
	}

	if (RandomArray[rows]){
		delete[]RandomArray[rows];
	}

	RandomArray[rows] = new long[10000];

	if (!RandomArray[rows]){
		return;
	}

	srand(ssEeed);

	for (int n = 0; n < 10000; n++){
		RandomArray[rows][n] = rand();
	}

	long * temp = RandomArray[rows];
	RandomArray[rows] = RandomArray[0];
	RandomArray[0] = temp;
	cursorY = 0;
	cursorX = 0;

	if (LastOldFile){
		delete[]LastOldFile;
		LastOldFile = NULL;
	}
}

void RandomOrg::EmptyRandom(){

	if (RandomArray){

		for (int n = 0; n < 100; n++){

			if (RandomArray[n])
				delete[]RandomArray[n];
		}

		delete[]RandomArray;
	}
	full = false;
	rows = 0;
	NeedsNewNumbers = true;
}

int RandomOrg::Random(){

	if (!RandomArray || !RandomArray[0] || NeedsNewNumbers){

		NeedsNewNumbers = true;
		if (!DownloadRandom() && !LoadoldRandom())
			StockRandom();
	}

	if (cursorX >= 10000){

		int nRealRows = full ? 100 : rows;

		cursorX = 0;

		if (++cursorY >= nRealRows ){

			NeedsNewNumbers = true;

			if (!DownloadRandom() && !LoadoldRandom())
				StockRandom();
		}
	}

	return RandomArray[cursorY][cursorX++];
}

time_t RandomOrg::FileTimeToUnixTime(FILETIME * ft){

	SYSTEMTIME sto;

	if (FileTimeToSystemTime(ft, &sto)){

		struct tm timeinfo;

		timeinfo.tm_mday = sto.wDay;
		timeinfo.tm_min = sto.wMinute;
		timeinfo.tm_sec = sto.wSecond;
		timeinfo.tm_wday = sto.wDayOfWeek;
		timeinfo.tm_isdst = -1;
		//Offset fix
		timeinfo.tm_hour = sto.wHour + 2;
		timeinfo.tm_mon = sto.wMonth - 1;
		timeinfo.tm_year = sto.wYear - 1900;

		return mktime(&timeinfo);
	}

	return 0;
}