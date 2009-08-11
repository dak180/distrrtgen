/*
	Copyright (C) 2008 Steve Thomas <SMT837784@yahoo.com>

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
#include <time.h>
#include <ctime>
#include <errno.h>
#include <stdlib.h>
#include "RTRead.h"

#ifdef HAVE_DIRENT_H
	#include <dirent.h>
#else
	// I know Microsoft's compiler, .Net 2005 or older, does not come with this file.
	// You can use this if you don't have dirent.h: http://www.cs.fiu.edu/~weiss/cop4338_spr06/dirent.h
	// Umm I'm not sure if the license on that file, which appears to be public domain, conflicts with GPL v3.
	// So only use that if you need it and the licenses don't conflict.
	#include "dirent.h"
#endif

#ifdef _WIN32
	#include <conio.h>
	#define DIRECTORY_SEPERATOR '\\'
#else
	#include <fcntl.h>
	#define DIRECTORY_SEPERATOR '/'
#endif

#ifndef NAME_MAX
	#define NAME_MAX 256
#endif

#include "RTRead.h"

RTRead::RTRead(char *dir, uint64 maxIndex, int verbose)
{
	int dirLen = strlen(dir), dirFileLen;
	char *file = new char[dirLen + NAME_MAX + 2];
	DIR *pDir;
	struct dirent *pDirFile;
	FileList *head = NULL, *current = NULL, *temp = new FileList;

	// Init
	m_verbose = verbose;
	m_maxIndex = maxIndex;
	m_chainsRead = 0;
	m_chains = 0;
	m_pqSize = 0;
	m_pq = NULL;
	temp->next = NULL;
	strncpy(file, dir, dirLen);
	if (dir[dirLen - 1] != DIRECTORY_SEPERATOR)
	{
		file[dirLen] = DIRECTORY_SEPERATOR;
		dirLen++;
	}

	// Open directory
	pDir = opendir(dir);
	if (pDir == NULL)
	{
		printf("Error #%u: Opening directory '%s'\n", errno, dir);
		exit(errno);
	}

	// Get files
	for (pDirFile = readdir(pDir); pDirFile != NULL; pDirFile = readdir(pDir))
	{
		dirFileLen = strlen(pDirFile->d_name);
		if (pDirFile->d_name[0] != '.' && dirFileLen > 5 &&
		    pDirFile->d_name[dirFileLen - 5] == '.' &&
		   (pDirFile->d_name[dirFileLen - 4] == 'p' || pDirFile->d_name[dirFileLen - 4] == 'P') &&
		   (pDirFile->d_name[dirFileLen - 3] == 'a' || pDirFile->d_name[dirFileLen - 3] == 'A') &&
		   (pDirFile->d_name[dirFileLen - 2] == 'r' || pDirFile->d_name[dirFileLen - 2] == 'R') &&
		   (pDirFile->d_name[dirFileLen - 1] == 't' || pDirFile->d_name[dirFileLen - 1] == 'T'))

		{
			strcpy(file + dirLen, pDirFile->d_name);
			if (getInfo(file, dirLen + dirFileLen, temp) == 0)
			{
				m_chains += temp->chains;
				m_pqSize++;
				if (head == NULL)
				{
					head = temp;
					current = temp;
				}
				else
				{
					current->next = temp;
					current = temp;
				}
				temp = new FileList;
				temp->next = NULL;
			}
			else
			{
				printf("Continue? (y/n): ");
				int ch = 0;
				while (ch != EOF && ch != 'y' && ch != 'Y' && ch != 'n' && ch != 'N')
				{
					ch = getchar();
				}
				if (ch != 'y' && ch != 'Y')
				{
					while (head != NULL)
					{
						current = head;
						head = head->next;
						fclose(current->pFile);
						delete [] current->name;
						delete current;
					}
					delete [] file;
					delete temp;
					closedir(pDir);
					exit(1);
				}
			}
		}
	}
	delete [] file;
	delete temp;
	closedir(pDir);
	if (head == NULL)
	{
		printf("Error no '*.part' input files found in directory '%s'.\n", dir);
		exit(1);
	}

	// Push file list into priority queue
	PriorityQueueNode tmpNode;
	unsigned int i = 0, node, par;

	m_pq = new PriorityQueueNode[m_pqSize];
	while (head != NULL)
	{
		if (readGoodChain(head->pFile, &(m_pq[i].chain)) == 0)
		{
			// Init priority queue node
			m_pq[i].pFile = head->pFile;
			m_pq[i].fName = head->name;

			// Insert into priority queue
			if (i != 0)
			{
				node = i;
				par = (node - 1) >> 1;
				if (m_pq[par].chain.endpt > m_pq[node].chain.endpt)
				{
					tmpNode = m_pq[node];
					do
					{
						m_pq[node] = m_pq[par];
						node = par;
						if (par == 0)
						{
							break;
						}
						par = (node - 1) >> 1;
					} while (m_pq[par].chain.endpt > tmpNode.chain.endpt);
				}
				if (i != node)
				{
					m_pq[node] = tmpNode;
				}
			}
			i++;
		}
		else
		{
			m_pqSize--;
			printf("File had no valid chains '%s'\n", head->name);
			delete [] head->name;
		}

		// Delete file list node
		current = head;
		head = head->next;
		delete current;
	}

	printf("Starting with %u files.\n", m_pqSize);
	m_startTime = clock();
}

RTRead::~RTRead()
{
	if (m_pq != NULL)
	{
		for (unsigned int i = 0; i < m_pqSize; i++)
		{
			if (m_pq[i].pFile != NULL)
			{
				fclose(m_pq[i].pFile);
			}
			if (m_pq[i].fName != NULL)
			{
				delete [] m_pq[i].fName;
			}
		}
		delete [] m_pq;
	}
}

int RTRead::readChain(RTChainCP *chain)
{
	PriorityQueueNode tmp;
	unsigned int child, node;

	if (m_pq == NULL)
	{
		return 1;
	}
	*chain = m_pq[0].chain;
	bool remove = (readGoodChain(m_pq[0].pFile, &(m_pq[0].chain)) != 0);

	if (remove)
	{
		if (m_verbose)
		{
			printf("Finished with file '%s'\n", m_pq[0].fName);
		}
		delete [] m_pq[0].fName;

		// Remove node from queue
		m_pqSize--;
		if (m_pqSize == 2)
		{
			if (m_pq[1].chain.endpt < m_pq[2].chain.endpt)
			{
				m_pq[0] = m_pq[1];
				m_pq[1] = m_pq[2];
			}
			else
			{
				m_pq[0] = m_pq[2];
			}
			m_pq[2].fName = NULL;
			m_pq[2].pFile = NULL;
		}
		else if (m_pqSize == 1)
		{
			m_pq[0] = m_pq[1];
			m_pq[1].fName = NULL;
			m_pq[1].pFile = NULL;
		}
		else if (m_pqSize == 0)
		{
			delete [] m_pq;
			m_pq = NULL;
		}
	}
	if (m_pqSize > 2)
	{
		if (remove)
		{
			tmp = m_pq[m_pqSize];
			m_pq[m_pqSize].fName = NULL;
			m_pq[m_pqSize].pFile = NULL;
		}
		else
		{			
			tmp = m_pq[0];
		}
		// Move tmp node to proper posistion in queue
		node = 0;
		child = 1;
		if (m_pq[1].chain.endpt > m_pq[2].chain.endpt)
		{
			child = 2;
		}
		while (tmp.chain.endpt > m_pq[child].chain.endpt)
		{
			m_pq[node] = m_pq[child];
			node = child;
			// child = left node
			child = (node << 1) + 1;
			if (child >= m_pqSize)
			{
				break;
			}
			if (child + 1 < m_pqSize && m_pq[child].chain.endpt > m_pq[child + 1].chain.endpt)
			{
				// child = right node
				child++;
			}
		}
		if (node != 0 || remove)
		{
			m_pq[node] = tmp;
		}
	}
	else if (m_pqSize == 2 && !remove)
	{
		if (m_pq[0].chain.endpt > m_pq[1].chain.endpt)
		{
			tmp = m_pq[1];
			m_pq[1] = m_pq[0];
			m_pq[0] = tmp;
		}
	}
	return 0;
}

int RTRead::readGoodChain(FILE *pFile, RTChainCP *chain)
{
	size_t ret;

	if (pFile == NULL)
	{
		return 1;
	}
	do
	{
		ret = fread((void*)chain, 16, 1, pFile);
		if(ret == 1)
			ret = fread((void*)&chain->checkpoint, 2, 1, pFile);
		if (ret != 1)
		{
			if (ferror(pFile) != 0)
			{
				printf("Error reading file.\n");
				printf("Continue? (y/n): ");
				int ch = 0;
				while (ch != EOF && ch != 'y' && ch != 'Y' && ch != 'n' && ch != 'N')
				{
					ch = getchar();
				}
				if (ch != 'y' && ch != 'Y')
				{
					exit(1);
				}
				clearerr(pFile);
			}
			else if (feof(pFile) != 0)
			{
				fclose(pFile);
				return 1;
			}
		}
		else
		{
			m_chainsRead++;
		}
	} while (ret != 1 || chain->startpt > m_maxIndex || chain->endpt > m_maxIndex);
	return 0;
}

void RTRead::printStatus()
{
	double percent = ((double) m_chainsRead / (double) m_chains) * 100.0;
	double timeElapsed = (clock() - m_startTime) / (double)CLOCKS_PER_SEC;

	printf("\n*** Status ***\n");
#ifdef _WIN32
	printf("  Chains Read:   %I64u\n", m_chainsRead);
	printf("  Total Chains:  %I64u\n", m_chains);
#else
	printf("  Chains Read:   %llu\n", m_chainsRead);
	printf("  Total Chains:  %llu\n", m_chains);
#endif
	printf("  Files Open:    %u\n", m_pqSize);
	printf("  Percent:       %0.1f\n", percent);
	printf("  Time Elapsed:  %0.0f sec\n", timeElapsed);
	printf("  Time Left:     %0.0f sec\n\n", timeElapsed / (percent / 100.0) - timeElapsed);
}

int RTRead::getInfo(char *file, int len, FileList *ret)
{
	FILE *pFile;
	unsigned int size;

	if (m_verbose)
	{
		printf("Opening file '%s'\n", file);
	}
	pFile = fopen(file, "rb");
	if (pFile == NULL)
	{
		perror(file);
		return 1;
	}

	// Get file size
	fseek(pFile, 0, SEEK_END);
	size = ftell(pFile);
	if (size & 15 != 0)
	{
		fclose(pFile);
		printf("Error file size of '%s' is not a multible of 16 bytes.\n", file);
		return 1;
	}
	fseek(pFile, 0, SEEK_SET);
	ret->chains = size >> 4;
	ret->pFile = pFile;

	ret->name = new char[len + 1];
	strncpy(ret->name, file, len);
	ret->name[len] = '\0';
	return 0;
}

