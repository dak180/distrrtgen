/*
   RainbowCrack - a general propose implementation of Philippe Oechslin's faster time-memory trade-off technique.

   Copyright (C) Zhu Shuanglei <shuanglei@hotmail.com>
*/

#ifdef _WIN32
	#pragma warning(disable : 4786)
#else
#define _atoi64 atoll
#define stricmp strcasecmp
#endif

#include "HashSet.h"
#include <string.h>
CHashSet::CHashSet()
{
}

CHashSet::~CHashSet()
{
	for(int i = 0; i < m_vIndices.size(); i++)
	{
		delete m_vIndices[i];		
	}
}
int CHashSet::GetLookupID(string sHash)
{
	int i;
	for (i = 0; i < m_vHash.size(); i++)
	{
		if(stricmp(m_vHash[i].c_str(),sHash.c_str()) == 0)
		{
			return m_vLookupIDs[i];
		}
	}

}
void CHashSet::AddHash(string sHash, uint64 *pIndices, int nLookupID)
{
	if (sHash == "aad3b435b51404ee")
		return;

	int i;
	for (i = 0; i < m_vHash.size(); i++)
	{
		if (m_vHash[i] == sHash)
			return;
	}

	//printf("debug: adding hash %s\n", sHash.c_str());

	m_vHash.push_back(sHash);
	m_vFound.push_back(false);
	m_vPlain.push_back("");
	m_vBinary.push_back("");
	m_vIndices.push_back(pIndices);
	m_vLookupIDs.push_back(nLookupID);
	VECTOR_CHAIN vec;
	m_vFoundChains.push_back(vec);
}

bool CHashSet::AnyhashLeft()
{
	int i;
	for (i = 0; i < m_vHash.size(); i++)
	{
		if (!m_vFound[i])
			return true;
	}

	return false;
}

bool CHashSet::AnyHashLeftWithLen(int nLen)
{
	int i;
	for (i = 0; i < m_vHash.size(); i++)
	{
		if (!m_vFound[i])
			if (m_vHash[i].size() == nLen * 2)
				return true;
	}

	return false;
}

void CHashSet::GetLeftHashWithLen(vector<string>& vHash, vector<uint64 *>& vIndices, int nLen)
{
	vHash.clear();
	vIndices.clear();
	int i;
	for (i = 0; i < m_vHash.size(); i++)
	{
		if (!m_vFound[i])
			if (m_vHash[i].size() == nLen * 2)
			{
				vHash.push_back(m_vHash[i]);
				vIndices.push_back(m_vIndices[i]);
			}
	}
}

void CHashSet::SetPlain(string sHash, string sPlain, string sBinary)
{
	int i;
	for (i = 0; i < m_vHash.size(); i++)
	{
		if(stricmp(m_vHash[i].c_str(),sHash.c_str()) == 0)
		{
			m_vFound[i]    = true;
			m_vPlain[i]    = sPlain;
			m_vBinary[i]   = sBinary;
			return;
		}
	}
}
// Add a chain to verify (matching endpoint)
void CHashSet::AddChain(string sHash, FoundRainbowChain &Chain)
{
	int i;
	for (i = 0; i < m_vHash.size(); i++)
	{
		if(stricmp(m_vHash[i].c_str(),sHash.c_str()) == 0)
		{
			m_vFoundChains[i].push_back(Chain);
			return;
		}
	}
}
/*
vector<RainbowChain> CHashSet::GetChains(string sHash)
{
	int i;
	for (i = 0; i < m_vHash.size(); i++)
	{
		if(stricmp(m_vHash[i].c_str(),sHash.c_str()) == 0)
		{
			return m_vFoundChains[i];
		}
	}
	VECTOR_CHAIN vec;
	return vec;
}*/
bool CHashSet::GetPlain(string sHash, string& sPlain, string& sBinary)
{
	if (stricmp(sHash.c_str(),"aad3b435b51404ee") == 0)
	{
		sPlain  = "";
		sBinary = "";
		return true;
	}

	int i;
	for (i = 0; i < m_vHash.size(); i++)
	{
		if (m_vHash[i] == sHash)
		{
			if (m_vFound[i])
			{
				sPlain  = m_vPlain[i];
				sBinary = m_vBinary[i];
				return true;
			}
		}
	}

	return false;
}

int CHashSet::GetStatHashFound()
{
	int nHashFound = 0;
	int i;
	for (i = 0; i < m_vHash.size(); i++)
	{
		if (m_vFound[i])
			nHashFound++;
	}

	return nHashFound;
}

int CHashSet::GetStatHashTotal()
{
	return m_vHash.size();
}

void CHashSet::GetFoundChains(map<string, VECTOR_CHAIN> &mHashList)
{
//	map<string, string> *mHashList = new map<string, string>;
	for(int i = 0; i < m_vHash.size(); i++)
	{
		mHashList[m_vHash[i]] = m_vFoundChains[i];
	}
//	return mHashList;
}