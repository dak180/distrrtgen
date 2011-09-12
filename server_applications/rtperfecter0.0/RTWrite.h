/*
 * Copyright (C) 2008 Steve Thomas <SMT837784@yahoo.com>
 * Copyright 2011 James Nobis <quel@quelrod.net>
 * Copyright 2011 Andrew Dranse <aramith@aramith.org>
 *
 * This file is part of RT Perfecter v0.0.
 *
 * RT Perfecter v0.0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RT Perfecter v0.0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RT Perfecter v0.0.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RTWRITE_H
#define RTWRITE_H

#include "RTCommon.h"

class RTWrite
{
public:
	 RTWrite(char *fileName, int maxChainsPerFile);
	~RTWrite();

	void writeChain(RTChain *c);

private:
	uint64 m_prevEndpt;
	unsigned int m_curFile, m_curFileChains, m_chainsPerFile;
	char *m_file, *m_fileTemp;
	FILE *m_pFile;
	int m_verbose;
};

#endif

