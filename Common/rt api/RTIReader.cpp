#include "RTIReader.h"

RTIReader::RTIReader(string Filename)
{
	m_pIndex = NULL;
	m_pFile = fopen(Filename.c_str(), "rb");
	if(m_pFile == NULL) {
		printf("could not open file %s\n", Filename.c_str());
		return;		
	}
	string sIndex = Filename.append(".index").c_str();
	FILE *pFileIndex = fopen(sIndex.c_str(), "rb");
	if(pFileIndex == NULL) {
		printf("could not open index file %s\n", sIndex.c_str());
		return;
	}
	m_chainPosition = 0;

	// Load the index file
	long nIndexFileLen = GetFileLen(pFileIndex);
	long nFileLen = GetFileLen(m_pFile);
	unsigned int nTotalChainCount = nFileLen / 8;
	if (nFileLen % 8 != 0)
		printf("file length mismatch (%ld bytes)\n", nFileLen);
	else
	{
		// File length check
		if (nIndexFileLen % 11 != 0)
			printf("index file length mismatch (%ld bytes)\n", nIndexFileLen);
		else
		{
			m_pIndex = new IndexChain[nIndexFileLen / 11];
			memset(m_pIndex, 0x00, sizeof(IndexChain) * (nIndexFileLen / 11));
			fseek(pFileIndex, 0, SEEK_SET);
			int nRows;
			for(nRows = 0; (nRows * 11) < nIndexFileLen; nRows++)
			{
				if(fread(&m_pIndex[nRows].nPrefix, 5, 1, pFileIndex) != 1) break;							
				if(fread(&m_pIndex[nRows].nFirstChain, 4, 1, pFileIndex) != 1) break;							
				if(fread(&m_pIndex[nRows].nChainCount, 2, 1, pFileIndex) != 1) break;							
				// Index checking part
				if(nRows != 0 && m_pIndex[nRows].nFirstChain < m_pIndex[nRows-1].nFirstChain)
				{
					printf("Corrupted index detected (FirstChain is lower than previous)\n");
					exit(-1);
				}
				else if(nRows != 0 && m_pIndex[nRows].nFirstChain != m_pIndex[nRows-1].nFirstChain + m_pIndex[nRows-1].nChainCount)
				{
					printf("Corrupted index detected (LastChain + nChainCount != FirstChain)\n");
					exit(-1);
				}
				
			}
			m_nIndexSize = nRows;
			if(m_pIndex[m_nIndexSize - 1].nFirstChain + m_pIndex[m_nIndexSize - 1].nChainCount + 1 <= nTotalChainCount) // +1 is needed.
			{
				printf("Corrupted index detected: Not covering the entire file\n");
				exit(-1);
			}
			if(m_pIndex[m_nIndexSize - 1].nFirstChain + m_pIndex[m_nIndexSize - 1].nChainCount > nTotalChainCount) // +1 is not needed here
			{
				printf("Corrupted index detected: The index is covering more than the file\n");
				exit(-1);
			}

	/*					if(nIndexSize != pIndex[i].nFirstChain + pIndex[i].nChainCount)
			{
				printf("Index is not covering the entire tables\n");
			}*/
			fclose(pFileIndex);		
	//					printf("debug: Index loaded successfully (%u entries)\n", nIndexSize);
		}		
	}


}

int RTIReader::ReadChains(unsigned int &numChains, RainbowChainCP *pData)
{	
	// We HAVE to reset the data to 0x00's or we will get in trouble
	memset(pData, 0x00, sizeof(RainbowChainCP) * numChains);
	unsigned int readChains = 0;
	unsigned int chainsleft = GetChainsLeft();
	for(unsigned int i = 0; i < m_nIndexSize; i++)
	{
		if(m_chainPosition + readChains > m_pIndex[i].nFirstChain + m_pIndex[i].nChainCount) // We found the matching index
			continue;
		while(m_chainPosition + readChains < m_pIndex[i].nFirstChain + m_pIndex[i].nChainCount)
		{
			pData[readChains].nIndexE = m_pIndex[i].nPrefix << 16;
			int endpoint = 0; // We have to set it to 0
			fread(&pData[readChains].nIndexS, 6, 1, m_pFile);
			fread(&endpoint, 2, 1, m_pFile);
			pData[readChains].nIndexE += endpoint;
			readChains++;
			if(readChains == numChains || readChains == chainsleft) break;
		}
		if(readChains == numChains) break;		
	}
	if(readChains != numChains) numChains = readChains; // Update how many chains we read
	m_chainPosition += readChains;
	return 0;
}

unsigned int RTIReader::GetChainsLeft()
{
	int len = GetFileLen(m_pFile) / 8 - m_chainPosition;
	return len;
}

RTIReader::~RTIReader(void)
{
	if(m_pIndex != NULL)
		delete m_pIndex;
	if(m_pFile != NULL)
		fclose(m_pFile);

}
