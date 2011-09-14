/*
* rti2rto is a tool to convert from RTI and RTI2 to RT
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
	printf("-drop_high_sp_n_chains=N -sptl=startpt_bits - use only when you know you must\n");
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
#if defined(_WIN32) && !defined(__GNUC__)
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

void ConvertRainbowTable( std::string pathName, std::string resultFileName, std::string sType, bool debug, uint32 dropLastNchains, uint32 dropHighSPcount, int sptl )
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
		reader = new RTI2Reader( pathName );
	else if(sType == "RTI")
		reader = new RTIReader( pathName );
	else 
	{
		printf("Invalid table type '%s'", sType.c_str());
		return ;
	}

	if ( debug )
		reader->Dump();

	uint64 size = reader->getChainsLeft() * sizeof(RainbowChainO);
	uint64 rainbowChainCount = reader->getChainsLeft();
	uint64 chainsLeft;

	rainbowChainCount -= dropLastNchains;
	rainbowChainCount -= dropHighSPcount;

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
		while( ( chainsLeft = reader->getChainsLeft() ) > 0 && chainsLeft > dropLastNchains )
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
				if ( dropHighSPcount > 0 && GetMaxBits(pChain[i].nIndexS) > sptl )
				{
					//dropHighSPcount++;
				}
				else if ( dropLastNchains > 0 && dropLastNchains >= ( chainsLeft - i ) )
				{
				}
				else
					fwrite(&pChain[i], 1, 16, fResult);
			}
		}
	}
	fclose(fResult);

	if(reader != NULL)
		delete reader;

	if ( dropHighSPcount > 0 )
	{
		std::string::size_type lastX = resultFileName.find_last_of('x');

		if ( lastX == std::string::npos )
		{
			std::cout << "Could not parse the filename to drop the high SP chains"
				<< std::endl;
			exit( -1 );
		}
		
		std::string::size_type firstSplit
			= resultFileName.find_first_of('_',lastX);

		if ( firstSplit == std::string::npos )
		{
			std::cout << "Could not parse the filename to drop the high SP chains"
				<< std::endl;
			exit( -1 );
		}

		std::string newResultFileName = resultFileName;

		newResultFileName.replace( lastX + 1, firstSplit - lastX - 1, uint64tostr( rainbowChainCount ) );
		
		if ( rename( resultFileName.c_str(), newResultFileName.c_str() ) != 0 )
		{
			std::cout << "Could not parse the filename to drop the high SP chains"
				<< std::endl;
			exit( -1 );
		}
	}
}

int main(int argc, char* argv[])
{
	bool debug = false;
	uint32 dropLastNchains = 0;
	uint32 dropHighSPcount = 0;
	int sptl = 0;

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
		else if ( strncmp( argv[i], "-drop_high_sp_n_chains=", 23 ) == 0 )
		{
			uint32 j;

			for ( j = 23; argv[i][j] >= '0' && argv[i][j] <= '9'; j++ )
			{
				dropHighSPcount *= 10;
				dropHighSPcount += ((int) argv[i][j] ) - 0x30;
			}

			if ( argv[i][j] != '\0' )
			{
				printf("Error: Invalid drop_high_sp_n_chains number.\n\n");
				usage();
				exit( 1 );
			}
		}
		else if ( strncmp( argv[i], "-sptl=", 6 ) == 0 )
		{
			uint32 j;

			for ( j = 6; argv[i][j] >= '0' && argv[i][j] <= '9'; j++ )
			{
				sptl *= 10;
				sptl += ((int) argv[i][j] ) - 0x30;
			}

			if ( argv[i][j] != '\0' )
			{
				printf("Error: Invalid sptl number.\n\n");
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

		ConvertRainbowTable( pathNames[i], resultFile, sType, debug, dropLastNchains, dropHighSPcount, sptl );
		dropLastNchains = 0;
		dropHighSPcount = 0;

		printf("\n");
	}
	return 0;
}
