/*
 * freerainbowtables is a project for generating, distributing, and using
 * perfect rainbow tables
 *
 * Copyright (C) Zhu Shuanglei <shuanglei@hotmail.com>
 * Copyright 2009, 2010 Daniël Niggebrugge <niggebrugge@fox-it.com>
 * Copyright 2009, 2010 James Nobis <frt@quelrod.net>
 *
 * This file is part of freerainbowtables.
 *
 * freerainbowtables is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License.
 *
 * freerainbowtables is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with freerainbowtables.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _PUBLIC_H
#define _PUBLIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>
#include <list>

#include "global.h"

using namespace std;

struct RainbowChain
{
	uint64 nIndexS;
	uint64 nIndexE;
};

struct RainbowChainCP
{
	uint64 nIndexS;
	uint64 nIndexE;
	unsigned short nCheckPoint;
};
struct IndexChain
{
	uint64 nPrefix;
	uint32 nFirstChain;
	uint32 nChainCount;
};
struct FoundRainbowChain
{
	uint64 nIndexS;
	int nIndexE;
	int nCheckPoint;
	int nGuessedPos;
};
struct ChainCheckChain
{
	uint64 nIndexS;
	int nGuessedPos;
};
struct IndexRow
{
	uint64 prefix;
	unsigned int prefixstart, numchains;
};

typedef struct
{
	string sName;
	int nPlainLenMin;
	int nPlainLenMax;
} tCharset;

#define MAX_PLAIN_LEN 256
#define MIN_HASH_LEN  8
#define MAX_HASH_LEN  256
#define MAX_SALT_LEN  256

// XXX nmap is GPL2, will check newer releases regarding license
// Code comes from nmap, used for the linux implementation of kbhit()
#ifndef _WIN32
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

int tty_getchar();
void tty_done();
void tty_init();
void tty_flush(void);
// end nmap code

#endif

#if defined(_WIN32) && !defined(__GNUC__)
	int gettimeofday( struct timeval *tv, struct timezone *tz );
#else
	#include <sys/time.h>
#endif

timeval sub_timeofday( timeval tv2, timeval tv );

long GetFileLen(FILE* file);
string TrimString(string s);
bool boinc_ReadLinesFromFile(string sPathName, vector<string>& vLine);
bool ReadLinesFromFile(string sPathName, vector<string>& vLine);
bool SeperateString(string s, string sSeperator, vector<string>& vPart);
string uint64tostr(uint64 n);
string uint64tohexstr(uint64 n);
string HexToStr(const unsigned char* pData, int nLen);
unsigned long GetAvailPhysMemorySize();
string GetApplicationPath();
void ParseHash(string sHash, unsigned char* pHash, int& nHashLen);
bool GetHybridCharsets(string sCharset, vector<tCharset>& vCharset);
void Logo();
bool writeResultLineToFile(string sOutputFile, string sHash, string sPlain, string sBinary);

#endif
