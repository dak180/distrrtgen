/*
 * freerainbowtables is a project for generating, distributing, and using
 * perfect rainbow tables
 *
 * Copyright 2011 James Nobis <quel@quelrod.net>
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

#ifndef _RTI2WRITER_H
#define _RTI2WRITER_H

#include "Public.h"
#include <string>
#include <iostream>

#if defined(_WIN32) && !defined(__GNUC__)
	#include <io.h>
#endif

#include <vector>
#include "BaseRTWriter.h"
#include "RTI2Common.h"

#define HEADER_LEN 4

class RTI2Writer : BaseRTWriter
{
private:
	FILE *pFile;
	uint32 chainPosition;
	uint8 *dataProc,*dataPos;
	RTI20_File out;
	uint8 *pIndex;
	uint32 chainSize;
	uint32 chainSizeBytes;
	uint32 chainCount;
	uint32 prefixCount;
	std::vector<uint8> prefixIndexChainCount;
	
public:
	RTI2Writer( std::string filename );
	~RTI2Writer(void);

	void setAlgorithm( std::string hashRoutineName );
	void setChainCount( uint32 chains );
	void setChainLength( uint32 chainLen );
	void setChainSize( uint32 chainSize );
	void setCheckPointLen( uint8 checkPointLen );
	void setCheckPointPos( std::vector<uint32> checkPointPos );
	void setEndPointLen( uint8 endPointLen );
	void setFileIndex( uint32 fileIndex );
	void setFileCount( uint32 fileCount );
	//void setIndexNumberChainsLen( uint8 indexNumberChainsLen );
	int setMinimumStartPoint( uint64 tmpMinimumStartPoint );
	void setPrefixCount( uint32 prefixIndexCount );
	void setPrefixStart( uint64 prefixStart );
	void setSalt( std::string salt );
	void setStartPointLen( uint8 startPointLen );
	int setSubKeySpaces( std::vector<SubKeySpace> tmpSubKeySpaces );
	void setTableIndex( uint32 tableIndex );

	void addDataChain( uint64 *chain );
	int addIndexChainCount( uint8 count );

	void Dump();

	int writeChains(uint32 &numChains, RainbowChainO *pData);
	int writeHeader();
	int writeData();
	int writeIndex();
};

#endif
