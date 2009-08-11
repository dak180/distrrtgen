/*
   RainbowCrack - a general propose implementation of Philippe Oechslin's faster time-memory trade-off technique.

   Copyright (C) Zhu Shuanglei <shuanglei@hotmail.com>
*/

#ifndef _HASHSET_H
#define _HASHSET_H

#include "Public.h"
#include <map>
using std::map;
class CHashSet
{
public:
	CHashSet();
	~CHashSet();

private:
	vector<string> m_vHash;
	vector<bool>   m_vFound;
	vector<string> m_vPlain;
	vector<string> m_vBinary;
	vector<uint64*> m_vIndices;
	vector<int>	m_vLookupIDs;
	typedef vector<FoundRainbowChain> VECTOR_CHAIN;
	vector<VECTOR_CHAIN> m_vFoundChains;
public:
	
	void AddHash(string sHash, uint64 *pIndices, int nLookupID);		// lowercase, len % 2 == 0, MIN_HASH_LEN * 2 <= len <= MAX_HASH_LEN * 2
	bool AnyhashLeft();
	bool AnyHashLeftWithLen(int nLen);
	void GetLeftHashWithLen(vector<string>& vHash, vector<uint64 *>& vIndices, int nLen);
	void AddChain(string sHash, FoundRainbowChain &Chain);
	void SetPlain(string sHash, string sPlain, string sBinary);
	bool GetPlain(string sHash, string& sPlain, string& sBinary);
	int GetLookupID(string sHash);
	//vector<FoundRainbowChain> CHashSet::GetChains(string sHash);
	int GetStatHashFound();
	int GetStatHashTotal();
	void GetFoundChains(map<string, VECTOR_CHAIN> &mHashList);
};

#endif
