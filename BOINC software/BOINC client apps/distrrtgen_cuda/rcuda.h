#ifndef RCUDA_H
#define RCUDA_H

#include "Public.h"

namespace rcuda {

enum RHash { RHASH_UNDEF = -1, RHASH_LM, RHASH_MD4, RHASH_MD5, RHASH_SHA1, RHASH_NTLM };

struct RCudaTask {
	RHash hash;
	uint64 startIdx;
	int idxCount;
	unsigned char* stPlain;
	int stPlainSize;
	unsigned int* dimVec;
	int dimVecSize;
	unsigned char* charSet;
	int charSetSize;
	int *cpPositions;
	int cpPosSize;
	uint64 reduceOffset;
	uint64 plainSpaceTotal;
	unsigned int rainbowChainLen;
};

extern "C" int SetCudaDevice(int device);
extern "C" int GetChainsBufferSize(int minSize);
extern "C" int CalcChainsOnCUDA(const RCudaTask* task, uint64 *resultBuff);

}

#endif //RCUDA_H
