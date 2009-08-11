#include "RTReader.h"

RTReader::RTReader(string Filename)
{
	m_pFile = fopen(Filename.c_str(), "rb");
}

RTReader::~RTReader(void)
{
}

int RTReader::ReadChains(unsigned int &numChains, RainbowChainCP *pData)
{
	unsigned int numRead = fread(pData, 1, 16 * numChains, m_pFile);
	numChains = numRead / 16;
	return 0;
}
unsigned int RTReader::GetChainsLeft()
{
	unsigned int len = GetFileLen(m_pFile);
	return len / 16 - m_chainPosition;
}
