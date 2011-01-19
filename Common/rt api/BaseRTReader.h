/*
 * freerainbowtables is a project for generating, distributing, and using
 * perfect rainbow tables
 *
 * Copyright 2010, 2011 Martin Westergaard Jørgensen <martinwj2005@gmail.com>
 * Copyright 2010 Daniël Niggebrugge <niggebrugge@fox-it.com>
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

#ifndef __BASERTREADER_H__
#define __BASERTREADER_H__

#include "Public.h"
#include <string>

#if defined(_WIN32) && !defined(__GNUC__)
	#include <io.h>
#endif

using namespace std;

class BaseRTReader
{
public:
	virtual int ReadChains(uint32 &numChains, RainbowChain *pData) = 0;
	virtual uint32 GetChainsLeft() = 0;
	
	virtual ~BaseRTReader()  { };
};

#endif
