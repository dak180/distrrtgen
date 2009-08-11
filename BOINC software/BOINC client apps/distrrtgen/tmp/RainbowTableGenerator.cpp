
#include "RainbowTableGenerator.h"
#include "ChainWalkContext.h"
#include <iostream>
#include <sstream>
#include <time.h>
#include <conio.h>

CRainbowTableGenerator::CRainbowTableGenerator()
{
	m_nCurrentCalculatedChains = 0;
}

CRainbowTableGenerator::~CRainbowTableGenerator(void)
{
}

int CRainbowTableGenerator::CalculateTable(std::string sFilename, int nRainbowChainCount, std::string sHashRoutineName, std::string sCharsetName, int nPlainLenMin, int nPlainLenMax, int nRainbowTableIndex, int nRainbowChainLen, uint64 nChainStart, std::string sSalt)
{
	return 0;
}

