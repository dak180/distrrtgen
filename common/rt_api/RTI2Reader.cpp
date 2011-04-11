/*
 * freerainbowtables is a project for generating, distributing, and using
 * perfect rainbow tables
 *
 * Copyright 2010, 2011 Martin Westergaard Jørgensen <martinwj2005@gmail.com>
 * Copyright 2010, 2011 James Nobis <frt@quelrod.net>
 *
 * This file is part of freerainbowtables.
 *
 * freerainbowtables is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * freerainbowtables is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with freerainbowtables.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "BaseRTReader.h"
#include "RTI2Reader.h"

#include <math.h>

RTI2Reader::RTI2Reader( std::string filename )
{
	SubKeySpace subKeySpace;
	CharacterSet charSet;
	char *str;
	uint8 *indexTmp;
	uint32 a, b, c, subKeySpacesTmp = 0, hybridSets = 0, passwordLength = 0
		, count, sum = 0, chainSize;
	int ret;
	uint8 characterSetFlags;

	fin.open( filename.c_str(), std::ios_base::binary | std::ios_base::in );

	if( !fin.read( (char*) (&header), sizeof(header) ).good() )
	{
		std::cerr << "readHeader fin.read() error" << std::endl;
		exit( 1 ); // file error
	}

	if ( header.tag != 0x32495452 ) // RTI2
		exit( 3 ); // bad tag

	if ( header.minor != 0 )
	{
		std::cerr << "readHeader bad minor version" << std::endl;
		exit( 4 ); // bad minor version
	}

	if ( header.startPointBits == 0 || header.startPointBits > 64 )
	{
		std::cerr << "readHeader parsing error" << std::endl;
		std::cerr << "header.startPointBits: " << header.startPointBits << std::endl;
		exit( 2 ); // invalid header
	}

	if ( header.endPointBits == 0 || header.endPointBits > 64 )
	{
		std::cerr << "readHeader parsing error" << std::endl;
		std::cerr << "header.endPointBits: " << header.endPointBits << std::endl;
		exit( 2 ); // invalid header
	}

	if ( header.checkPointBits > 64 )
	{
		std::cerr << "readHeader parsing error" << std::endl;
		std::cerr << "header.checkPointBits: " << header.checkPointBits << std::endl;
		exit( 2 ); // invalid header
	}

	if ( header.startPointBits + header.checkPointBits + header.endPointBits
		> 64 )
	{
		std::cerr << "readHeader parsing error" << std::endl;
		std::cerr << "header.startPointBits + header.checkPointBits + header.endPointBits > 64" << std::endl;
		std::cerr << "header.startPointBits: " << header.startPointBits << std::endl;
		std::cerr << "header.endPointBits: " << header.endPointBits << std::endl;
		std::cerr << "header.checkPointBits: " << header.checkPointBits << std::endl;
		exit( 2 ); // invalid header
	}

	if ( header.fileIndex > header.files )
	{
		std::cerr << "readHeader parsing error" << std::endl;
		std::cerr << "fileIndex: " << header.fileIndex << std::endl;
		std::cerr << "files: " << header.files << std::endl;
		exit( 2 ); // invalid header
	}

	if ( header.algorithm > 19 )
	{
		std::cerr << "readHeader parsing error" << std::endl;
		std::cerr << "undefined algorithm: " << header.algorithm << std::endl;
		exit( 2 ); // invalid header
	}

	if (  header.reductionFunction > 3 
		|| (header.reductionFunction <  2 && header.tableIndex > 65535)
		|| (header.reductionFunction == 2 && header.tableIndex >   255))
	{
		std::cerr << "readHeader parsing error" << std::endl;
		std::cerr << "invalid reductionFunction parameters" << std::endl;
		std::cerr << "header.reductionFunction: " << header.reductionFunction << std::endl;
		std::cerr << "header.tableIndex: " << header.tableIndex << std::endl;
		exit( 2 ); // invalid header
	}

	if ( header.tableIndex != 0
		&&  ((header.reductionFunction < 2 && header.chainLength - 2 > header.tableIndex << 16)
			|| (header.reductionFunction == 3 && header.chainLength > header.tableIndex))) // I think this might be "header.chainLength - 2 > header.tableIndex" need to double check
	{
		// Sc00bz remarks "(these tables suck)"
		std::cerr << "WARNING: Table index is not large enough for this chain length"
			<< std::endl;
	}

	// XXX copies from header to setters or full RTI20_File struct?

	str = new char[1024];

	// Salt
	setSalt( "" );

	if ( header.algorithm == 0
		|| ( header.algorithm >= 15 && header.algorithm <= 19 ))
	{
		ret = readRTI2String( fin, str );

		if ( ret < 0 )
		{
			delete [] str;
			exit( -ret );
		}

		str[ret] = 0;
		setSalt( str );
	}
	
	// Sub keyspaces
	subKeySpaces.clear();

	if ( !fin.read( (char*) (&subKeySpacesTmp), 1 ).good() )
	{
		std::cerr << "readHeader fin.read() error" << std::endl;
		delete [] str;
		exit( 1 ); // file error
	}

	if  ( subKeySpacesTmp == 0 )
	{
		std::cerr << "readHeader fin.read() error" << std::endl;
		std::cerr << "subKeySpaces missing from header" << std::endl;
		delete [] str;
		exit( 2 ); // invalid header
	}

	for ( a = 0; a < subKeySpacesTmp; a++ )
	{
		subKeySpace.perPositionCharacterSets.clear();

		if ( !fin.read( (char*) (&hybridSets), 1 ).good() )
		{
			std::cerr << "readHeader fin.read() error" << std::endl;
			delete [] str;
			exit( 1 ); // file error
		}

		if ( hybridSets == 0 )
		{
			delete [] str;
			exit( 2 ); // invalid header
		}

		// Hybrid sets
		for ( b = 0; b < hybridSets; b++ )
		{
			// Password length
			if ( !fin.read( (char*) (&passwordLength), 1 ).good() )
			{
				std::cerr << "readHeader fin.read() error" << std::endl;
				delete [] str;
				exit( 1 ); // file error
			}

			if ( passwordLength == 0 )
			{
				delete [] str;
				exit( 2 ); //invalid header
			}

			// Character set flags
			if ( !fin.read( (char*) (&characterSetFlags), 1 ).good() )
			{
				std::cerr << "readHeader fin.read() error" << std::endl;
				delete [] str;
				exit( 1 ); // file error
			}

			if ( characterSetFlags == 0 )
			{
				delete [] str;
				exit( 2 ); // invalid header
			}

			// Character set
			charSet.characterSet1.clear();
			charSet.characterSet2.clear();
			charSet.characterSet3.clear();
			charSet.characterSet4.clear();

			if ( characterSetFlags & 1 )
			{
				ret = readRTI2String( fin, str, 1 );
				if ( ret < 0 )
				{
					delete [] str;
					exit( -ret );
				}

				charSet.characterSet1.assign((uint8*) str, ((uint8*) str) + ret);
			}

			if ( characterSetFlags & 2 )
			{
				ret = readRTI2String( fin, str, 2 );

				if ( ret < 0 )
				{
					delete [] str;
					exit( -ret );
				}

				charSet.characterSet2.assign((uint16*) str, ((uint16*) str) + ret);
			}

			if ( characterSetFlags & 4 )
			{
				ret = readRTI2String( fin, str, 3 );

				if ( ret < 0 )
				{
					delete [] str;
					exit( -ret );
				}

				charSet.characterSet3.assign((uint24*) str, ((uint24*) str) + ret);
			}

			if ( characterSetFlags & 8 )
			{
				ret = readRTI2String( fin, str, 4 );

				if ( ret < 0 )
				{
					delete [] str;
					exit( -ret );
				}

				charSet.characterSet4.assign((uint32*) str, ((uint32*) str) + ret);
			}

			for ( c = 0; c < passwordLength; c++ )
			{
				subKeySpace.perPositionCharacterSets.push_back(charSet);
			}
		}
	}

	subKeySpaces.push_back(subKeySpace);

	// Check point positions
	if ( !fin.read( str, 4 * header.checkPointBits ).good() )
	{
		std::cerr << "readHeader fin.read() error" << std::endl;
		delete [] str;
		exit( 1 ); // file error
	}

	checkPointPositions.assign((uint32*) str, ((uint32*) str) + header.checkPointBits);
	delete [] str;

	// *** Index ***
	if ( !fin.read( (char*) (index.firstPrefix), 8 ).good() )
	{
		std::cerr << "readIndex fin.read() error" << std::endl;
		exit( 1 ); // file error
	}

	if ( !fin.read((char*) (&count), 4 ).good() )
	{
		std::cerr << "readIndex fin.read() error" << std::endl;
		exit( 1 ); // file error
	}

	if ( count == 0 )
	{
		exit( 1 ); // file error
	}

	indexTmp = new uint8[count];

	if ( !fin.read( (char*) indexTmp, count ).good() )
	{
		std::cerr << "readIndex fin.read() error" << std::endl;
		delete [] indexTmp;
		exit( 1 ); // file error
	}

	index.prefixIndex.reserve(count + 1);
	index.prefixIndex.push_back(sum);

	for (a = 0; a < count; a++)
	{
		sum += indexTmp[a];
		index.prefixIndex.push_back(sum);
		count--;
	}

	delete [] indexTmp;

	// *** Data ***
	chainSize = (header.startPointBits + header.checkPointBits + header.endPointBits + 7) >> 3;
	data = new uint8[chainSize * sum + 8 - chainSize]; // (8 - chainSize) to avoid "reading past the end of the array" error

	if ( !fin.read( (char*) (data), chainSize * sum ).good() )
	{
		std::cerr << "readIndex fin.read() error" << std::endl;
		delete [] data;
		data = NULL;
		exit( 1 ); // file error
	}
}

RTI2Reader::~RTI2Reader(void)
{
	if(m_pIndex != NULL) delete m_pIndex;
	if(m_pFile != NULL) fclose(m_pFile);

}

int RTI2Reader::readRTI2String( std::ifstream &fin, void *str, uint32 charSize )
{
/*
 * String length (1 byte)
 * String (String length bytes or (charSize * (String length + 1)) bytes)
*/
	uint32 size = 0;
	int ret = 0;

	if ( !fin.read( (char*) (&ret), 1 ).good() )
	{
		std::cerr << "readHeader find.read() error" << std::endl;
		return -1; // file error
	}

	if ( charSize )
	{
		ret++;
		size = charSize * ret;
	}

	if ( size > 0 )
	{
		if ( !fin.read( (char*) str, size ).good() )
		{
			std::cerr << "readHeader fin.read() error" << std::endl;
			return -1; //file error
		}
	}

	return ret;
}

uint32 RTI2Reader::GetChainsLeft()
{
	/*  XXX
	long len = GetFileLen(m_pFile);
	return len / m_chainsizebytes - m_chainPosition;
	*/

	return 0;
}

int RTI2Reader::ReadChains(unsigned int &numChains, RainbowChain *pData)
{
	return 0;
}
