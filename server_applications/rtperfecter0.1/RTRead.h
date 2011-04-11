/*
	Copyright (C) 2008 Steve Thomas <SMT837784@yahoo.com>

	This file is part of RT Perfecter v0.1.

	RT Perfecter v0.1 is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	RT Perfecter v0.1 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with RT Perfecter v0.1.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RTREAD_H
#define RTREAD_H

#include <sys/types.h>
#include "RTCommon.h"

struct FileList
{
	char *name;
	unsigned int chains;
	FILE *pFile;
	FileList *next;
};

struct PriorityQueueNode
{
	RTChain chain;
	FILE *pFile;
	char *fName;
};

class RTRead
{
public:
	 RTRead(char *dir, uint64 maxIndex, int verbose);
	~RTRead();

	int readChain(RTChain *c);
	void printStatus();

private:
	int getInfo(char *file, int len, FileList *ret);
	int readGoodChain(FILE *pFile, RTChain *chain);

	uint64 m_chains, m_chainsRead, m_maxIndex;
	PriorityQueueNode *m_pq;
	clock_t m_startTime;
	unsigned int m_pqSize;
	int m_verbose;
};

#endif

