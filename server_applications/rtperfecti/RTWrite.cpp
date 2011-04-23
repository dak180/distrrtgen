/*
	Copyright (C) 2008 Steve Thomas <SMT837784@yahoo.com>
	Copyright 2011 James Nobis <quel@quelrod.net>

	This file is part of RT Perfecter.

	RT Perfecter is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	RT Perfecter is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with RT Perfecter.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "RTWrite.h"
#include <string>

RTWrite::RTWrite(char *fileName, int maxChainsPerFile)
{
	unsigned int len = strlen(fileName);

	m_file = new char[len + 1];
	m_fileTemp = new char[len + 21];
#ifdef VERIFYCHAINS
	int nChainCount;
	CChainWalkContext::SetupWithPathName(fileName, m_nRainbowChainLen, nChainCount);
#endif
	strncpy(m_file, fileName, len);
	m_file[len] = '\0';

	m_curFile = 0;
	m_curFileChains = 0;
	m_chainsPerFile = maxChainsPerFile;
	m_pFile = NULL;
	m_prevEndpt = 0;
	m_curPrefix = 0;
	m_prefixStart = 0;
	m_numIndexRows = 0;
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
		printf("writing index before closing file...\n");
		printf("Covering %u - %u\n", m_prefixStart, (m_prefixStart + m_curFileChains - m_prefixStart));
		unsigned char index[11] = {0}; // [0 - 10]
		// The 5 byte prefix of the endingpoint [0 - 4]
		uint64 *prefix = (uint64 *)index;
		*prefix = m_curPrefix;	
		printf("value is %02x%02x%02x%02x%02x%02x%02x%02x (%llu)\n", index[0], index[1], index[2], index[3], index[4], index[5], index[6], index[7], m_curPrefix);
		int *iprefix;
		// The 4 bytes index where to look in the file for the chains [5 - 8]
		iprefix = (int*)&index[5];
		*iprefix = m_prefixStart;
		short *iprefixs = (short*)&index[9]; // The number of chains (2 bytes, max 65535 chains) with this prefix [9 - 10]
		*iprefixs = (short)(m_curFileChains - m_prefixStart);
		fwrite(&index, 1, 11, m_pFileIndex); // Position of these chains is: First chain: FILESTART + (iprefix * 8)
											 //								 Last chain:  FILESTART + (iprefix * 8) + (iprefixs * 8)
		
		std::string sIndex(m_fileTemp);
		sIndex.append(".index");
		if (rename("temp.rt.index", sIndex.c_str()) != 0)
		{
			perror(sIndex.c_str());
			exit(1);
		}
/*
		if(m_pFileCont != NULL)
			fclose(m_pFileCont);
			*/
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

void RTWrite::writeChain(RTChain *chain)
{
	bool bIndexWritten = false;
	if (m_pFile == NULL)
	{
		m_pFile = fopen("temp.rt", "wb");
		if (m_pFile == NULL)
		{
			perror(m_fileTemp);
			exit(1);
		}
		m_pFileIndex = fopen("temp.rt.index", "wb");
		if (m_pFileIndex == NULL)
		{
			perror(m_fileTemp);
			exit(1);
		}
		/*
		m_pFileCont = fopen("temp.rt.cont", "wb");
		if (m_pFileCont == NULL)
		{
			perror(m_fileTemp);
			exit(1);
		}*/
		
	}
//							int cp = 0;
//							if (cwc.GetIndex() == chain->endpt)
//							{
								if(chain->startpt == 0)
									return;
								if (m_prevEndpt >= chain->endpt && chain->endpt != 0)
								{
									fprintf(stderr, "**** Error writeChain() in %i: Tring to write unsorted data. %llu >= %llu****\n", m_curFileChains, m_prevEndpt, chain->endpt);
//									exit(1);
									return;
								}
#ifdef _WIN32
								if(chain->startpt > 0x0000ffffffffffffI64)
#else
								if(chain->startpt > 0x0000ffffffffffffllu)
#endif
								{
									fprintf(stderr, "**** Error writeChain() in %i: Startpoint is bigger than 6 bytes. (%llx) ****\n", m_curFileChains, chain->startpt);
//									exit(1);										
									return;
								}
#ifdef _WIN32
								if(chain->endpt > 0x00ffffffffffffffI64)
#else
								if(chain->endpt > 0x00fffffffffffffllu)
#endif
								{
									fprintf(stderr, "**** Error writeChain() in %i: Endpoint is bigger than 7 bytes. (%llx) ****\n", m_curFileChains, chain->endpt);
//									exit(1);										
									return;
								}

								// Disable checkpoints in this version
//								cp = cp >> 24;
								fwrite(&chain->startpt, 1, 6, m_pFile); // Prefix increased to 6 bytes in this version
								fwrite(&chain->endpt, 1, 2, m_pFile);
								m_prevEndpt = chain->endpt;
//								fwrite(&cp, 1, 1, m_pFile);

								uint64 nPrefix = chain->endpt >> 16;
								if(nPrefix != m_curPrefix) // Next index. Write the current one down
								{		
									// nPrefix should NOT be smaller. If it is *something* is wrong
									if(nPrefix < m_curPrefix)
									{
										fprintf(stderr, "**** Error writeChain() in %i of file %i: Prefix is smaller than previous prefix. %llu < %llu**** \n", m_curFileChains, m_curFile, nPrefix, m_curPrefix);
										exit(1);									
									}
									unsigned char index[11] = {0}; // [0 - 10]
									// The 5 byte prefix of the endingpoint [0 - 4]
									uint64 *prefix = (uint64 *)index;
									*prefix = m_curPrefix;	
									int *iprefix;
									// The 4 bytes index where to look in the file for the chains [5 - 8]
									iprefix = (int*)&index[5]; 
									*iprefix = m_prefixStart;
									short *iprefixs = (short*)&index[9]; // The number of chains (2 bytes, max 65535 chains) with this prefix [9 - 10]
									*iprefixs = (short)(m_curFileChains - m_prefixStart);
									fwrite(&index, 1, 11, m_pFileIndex); // Position of these chains is: First chain: FILESTART + (iprefix * 8)
																		 //								 Last chain:  FILESTART + (iprefix * 8) + (iprefixs * 8)
									m_numIndexRows++;
//									fflush(m_pFileIndex);
									m_prefixStart = m_curFileChains;
									m_curPrefix = nPrefix; 
									bIndexWritten = true;
									/*fseek(m_pFileCont, 0, SEEK_SET);
									fwrite(m_curFileChains, 0, sizeof(int), m_pFileCont);
									fflush(m_curFileChains);
									*/
								}
								m_curFileChains++;
								//nChainNum++;
								//return 3;
//							}
/*							else
							{
//								nFailed++;
							}
*/	
/*	if (fwrite((void*)chain, 16, 1, m_pFile) != 1)
	{
		perror("temp.rt");
		exit(1);
	}*/
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

/*		if(bIndexWritten == false) // If we haven't written the index in this round, its time to do it before we close the file
		{*/
			printf("writing index before closing file...\n");
			printf("Covering %u - %u\n", m_prefixStart, (m_prefixStart + m_curFileChains - m_prefixStart));
			unsigned char index[11] = {0}; // [0 - 10]
			// The 5 byte prefix of the endingpoint [0 - 4]
			uint64 *prefix = (uint64 *)index;
			*prefix = m_curPrefix;	
			printf("value is %02x%02x%02x%02x%02x%02x%02x%02x (%llu)\n", index[0], index[1], index[2], index[3], index[4], index[5], index[6], index[7], m_curPrefix);
			int *iprefix;
			// The 4 bytes index where to look in the file for the chains [5 - 8]
			iprefix = (int*)&index[5];
			*iprefix = m_prefixStart;
			short *iprefixs = (short*)&index[9]; // The number of chains (2 bytes, max 65535 chains) with this prefix [9 - 10]
			*iprefixs = (short)(m_curFileChains - m_prefixStart);
			fwrite(&index, 1, 11, m_pFileIndex); // Position of these chains is: First chain: FILESTART + (iprefix * 8)
												 //								 Last chain:  FILESTART + (iprefix * 8) + (iprefixs * 8)
			m_numIndexRows++;
		/*	
		}
		else
		{
			printf("Index already written in this round.. skipping!\n");
		}
*/
		fclose(m_pFileIndex);
		std::string sIndex(m_fileTemp);
		sIndex.append(".index");
		printf("%s: %u index rows\n\n", sIndex.c_str(), m_numIndexRows);
		if (rename("temp.rt.index", sIndex.c_str()) != 0)
		{
			perror(m_fileTemp);
			exit(1);
		}


		m_curFile++;
		m_curFileChains = 0;
		m_prefixStart = 0;
		m_numIndexRows = 0;
	}
}

