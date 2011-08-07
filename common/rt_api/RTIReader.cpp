/*
 * freerainbowtables is a project for generating, distributing, and using
 * perfect rainbow tables
 *
 * Copyright 2010, 2011 Martin Westergaard Jørgensen <martinwj2005@gmail.com>
 * Copyright 2010, 2011 James Nobis <quel@quelrod.net>
 * Copyright 2011 Logan Watt <logan.watt@gmail.com>
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

#include "RTIReader.h"

/// Default constructor
RTIReader::RTIReader()
{
	this->chainSize = 8;
}


/** Constructor with filename
 * @param std::string file name on disk
 * index file is std::string + '.index'
 */
RTIReader::RTIReader( std::string fname )
{
	this->chainSize = 8;
	this->indexFileName = fname + ".index";
	filename = fname;

	if( stat( filename.c_str(), &fileStats ) == -1 )
	{
		std::cerr << "ERROR: stat() for file: " << filename.c_str() << " FAILED! " << std::endl;	
		exit(-1);
	}

	if( stat( indexFileName.c_str(), &indexFileStats ) == -1 )
	{
		std::cerr << "ERROR: stat() for file: " << indexFileName.c_str() << " FAILED! " << std::endl;
		exit(-1);
	}

	indexFileData = fopen( indexFileName.c_str(), "rb");
	if( indexFileData == NULL )
	{
		std::cerr << "ERROR: could not open index file: " << indexFileName.c_str() << " EXITING!" << std::endl;
		exit(-1);
	}
}

/**
 * Argument Constructor
 * @param uint32 number of chains in the file
 * @param uint32 size of the chain
 * @param uint32 reduction function index offset
 * @param uint32 start point in the chain
 * @param uint32 end point in the chain
 * @param std::string name of the file on disk
 * @param std::string salt used for hash
 */
RTIReader::RTIReader(uint32 chCount, uint32 chLength, uint32 tblIdx, uint32 stPt, uint32 endPt, std::string fname, std::string slt)
{
	RTIReader( fname );
	BaseRTReader(chCount, chLength, tblIdx, stPt, endPt, fname, slt);
}

/// getChainSize
uint32 RTIReader::getChainSize()
{
	return this->chainSize();
}


/**
 * reads data chains into memory
 * @param uint32 reference to the number of chains to read
 * @param RanbowChain0* pointer in memory to read the chains to
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int RTIReader::readChains(uint32 &numChains, RainbowChainO *pData)
{
}

uint32 RTIReader::getChainsLeft()
{
	return ( fileStats.st_size / chainSize ) - chainPosition;
}

uint64 RTIReader::getMinimumStartPoint()
{
	uint64 tmpStartPoint = 0;
	uint64 minimumStartPoint = (uint64)-1;
	uint16 tmpEndPoint;
	long originalFilePos = ftell( data );

	rewind( data );

	while ( !feof( data ) )
	{
		fread( &tmpStartPoint, 6, 1, data );
		fread( &tmpEndPoint, 2, 1, data );

		if ( tmpStartPoint < minimumStartPoint )
			minimumStartPoint = tmpStartPoint;
	}

	fseek( dataFile, originalFilePos, SEEK_SET );
	return minimumStartPoint;

}

void RTIReader::Dump()
{
}

/// Destructor
RTIReader::~RTIReader()
{
}

