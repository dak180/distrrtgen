/*
   RainbowCrack - a general propose implementation of Philippe Oechslin's faster time-memory trade-off technique.

   Copyright (C) Zhu Shuanglei <shuanglei@hotmail.com>
*/

#ifndef _CRACKENGINE_H
#define _CRACKENGINE_H

#include "Public.h"
#include "HashSet.h"
#include "ChainWalkContext.h"
#include "MemoryPool.h"
#include "ChainWalkSet.h"
#include "RTI2Reader.h"
enum RTTYPE { RT, RTI, RTI2 };
class CCrackEngine
{
public:
	CCrackEngine();
	virtual ~CCrackEngine();

private:
	CChainWalkSet m_cws;
	RTI2Header *m_pHeader;
	// Statistics
	float m_fTotalDiskAccessTime;
	float m_fTotalCryptanalysisTime;
	int m_nTotalChainWalkStep;
	int m_nTotalFalseAlarm;
	int m_nTotalChainWalkStepDueToFalseAlarm;
	FILE *m_fChains;
	RTTYPE m_TableType;
private:
	void ResetStatistics();
	int BinarySearch(RainbowChainCP* pChain, int nRainbowChainCount, uint64 nIndex);
	void SearchTableChunk(RainbowChainCP* pChain, int nRainbowChainLen, int nRainbowChainCount, CHashSet& hs);
	bool CheckAlarm(RainbowChainCP* pChain, int nGuessedPos, unsigned char* pHash, CHashSet& hs);
void GetChainIndexRangeWithSameEndpoint(RainbowChainCP* pChain,
													  int nRainbowChainCount,
													  int nMatchingIndexE,
													  int& nMatchingIndexEFrom,
													  int& nMatchingIndexETo);
public:
	void SearchRainbowTable(string sPathName, CHashSet& hs);
	void Run(vector<string> vPathName, CHashSet& hs);
	float GetStatTotalDiskAccessTime();
	float GetStatTotalCryptanalysisTime();
	int   GetStatTotalChainWalkStep();
	int   GetStatTotalFalseAlarm();
	int   GetStatTotalChainWalkStepDueToFalseAlarm();
};

#endif
