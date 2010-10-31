#include "RTReader.h"

RTReader::RTReader(string Filename)
{
	m_pFile = fopen(Filename.c_str(), "rb");
}

RTReader::~RTReader(void)
{
}

int RTReader::ReadChains(unsigned int &numChains, RainbowChain *pData)
{
	unsigned int numRead = fread(pData, 1, 16 * numChains, m_pFile);
	numChains = numRead / 16;
	m_chainPosition += numChains;
	return 0;
}
UINT4 RTReader::GetChainsLeft()
{
	return (GetFileLen(m_pFile) / 16) - m_chainPosition;
}
