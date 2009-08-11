/*
   RainbowCrack - a general propose implementation of Philippe Oechslin's faster time-memory trade-off technique.

   Copyright (C) Zhu Shuanglei <shuanglei@hotmail.com>
*/

#ifdef _WIN32
	#pragma warning(disable : 4786)
#endif

#include "CrackEngine.h"
#include "BaseRTReader.h"
#include "RTReader.h"
#include "RTIReader.h"
#include "RTI2Reader.h"
#include <time.h>

CCrackEngine::CCrackEngine()
{
	ResetStatistics();
}

CCrackEngine::~CCrackEngine()
{
}

//////////////////////////////////////////////////////////////////////

void CCrackEngine::ResetStatistics()
{
	m_fTotalDiskAccessTime               = 0.0f;
	m_fTotalCryptanalysisTime            = 0.0f;
	m_nTotalChainWalkStep                = 0;
	m_nTotalFalseAlarm                   = 0;
	m_nTotalChainWalkStepDueToFalseAlarm = 0;
//	m_nTotalFalseAlarmSkipped			 = 0;
}

int CCrackEngine::BinarySearch(RainbowChainCP* pChain, int nRainbowChainCount, uint64 nIndex)
{
	int nLow = 0;
	int nHigh = nRainbowChainCount - 1;
	while (nLow <= nHigh)
	{
		int nMid = (nLow + nHigh) / 2;
		if (nIndex == pChain[nMid].nIndexE)
			return nMid;
		else if (nIndex < pChain[nMid].nIndexE)
			nHigh = nMid - 1;
		else
			nLow = nMid + 1;
	}

	return -1;
}
bool CCrackEngine::CheckAlarm(RainbowChainCP* pChain, int nGuessedPos, unsigned char* pHash, CHashSet& hs)
{
	CChainWalkContext cwc;
	cwc.SetIndex(pChain->nIndexS);
//	printf("Checking alarm for %ui64\n", pChain->nIndexS);
	int nPos, i = 0;
	if(m_pHeader != NULL)
	{
		int nNextPos = m_pHeader->m_cppos[i];
		for (nPos = 0; nPos < nGuessedPos; nPos++)
		{
			cwc.IndexToPlain();
			cwc.PlainToHash();
			cwc.HashToIndex(nPos);
			if(nPos == nNextPos) // Check if we reached the next checkpoint position
			{
				if(i <= m_pHeader->rti_cplength) 
					nNextPos = m_pHeader->m_cppos[++i];
				if((cwc.GetIndex() & 0x00000001) != (pChain->nCheckPoint & (1 << 15 - m_pHeader->rti_cplength - i) >> 15 - m_pHeader->rti_cplength - i))
				{
//					m_nTotalFalseAlarmSkipped += 10000 - 5000;
					printf("CheckPoint caught false alarm at position %i\n", nPos);
					return false;
				}				
			}
		}
	}
	else
	{
		for (nPos = 0; nPos < nGuessedPos; nPos++)
		{
			cwc.IndexToPlain();
			cwc.PlainToHash();
			cwc.HashToIndex(nPos);
		}
	}
	cwc.IndexToPlain();
	cwc.PlainToHash();

	if (cwc.CheckHash(pHash))
	{
		printf("plaintext of %s is %s\n", cwc.GetHash().c_str(), cwc.GetPlain().c_str());
		hs.SetPlain(cwc.GetHash(), cwc.GetPlain(), cwc.GetBinary());
		return true;
	}

	return false;
}


void CCrackEngine::GetChainIndexRangeWithSameEndpoint(RainbowChainCP* pChain,
													  int nRainbowChainCount,
													  int nMatchingIndexE,
													  int& nMatchingIndexEFrom,
													  int& nMatchingIndexETo)
{
	nMatchingIndexEFrom = nMatchingIndexE;
	nMatchingIndexETo   = nMatchingIndexE;
	while (nMatchingIndexEFrom > 0)
	{
		if (pChain[nMatchingIndexEFrom - 1].nIndexE == pChain[nMatchingIndexE].nIndexE)
			nMatchingIndexEFrom--;
		else
			break;
	}
	while (nMatchingIndexETo < nRainbowChainCount - 1)
	{
		if (pChain[nMatchingIndexETo + 1].nIndexE == pChain[nMatchingIndexE].nIndexE)
			nMatchingIndexETo++;
		else
			break;
	}
}

void CCrackEngine::SearchTableChunk(RainbowChainCP* pChain, int nRainbowChainLen, int nRainbowChainCount, CHashSet& hs)
{
	vector<string> vHash;
	hs.GetLeftHashWithLen(vHash, CChainWalkContext::GetHashLen());
	printf("searching for %d hash%s...\n", vHash.size(),
										   vHash.size() > 1 ? "es" : "");

	int nChainWalkStep = 0;
	int nFalseAlarm = 0;
	int nChainWalkStepDueToFalseAlarm = 0;

	int nHashIndex;
	for (nHashIndex = 0; nHashIndex < vHash.size(); nHashIndex++)
	{
		unsigned char TargetHash[MAX_HASH_LEN];
		int nHashLen;
		ParseHash(vHash[nHashIndex], TargetHash, nHashLen);
		if (nHashLen != CChainWalkContext::GetHashLen())
			printf("debug: nHashLen mismatch\n");

		// Rqeuest ChainWalk
		bool fNewlyGenerated;
		uint64* pStartPosIndexE = m_cws.RequestWalk(TargetHash,
													nHashLen,
													CChainWalkContext::GetHashRoutineName(),
													CChainWalkContext::GetPlainCharsetName(),
													CChainWalkContext::GetPlainLenMin(),
													CChainWalkContext::GetPlainLenMax(),
													CChainWalkContext::GetRainbowTableIndex(),
													nRainbowChainLen,
													fNewlyGenerated);
		//printf("debug: using %s walk for %s\n", fNewlyGenerated ? "newly generated" : "existing",
		//										vHash[nHashIndex].c_str());

		// Walk
		int nPos;
		for (nPos = nRainbowChainLen - 2; nPos >= 0; nPos--)
		{
			if (fNewlyGenerated)
			{
				CChainWalkContext cwc;
				cwc.SetHash(TargetHash);
				cwc.HashToIndex(nPos);
				int i;
				for (i = nPos + 1; i <= nRainbowChainLen - 2; i++)
				{
					cwc.IndexToPlain();
					cwc.PlainToHash();
					cwc.HashToIndex(i);
				}

				pStartPosIndexE[nPos] = cwc.GetIndex();
				nChainWalkStep += nRainbowChainLen - 2 - nPos;
			}
			uint64 nIndexEOfCurPos = pStartPosIndexE[nPos];

			// Search matching nIndexE
			int nMatchingIndexE = BinarySearch(pChain, nRainbowChainCount, nIndexEOfCurPos);
			if (nMatchingIndexE != -1)
			{
				int nMatchingIndexEFrom, nMatchingIndexETo;
				GetChainIndexRangeWithSameEndpoint(pChain, nRainbowChainCount,
												   nMatchingIndexE,
												   nMatchingIndexEFrom, nMatchingIndexETo);
				int i;
				for (i = nMatchingIndexEFrom; i <= nMatchingIndexETo; i++)
				{
					if (CheckAlarm(pChain + i, nPos, TargetHash, hs))
					{
						//printf("debug: discarding walk for %s\n", vHash[nHashIndex].c_str());
						m_cws.DiscardWalk(pStartPosIndexE);
						goto NEXT_HASH;
					}
					else
					{
						nChainWalkStepDueToFalseAlarm += nPos + 1;
						nFalseAlarm++;
					}
				}
			}
		}
NEXT_HASH:;
	}

	//printf("debug: chain walk step: %d\n", nChainWalkStep);
	//printf("debug: false alarm: %d\n", nFalseAlarm);
	//printf("debug: chain walk step due to false alarm: %d\n", nChainWalkStepDueToFalseAlarm);

	m_nTotalChainWalkStep += nChainWalkStep;
	m_nTotalFalseAlarm += nFalseAlarm;
	m_nTotalChainWalkStepDueToFalseAlarm += nChainWalkStepDueToFalseAlarm;
}

void CCrackEngine::SearchRainbowTable(string sPathName, CHashSet& hs)
{
	// FileName
#ifdef _WIN32
	int nIndex = sPathName.find_last_of('\\');
#else
	int nIndex = sPathName.find_last_of('/');
#endif
	string sFileName;
	if (nIndex != -1)
		sFileName = sPathName.substr(nIndex + 1);
	else
		sFileName = sPathName;

	// Info
	printf("%s:\n", sFileName.c_str());

	// Setup
	int nRainbowChainLen, nRainbowChainCount;
	if (!CChainWalkContext::SetupWithPathName(sPathName, nRainbowChainLen, nRainbowChainCount))
		return;
	//printf("keyspace: %u\n", CChainWalkContext::GetPlainSpaceTotal());
	// Already finished?
	if (!hs.AnyHashLeftWithLen(CChainWalkContext::GetHashLen()))
	{
		printf("this table contains hashes with length %d only\n", CChainWalkContext::GetHashLen());
		return;
	}
	BaseRTReader *reader = NULL;
	if(sFileName.substr(sFileName.length() - 4, sFileName.length()) == "rti2")
	{
		reader = (BaseRTReader*)new RTI2Reader(sPathName);
		m_pHeader = ((RTI2Reader*)reader)->GetHeader();
		m_TableType = RTI2;
	}
	else if(sFileName.substr(sFileName.length() - 3, sFileName.length()) == "rti")
	{
		reader = (BaseRTReader*)new RTIReader(sPathName);		
		m_TableType = RTI;
	}
	else if(sFileName.substr(sFileName.length() - 2, sFileName.length()) == "rt")
	{
		reader = (BaseRTReader*)new RTReader(sPathName);
		m_TableType = RT;
	}
	else 
	{
		printf("Invalid rainbow table type");
		return;
	}
	static CMemoryPool mp;
	unsigned int nAllocatedSize;
	int chainsleft = reader->GetChainsLeft();
	RainbowChainCP* pChain = (RainbowChainCP*)mp.Allocate(chainsleft * sizeof(RainbowChainCP), nAllocatedSize);
	nAllocatedSize = nAllocatedSize / sizeof(RainbowChainCP) * sizeof(RainbowChainCP);		// Round to boundary
	unsigned int nChains = nAllocatedSize / sizeof(RainbowChainCP);
	bool fVerified = false;
	while (reader->GetChainsLeft() > 0)	// Chunk read loop
	{
		clock_t t1 = clock();
		reader->ReadChains(nChains, pChain);
		clock_t t2 = clock();
		float fTime = 1.0f * (t2 - t1) / CLOCKS_PER_SEC;
		printf("%u chains read, disk access time: %.2f s\n", nChains, fTime);
		m_fTotalDiskAccessTime += fTime;
		int nRainbowChainCountRead = nChains;

		if (!fVerified)
		{
			printf("verifying the file...\n");

			// Chain length test
			int nIndexToVerify = nRainbowChainCountRead / 2;
			CChainWalkContext cwc;
			cwc.SetIndex(pChain[nIndexToVerify].nIndexS);
			int nPos;
			for (nPos = 0; nPos < nRainbowChainLen - 1; nPos++)
			{
				cwc.IndexToPlain();
				cwc.PlainToHash();
				cwc.HashToIndex(nPos);
			}
			if (cwc.GetIndex() != pChain[nIndexToVerify].nIndexE)
			{
				printf("rainbow chain length verify fail\n");
				break;
			}

			// Chain sort test
			int i;
			for (i = 0; i < nRainbowChainCountRead - 1; i++)
			{
				if (pChain[i].nIndexE > pChain[i + 1].nIndexE)
					break;
			}
			if (i != nRainbowChainCountRead - 1)
			{
				printf("this file is not sorted\n");
				break;
			}

			fVerified = true;

		}
		// Search table chunk
		t1 = clock();
		SearchTableChunk(pChain, nRainbowChainLen, nRainbowChainCountRead, hs);
		t2 = clock();
		fTime = 1.0f * (t2 - t1) / CLOCKS_PER_SEC;
		printf("cryptanalysis time: %.2f s\n", fTime);
		m_fTotalCryptanalysisTime += fTime;

		// Already finished?
		if (!hs.AnyHashLeftWithLen(CChainWalkContext::GetHashLen()))
			break;
	}
}

void CCrackEngine::Run(vector<string> vPathName, CHashSet& hs)
{
	// Reset statistics
	ResetStatistics();

	// Sort vPathName (CChainWalkSet need it)
	int i, j;
	for (i = 0; i < vPathName.size() - 1; i++)
		for (j = 0; j < vPathName.size() - i - 1; j++)
		{
			if (vPathName[j] > vPathName[j + 1])
			{
				string sTemp;
				sTemp = vPathName[j];
				vPathName[j] = vPathName[j + 1];
				vPathName[j + 1] = sTemp;
			}
		}

	// Run
	for (i = 0; i < vPathName.size() && hs.AnyhashLeft(); i++)
	{
		SearchRainbowTable(vPathName[i], hs);
		printf("\n");
	}
}

float CCrackEngine::GetStatTotalDiskAccessTime()
{
	return m_fTotalDiskAccessTime;
}
/*float CCrackEngine::GetWastedTime()
{
	return m_fIndexTime;
}*/
float CCrackEngine::GetStatTotalCryptanalysisTime()
{
	return m_fTotalCryptanalysisTime;
}

int CCrackEngine::GetStatTotalChainWalkStep()
{
	return m_nTotalChainWalkStep;
}

int CCrackEngine::GetStatTotalFalseAlarm()
{
	return m_nTotalFalseAlarm;
}

int CCrackEngine::GetStatTotalChainWalkStepDueToFalseAlarm()
{
	return m_nTotalChainWalkStepDueToFalseAlarm;
}
