#ifndef __BASERTREADER_H__
#define __BASERTREADER_H__

#include "Public.h"
#include <string>
#ifdef WIN32
#include <io.h>
#endif
using namespace std;

class BaseRTReader
{
public:
	virtual int ReadChains(unsigned int &numChains, RainbowChainCP *pData) = 0;
	virtual unsigned int GetChainsLeft() = 0;
	
};

#endif
