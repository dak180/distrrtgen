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

#ifndef _RTIREADER_H
#define _RTIREADER_H

#include "BaseRTReader.h"

class RTIReader : public BaseRTReader
{
	private:
		uint32 chainSize;
		std::string indexFileName;
		FILE *indexFileData;
		struct stat fileStats;
		struct stat indexFileStats;
	//unsigned int m_nIndexSize;
	//IndexChain *m_pIndex;

	protected:
		/// Set methods
		void setChainSize();
		void setIndexFileName();

	public:
		/// Default Constructor
		RTIReader();

		/** Constructor with filename
		 * @param std::string file name on disk
		 * index file is std::string + '.index'
		 */
		RTIReader( std::string filename );

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
		RTIReader(uint32, uint32, uint32, uint32, uint32, std::string, std::string);

		/// Destructor
		~RTIReader();

		/// Get Methods
		std::string getIndexFileName();
		uint32 getChainSize();

		int readChains(uint32 &numChains, RainbowChainO *pData);
		uint32 getChainsLeft();
		uint64 getMinimumStartPoint();

		void Dump();
};

#endif
