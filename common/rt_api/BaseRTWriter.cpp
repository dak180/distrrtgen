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

//using namespace std;

int BaseRTWriter::addIndexChain( uint32 chain )
{
	return EXIT_SUCCESS;
}

std::string BaseRTWriter::getAlgorithm()
{
	return hashAlgorithm;
}

std::string BaseRTWriter::getSalt()
{
	return salt;
}

void BaseRTWriter::Dump()
{

}

void BaseRTWriter::setAlgorithm( std::string hashRoutineName )
{
	hashAlgorithm = hashRoutineName;
}

void BaseRTWriter::setFileCount( uint32 fileCount )
{

}

void BaseRTWriter::setSalt( std::string salt )
{
	this->salt = salt;
}

int BaseRTWriter::setSubKeySpaces( std::vector<SubKeySpace> tmpSubKeySpaces )
{

	return 0;
}

int BaseRTWriter::setMinimumStartPoint( uint64 tmpMinimumStartPoint )
{

	return 0;
}

int BaseRTWriter::writeData()
{
	return EXIT_SUCCESS;
}

int BaseRTWriter::writeHeader()
{
	return EXIT_SUCCESS;
}

int BaseRTWriter::writeIndex()
{
	return EXIT_SUCCESS;
}
