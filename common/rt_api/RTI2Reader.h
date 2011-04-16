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

#ifndef __RTI2READER_H__
#define __RTI2READER_H__

#include "Public.h"
#include <string>

#if defined(_WIN32) && !defined(__GNUC__)
	#include <io.h>
#endif

#include <fstream>
#include <iostream>
#include <vector>
#include "BaseRTReader.h"
#include "RTI2Common.h"

//using namespace std;

class RTI2Reader : BaseRTReader
{
private:
	std::ifstream fin;
	uint32 chainPosition;
	uint8 *dataPos,*indexPos;
	RTI20_File in;
	RTI20_File_Header header;
	RTI20_Index index;
	uint8 *data;
	uint32 chainSizeBytes;
	uint32 m_indexrowsizebytes;
	uint32 numChains;
	std::vector<SubKeySpace> subKeySpaces;
	std::vector<uint32> checkPointPositions;
	
	int readRTI2String( std::ifstream &fin, void *str, uint32 charSize = 0 );

public:
	RTI2Reader( std::string filename );
	~RTI2Reader(void);
	int ReadChains(uint32 &numChains, RainbowChain *pData);
	uint32 GetChainsLeft();

	void Dump();
};


#endif
