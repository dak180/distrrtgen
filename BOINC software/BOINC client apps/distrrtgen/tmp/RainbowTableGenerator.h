#ifndef __RAINBOWTABLEGENERATOR_H_
#define __RAINBOWTABLEGENERATOR_H_


class CRainbowTableGenerator
{
private:

	//void CRainbowTableGenerator::GetCharset(std::string sCharsetName, char **pCharset);
public:
	CRainbowTableGenerator();
public:
	~CRainbowTableGenerator(void);
	int CalculateTable(std::string sFilename, int nRainbowChainCount, std::string sHashRoutineName, std::string sCharsetName, int nPlainLenMin, int nPlainLenMax, int nRainbowTableIndex, int nRainbowChainLen, uint64 nChainStart, std::string sSalt = "");
private:
	int m_nCurrentCalculatedChains;
};

#endif
