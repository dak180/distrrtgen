#ifndef __RTREADER_H__
#define __RTREADER_H__

#include <string>
#ifdef WIN32
#include <io.h>
#endif
#include "Public.h"
#include "BaseRTReader.h"
using namespace std;

class RTReader : BaseRTReader
{
private:
	FILE *m_pFile;
	unsigned int m_chainPosition;

public:
	RTReader(string Filename);
	~RTReader(void);
	int ReadChains(unsigned int &numChains, RainbowChainCP *pData);
	unsigned int GetChainsLeft();

};

#endif
