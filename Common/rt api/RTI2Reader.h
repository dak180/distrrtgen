#ifndef __RTI2READER_H__
#define __RTI2READER_H__

#include "Public.h"
#include <string>
#ifdef WIN32
#include <io.h>
#endif
#include <vector>
#include "BaseRTReader.h"
using namespace std;

typedef struct 
{
	char header[4];
	unsigned char rti_startptlength, rti_endptlength, rti_cplength, rti_index_numchainslength;
	uint64 prefixstart;
	unsigned int *m_cppos;
} RTI2Header;

class RTI2Reader : BaseRTReader
{
private:
	FILE *m_pFile;
	unsigned int m_chainPosition;
	unsigned char *m_pPos, *m_pChainPos;
	static RTI2Header *m_pHeader;
	unsigned char *m_pIndex;
	unsigned int m_chainsizebytes;
	unsigned int m_indexrowsizebytes;
	

public:
	RTI2Reader(string Filename);
	~RTI2Reader(void);
	int ReadChains(UINT4 &numChains, RainbowChain *pData);
	unsigned int GetChainsLeft();
	static RTI2Header *GetHeader() { return m_pHeader; }
};


#endif
