/*
 * freerainbowtables is a project for generating, distributing, and using
 * perfect rainbow tables
 *
 * Copyright 2011 James Nobis <frt@quelrod.net>
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

#include "BaseRTWriter.h"
#include "RTI2Writer.h"

#include <math.h>

RTI2Writer::RTI2Writer( std::string filename )
{
	if ( (pFile = fopen(filename.c_str(), "wb")) == NULL )
	{
		printf("Unable to open file %s\n", filename.c_str());
		exit(1);
	}

	out.header.major = 2;
	out.header.minor = 0;
	out.header.rtParams.reductionFunction = 0;
	out.header.rtParams.salt = "";
	out.header.rtParams.subKeySpaces.clear();
	out.data = NULL;
	dataProc = NULL;
	dataPos = NULL;
}

RTI2Writer::~RTI2Writer()
{
	if(pFile != NULL)
		fclose(pFile);

	if ( dataProc != NULL )
		delete [] dataProc;
}

void RTI2Writer::setAlgorithm( std::string hashRoutineName )
{
	out.header.rtParams.algorithm = getAlgorithmId( hashRoutineName );
}

void RTI2Writer::setChainLength( uint32 chainLen )
{
	out.header.rtParams.chainLength = chainLen;
}

void RTI2Writer::setChainSize( uint32 chainSize )
{
	this->chainSize = chainSize;
	chainSizeBytes = chainSize / 8;

	// XXX clean this up
	// (8 - chainSize) to avoid reading past the end of the array
	dataProc = new (std::nothrow) uint8[chainSizeBytes * chainCount + 8 - chainSizeBytes];
	if ( dataProc == NULL )
	{
		std::cout << "You do not have enough free memory to run this application"
			<< std::endl;
		exit( 2 );
	}

	dataPos = dataProc;
}

void RTI2Writer::setCheckPointLen( uint8 checkPointLen )
{
	out.header.checkPointBits = checkPointLen;
}

void RTI2Writer::setCheckPointPos( std::vector<uint32> checkPointPos )
{
	out.header.rtParams.checkPointPositions = checkPointPos;
}

void RTI2Writer::setEndPointLen( uint8 endPointLen )
{
	out.header.endPointBits = endPointLen;
}

void RTI2Writer::setFileCount( uint32 fileCount )
{
	out.header.files = fileCount;
}

void RTI2Writer::setFileIndex( uint32 fileIndex )
{
	out.header.fileIndex = fileIndex;
}

int RTI2Writer::setMinimumStartPoint( uint64 tmpMinimumStartPoint )
{
	out.header.rtParams.minimumStartPoint = tmpMinimumStartPoint;

	return 0;
}

void RTI2Writer::setPrefixCount( uint32 prefixIndexCount )
{
	prefixCount = prefixIndexCount;

	out.index.prefixIndex.reserve( prefixCount );
}

void RTI2Writer::setPrefixStart( uint64 prefixStart )
{
	out.index.firstPrefix = prefixStart;
}

void RTI2Writer::setSalt( std::string salt )
{
	BaseRTWriter::setSalt( salt );
	out.header.rtParams.salt = salt; // XXX need to set this
}

void RTI2Writer::setStartPointLen( uint8 startPointLen )
{
	out.header.startPointBits = startPointLen;
}

int RTI2Writer::setSubKeySpaces( std::vector<SubKeySpace> tmpSubKeySpaces )
{
	out.header.rtParams.subKeySpaces.assign( tmpSubKeySpaces.begin(), tmpSubKeySpaces.end() );

	return 0;
}

void RTI2Writer::setTableIndex( uint32 tableIndex )
{
	out.header.rtParams.tableIndex = tableIndex;
}

void RTI2Writer::setChainCount( uint32 chains )
{
	chainCount = chains;
}

void RTI2Writer::addDataChain( uint64 *chain )
{
	for ( uint8 i = 0; i < chainSizeBytes; i++ )
	{
		*dataPos = *(((uint8 *)(chain)) + i);
		dataPos++;
	}
}

int RTI2Writer::addIndexChainCount( uint8 count )
{
	prefixIndexChainCount.push_back( count );

	return EXIT_SUCCESS;
}

void RTI2Writer::Dump()
{


}

int RTI2Writer::writeChains(unsigned int &numChains, RainbowChainO *pData)
{
	return 0;
}

int RTI2Writer::writeHeader()
{
	fwrite( "RTI2", 1, HEADER_LEN, pFile );
	fwrite( &out.header.minor, 1, sizeof(out.header.minor), pFile );
	fwrite( &out.header.startPointBits, 1, 1, pFile );
	fwrite( &out.header.endPointBits, 1, 1, pFile );
	fwrite( &out.header.checkPointBits, 1, 1, pFile );
	fwrite( &out.header.fileIndex, 1, sizeof( out.header.fileIndex ), pFile );
	fwrite( &out.header.files, 1, sizeof( out.header.files ), pFile );

	fwrite( &out.header.rtParams.minimumStartPoint, 1, 8, pFile );
	fwrite( &out.header.rtParams.chainLength, 1
		, sizeof(out.header.rtParams.chainLength), pFile );
	fwrite( &out.header.rtParams.tableIndex, 1
		, sizeof(out.header.rtParams.tableIndex), pFile );
	fwrite( &out.header.rtParams.algorithm, 1
		, sizeof( out.header.rtParams.algorithm ), pFile );
	fwrite( &out.header.rtParams.reductionFunction, 1
		, sizeof( out.header.rtParams.reductionFunction ), pFile );
	
	uint8 tmpUint8;

	// XXX centralize the check for algorithm has salt
	if ( out.header.rtParams.algorithm == 0 ||
		( out.header.rtParams.algorithm >= 15 
		  && out.header.rtParams.algorithm <= 19 )
		)
	{
		tmpUint8 = sizeof( char ) * out.header.rtParams.salt.length();
		fwrite( &tmpUint8, 1, 1, pFile );

		if ( tmpUint8 > 0 )
			fwrite( out.header.rtParams.salt.c_str(), 1, tmpUint8, pFile );
	}

	// subKeySpaces
	
	tmpUint8 = out.header.rtParams.subKeySpaces.size();
	
	fwrite( &tmpUint8, sizeof( uint8 ), 1, pFile );

	CharacterSet charSet;
	
	for ( uint32 i = 0; i < out.header.rtParams.subKeySpaces.size(); i++ )
	{
		SubKeySpace tmpSubKeySpace = out.header.rtParams.subKeySpaces[i];

		fwrite( &tmpSubKeySpace.hybridSets, 1, 1, pFile );

		for ( uint32 j = 0; j < tmpSubKeySpace.hybridSets; j++ )
		{
			fwrite( &tmpSubKeySpace.passwordLength[j], 1, 1, pFile );
			fwrite( &tmpSubKeySpace.charSetFlags[j], 1, 1, pFile );

			charSet = tmpSubKeySpace.perPositionCharacterSets[j];

			if ( tmpSubKeySpace.charSetFlags[j] & 1 )
			{
				tmpUint8 = charSet.characterSet1.size() - 1;
				fwrite( &tmpUint8, 1, sizeof( uint8 ), pFile );

				++tmpUint8;
				for ( uint32 k = 0; k < tmpUint8; k++ )
				{
					fwrite( &charSet.characterSet1[k], 1, sizeof( uint8 )
					, pFile );
				}
			}
			// XXX ???
			if ( tmpSubKeySpace.charSetFlags[j] & 2 )
			{
				tmpUint8 = ( charSet.characterSet2.size() - 1 ) / 2;
				fwrite( &tmpUint8, 1, sizeof( uint8 ), pFile );

				uint16 tmpUint16 = charSet.characterSet2.size();
				for ( uint32 k = 0; k < tmpUint16; k++ )
				{
					fwrite( &charSet.characterSet2[k], 1, sizeof( uint16 )
					, pFile );
				}
			}
			// XXX ???
			if ( tmpSubKeySpace.charSetFlags[j] & 4 )
			{
				tmpUint8 = ( charSet.characterSet3.size() - 1 ) / 3;
				fwrite( &tmpUint8, 1, sizeof( uint8 ), pFile );

				uint32 tmpUint32 = charSet.characterSet3.size();
				for ( uint32 k = 0; k < tmpUint32; k++ )
				{
					fwrite( &charSet.characterSet3, 1, sizeof( uint24 )
					, pFile );
				}
			}
			// XXX ???
			if ( tmpSubKeySpace.charSetFlags[j] & 8 )
			{
				tmpUint8 = (charSet.characterSet4.size() - 1) / 4;
				fwrite( &tmpUint8, 1, sizeof( uint8 ), pFile );

				uint32 tmpUint32 = charSet.characterSet4.size();
				for ( uint32 k = 0; k < tmpUint32; k++ )
				{
					fwrite( &charSet.characterSet4, 1, sizeof( uint32 )
					, pFile );
				}
			}
		}
	}

	for ( uint32 i = 0; i < out.header.rtParams.checkPointPositions.size(); i++ )
		fwrite( &out.header.rtParams.checkPointPositions[i], 1, 4, pFile );

	return EXIT_SUCCESS;
}

int RTI2Writer::writeData()
{
	uint8 *dataW;

	dataW = dataProc;

	while ( dataW != NULL && dataW != dataPos )
	{
		fwrite( dataW, 1, 1, pFile );
		dataW++;
	}

	return EXIT_SUCCESS;
}

int RTI2Writer::writeIndex()
{
	fwrite( &out.index.firstPrefix, 1, sizeof( out.index.firstPrefix ), pFile );
	fwrite( &prefixCount, 1, sizeof( prefixCount ), pFile );

	for( uint32 i = 0; i < prefixIndexChainCount.size(); i++)
	{
		fwrite( &prefixIndexChainCount[i], 1, sizeof( uint8 ), pFile );
	}

	return EXIT_SUCCESS;
}
