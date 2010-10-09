#include <string>
#include <vector>
#ifdef _WIN32
#include <io.h>
#else
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
#endif

#include <time.h>
#include "Public.h"
#include "MemoryPool.h"
#include "RTI2Reader.h"
#include "RTIReader.h"
using namespace std;

void Usage()
{
	printf("rti2rto - Indexed to Original rainbow table converter\n");
	printf("by Martin Westergaard <martinwj2005@gmail.com>\n");
	printf("http://www.freerainbowtables.com\n\n");

	printf("usage: rti2rto rainbow_table_pathname\n");
	printf("rainbow_table_pathname: pathname of the rainbow table(s), wildchar(*, ?) supported\n");
	printf("\n");
	printf("example: rti2rto *.rti\n");
	printf("         rti2rto md5_*.rti\n");
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
	if (handle != -1)
	{
		do
		{
			string sName = fd.name;
			if (sName != "." && sName != ".." && !(fd.attrib & _A_SUBDIR))
			{
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


void ConvertRainbowTable(string sPathName, string sResultFileName, string sType)
{
#ifdef _WIN32
	int nIndex = sPathName.find_last_of('\\');
#else
	int nIndex = sPathName.find_last_of('/');
#endif
	string sFileName;
	if (nIndex != -1)
		sFileName = sPathName.substr(nIndex + 1);
	else
		sFileName = sPathName;
	// Info
	printf("%s:\n", sFileName.c_str());
	FILE *fResult = fopen(sResultFileName.c_str(), "wb");
	if(fResult == NULL)
	{
		printf("Could not open %s for write access", sResultFileName.c_str());
		return;
	}
	static CMemoryPool mp;
	unsigned int nAllocatedSize;
	BaseRTReader *reader = NULL;

	if(sType == "RTI2")
		reader = (BaseRTReader*)new RTI2Reader(sFileName);
	else if(sType == "RTI")
		reader = (BaseRTReader*)new RTIReader(sFileName);
	else 
	{
		printf("Invalid table type '%s'", sType.c_str());
		return ;
	}

	RainbowChainCP* pChain = (RainbowChainCP*)mp.Allocate(reader->GetChainsLeft() * sizeof(RainbowChainCP), nAllocatedSize);
	if (pChain != NULL)
	{
		nAllocatedSize = nAllocatedSize / sizeof(RainbowChainCP) * sizeof(RainbowChainCP);		// Round to boundary
		unsigned int nChains = nAllocatedSize / sizeof(RainbowChainCP);
		while(reader->GetChainsLeft() > 0)
		{
			reader->ReadChains(nChains, pChain);
			for(UINT4 i = 0; i < nChains; i++)
			{
				fwrite(&pChain[i], 1, 16, fResult);
			}
		}
	}
	fclose(fResult);
	if(reader != NULL)
		delete reader;
}
int main(int argc, char* argv[])
{
#ifdef _WIN32
	if (argc != 2)
	{
		Usage();
		
		return 0;
	}
	string sWildCharPathName = argv[1];
	vector<string> vPathName;
	GetTableList(sWildCharPathName, vPathName);
#else
	if (argc < 2)
	{
		Usage();
		return 0;
	}
	for(int i = 0; i < argc; i++)
	{
		printf("%i: %s\n", i, argv[i]);
	}
	// vPathName
	vector<string> vPathName;
	GetTableList(argc, argv, vPathName);
#endif
	if (vPathName.size() == 0)
	{
		printf("no rainbow table found\n");
		return 0;
	}
	for (UINT4 i = 0; i < vPathName.size(); i++)
	{
		string sResultFile, sType;
			
		if(vPathName[i].substr(vPathName[i].length() - 4, vPathName[i].length()) == "rti2")
		{
			sResultFile = vPathName[i].substr(0, vPathName[i].length() - 2); // Resulting file is .rt, not .rti2
			sType = "RTI2";
		}
		else if(vPathName[i].substr(vPathName[i].length() - 3, vPathName[i].length()) == "rti")
		{
			sResultFile = vPathName[i].substr(0, vPathName[i].length() - 1); // Resulting file is .rt, not .rti
			sType = "RTI";
		}
		else 
		{
			printf("File %s is not a RTI or a RTI2 file", vPathName[i].c_str());
			continue;
		}
		ConvertRainbowTable(vPathName[i], sResultFile, sType);
		printf("\n");
	}
	return 0;
}
