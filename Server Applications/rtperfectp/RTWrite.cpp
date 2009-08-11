/*
	Copyright (C) 2008 Steve Thomas <SMT837784@yahoo.com>

	This file is part of RT Perfecter v0.0.

	RT Perfecter v0.0 is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	RT Perfecter v0.0 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with RT Perfecter v0.0.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "RTWrite.h"

RTWrite::RTWrite(char *fileName, int maxChainsPerFile)
{
	unsigned int len = strlen(fileName);

	m_file = new char[len + 1];
	m_fileTemp = new char[len + 21];
	strncpy(m_file, fileName, len);
	m_file[len] = '\0';

	m_curFile = 0;
	m_curFileChains = 0;
	m_chainsPerFile = maxChainsPerFile;
	m_pFile = NULL;
	m_prevEndpt = 0;
}

RTWrite::~RTWrite()
{
	if (m_pFile != NULL)
	{
		fclose(m_pFile);
		sprintf(m_fileTemp, m_file, m_curFileChains, m_curFile);
		if (rename("temp.rt", m_fileTemp) != 0)
		{
			perror(m_fileTemp);
			exit(1);
		}
	}
	if (m_file != NULL)
	{
		delete [] m_file;
	}
	if (m_fileTemp != NULL)
	{
		delete [] m_fileTemp;
	}
}

void RTWrite::writeChain(RTChainCP *chain)
{
	if(chain->startpt == 0 || chain->endpt == 0)
		return;

	if (m_prevEndpt > chain->endpt)
	{
		printf("**** Error writeChain(): Tring to write unsorted data. (%llu > %llu)****\n", m_prevEndpt, chain->endpt);
		exit(1);
	}
	/*
#ifdef _WIN32
	if(chain->startpt > 0x0000ffffffffffffI64)
#else
	if(chain->startpt > 0x0000ffffffffffff11u)
#endif
	{
		printf("**** Error writeChain(): Prefix is bigger than 6 bytes. (%llx) %u chains ****\n", chain->startpt, m_curFileChains);
		exit(1);										
		return;
	}
	*/
	if (m_pFile == NULL)
	{
		m_pFile = fopen("temp.rt", "wb");
		if (m_pFile == NULL)
		{
			perror(m_fileTemp);
			exit(1);
		}
	}
	if (fwrite((void*)chain, 16, 1, m_pFile) != 1)
	{
		perror("temp.rt");
		exit(1);
	}
	if (fwrite((void*)&chain->checkpoint, 2, 1, m_pFile) != 1)
	{
		perror("temp.rt");
		exit(1);
	}

	m_curFileChains++;
	if (m_curFileChains >= m_chainsPerFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
		sprintf(m_fileTemp, m_file, m_curFileChains, m_curFile);
		if (rename("temp.rt", m_fileTemp) != 0)
		{
			perror(m_fileTemp);
			exit(1);
		}
		m_curFile++;
		m_curFileChains = 0;
	}
}

