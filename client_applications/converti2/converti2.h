/*
* converti2 is a tool to convert from RT and RTI to RTI2
*
* Copyright 2011 James Nobis <quel@quelrod.net>
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

#ifndef _CONVERTI2_H
#define _CONVERTI2_H

#include <string>
#include <vector>

#if defined(_WIN32) && !defined(__GNUC__)
	#include <io.h>
	#include <conio.h>
#else
	#include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sstream>
#include <iostream>
#include "Public.h"
#include "MemoryPool.h"
#include "RTIReader.h"
#include "RTReader.h"
#include "RTI2Writer.h"

class Converti2
{
private:
	void setupSharedData();
	uint8 sptl;
	uint8 eptl;
	bool showDistribution;
	bool hasCheckPoints;
	int argc;
	char** argv;
	int argi;
	int argsUsed;
	int i;
	int checkPointBits;
	std::string pathName;
	std::vector<unsigned int> cppositions;
	std::vector<std::string> vPathName;

public:
	Converti2( int argc, char** argv );

	int GetMaxBits( uint64 highvalue );

#ifdef _WIN32
	void GetTableList( std::string sWildCharPathName );
#else
	void GetTableList();
#endif

	int sharedSetup();

	void convertRainbowTable( std::string sResultFileName, uint32 files );
	void convertRainbowTables();
	bool shouldShowDistribution();
	void doShowDistribution();
};
	
static void usage();

#endif
