#ifndef __RTIREADER_H__
#define __RTIREADER_H__

#include <string>
#ifdef WIN32
#include <io.h>
#endif
#include "Public.h"
#include "BaseRTReader.h"
using namespace std;

class RTIReader : BaseRTReader
{
private:
	FILE *m_pFile;
	unsigned int m_chainPosition;
	unsigned int m_nIndexSize;
	IndexChain *m_pIndex;
public:
	RTIReader(string Filename);
	~RTIReader(void);

	int ReadChains(unsigned int &numChains, RainbowChainCP *pData);
	unsigned int GetChainsLeft();

};

#endif
