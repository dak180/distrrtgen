/*
   RainbowCrack - a general propose implementation of Philippe Oechslin's faster time-memory trade-off technique.

   Copyright (C) Zhu Shuanglei <shuanglei@hotmail.com>
*/

#ifndef _PUBLIC_H
#define _PUBLIC_H

#include <stdio.h>

#include <string>
#include <vector>
#include <list>
using namespace std;

#ifdef _WIN32
	#define uint64 unsigned __int64
#else
	#define uint64 u_int64_t
#endif

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
	int nFirstChain;
	unsigned int nChainCount;
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

unsigned int GetFileLen(FILE* file);
string TrimString(string s);
bool boinc_ReadLinesFromFile(string sPathName, vector<string>& vLine);
bool ReadLinesFromFile(string sPathName, vector<string>& vLine);
bool SeperateString(string s, string sSeperator, vector<string>& vPart);
string uint64tostr(uint64 n);
string uint64tohexstr(uint64 n);
string HexToStr(const unsigned char* pData, int nLen);
unsigned int GetAvailPhysMemorySize();
void ParseHash(string sHash, unsigned char* pHash, int& nHashLen);
bool GetHybridCharsets(string sCharset, vector<tCharset>& vCharset);
void Logo();

#endif
