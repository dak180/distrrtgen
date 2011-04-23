/*
* rti2rto is a tool to convert from RT and RTI to RTI2
*
* Copyright 2009, 2010, 2011 Martin Westergaard Jørgensen <martinwj2005@gmail.com>
* Copyright 2010, 2011 James Nobis <quel@quelrod.net>
*
* This file is part of rti2rto.
*
* rti2rto is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* rti2rto is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with rti2rto.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string>
#include <vector>

#if defined(_WIN32) && !defined(__GNUC__)
	#include <io.h>
#else
	#include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "Public.h"
#include "MemoryPool.h"
#include "RTI2Reader.h"
#include "RTIReader.h"

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
void GetTableList( std::string sWildCharPathName, std::vector<std::string>& vPathName)
{
	vPathName.clear();

	std::string sPath;
	int n = sWildCharPathName.find_last_of('\\');
	if (n != -1)
		sPath = sWildCharPathName.substr(0, n + 1);

	_finddata_t fd;
	long handle = _findfirst(sWildCharPathName.c_str(), &fd);
	if (handle != -1)
	{
		do
		{
			std::string sName = fd.name;
			if (sName != "." && sName != ".." && !(fd.attrib & _A_SUBDIR))
			{
				std::string pathName = sPath + sName;
				vPathName.push_back(pathName);
			}
		} while (_findnext(handle, &fd) == 0);

		_findclose(handle);
	}
}
#else
void GetTableList(int argc, char* argv[], std::vector<std::string>& vPathName)
{
	vPathName.clear();

	int i;
	for (i = 1; i < argc; i++)
	{
		std::string pathName = argv[i];
		struct stat buf;
		if (lstat(pathName.c_str(), &buf) == 0)
		{
			if (S_ISREG(buf.st_mode))
				vPathName.push_back(pathName);

		}
	}
}
#endif

// ConvertRainbowTable(vPathName[i], resultFile, sType);
void ConvertRainbowTable( std::string pathName, std::string resultFileName, std::string sType )
{
#ifdef _WIN32
	std::string::size_type nIndex = pathName.find_last_of('\\');
#else
	std::string::size_type nIndex = pathName.find_last_of('/');
#endif
	std::string fileName;

	if ( nIndex != std::string::npos )
		fileName = pathName.substr( nIndex + 1 );
	else
		fileName = pathName;

	// Info
	printf("%s:\n", fileName.c_str());
	FILE *fResult = fopen(resultFileName.c_str(), "wb");
	if(fResult == NULL)
	{
		printf("Could not open %s for write access", resultFileName.c_str());
		return;
	}
	static CMemoryPool mp;
	uint64 nAllocatedSize;
	BaseRTReader *reader = NULL;
	if(sType == "RTI2")
		reader = (BaseRTReader*)new RTI2Reader( pathName );
	else if(sType == "RTI")
		reader = (BaseRTReader*)new RTIReader( pathName );
	else 
	{
		printf("Invalid table type '%s'", sType.c_str());
		return ;
	}

	reader->Dump();

	uint64 size = reader->getChainsLeft() * sizeof(RainbowChain);
#ifdef _MEMORYDEBUG
	printf("Starting allocation of %i bytes\n", size);
#endif
	RainbowChain* pChain = (RainbowChain*)mp.Allocate(size, nAllocatedSize);
#ifdef _MEMORYDEBUG
	printf("Finished. Got %i bytes\n", nAllocatedSize);
#endif
	if (pChain != NULL)
	{
		nAllocatedSize = nAllocatedSize / sizeof(RainbowChain) * sizeof(RainbowChain);		// Round to boundary
		unsigned int nChains = nAllocatedSize / sizeof(RainbowChain);
		while(reader->getChainsLeft() > 0)
		{
#ifdef _MEMORYDEBUG
			printf("Grabbing %i chains from file\n", nChains);
#endif
			reader->readChains(nChains, pChain);
#ifdef _MEMORYDEBUG
			printf("Recieved %i chains from file\n", nChains);
#endif
			for(uint32 i = 0; i < nChains; i++)
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
	std::string sWildCharPathName = argv[1];
	std::vector<std::string> vPathName;
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
	std::vector<std::string> vPathName;
	GetTableList(argc, argv, vPathName);
#endif
	if (vPathName.size() == 0)
	{
		printf("no rainbow table found\n");
		return 0;
	}
	for (uint32 i = 0; i < vPathName.size(); i++)
	{
		std::string resultFile, sType;
			
		if(vPathName[i].substr(vPathName[i].length() - 4, vPathName[i].length()) == "rti2")
		{
			resultFile = vPathName[i].substr(0, vPathName[i].length() - 2); // Resulting file is .rt, not .rti2
			sType = "RTI2";
		}
		else if(vPathName[i].substr(vPathName[i].length() - 3, vPathName[i].length()) == "rti")
		{
			resultFile = vPathName[i].substr(0, vPathName[i].length() - 1); // Resulting file is .rt, not .rti
			sType = "RTI";
		}
		else 
		{
			printf("File %s is not a RTI or a RTI2 file", vPathName[i].c_str());
			continue;
		}
		ConvertRainbowTable(vPathName[i], resultFile, sType);
		printf("\n");
	}
	return 0;
}
