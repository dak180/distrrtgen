/*
   RainbowCrack - a general propose implementation of Philippe Oechslin's faster time-memory trade-off technique.

   Copyright (C) Zhu Shuanglei <shuanglei@hotmail.com>
*/

#ifdef _WIN32
	#pragma warning(disable : 4786)
#endif

#include "CrackEngine.h"

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
}

RainbowChain *CCrackEngine::BinarySearch(RainbowChain *pChain, int nChainCountRead, uint64 nIndex, IndexChain *pIndex, int nIndexSize, int nIndexStart)
{
	uint64 nPrefix = nIndex >> 16;
	int nLow, nHigh;	
	//vector<RainbowChain> vChain;
	//clock_t t1Search = clock();
//	for (int j = 0; j < vIndex.size(); j++)
//	{
		bool found = false;
		int nChains = 0;
		
		if(nPrefix > pIndex[nIndexSize-1].nPrefix) // check if its in the index file
		{
			return NULL;
		}
		/*
		clock_t t1Index = clock();		
		for(int i = 0; i < vIndexSize[j]; i++)
		{
			
			if(nPrefix > pIndex[i].nPrefix)
			{
		//		nChains += pIndex[i].nChainCount;
				continue;
			}
			else if(nPrefix == pIndex[i].nPrefix)
			{
		//		nLow = nChains;
		//		nHigh = nLow + pIndex[i].nChainCount;
				nLow = 5;
				nHigh = 8;
		//		unsigned int nChunk = pIndex[i].nChainCount * 8;
				//printf("prefix found %i. Limiting chunk size to %i chains\n", pIndex[i].nPrefix, pIndex[i].nChainCount);
				found = true;
				break;
			}
			else 
			{
				break;
			}
		}
		clock_t t2Index = clock();
		m_fIndexSearchTime += 1.0f * (t2Index - t1Index) / CLOCKS_PER_SEC;
		*/

		clock_t t1Index = clock();		
		int nBLow = 0;
		int nBHigh = nIndexSize - 1;
		while (nBLow <= nBHigh)
		{
			int nBMid = (nBLow + nBHigh) / 2;
			if (nPrefix == pIndex[nBMid].nPrefix)
			{
				//nLow = nChains;
				int nChains = 0;
/*				for(int i = 0; i < nBMid; i++)
				{
					nChains += pIndex[i].nChainCount;
				}*/

				nLow = pIndex[nBMid].nFirstChain;
				nHigh = nLow + pIndex[nBMid].nChainCount;
				if(nLow >= nIndexStart && nLow <= nIndexStart + nChainCountRead) 
				{					
					if(nHigh > nIndexStart + nChainCountRead)
						nHigh = nIndexStart + nChainCountRead;
				}
				else if(nLow < nIndexStart && nHigh >= nIndexStart)
				{
					nLow = nIndexStart;
				}
				else break;					

//				printf("Chain Count: %u\n", pIndex[nBMid].nChainCount);
//				unsigned int nChunk = pIndex[nBMid].nChainCount * 8;
				//printf("prefix found %i. Limiting chunk size to %i chains\n", pIndex[i].nPrefix, pIndex[i].nChainCount);

				found = true;
				break;
			}
			else if (nPrefix < pIndex[nBMid].nPrefix)
				nBHigh = nBMid - 1;
			else
				nBLow = nBMid + 1;
		}
		clock_t t2Index = clock();
//		m_fIndexSearchTime += 1.0f * (t2Index - t1Index) / CLOCKS_PER_SEC;

//		printf("Time: %.2f nLow: %i nHigh %i\n", (1.0f * (t2Index - t1Index) / CLOCKS_PER_SEC), nLow, nHigh);
		if(found == true)
		{
/*			for(int i = 0; i < numChains; i++)
			{
				RainbowChain *chains = new RainbowChain();
				memcpy(chains, pChain[nLow + i], sizeof(RainbowChain));
			}*/
//			printf("Numchains: %i ", numChains);
			//			clock_t t1 = clock();
			if(pChain == NULL) // The chains are not preloaded. We need to seek the file for the chains
			{
				int numChains = nHigh - nLow;
				fseek(m_fChains, nLow * 8, SEEK_SET);
				RainbowChain *tmpChain = (RainbowChain*) new unsigned char[numChains * sizeof(RainbowChain)];
				memset(tmpChain, 0x00, numChains * sizeof(RainbowChain));
				unsigned char *data = new unsigned char[numChains * 8];
				fread(data, 1, numChains * 8, m_fChains);

				for(int i = 0; i < numChains; i++)
				{
					memcpy(&tmpChain[i].nIndexS, &data[i * 8], 5);
					memcpy(&tmpChain[i].nIndexE, &data[i * 8 + 5], 2);
					memcpy(&tmpChain[i].nCheckPoint, &data[i * 8 + 7], 1);				
				}
				for(int i = 0; i < numChains; i++)
				{
					// TODO: Seek to the position in the file, read the chains and check them
					int nSIndex = ((int)nIndex) & 0x0000FFFF;				
					if (nSIndex == tmpChain[i].nIndexE)
					{
						RainbowChain *chain = new RainbowChain();
						memcpy(chain, &tmpChain[i], sizeof(tmpChain));
						delete tmpChain;
						return chain;
					}				
					else if(tmpChain[i].nIndexE > nSIndex)
						break;
				}
				delete tmpChain;
			}
			else
			{
				for(int i = nLow - nIndexStart; i < nHigh - nIndexStart; i++)
				{
					// TODO: Seek to the position in the file, read the chains and check them
					int nSIndex = ((int)nIndex) & 0x0000FFFF;				
					if (nSIndex == pChain[i].nIndexE)
					{
						return &pChain[i];
					}				
					else if(pChain[i].nIndexE > nSIndex)
						break;
				}
			}
		}
//	}
//	clock_t t2Search = clock();
//	m_fIndexSearchTime += 1.0f * (t2Search - t1Search) / CLOCKS_PER_SEC;
	
	return NULL;
}


void CCrackEngine::SearchTableChunk(RainbowChain* pChain, int nRainbowChainLen, int nRainbowChainCount, CHashSet& hs, IndexChain *pIndex, int nIndexSize, int nChainStart)
{
	vector<string> vHash;
	vector<uint64 *> vIndices;
	vector<RainbowChain *> vChains;
	hs.GetLeftHashWithLen(vHash, vIndices, CChainWalkContext::GetHashLen());
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
//		printf("\nParsing hash...");
		ParseHash(vHash[nHashIndex], TargetHash, nHashLen);
//		printf("Done!\n");
		if (nHashLen != CChainWalkContext::GetHashLen())
			printf("debug: nHashLen mismatch\n");

		// Rqeuest ChainWalk
		bool fNewlyGenerated;
//		printf("Requesting walk...");
		//uint64* pStartPosIndexE = 
		uint64 *pStartPosIndexE = vIndices[nHashIndex];
/*
			m_cws.RequestWalk(TargetHash,
													nHashLen,
													CChainWalkContext::GetHashRoutineName(),
													CChainWalkContext::GetPlainCharsetName(),
													CChainWalkContext::GetPlainLenMin(),
													CChainWalkContext::GetPlainLenMax(),
													CChainWalkContext::GetRainbowTableIndex(),
													nRainbowChainLen,
													fNewlyGenerated);
													*/
//		printf("done!\n");
//		printf("debug: using %s walk for %s\n", fNewlyGenerated ? "newly generated" : "existing",
//												vHash[nHashIndex].c_str());

		// Walk
		int nPos;
			/*
		if (fNewlyGenerated)
		{
			printf("Pregenerating index...");
			for (nPos = nRainbowChainLen - 2; nPos >= 0; nPos--)
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
			printf("ok\n");

		}
		*/
		
		for (nPos = nRainbowChainLen - 2; nPos >= 0; nPos--)
		{
			uint64 nIndexEOfCurPos = pStartPosIndexE[nPos];
		
//			printf("%I64u,\n", nIndexEOfCurPos);
			
			// Search matching nIndexE
			RainbowChain *pChainFound = BinarySearch(pChain, nRainbowChainCount, nIndexEOfCurPos, pIndex, nIndexSize, nChainStart);
			if (pChainFound != NULL)
			{
				
				FoundRainbowChain chain; // Convert to FoundRainbowChain which allows us to add a line at which position we found the chain				
				memcpy(&chain, pChainFound, sizeof(RainbowChain));
				chain.nGuessedPos = nPos;
				hs.AddChain(vHash[nHashIndex], chain);
				if(pChain == NULL) // We need to delete the chain because its only temporarily loaded
					delete pChainFound;
/*				int nMatchingIndexEFrom, nMatchingIndexETo;
				GetChainIndexRangeWithSameEndpoint(pChain, nRainbowChainCount,
												   nMatchingIndexE,
												   nMatchingIndexEFrom, nMatchingIndexETo);
												   */
//				int i;
			//	printf("%i - %i = \n", nMatchingIndexEFrom, nMatchingIndexETo, ((nMatchingIndexETo - nMatchingIndexEFrom) +1));
/*				for (i = 0; i < vChain.size(); i++)
//				{

/*					if (CheckAlarm(&vChain[i], nPos, TargetHash, hs))
					{
						//printf("debug: discarding walk for %s\n", vHash[nHashIndex].c_str());
						//m_cws.DiscardWalk(pStartPosIndexE);
						goto NEXT_HASH;
					}
					else
					{
						nChainWalkStepDueToFalseAlarm += nPos + 1;
						nFalseAlarm++;
					}*/
//				}
			}
		}
NEXT_HASH:;
	}

	//printf("debug: chain walk step: %d\n", nChainWalkStep);
	//printf("debug: false alarm: %d\n", nFalseAlarm);
	//printf("debug: chain walk step due to false alarm: %d\n", nChainWalkStepDueToFalseAlarm);
/*
	m_nTotalChainWalkStep += nChainWalkStep;
	m_nTotalFalseAlarm += nFalseAlarm;
	m_nTotalChainWalkStepDueToFalseAlarm += nChainWalkStepDueToFalseAlarm;
	*/
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

	// Already finished?
	if (!hs.AnyHashLeftWithLen(CChainWalkContext::GetHashLen()))
	{
		printf("this table contains hashes with length %d only\n", CChainWalkContext::GetHashLen());
		return;
	}

	// Open
	FILE* file = fopen(sPathName.c_str(), "rb");
	if (file != NULL)
	{
		// File length check
		unsigned int nFileLen = GetFileLen(file);
		if (nFileLen % 8 != 0 || nRainbowChainCount * 8 != nFileLen)
			printf("file length mismatch\n");
		else
		{
			FILE* fIndex = fopen(((string)(sPathName + string(".index"))).c_str(), "rb");
			IndexChain *pIndex = NULL;
			int nIndexSize = 0;
			if(fIndex != NULL)
			{
				// File length check
				unsigned int nTotalChainCount = nFileLen / 8;
				unsigned int nIndexFileLen = GetFileLen(fIndex);

				unsigned int nRows = nIndexFileLen / 11;
				unsigned int nSize = nRows * sizeof(IndexChain);
				if (nIndexFileLen % 11 != 0)
					printf("index file length mismatch (%u bytes)\n", nIndexFileLen);
				else
				{
					pIndex = (IndexChain*)new unsigned char[nSize];
					memset(pIndex, 0x00, nSize);
					fseek(fIndex, 0, SEEK_SET);
					unsigned char *pData = new unsigned char[11];
					int nRead = 0;
					uint64 nLastPrefix = 0;
					for(int i = 0; (i * 11) < nIndexFileLen; i++)
					{
						nRead = fread(pData, 1, 11, fIndex);
						if(nRead == 11)
						{
//							nDataRead += nRead;
							memcpy(&pIndex[i].nPrefix, &pData[0], 5);
							memcpy(&pIndex[i].nFirstChain, &pData[5], 4);
							memcpy(&pIndex[i].nChainCount, &pData[9], 2);
						}
						else break;
						// Index checking part
						if(i != 0 && pIndex[i].nFirstChain < pIndex[i-1].nFirstChain)
						{
							printf("Corrupted index detected (FirstChain is lower than previous)\n");
							exit(-1);
						}
						else if(i != 0 && pIndex[i].nFirstChain != pIndex[i-1].nFirstChain + pIndex[i-1].nChainCount)
						{
							printf("Corrupted index detected (LastChain + nChainCount != FirstChain)\n");
							exit(-1);
						}
						else if(pIndex[i].nPrefix < nLastPrefix)
						{
							printf("Corrupted index detected (Prefix is decreasing)\n");
							exit(-1);
						}
						nLastPrefix = pIndex[i].nPrefix;

					}
					nIndexSize = nRows;
					if(pIndex[nIndexSize - 1].nFirstChain + pIndex[nIndexSize - 1].nChainCount + 1 <= nTotalChainCount) // +1 is needed.
					{
						printf("Corrupted index detected: Not covering the entire file\n");
						exit(-1);
					}
					if(pIndex[nIndexSize - 1].nFirstChain + pIndex[nIndexSize - 1].nChainCount > nTotalChainCount) // +1 is not needed here
					{
						printf("Corrupted index detected: The index is covering more than the file. Covering %u of %u chains\n", pIndex[nIndexSize - 1].nFirstChain + pIndex[nIndexSize - 1].nChainCount, nTotalChainCount);
						exit(-1);
					}
					fclose(fIndex);
					delete pData;
					printf("Index loaded successfully\n");
				}		
			}
			else 
			{
				printf("Can't load index\n");
				return;
			}
/*			if(hs.GetStatHashTotal() > 10)
			{*/
				static CMemoryPool mp;
				unsigned int nAllocatedSize;
				RainbowChain* pChain = (RainbowChain*)mp.Allocate(nFileLen * 2, nAllocatedSize);
				if (pChain != NULL)
				{
					nAllocatedSize = nAllocatedSize / 16 * 16;		// Round to 16-byte boundary

					fseek(file, 0, SEEK_SET);
					bool fVerified = false;
					int nProcessedChains = 0;
					while (true)	// Chunk read loop
					{
						if (ftell(file) == nFileLen)
							break;

						// Load table chunk
						memset(pChain, 0x00, nAllocatedSize);
						printf("reading...\n");
						unsigned char *pData = new unsigned char[8];
						unsigned int nDataRead = 0;
						unsigned int nRead = 0;
						clock_t t1 = clock();
						for(int i = 0; i < nAllocatedSize / 16; i++) // Chain size is 16 bytes
						{
							nRead = fread(pData, 1, 8, file);
							if(nRead == 8)
							{
								nDataRead += nRead;
								memcpy(&pChain[i].nIndexS, &pData[0], 6);
								memcpy(&pChain[i].nIndexE, &pData[6], 2);
//								memcpy(&pChain[i].nCheckPoint, &pData[7], 1);						
							}
							else break;
						}
						clock_t t2 = clock();
						delete pData;
	//					unsigned int nDataRead = fread(pChain, 1, nAllocatedSize, file);
						float fTime = 1.0f * (t2 - t1) / CLOCKS_PER_SEC;
						printf("%u bytes read, disk access time: %.2f s\n", nDataRead, fTime);
						m_fTotalDiskAccessTime += fTime;
						int nRainbowChainCountRead = nDataRead / 8;
						// Verify table chunk
						
						if (!fVerified)
						{
							printf("verifying the file...\n");

							// Chain length test
							int nIndexToVerify = nRainbowChainCountRead / 2;
/*							CChainWalkContext cwc;
							cwc.SetIndex(pChain[nIndexToVerify].nIndexS);
							int nPos;
							for (nPos = 0; nPos < nRainbowChainLen - 1; nPos++)
							{
								cwc.IndexToPlain();
								cwc.PlainToHash();
								cwc.HashToIndex(nPos);
							}
							
							uint64 nEndPoint = 0;
							for(int i = 0; i < nIndexSize; i++)
							{
								if(nIndexToVerify >= pIndex[i].nFirstChain && nIndexToVerify < pIndex[i].nFirstChain + pIndex[i].nChainCount) // We found the matching index
								{ // Now we need to seek nIndexToVerify into the chains
									nEndPoint += pIndex[i].nPrefix << 16;
									nEndPoint += pChain[nIndexToVerify].nIndexE;
									break;
								}
							}
							if (cwc.GetIndex() != nEndPoint)
							{
								printf("rainbow chain length verify fail\n");
								break;
							}
*/
							// Chain sort test
							// We assume its sorted in the indexing process
							/*
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
							*/
							fVerified = true;
						}
	
						// Search table chunk
						t1 = clock();
						SearchTableChunk(pChain, nRainbowChainLen, nRainbowChainCountRead, hs, pIndex, nIndexSize, nProcessedChains);
						t2 = clock();
						fTime = 1.0f * (t2 - t1) / CLOCKS_PER_SEC;
						printf("cryptanalysis time: %.2f s\n", fTime);
						m_fTotalCryptanalysisTime += fTime;
						nProcessedChains += nRainbowChainCountRead;
						// Already finished?
						if (!hs.AnyHashLeftWithLen(CChainWalkContext::GetHashLen()))
							break;
					}
				}
				else printf("memory allocation fail\n");
			//}
/*			else // So few hashes to search for. No need to load the entire chain file.
			{
				clock_t t1 = clock();
				m_fChains = file;
				SearchTableChunk(NULL, nRainbowChainLen, 0, hs, pIndex, nIndexSize, 0);
				clock_t t2 = clock();
				float fTime = 1.0f * (t2 - t1) / CLOCKS_PER_SEC;
				printf("cryptanalysis time: %.2f s\n", fTime);
				m_fTotalCryptanalysisTime += fTime;

			}*/

			delete pIndex;
		}
		fclose(file);
	}
	else
		printf("can't open file\n");
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
