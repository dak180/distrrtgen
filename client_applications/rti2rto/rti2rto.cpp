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
	#include <dirent.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "Public.h"
#include "MemoryPool.h"
#include "RTI2Reader.h"
#include "RTIReader.h"

void usage()
{
	printf("rti2rto - Indexed to Original rainbow table converter\n");
	printf("by Martin Westergaard <martinwj2005@gmail.com>\n");
	printf("http://www.freerainbowtables.com\n\n");

	printf("usage: rti2rto rainbow_table_pathname\n");
	printf("-v show debug information\n");
	printf("-drop_last_n_chains=N - use only when you know you must\n");
	printf("\n");
	printf("rainbow_table_pathname: pathname of the rainbow table(s), wildchar(*, ?) supported\n");
	printf("\n");
	printf("example: rti2rto *.rti\n");
	printf("         rti2rto md5_*.rti\n");
}
#ifdef _WIN32
void GetTableList( std::string wildCharPathName, std::vector<std::string>& pathNames )
{
	std::string sPath;
	std::string::size_type n = wildCharPathName.find_last_of('\\');

	if ( n != std::string::npos )
		sPath = wildCharPathName.substr(0, n + 1);

	_finddata_t fd;
	long handle = _findfirst( wildCharPathName.c_str(), &fd);
	if (handle != -1)
	{
		do
		{
			std::string sName = fd.name;
			if (sName != "." && sName != ".." && !(fd.attrib & _A_SUBDIR))
			{
				std::string pathName = sPath + sName;
				pathNames.push_back(pathName);
			}
		} while (_findnext(handle, &fd) == 0);

		_findclose(handle);
	}
}
#else
void GetTableList( std::string wildCharPathName, std::vector<std::string>& pathNames )
{
	struct stat buf;
	if ( lstat( wildCharPathName.c_str(), &buf ) == 0 )
	{
		if ( S_ISDIR(buf.st_mode) )
		{
			DIR *dir = opendir( wildCharPathName.c_str() );

			if ( dir )
			{
				struct dirent *dirEntry = NULL;
				while ( ( dirEntry = readdir( dir ) ) != NULL )
				{
					std::string filename = "";
					filename += (*dirEntry).d_name;

					if ( filename != "." && filename != ".." )
					{
						std::string new_filename = wildCharPathName + '/' + filename;
						GetTableList( new_filename, pathNames );
					}
				}
			}

			closedir( dir );
		}
		else if ( S_ISREG(buf.st_mode) )
		{
			pathNames.push_back( wildCharPathName );
		}
	}
}
#endif

void ConvertRainbowTable( std::string pathName, std::string resultFileName, std::string sType, bool debug, uint32 dropLastNchains )
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

	if ( debug )
		reader->Dump();

	uint64 size = reader->getChainsLeft() * sizeof(RainbowChainO);

	size -= sizeof(RainbowChainO) * dropLastNchains;

#ifdef _MEMORYDEBUG
	printf("Starting allocation of %i bytes\n", size);
#endif
	RainbowChainO* pChain = (RainbowChainO*)mp.Allocate(size, nAllocatedSize);
#ifdef _MEMORYDEBUG
	printf("Finished. Got %i bytes\n", nAllocatedSize);
#endif
	if (pChain != NULL)
	{
		nAllocatedSize = nAllocatedSize / sizeof(RainbowChainO) * sizeof(RainbowChainO);		// Round to boundary
		unsigned int nChains = nAllocatedSize / sizeof(RainbowChainO);
		while( reader->getChainsLeft() > 0 && reader->getChainsLeft() > dropLastNchains )
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
	bool debug = false;
	uint32 dropLastNchains = 0;

	if (argc < 2)
	{
		usage();
		return 0;
	}
	
	std::vector<std::string> pathNames;

	// Parse command line args
	int i;
	for( i = 1; i < argc; i++ )
	{
		if ( strncmp( argv[i], "-v", 2 ) == 0 )
			debug = true;
		else if ( strncmp( argv[i], "-drop_last_n_chains=", 20 ) == 0 )
		{
			uint32 j;

			for ( j = 20; argv[i][j] >= '0' && argv[i][j] <= '9'; j++ )
			{
				dropLastNchains *= 10;
				dropLastNchains += ((int) argv[i][j] ) - 0x30;
			}

			if ( argv[i][j] != '\0' )
			{
				printf("Error: Invalid drop_last_n_chains number.\n\n");
				usage();
				exit( 1 );
			}
		}
		else
			GetTableList( argv[i], pathNames );
	}

	if ( debug )
	{
		for( int i = 0; i < argc; i++ )
			printf("%i: %s\n", i, argv[i]);
	}

	if ( pathNames.size() == 0 )
	{
		printf("no rainbow table found\n");
		return 0;
	}

	std::string resultFile, sType;

	for ( uint32 i = 0; i < pathNames.size(); i++ )
	{
		if( pathNames[i].substr( pathNames[i].length() - 4, pathNames[i].length()) == "rti2")
		{
			resultFile = pathNames[i].substr(0, pathNames[i].length() - 2); // Resulting file is .rt, not .rti2
			sType = "RTI2";
		}
		else if( pathNames[i].substr( pathNames[i].length() - 3, pathNames[i].length()) == "rti")
		{
			resultFile = pathNames[i].substr( 0, pathNames[i].length() - 1 ); // Resulting file is .rt, not .rti
			sType = "RTI";
		}
		else 
		{
			printf("File %s is not a RTI or a RTI2 file", pathNames[i].c_str() );
			continue;
		}
		
		// XXX this assumes someone is converting either just the last file
		// *or* doing a whole set which will read the last file first
		if ( dropLastNchains > 0 && i == 0 )
		{
			std::string::size_type lastX = resultFile.find_last_of('x');

			if ( lastX == std::string::npos )
			{
				std::cout << "Could not parse the filename to drop the last chains"
					<< std::endl;
				exit( -1 );
			}
			
			std::string::size_type firstSplit
				= resultFile.find_first_of('_',lastX);

			#if defined(_WIN32) && !defined(__GNUC__)
				uint64 chains = _atoi64( resultFile.substr( lastX + 1, firstSplit - lastX - 1).c_str() );
			#else
				uint64 chains = atoll( resultFile.substr( lastX + 1, firstSplit - lastX - 1 ).c_str() );
			#endif

			chains -= dropLastNchains;

			resultFile.replace( lastX + 1, firstSplit - lastX - 1, uint64tostr( chains ) );
		}
			
		ConvertRainbowTable( pathNames[i], resultFile, sType, debug, dropLastNchains );
		dropLastNchains = 0;

		printf("\n");
	}
	return 0;
}
