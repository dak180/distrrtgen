#include <string>
#include <vector>
#ifdef _WIN32
#include <io.h>
#else
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <conio.h>
#include <sstream>
#include "Public.h"
#include "MemoryPool.h"
#include "RTIReader.h"
#include "RTReader.h"

using namespace std;

void Usage()
{
	printf("converti2 - Original to Indexed rainbow table converter\n");
	printf("by Martin Westergaard <martinwj2005@gmail.com>\n");
	printf("http://www.freerainbowtables.com\n\n");

	printf("usage: converti2 rainbow_table_pathname\n");
	printf("rainbow_table_pathname: pathname of the rainbow table(s), wildchar(*, ?) supported\n");
	printf("\n");
	printf("example: converti2 *.rt\n");
	printf("         converti2 md5_*.rt\n");
}


int GetMaxBits(uint64 highvalue)
{
	if(highvalue < 0x02)
		return 1;
	if(highvalue < 0x04)
		return 2;
	if(highvalue < 0x08)
		return 3;
	if(highvalue < 0x10)
		return 4;
	if(highvalue < 0x20)
		return 5;
	if(highvalue < 0x40)
		return 6;
	if(highvalue < 0x80)
		return 7;
	if(highvalue < 0x100)
		return 8;
	if(highvalue < 0x200)
		return 9;
	if(highvalue < 0x400)
		return 10;
	if(highvalue < 0x800)
		return 11;
	if(highvalue < 0x1000)
		return 12;
	if(highvalue < 0x2000)
		return 13;
	if(highvalue < 0x4000)
		return 14;
	if(highvalue < 0x8000)
		return 15;
	if(highvalue < 0x10000)
		return 16;
	if(highvalue < 0x20000)
		return 17;
	if(highvalue < 0x40000)
		return 18;
	if(highvalue < 0x80000)
		return 19;
	if(highvalue < 0x100000)
		return 20;
	if(highvalue < 0x200000)
		return 21;
	if(highvalue < 0x400000)
		return 22;
	if(highvalue < 0x800000)
		return 23;
	if(highvalue < 0x1000000)
		return 24;
	if(highvalue < 0x2000000)
		return 25;
	if(highvalue < 0x4000000)
		return 26;
	if(highvalue < 0x8000000)
		return 27;
	if(highvalue < 0x10000000)
		return 28;
	if(highvalue < 0x20000000)
		return 29;
	if(highvalue < 0x40000000)
		return 30;
	if(highvalue < 0x80000000)
		return 31;
#ifdef WIN32
	if(highvalue < 0x0000000100000000I64)
		return 32;
	if(highvalue < 0x0000000200000000I64)
		return 33;
	if(highvalue < 0x0000000400000000I64)
		return 34;
	if(highvalue < 0x0000000800000000I64)
		return 35;
	if(highvalue < 0x0000001000000000I64)
		return 36;
	if(highvalue < 0x0000002000000000I64)
		return 37;
	if(highvalue < 0x0000004000000000I64)
		return 38;
	if(highvalue < 0x0000008000000000I64)
		return 39;
	if(highvalue < 0x0000010000000000I64)
		return 40;
	if(highvalue < 0x0000020000000000I64)
		return 41;
	if(highvalue < 0x0000040000000000I64)
		return 42;
	if(highvalue < 0x0000080000000000I64)
		return 43;
	if(highvalue < 0x0000100000000000I64)
		return 44;
	if(highvalue < 0x0000200000000000I64)
		return 45;
	if(highvalue < 0x0000400000000000I64)
		return 46;
	if(highvalue < 0x0000800000000000I64)
		return 47;
	if(highvalue < 0x0001000000000000I64)
		return 48;
	if(highvalue < 0x0002000000000000I64)
		return 49;
	if(highvalue < 0x0004000000000000I64)
		return 50;
	if(highvalue < 0x0008000000000000I64)
		return 51;
	if(highvalue < 0x0010000000000000I64)
		return 52;
	if(highvalue < 0x0020000000000000I64)
		return 53;
	if(highvalue < 0x0040000000000000I64)
		return 54;
	if(highvalue < 0x0080000000000000I64)
		return 55;
	if(highvalue < 0x0100000000000000I64)
		return 56;
	if(highvalue < 0x0200000000000000I64)
		return 57;
	if(highvalue < 0x0400000000000000I64)
		return 58;
	if(highvalue < 0x0800000000000000I64)
		return 59;
	if(highvalue < 0x1000000000000000I64)
		return 60;
	if(highvalue < 0x2000000000000000I64)
		return 61;
	if(highvalue < 0x4000000000000000I64)
		return 62;
	if(highvalue < 0x8000000000000000I64)
		return 63;
#else
	if(highvalue < 0x0000000100000000LL)
		return 32;
	if(highvalue < 0x0000000200000000LL)
		return 33;
	if(highvalue < 0x0000000400000000LL)
		return 34;
	if(highvalue < 0x0000000800000000LL)
		return 35;
	if(highvalue < 0x0000001000000000LL)
		return 36;
	if(highvalue < 0x0000002000000000LL)
		return 37;
	if(highvalue < 0x0000004000000000LL)
		return 38;
	if(highvalue < 0x0000008000000000LL)
		return 39;
	if(highvalue < 0x0000010000000000LL)
		return 40;
	if(highvalue < 0x0000020000000000LL)
		return 41;
	if(highvalue < 0x0000040000000000LL)
		return 42;
	if(highvalue < 0x0000080000000000LL)
		return 43;
	if(highvalue < 0x0000100000000000LL)
		return 44;
	if(highvalue < 0x0000200000000000LL)
		return 45;
	if(highvalue < 0x0000400000000000LL)
		return 46;
	if(highvalue < 0x0000800000000000LL)
		return 47;
	if(highvalue < 0x0001000000000000LL)
		return 48;
	if(highvalue < 0x0002000000000000LL)
		return 49;
	if(highvalue < 0x0004000000000000LL)
		return 50;
	if(highvalue < 0x0008000000000000LL)
		return 51;
	if(highvalue < 0x0010000000000000LL)
		return 52;
	if(highvalue < 0x0020000000000000LL)
		return 53;
	if(highvalue < 0x0040000000000000LL)
		return 54;
	if(highvalue < 0x0080000000000000LL)
		return 55;
	if(highvalue < 0x0100000000000000LL)
		return 56;
	if(highvalue < 0x0200000000000000LL)
		return 57;
	if(highvalue < 0x0400000000000000LL)
		return 58;
	if(highvalue < 0x0800000000000000LL)
		return 59;
	if(highvalue < 0x1000000000000000LL)
		return 60;
	if(highvalue < 0x2000000000000000LL)
		return 61;
	if(highvalue < 0x4000000000000000LL)
		return 62;
	if(highvalue < 0x8000000000000000LL)
		return 63;

#endif
	return 64;

}

#ifdef _WIN32
void GetTableList(string sWildCharPathName, vector<string>& vPathName)
{
	vPathName.clear();

	string sPath;
	int n = sWildCharPathName.find_last_of('\\');
	if (n != -1)
		sPath = sWildCharPathName.substr(0, n + 1);

	_finddata_t fd;
	long handle = _findfirst(sWildCharPathName.c_str(), &fd);
	if (handle != -1) {
		do	{
			string sName = fd.name;
			if (sName != "." && sName != ".." && !(fd.attrib & _A_SUBDIR))	{
				string sPathName = sPath + sName;
				vPathName.push_back(sPathName);
			}
		} while (_findnext(handle, &fd) == 0);

		_findclose(handle);
	}
}
#else
void GetTableList(int argc, char* argv[], vector<string>& vPathName)
{
	vPathName.clear();

	int i;
	for (i = 1; i < argc; i++)
	{
		string sPathName = argv[i];
		struct stat buf;
		if (lstat(sPathName.c_str(), &buf) == 0)
		{
			if (S_ISREG(buf.st_mode))
				vPathName.push_back(sPathName);

		}
	}
}
#endif


void ConvertRainbowTable(string sPathName, string sResultFileName, unsigned int rti_startptlength, unsigned int rti_endptlength, int showDistribution, int hascp, int rti_cplength, vector<unsigned int> rti_cppos)
{
#ifdef _WIN32
	int nIndex = sPathName.find_last_of('\\');
#else
	int nIndex = sPathName.find_last_of('/');
#endif
	string sFileName;
	if (nIndex != -1) {
		sFileName = sPathName.substr(nIndex + 1);
	}
	else {
		sFileName = sPathName;
	}
	unsigned int distribution[64] = {0};
	unsigned int numProcessedChains = 0;
	FILE* fileR;
	BaseRTReader *reader = NULL;
	if(sPathName.substr(sPathName.length() - 2, sPathName.length()) == "rt")
		reader = (BaseRTReader*)new RTReader(sPathName);
	else if(sPathName.substr(sPathName.length() - 3, sPathName.length()) == "rti")
		reader = (BaseRTReader*)new RTIReader(sPathName);
	if(reader == NULL) {
		printf("%s is not a supported file (Only RT and RTI is supported)\n", sPathName.c_str());
		return;
	}
	// Info
	printf("%s:\n", sFileName.c_str());
	if(showDistribution == 0) {
		fileR = fopen(sResultFileName.c_str(), "wb");
	}
	if (fileR != NULL || showDistribution == 1) {
		// File length check

		int size = reader->GetChainsLeft() * sizeof(RainbowChain);
			static CMemoryPool mp;
			unsigned int nAllocatedSize;
			RainbowChain* pChain = (RainbowChain*)mp.Allocate(size, nAllocatedSize);			
			unsigned int chainrowsize = ceil((float)(rti_startptlength + rti_endptlength + rti_cplength) / 8) * 8 ; // The size in bits (in whole bytes)
			unsigned int chainrowsizebytes = chainrowsize / 8;

			if (pChain != NULL)	{
				nAllocatedSize = nAllocatedSize / sizeof(RainbowChain) * sizeof(RainbowChain);
				unsigned int nChains = nAllocatedSize / sizeof(RainbowChain);
				uint64 curPrefix = 0, prefixStart = 0;
				vector<IndexRow> indexes;
				unsigned int chainsLeft;
				while((chainsLeft = reader->GetChainsLeft()) > 0) {
					
/*					if (ftell(file) == nFileLen)
						break;*/
					printf("%u chains left to read\n", chainsLeft);
					int nReadThisRound;
					clock_t t1 = clock();
					printf("reading...\n");
#ifdef _MEMORYDEBUG
			printf("Grabbing %i chains from file\n", nChains);
#endif
					reader->ReadChains(nChains, pChain);
#ifdef _MEMORYDEBUG
			printf("Recieved %i chains from file\n", nChains);
#endif
					clock_t t2 = clock();
					float fTime = 1.0f * (t2 - t1) / CLOCKS_PER_SEC;
					printf("reading time: %.2f s\n", fTime);		
					printf("converting %i chains...\n", nChains);
					t1 = clock();
					for(int i = 0; i < nChains; i++)	{
						if(showDistribution == 1) {
							distribution[GetMaxBits(pChain[i].nIndexS)-1]++;
						}
						else
						{
							uint64 chainrow = pChain[i].nIndexS; // Insert the complete start point								 
							chainrow |= ((uint64)pChain[i].nIndexE & (0xffffffff >> (32 - rti_endptlength))) << rti_startptlength; // 
/*							if(hascp == 1 && rti_cplength > 0) {
								chainrow |= (uint64)pChain[i].nCheckPoint << rti_startptlength + rti_endptlength;
							}*/
							fwrite(&chainrow, 1, chainrowsizebytes, fileR);			
							uint64 prefix = pChain[i].nIndexE >> rti_endptlength;
							if(i == 0) curPrefix = prefix;
							if(prefix != curPrefix && numProcessedChains - prefixStart > 0)	{
								if(prefix < curPrefix) {
									printf("**** Error writeChain(): Prefix is smaller than previous prefix. %llu < %llu****\n", prefix, curPrefix);
									exit(1);									
								}
								//unsigned char index[11] = {0}; // [0 - 10]
								unsigned int numchains = numProcessedChains - prefixStart;
								IndexRow index;
								index.prefix = curPrefix;
//										index.prefixstart = prefixStart;
								index.numchains = numchains;
								indexes.push_back(index);
								prefixStart = numProcessedChains;
								curPrefix = prefix; 
							}
						}
						numProcessedChains++;
					}		
					t2 = clock();
					fTime = 1.0f * (t2 - t1) / CLOCKS_PER_SEC;
					printf("conversion time: %.2f s\n", fTime);		
					if(showDistribution == 1) {
						for(int i = 0; i < 64; i++)	{
							printf("%u - %u\n", (i+1), distribution[i]);
						}
						delete reader;
						return;
					}

				}
	
		
	
				// We need to write the last index down
				IndexRow index;
				index.prefix = curPrefix;
				index.prefixstart = prefixStart;
				index.numchains = numProcessedChains - prefixStart;
				indexes.push_back(index);

				IndexRow high = {0}; // Used to find the highest numbers. This tells us how much we can pack the index bits
				for(UINT4 i = 0; i < indexes.size(); i++)
				{
					if(indexes[i].numchains > high.numchains)
						high.numchains = indexes[i].numchains;
/*						if(indexes[i].prefixstart > high.prefixstart)
						high.prefixstart = indexes[i].prefixstart;
					if(indexes[i].prefix > high.prefix)
						high.prefix = indexes[i].prefix;
*/
				}
				high.prefix = indexes[indexes.size()-1].prefix; // The last prefix is always the highest prefix
//					unsigned int m_rti_index_prefixlength = GetMaxBits(high.prefix);
				unsigned int m_rti_index_numchainslength = GetMaxBits(high.numchains);
//					unsigned int m_rti_index_indexlength = GetMaxBits(high.prefixstart);
				unsigned int m_indexrowsize = ceil((float)(/*m_rti_index_indexlength + */m_rti_index_numchainslength) / 8) * 8; // The size in bits (in whole bytes)	
				unsigned int m_indexrowsizebytes = m_indexrowsize / 8;
				FILE *pFileIndex = fopen(sResultFileName.append(".index").c_str(), "wb");
				fwrite("RTI2", 1, 4, pFileIndex);
				fwrite(&rti_startptlength, 1, 1, pFileIndex);
				fwrite(&rti_endptlength, 1, 1, pFileIndex);
				fwrite(&rti_cplength, 1, 1, pFileIndex);
//					fwrite(&m_rti_index_indexlength , 1, 1, pFileIndex);

				fwrite(&m_rti_index_numchainslength, 1, 1, pFileIndex);
				for(UINT4 i = 0; i < rti_cppos.size(); i++)	{
					fwrite(&rti_cppos[i], 1, 4, pFileIndex); // The position of the checkpoints
				}
//					fwrite(&m_rti_index_prefixlength, 1, 1, pFileIndex);
				int zero = 0;
				fwrite(&indexes[0].prefix, 1, 8, pFileIndex); // Write the first prefix
				unsigned int lastPrefix = 0;
				for(UINT4 i = 0; i < indexes.size(); i++)	{
					if(i == 0) {
						lastPrefix = indexes[0].prefix;
					}
					unsigned int indexrow = 0;
					// Checks how big a distance there is between the current and the next prefix. eg cur is 3 and next is 10 = 7.
					unsigned int diffSize = indexes[i].prefix - lastPrefix; 
					if(i > 0 && diffSize > 1) {
						//indexrow |= indexes[i].prefixstart;
						//printf("Diffsize is %u\n", diffSize);

						// then write the distance amount of 00's
						if(diffSize > 1000) {
							printf("WARNING! The distance to the next prefix is %i. Do you want to continue writing %i bytes of 0x00? Press y to continue", diffSize, (diffSize*m_indexrowsizebytes));
							#ifdef _WIN32
							if ( _getch() != 'y' ) {
							#else
							if ( tty_getchar() != 'y' ) {
							#endif
								printf("Aborting...");
								exit(1);
							}
						}
						for(UINT4 j = 1; j < diffSize; j++)
						{								
							fwrite(&zero, 1, m_indexrowsizebytes, pFileIndex);
						}
					}					
					fwrite(&indexes[i].numchains, 1, m_indexrowsizebytes, pFileIndex);
					lastPrefix = indexes[i].prefix;
				}
				fclose(pFileIndex);
			}
			else {
				printf("memory allocation fail\n");
			}			
			// Already finished?
	}
	else {
		printf("can't open file\n");
	}
	if(reader != NULL)
		delete reader;
	if(fileR != NULL) {
		fclose(fileR);
	}
		
}

int main(int argc, char* argv[])
{
	int argi = 1, i, argsUsed = 0;
	unsigned int sptl = 40, eptl = 16;
	int showDistribution = 0;
	int usecp = 0;// How many bits to use from the index
	int hascp = 0; 
	vector<unsigned int> cppositions;
	if (argc == 1) {
		Usage();		
		return 0;
	}
	else if(argc > 2) {
		for (; argi < argc; argi++)
		{
			if(strcmp(argv[argi], "-d") == 0 && (argsUsed & 0x8) == 0) {
				// Enable verbose mode
				argsUsed |= 0x8;				
				showDistribution = 1;
			}			
			else if (strncmp(argv[argi], "-sptl=", 6) == 0 && (argsUsed & 0x1) == 0) {
				// Maximum index for starting point
				argsUsed |= 0x1;
				sptl = 0;
				for (i = 6; argv[argi][i] >= '0' && argv[argi][i] <= '9'; i++) {
					sptl *= 10;
					sptl += ((int) argv[argi][i]) - 0x30;
				}
				if (argv[argi][i] != '\0') {
					printf("Error: Invalid number.\n\n");
					Usage();
					return 1;
				}
				if (i > 23) { // i - 3 > 20				
					printf("Error: Number is too large.\n\n");
					Usage();
					return 1;
				}			
			}

			else if (strncmp(argv[argi], "-eptl=", 6) == 0 && (argsUsed & 0x2) == 0) {
				// Maximum index for ending points
				argsUsed |= 0x2;
				eptl = 0;
				for (i = 6; argv[argi][i] >= '0' && argv[argi][i] <= '9'; i++) {
					eptl *= 10;
					eptl += ((int) argv[argi][i]) - 0x30;
				}
				if (argv[argi][i] != '\0') {
					printf("Error: Invalid number.\n\n");
					Usage();
					return 1;
				}
				if (i > 23) { // i - 3 > 20				
					printf("Error: Number is too large.\n\n");
					Usage();
					return 1;
				}			
			}
			else if(strncmp(argv[argi], "-usecp=", 7) == 0 && (argsUsed & 0x4) == 0) {
				argsUsed |= 0x4;
				hascp = 1;
				usecp = 0;
				unsigned int cppos = 0;
				for(i = 7; argv[argi][i] != ' ' && argv[argi][i] != '\n' && argv[argi][i] != 0;) {
					if(cppositions.size() > 0) i++;
					for (; argv[argi][i] >= '0' && argv[argi][i] <= '9'; i++)
					{
						cppos *= 10;
						cppos += ((int) argv[argi][i]) - 0x30;
					}
/*					if(argv[argi][i] == ',')
					{*/
						cppositions.push_back(cppos);
						usecp++;
						cppos = 0;
					//}
				}
				if (argv[argi][i] != '\0') {
					printf("Error: Invalid number.\n\n");
					Usage();
					return 1;
				}
				if (usecp > 16) { // i - 3 > 20
					printf("Error: Number is too large.\n\n");
					Usage();
					return 1;
				}				
				else {
					printf("Using %i bits of the checkpoints\n", usecp);
				}
			}

		}		
	}
	vector<string> vPathName;
#ifdef WIN32
	string sWildCharPathName = argv[1];
	GetTableList(sWildCharPathName, vPathName);
#else
	GetTableList(argc, argv, vPathName);
#endif
	if (vPathName.size() == 0) {
		printf("no rainbow table found\n");
		return 0;
	}
	for (UINT4 i = 0; i < vPathName.size(); i++) {
		string sResultFile;
		int n = vPathName[i].find_last_of('\\');
		if (n != -1) {
			if(vPathName[i].substr(vPathName[i].length() - 3, vPathName[i].length()) == "rti")	{
				sResultFile = vPathName[i].substr(n+1, vPathName[i].length()) + "2";				
			}
			else {
				sResultFile = vPathName[i].substr(n+1, vPathName[i].length()) + "i2";
			}
		}
		else {
			if(vPathName[i].substr(vPathName[i].length() - 3, vPathName[i].length()) == "rti")	{
				sResultFile = vPathName[i] + "2";				
			} else {
				sResultFile = vPathName[i] + "i2"; // Resulting file is .rt, not .rti
			}
		}
		if(usecp == 0 && showDistribution == 0) {
			printf("Using %i of 64 bits. sptl: %i, eptl: %i, cp: %i. Chains will be %i bytes in size\n", (sptl + eptl + usecp), sptl, eptl, usecp, ((sptl + eptl + usecp) / 8));
		}
		if(sptl + eptl + usecp > 64) {
			exit(1);
		}
		ConvertRainbowTable(vPathName[i], sResultFile, sptl, eptl, showDistribution, hascp, usecp, cppositions);
		printf("\n");
	}
	return 0;
}