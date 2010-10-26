#ifndef RCUDA_H
#define RCUDA_H

namespace rcuda {

enum RHash { RHASH_UNDEF = -1, RHASH_LM, RHASH_MD4, RHASH_MD5, RHASH_SHA1, RHASH_NTLM };

struct RCudaTask {
	RHash hash;
	unsigned __int64 startIdx;
	int idxCount;
	unsigned char* stPlain;
	int stPlainSize;
	unsigned int* dimVec;
	int dimVecSize;
	unsigned char* charSet;
	int charSetSize;
	int *cpPositions;
	int cpPosSize;
	unsigned __int64 reduceOffset;
	unsigned __int64 plainSpaceTotal;
	unsigned int rainbowChainLen;
};

extern "C" int CalcChainsOnCUDA(const RCudaTask* task, unsigned __int64 *resultBuff);
extern "C" int GetChainsBufferSize(int minSize);

}

#endif //RCUDA_H
