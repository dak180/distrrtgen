#ifndef RCUDA_EXT_H
#define RCUDA_EXT_H

#include "rcuda.h"
#include "ChainWalkContext.h"

class CudaCWCExtender {
public:
	CudaCWCExtender(CChainWalkContext *cwc);
	void Init(void);
	int IndexToStartPlain(const uint64 nIndex, std::vector<unsigned char>& stPlain);

	inline rcuda::RHash GetHash(void) { return hash; }
	inline unsigned int* GetPlainDimVec(void) { return &plainDimVec[0]; }
	inline int GetPlainDimVecSize(void) { return plainDimVec.size(); }
	inline unsigned char* GetCharSet(void) { return (unsigned char*)plainCharSet.c_str(); }
	inline int GetCharSetSize(void) { return plainCharSet.size(); }
	inline uint64 GetPlainSpaceTotal(void) { return CChainWalkContext::m_nPlainSpaceTotal; }
	inline uint64 GetRainbowTableIndex(void) { return CChainWalkContext::m_nRainbowTableIndex; }
	inline uint64 GetReduceOffset(void) { return CChainWalkContext::m_nReduceOffset; }

protected:
	CChainWalkContext *cwc;
	rcuda::RHash hash;
	std::vector<unsigned int> plainDimVec;
	std::string plainCharSet;
};

#endif //RCUDA_EXT_H
