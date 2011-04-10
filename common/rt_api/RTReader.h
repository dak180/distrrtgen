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

#ifndef __RTREADER_H__
#define __RTREADER_H__

#include <string>
#ifdef WIN32
#include <io.h>
#endif
#include "Public.h"
#include "BaseRTReader.h"

//using namespace std;

class RTReader : BaseRTReader
{
private:
	FILE *m_pFile;
	unsigned int m_chainPosition;

public:
	RTReader( std::string Filename );
	~RTReader(void);
	int ReadChains(uint32 &numChains, RainbowChain *pData);
	uint32 GetChainsLeft();
	uint32 getChainLength();
};

#endif
