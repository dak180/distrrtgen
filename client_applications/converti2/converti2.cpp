/*
* converti2 is a tool to convert from RT and RTI to RTI2
*
* Copyright 2009, 2010, 2011 Martin Westergaard Jørgensen <martinwj2005@gmail.com>
* Copyright 2010, 2011 James Nobis <quel@quelrod.net>
*
* This file is part of converti2.
*
* converti2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* converti2 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with converti2.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "converti2.h"

Converti2::Converti2( int argc, char** argv )
{
	sptl = 40;
	eptl = 16;
	showDistribution = false;
	hasCheckPoints = false;
	argi = 1;
	argsUsed = 0;
	checkPointBits = 0;
	this->argc = argc;
	this->argv = argv;
}

bool Converti2::shouldShowDistribution()
{
	return showDistribution;
}

int Converti2::GetMaxBits(uint64 highvalue)
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

#if defined(_WIN32) && !defined(__GNUC__)
void Converti2::GetTableList( std::string sWildCharPathName)
{
	vPathName.clear();

	std::string path;
	std::string::size_type n = sWildCharPathName.find_last_of('\\');
	if ( n != std::string::npos )
		path = sWildCharPathName.substr(0, n + 1);

	_finddata_t fd;
	long handle = _findfirst(sWildCharPathName.c_str(), &fd);
	if (handle != -1)
	{
		do	{
			std::string name = fd.name;
			if (name != "." && name != ".." && !(fd.attrib & _A_SUBDIR))
			{
				pathName = path + name;
				vPathName.push_back(pathName);
			}
		} while (_findnext(handle, &fd) == 0);

		_findclose(handle);
	}
}
#else
void Converti2::GetTableList()
{
	vPathName.clear();

	int i;
	for (i = 1; i < argc; i++)
	{
		pathName = argv[i];
		struct stat buf;
		if (lstat(pathName.c_str(), &buf) == 0)
		{
			if (S_ISREG(buf.st_mode))
				vPathName.push_back(pathName);
		}
	}
}
#endif

int Converti2::sharedSetup()
{
	if(argc > 2)
	{
		for (; argi < argc; argi++)
		{
			if(strcmp(argv[argi], "-d") == 0 && (argsUsed & 0x8) == 0)
			{
				// Enable verbose mode
				argsUsed |= 0x8;				
				showDistribution = true;
			}			
			else if (strncmp(argv[argi], "-sptl=", 6) == 0 && (argsUsed & 0x1) == 0)
			{
				// Maximum index for starting point
				argsUsed |= 0x1;
				sptl = 0;
				for (i = 6; argv[argi][i] >= '0' && argv[argi][i] <= '9'; i++)
				{
					sptl *= 10;
					sptl += ((int) argv[argi][i]) - 0x30;
				}

				if (argv[argi][i] != '\0')
				{
					printf("Error: Invalid number.\n\n");
					usage();
					exit( 1 );
				}

				if (i > 23)
				{
					// i - 3 > 20				
					printf("Error: Number is too large.\n\n");
					usage();
					exit( 1 );
				}			
			}

			else if (strncmp(argv[argi], "-eptl=", 6) == 0 && (argsUsed & 0x2) == 0)
			{
				// Maximum index for ending points
				argsUsed |= 0x2;
				eptl = 0;
				for (i = 6; argv[argi][i] >= '0' && argv[argi][i] <= '9'; i++)
				{
					eptl *= 10;
					eptl += ((int) argv[argi][i]) - 0x30;
				}
				if (argv[argi][i] != '\0')
				{
					printf("Error: Invalid number.\n\n");
					usage();
					exit( 1 );
				}
				if (i > 23)
				{
					// i - 3 > 20				
					printf("Error: Number is too large.\n\n");
					usage();
					exit( 1 );
				}			
			}
			else if(strncmp(argv[argi], "-usecp=", 7) == 0 && (argsUsed & 0x4) == 0)
			{
				argsUsed |= 0x4;
				hasCheckPoints = 1;
				checkPointBits = 0;
				unsigned int cppos = 0;
				for(i = 7; argv[argi][i] != ' ' && argv[argi][i] != '\n' && argv[argi][i] != 0;)
				{
					if(cppositions.size() > 0) i++;
					for (; argv[argi][i] >= '0' && argv[argi][i] <= '9'; i++)
					{
						cppos *= 10;
						cppos += ((int) argv[argi][i]) - 0x30;
					}

					cppositions.push_back(cppos);
					checkPointBits++;
					cppos = 0;
				}
				if (argv[argi][i] != '\0')
				{
					printf("Error: Invalid number.\n\n");
					usage();
					return 1;
				}
				if ( checkPointBits > 16)
				{ // i - 3 > 20
					printf("Error: Number is too large.\n\n");
					usage();
					exit( 1 );
				}				
				else
				{
					printf("Using %i bits of the checkpoints\n", checkPointBits );
				}
			}
		}		
	}

#ifdef _WIN32
	std::string sWildCharPathName = argv[1];
	GetTableList(sWildCharPathName);
#else
	GetTableList();
#endif
	if (vPathName.size() == 0)
	{
		printf("no rainbow table found\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void Converti2::doShowDistribution()
{


}

void Converti2::convertRainbowTables()
{
	std::string resultFile;

	for (uint32 i = 0; i < vPathName.size(); i++)
	{
		std::string::size_type n = vPathName[i].find_last_of('\\');
		if ( n != std::string::npos )
		{
			if(vPathName[i].substr(vPathName[i].length() - 3, vPathName[i].length()) == "rti")
				resultFile = vPathName[i].substr(n+1, vPathName[i].length()) + "2";				
			else
				resultFile = vPathName[i].substr(n+1, vPathName[i].length()) + "i2";
		}
		else
		{
			if(vPathName[i].substr(vPathName[i].length() - 3, vPathName[i].length()) == "rti")
				resultFile = vPathName[i] + "2";
			else
				resultFile = vPathName[i] + "i2"; // Resulting file is .rt, not .rti
		}

		if( checkPointBits == 0 && !shouldShowDistribution() )
		{
			printf("Using %i of 64 bits. sptl: %i, eptl: %i, cp: %i. Chains will be %i bytes in size\n", (sptl + eptl + checkPointBits), sptl, eptl, checkPointBits, ((sptl + eptl + checkPointBits) / 8));
		}

		if( (sptl + eptl + checkPointBits) > 64)
		{
			fprintf( stderr, "(sptl + eptl + checkPointBits) > 64\n" );
			exit(1);
		}

		pathName = vPathName[i];
		convertRainbowTable( resultFile, vPathName.size() );
	}
}

void Converti2::convertRainbowTable( std::string resultFileName, uint32 files )
{
#ifdef _WIN32
	std::string::size_type nIndex = pathName.find_last_of('\\');
#else
	std::string::size_type nIndex = pathName.find_last_of('/');
#endif
	std::string fileName;

	if ( nIndex != std::string::npos )
		fileName = pathName.substr(nIndex + 1);
	else
		fileName = pathName;

	unsigned int distribution[64] = {0};
	unsigned int numProcessedChains = 0;
	BaseRTReader *reader = NULL;
	BaseRTWriter *writer = NULL;
	bool isOldRtFormat = false;
	CharacterSet charSet;
	uint64 endPointMask, startPointMask, startPointShift;

	if ( fileName.length() < 3 )
	{
		printf( "%s is not a rainbow table\n", pathName.c_str() );
		exit( 1 );
	}

	if ( fileName.substr( fileName.length() - 4 ) == ".rti" )
		isOldRtFormat = false;
	else if ( fileName.substr( fileName.length() - 3 ) == ".rt" )
		isOldRtFormat = true;

	if ( isOldRtFormat )
		reader = (BaseRTReader*)new RTReader(pathName);
	else 
		reader = (BaseRTReader*)new RTIReader(pathName);

	if(reader == NULL)
	{
		printf("%s is not a supported file (Only RT and RTI is supported)\n", pathName.c_str());
		return;
	}

	reader->setMinimumStartPoint();

	std::vector<std::string> vPart;

	if ( !SeperateString( fileName, "___x_", vPart ) )
	{
		printf("filename %s not identified\n", pathName.c_str());
		exit( 1 );
	}

	std::string sHashRoutineName = vPart[0];
	uint32 rainbowTableIndex = atoi(vPart[2].c_str());
	uint32 rainbowChainLen = atoi(vPart[3].c_str());
	uint32 rainbowChainCount = atoi(vPart[4].c_str());

	std::vector<std::string> vPart2;
	// vPart[5] ex distrrtgen[p][i]_00.rti
	if ( !SeperateString( vPart[5], "_.", vPart2 ) )
	{
		printf( "filename %s not identified\n", pathName.c_str() );
		exit( 1 );
	}

	uint32 fileIndex = atoi( vPart2[1].c_str() );

	// Parse charset definition
	std::string sCharsetDefinition = vPart[1];
	std::string sCharsetName;
	int nPlainLenMin = 0, nPlainLenMax = 0;
	std::vector<SubKeySpace> tmpSubKeySpaces;
	SubKeySpace tmpSubKeySpace;

	tmpSubKeySpaces.clear();

	// XXX handle hybrid/hybrid2
	if(sCharsetDefinition.substr(0, 6) == "hybrid") // Hybrid table
	{
		sCharsetName = sCharsetDefinition;
	}
	else
	{
		// For backward compatibility, "#1-7" is implied
		if ( sCharsetDefinition.find('#') == std::string::npos )
		{
			sCharsetName = sCharsetDefinition;
			nPlainLenMin = 1;
			nPlainLenMax = 7;
		}
		else
		{
			std::vector<std::string> vCharsetDefinitionPart;
			if (!SeperateString(sCharsetDefinition, "#-", vCharsetDefinitionPart))
			{
				printf("filename %s not identified\n", pathName.c_str());
				exit( 1 );
			}
			else
			{
				sCharsetName = vCharsetDefinitionPart[0];
				nPlainLenMin = atoi(vCharsetDefinitionPart[1].c_str());
				nPlainLenMax = atoi(vCharsetDefinitionPart[2].c_str());
			}
		}
	}

	// XXX implement
	if ( sCharsetName == "byte" )
	{
		
	}
	else if ( sCharsetName.substr( 0, 6 ) == "hybrid" )
	{

	}

	bool readCharset = false;
	bool foundCharset = false;

	std::vector<std::string> line;

	if ( ReadLinesFromFile("charset.txt", line) )
		readCharset = true;
	else if ( ReadLinesFromFile(GetApplicationPath() + "charset.txt", line ) )
		readCharset = true;

	if ( readCharset )
	{
		uint32 i;

		for ( i = 0; i < line.size(); i++ )
		{
			// Filter comment
			if ( line[i][0] == '#' )
				continue;

			std::vector<std::string> part;
			if ( SeperateString( line[i], "=", part ) )
			{
				std::string tmpCharsetName = TrimString( part[0] );

				if ( tmpCharsetName == "" )
					continue;

				// check charset
				bool charsetNameCheckPass = true;
				uint32 j;

				for ( j = 0; j < tmpCharsetName.size(); j++ )
				{
					if ( !isalpha( tmpCharsetName[j] )
						&& !isdigit( tmpCharsetName[j] )
						&& ( tmpCharsetName[j] != '-' ) )
					{
						charsetNameCheckPass = false;
						break;
					}
				}

				if ( !charsetNameCheckPass )
				{
					std::cerr << "invalid charset name " << tmpCharsetName
						<< " in charset configuration file" << std::endl;
					continue;
				}

				std::string charsetContent = TrimString( part[1] );
				if ( charsetContent == "" || charsetContent == "[]" )
					continue;
				if ( charsetContent[0] != '['
					|| charsetContent[charsetContent.size() - 1 ] != ']' )
				{
					std::cerr << "invalid charset content " << tmpCharsetName
						<< " in charset configuration file" << std::endl;
					continue;
				}
				charsetContent = charsetContent.substr( 1, charsetContent.size() - 2 );

				if ( charsetContent.size() > MAX_PLAIN_LEN )
				{
					std::cerr << "charset content " << tmpCharsetName
						<< " is too long" << std::endl;
					continue;
				}
				
				// XXX if hybrid
				if ( false )
				{

				}	
				else if ( tmpCharsetName == sCharsetName )
				{
					foundCharset = true;

					tmpSubKeySpace.perPositionCharacterSets.clear();

					tmpSubKeySpace.hybridSets = 0x1;
					
					charSet.characterSet1.clear();
					charSet.characterSet2.clear();
					charSet.characterSet3.clear();
					charSet.characterSet4.clear();
					
					charSet.characterSet1.assign( (uint8*) charsetContent.c_str(), ((uint8*) charsetContent.c_str()) + charsetContent.size() );

					for ( int a = 0; a < nPlainLenMax; a++ )
					{
						tmpSubKeySpace.passwordLength.clear();
						tmpSubKeySpace.passwordLength.push_back( a + 1 );
						tmpSubKeySpace.charSetFlags.push_back( 1 );
						tmpSubKeySpace.perPositionCharacterSets.push_back( charSet );

						tmpSubKeySpaces.push_back( tmpSubKeySpace );
					}

					break;
				}
			}
		}

		if ( !foundCharset )
		{
			std::cerr << "charset " << sCharsetName << " not found in charset.txt"
				<< std::endl;
		}
	}
	else
	{
		std::cerr << "can't open charset configuration file" << std::endl;
		exit( 1 );
	}

	// Info
	printf("%s:\n", fileName.c_str());

	endPointMask = (((uint64) 1) >> eptl) + 1;
	startPointMask = (((uint64) 1) >> sptl) + 1;
	startPointShift = eptl;

	// XXX showDistribution shouldn't be mixed in here
	if( !showDistribution )
	{
		writer = (BaseRTWriter*)new RTI2Writer( resultFileName );
		writer->setStartPointLen( sptl );
		writer->setEndPointLen( eptl );
		writer->setCheckPointLen( checkPointBits );
		writer->setCheckPointPos( cppositions );
		writer->setChainLength( rainbowChainLen );
		writer->setTableIndex( rainbowTableIndex );
		writer->setChainCount( rainbowChainCount );
		writer->setFileIndex( fileIndex );
		// XXX this is just taking input of the number of rt/rti files to be
		// converted and needs to be more robust, such as scanning the directory
		writer->setFileCount( files );

		writer->setAlgorithm( sHashRoutineName );
		writer->setSubKeySpaces( tmpSubKeySpaces );
	}

	if (writer != NULL || showDistribution )
	{
		// File length check

		int size = reader->getChainsLeft() * sizeof(RainbowChain);
		static CMemoryPool mp;
		uint64 nAllocatedSize;
		RainbowChain* pChain = (RainbowChain*)mp.Allocate(size, nAllocatedSize);			
		uint32 chainSize = (uint32)ceil((float)(sptl + eptl + checkPointBits) / 8) * 8; 
		if ( writer != NULL )
			writer->setChainSize( chainSize );

		if (pChain != NULL)
		{
			nAllocatedSize = nAllocatedSize / sizeof(RainbowChain) * sizeof(RainbowChain);
			// XXX safe for now...fix to use uint64 throughout
			unsigned int nChains = (uint32) nAllocatedSize / sizeof(RainbowChain);
			uint64 curPrefix = 0, prefixStart = 0;
			std::vector<IndexRow> indexes;
			unsigned int chainsLeft;

			while((chainsLeft = reader->getChainsLeft()) > 0)
			{
				printf("%u chains left to read\n", chainsLeft);
				//int nReadThisRound;
				clock_t t1 = clock();
				printf("reading...\n");
#ifdef _MEMORYDEBUG
				printf("Grabbing %i chains from file\n", nChains);
#endif
				reader->readChains(nChains, pChain);
#ifdef _MEMORYDEBUG
				printf("Recieved %i chains from file\n", nChains);
#endif
				clock_t t2 = clock();
				float fTime = 1.0f * (t2 - t1) / CLOCKS_PER_SEC;
				printf("reading time: %.2f s\n", fTime);		
				printf("converting %i chains...\n", nChains);
				t1 = clock();

				for(unsigned int i = 0; i < nChains; i++)
				{
					if( showDistribution )
						distribution[GetMaxBits(pChain[i].nIndexS)-1]++;
					else
					{
						uint64 chainrow = pChain[i].nIndexE;

						chainrow |= ( ((uint64)(pChain[i].nIndexS - reader->getMinimumStartPoint()) | startPointMask )) << startPointShift;

						// XXX check points go here

/*						if(hasCheckPoints == 1 && checkPointBits > 0)
						{
							chainrow |= (uint64)pChain[i].nCheckPoint << sptl + eptl;
						}*/
						
						if ( writer != NULL )
							writer->addDataChain( &chainrow );

						uint64 prefix = pChain[i].nIndexE >> eptl;

						if(i == 0)
							curPrefix = prefix;

						if ( prefix != curPrefix && (numProcessedChains - prefixStart) > 0)
						{
							if(prefix < curPrefix)
							{
								printf("**** Error writeChain(): Prefix is smaller than previous prefix. %llu < %llu****\n", prefix, curPrefix);
								exit(1);									
							}

							// XXX 32-bit = 32-bit - 64-bit
							unsigned int numchains = numProcessedChains - prefixStart;

							IndexRow index;
							index.prefix = curPrefix;
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
				if( showDistribution )
				{
					for(int i = 0; i < 64; i++)
						printf("%u - %u\n", (i+1), distribution[i]);

					delete reader;
					return;
				}
			}

			// We need to write the last index down
			IndexRow index;
			// XXX 32-bit = 64-bit
			index.prefix = curPrefix;
			// XXX 32-bit = 64-bit
			index.prefixstart = prefixStart;
			index.numchains = numProcessedChains - prefixStart;
			indexes.push_back(index);

			IndexRow high = {0}; // Used to find the highest numbers. This tells us how much we can pack the index bits
			for(uint32 i = 0; i < indexes.size(); i++)
			{
				if(indexes[i].numchains > high.numchains)
					high.numchains = indexes[i].numchains;
			}

			high.prefix = indexes[indexes.size()-1].prefix; // The last prefix is always the highest prefix

			// m_rti_index_numchainslength == index bit length "N"

			/*
			for(uint32 i = 0; i < rti_cppos.size(); i++) {
				fwrite(&rti_cppos[i], 1, 4, pFileIndex); // The position of the checkpoints
			}
			*/

			//int zero = 0;

			if ( writer != NULL )
			{
				writer->setPrefixStart( indexes[0].prefix );
				writer->setPrefixCount( indexes.size() );
			}

			unsigned int lastPrefix = 0;
			for(uint32 i = 0; i < indexes.size(); i++)
			{
				if(i == 0)
					lastPrefix = indexes[0].prefix;

				// Checks how big a distance there is between the current and the next prefix. eg cur is 3 and next is 10 = 7.
				unsigned int diffSize = indexes[i].prefix - lastPrefix; 
				if(i > 0 && diffSize > 1)
				{
					// then write the distance amount of 00's
					if(diffSize > 1000)
					{
						std::cout << "WARNING! The distance to the next prefix is "
							<< diffSize << "." << std::endl;
						std::cout << "Aborting..." << std::endl;
						exit(1);
					}

					/*
						for(uint32 j = 1; j < diffSize; j++)
						fwrite(&zero, 1, m_indexrowsizebytes, pFileIndex);
					*/
				}

				//writer->addIndexChain( indexes[i].numchains * chainSizeBytes );
				writer->addIndexChain( indexes[i].numchains );
				
				lastPrefix = indexes[i].prefix;
			}
		}
		else
			printf("memory allocation fail\n");

		if(reader != NULL)
			delete reader;

		if ( writer != NULL )
		{
			writer->writeHeader();
			writer->writeIndex();
			writer->writeData();
			delete writer;
		}
	}
}

static void usage()
{
	printf("converti2 - Original to Indexed rainbow table converter\n");
	printf("by Martin Westergaard <martinwj2005@gmail.com>\n");
	printf("http://www.freerainbowtables.com\n\n");

	printf("usage: converti2 rainbow_table_pathname\n");
	printf("rainbow_table_pathname: pathname of the rainbow table(s), wildchar(*, ?) supported\n");
	printf("\n");
	printf("example: converti2 *.rt[i]\n");
	printf("         converti2 md5_*.rt[i]\n");
	printf("list SP: converti2 -d *.rt[i]\n");
}

int main(int argc, char** argv)
{
	Converti2 *converti2;

	if (argc == 1)
	{
		usage();		
		return 0;
	}

	converti2 = new Converti2( argc, argv );
	if ( converti2->sharedSetup() != EXIT_SUCCESS )
	{
		printf( "fatal error in sharedSetup()\n" );
		return EXIT_FAILURE;
	}

	converti2->convertRainbowTables();

	printf("\n");

	return EXIT_SUCCESS;
}
