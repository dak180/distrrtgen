/*
 * freerainbowtables is a project for generating, distributing, and using
 * perfect rainbow tables
 *
 * Copyright 2010, 2011 Martin Westergaard Jørgensen <martinwj2005@gmail.com>
 * Copyright 2010, 2011 James Nobis <quel@quelrod.net>
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

#ifndef _BASERTWRITER_H
#define _BASERTWRITER_H

#include <string>

#if defined(_WIN32) && !defined(__GNUC__)
	#include <io.h>
#endif

#include "Public.h"
#include "RTI2Common.h"

class BaseRTWriter
{
protected:
	std::string hashAlgorithm;
	std::string salt;

public:
	virtual std::string getAlgorithm();
	virtual std::string getSalt();

	virtual int addIndexChain( uint32 chain );
	virtual void addDataChain( uint64 *chain ) = 0;

	virtual void Dump();

	virtual void setAlgorithm( std::string hashRoutineName );
	virtual void setChainCount( uint32 chains ) = 0;
	virtual void setChainLength( uint32 chainLen ) = 0;
	virtual void setChainSize( uint32 chainSize ) = 0;
	virtual void setCheckPointLen( unsigned char checkPointLen ) = 0;
	virtual void setCheckPointPos( std::vector<unsigned int> checkPointPos ) = 0;
	virtual void setEndPointLen( unsigned char endPointLen ) = 0;
	virtual void setFileCount( uint32 fileCount );
	virtual void setFileIndex( uint32 fileIndex ) = 0;
	//virtual void setIndexNumberChainsLen( unsigned char indexNumberChainsLen ) = 0;
	virtual int setMinimumStartPoint( uint64 tmpMinimumStartPoint );
	virtual void setPrefixCount( uint32 prefixIndexCount ) = 0;
	virtual void setPrefixStart( uint64 prefixStart ) = 0;
	virtual void setSalt( std::string salt );
	virtual void setStartPointLen( unsigned char startPointLen ) = 0;
	virtual int setSubKeySpaces( std::vector<SubKeySpace> tmpSubKeySpaces );
	virtual void setTableIndex( uint32 tableIndex ) = 0;

   virtual int writeHeader();
	virtual int writeChains(uint32 &numChains, RainbowChain *pData) = 0;
	virtual int writeData();
	virtual int writeIndex();
	
	virtual ~BaseRTWriter()  { };
};

#endif
