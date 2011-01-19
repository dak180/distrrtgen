//============================================================================
// Name        : rcuda.cu
// Author      : Jan Kyska
// Version     : 1.00
// Description : Generator of FreeRainbowTables / MD5, MD4, NTLM, SHA1, LM
//============================================================================ 

#include <stdio.h>  
#include <cuda.h>  
#include "rcuda.h"

#define   GRID_X_L2     6
#define   GRID_Y_L2     6
#define   BLOCK_X_L2    6
#define   GRID_X_SIZE   (1<<GRID_X_L2)
#define   GRID_Y_SIZE   (1<<GRID_Y_L2)
#define   BLOCK_X_SIZE  (1<<BLOCK_X_L2)
#define   PLAIN_MAX_SIZE     20
#define   KERN_CHAIN_SIZE   100
#define   CHAR_SET_MAXLEN   512
#define   SHIDX(x)      ((x)<<4)


__device__ uint64 *dataHeap;
__device__ unsigned char *plStart;
__device__ uint3 *plDimVec;
__device__ unsigned char *plChrSet;
__device__ int *plCpPos;
__device__ int plCpPosSize;
__device__ uint64 reduceOffset;
__device__ uint64 plainSpaceTotal;
__device__ uint64 rPlainSpaceTotal;

#define RTGEN_PROLOGUE  \
	unsigned int *hData;  \
	unsigned int idx;  \
	uint64 idx64;  \
	unsigned int nPos, ii;  \
	unsigned int cpcheck, checkpoint;  \
	\
	__shared__ unsigned int shData[SHIDX(BLOCK_X_SIZE)];  \
	__shared__ unsigned char cplChrSet[CHAR_SET_MAXLEN];  \
	__shared__ unsigned char cplStart[PLAIN_MAX_SIZE];  \
	__shared__ uint3 cplDimVec[PLAIN_MAX_SIZE];  \
	\
	if(threadIdx.x == 0) {  \
		nPos = ((((blockIdx.y<<GRID_X_L2) + blockIdx.x)<<BLOCK_X_L2) + threadIdx.x) << 1;  \
		for(ii = 0; ii < BLOCK_X_SIZE; ii++, nPos+=2) {  \
			hData = shData + ((ii>>4)<<8)+(ii&15);  \
			hData[SHIDX(0)] = dataHeap[nPos];  \
			hData[SHIDX(1)] = dataHeap[nPos]>>32;  \
			hData[SHIDX(2)] = dataHeap[nPos+1];  \
		}  \
		memcpy(cplChrSet, plChrSet, CHAR_SET_MAXLEN);  \
		memcpy(cplStart, plStart, PLAIN_MAX_SIZE);  \
		memcpy(cplDimVec, plDimVec, PLAIN_MAX_SIZE*sizeof(uint3));  \
	}  \
	__syncthreads();  \
	\
	hData = shData + ((threadIdx.x>>4)<<8)+(threadIdx.x&15);  \
	\
	idx64 = hData[SHIDX(1)];  \
	idx64 = (idx64<<32) | hData[SHIDX(0)];  \
	cpcheck = hData[SHIDX(2)];  \
	checkpoint = cpcheck&0x0000ffff;  \
	cpcheck = cpcheck>>16;  \
	\
	for(nPos = chainStart; nPos < chainStop; nPos++) {


#define RTGEN_EPILOGUE  \
		idx64 = hData[SHIDX(1)];  \
		idx64 = (idx64<<32) | hData[SHIDX(0)];  \
		idx64 += reduceOffset + nPos;  \
		uiDiv64 = __umul64hi(idx64, rPlainSpaceTotal);  \
		idx64 -= uiDiv64*plainSpaceTotal;  \
		if(idx64 >= plainSpaceTotal)  \
			idx64 -= plainSpaceTotal;  \
		\
		if(cpcheck < plCpPosSize && nPos == plCpPos[cpcheck]) {  \
			checkpoint |= ((unsigned int)idx64&1) << cpcheck;  \
			cpcheck++;  \
		}  \
	}  \
	\
	hData[SHIDX(0)] = idx64;  \
	hData[SHIDX(1)] = idx64>>32;  \
	hData[SHIDX(2)] = (cpcheck<<16)|(checkpoint&0x0000ffff);  \
	__syncthreads();  \
	\
	if(threadIdx.x == 0) {  \
		nPos = ((((blockIdx.y<<GRID_X_L2) + blockIdx.x)<<BLOCK_X_L2) + threadIdx.x) << 1;  \
		for(ii = 0; ii < BLOCK_X_SIZE; ii++, nPos+=2) {  \
			hData = shData + ((ii>>4)<<8)+(ii&15);  \
			dataHeap[nPos] = ((uint64)hData[SHIDX(1)]<<32)|(uint64)hData[SHIDX(0)];  \
			dataHeap[nPos+1] = hData[SHIDX(2)];  \
		}  \
	}


#include "rcuda_md5.inc"
#include "rcuda_md4.inc"
#include "rcuda_ntlm.inc"
#include "rcuda_sha1.inc"
#include "rcuda_lm.inc"

extern "C" int CalcChainsOnCUDA(const rcuda::RCudaTask* task, uint64 *resultBuff) {
	cudaError_t cuErr;
	char buff[PLAIN_MAX_SIZE];
	uint64 *data;
	unsigned char *stPlain;
	uint3 *dimVec;
	unsigned char *charSet;
	int *cpPos;
	uint64 uiVal64;

	if(task->charSetSize > CHAR_SET_MAXLEN)
		return -1;

	switch(task->hash) {
	case rcuda::RHASH_MD5:
	case rcuda::RHASH_MD4:
	case rcuda::RHASH_NTLM:
	case rcuda::RHASH_SHA1:
	case rcuda::RHASH_LM:
		break;
	default:	
		return 0;
	}
	
	memset(buff, 0, PLAIN_MAX_SIZE);
	cudaMalloc((void**)&data, task->idxCount*2*sizeof(uint64));
	cudaMalloc((void**)&stPlain, PLAIN_MAX_SIZE);
	cudaMalloc((void**)&dimVec, max(task->dimVecSize, PLAIN_MAX_SIZE)*sizeof(uint3));
	cudaMalloc((void**)&charSet, CHAR_SET_MAXLEN);
	cudaMalloc((void**)&cpPos, task->cpPosSize*sizeof(int));

	cudaMemcpy(data, resultBuff, task->idxCount*2*sizeof(uint64), cudaMemcpyHostToDevice);
	cudaMemcpy(stPlain, buff, PLAIN_MAX_SIZE, cudaMemcpyHostToDevice);
	cudaMemcpy(stPlain, task->stPlain, min(task->stPlainSize, PLAIN_MAX_SIZE), cudaMemcpyHostToDevice);
	cudaMemcpy(dimVec, task->dimVec, min(task->dimVecSize, PLAIN_MAX_SIZE)*sizeof(uint3), cudaMemcpyHostToDevice);
	cudaMemcpy(charSet, task->charSet, min(task->charSetSize, CHAR_SET_MAXLEN), cudaMemcpyHostToDevice);
	cudaMemcpy(cpPos, task->cpPositions, task->cpPosSize*sizeof(int), cudaMemcpyHostToDevice);

	cudaMemcpyToSymbol(dataHeap, &data, sizeof(data));
	cudaMemcpyToSymbol(plStart, &stPlain, sizeof(stPlain));
	cudaMemcpyToSymbol(plDimVec, &dimVec, sizeof(dimVec));
	cudaMemcpyToSymbol(plChrSet, &charSet, sizeof(charSet));
	cudaMemcpyToSymbol(plCpPos, &cpPos, sizeof(cpPos));
	cudaMemcpyToSymbol(plCpPosSize, &task->cpPosSize, sizeof(task->cpPosSize));
	cudaMemcpyToSymbol(reduceOffset, &task->reduceOffset, sizeof(task->reduceOffset));
	cudaMemcpyToSymbol(plainSpaceTotal, &task->plainSpaceTotal, sizeof(task->plainSpaceTotal));
	uiVal64 = (uint64)-1/task->plainSpaceTotal;
	cudaMemcpyToSymbol(rPlainSpaceTotal, &uiVal64, sizeof(uiVal64));

	int grSizeX = (task->idxCount-1)/BLOCK_X_SIZE + 1;
	int grSizeY = (grSizeX-1)/GRID_X_SIZE + 1;
	grSizeX = GRID_X_SIZE;
	dim3 numBlocks(grSizeX, grSizeY);
	cuErr = cudaSuccess;
	
	cudaSetDeviceFlags(cudaDeviceBlockingSync);

	for(int idx = 0; idx < task->rainbowChainLen-1 && cuErr == cudaSuccess; idx+=KERN_CHAIN_SIZE) {
		switch(task->hash) {
		case rcuda::RHASH_MD5:
			RTGenMD5Kernel<<<numBlocks, BLOCK_X_SIZE>>>(idx, min(idx+KERN_CHAIN_SIZE, task->rainbowChainLen-1));
			break;
		case rcuda::RHASH_MD4:
			RTGenMD4Kernel<<<numBlocks, BLOCK_X_SIZE>>>(idx, min(idx+KERN_CHAIN_SIZE, task->rainbowChainLen-1));
			break;
		case rcuda::RHASH_NTLM:
			RTGenNTLMKernel<<<numBlocks, BLOCK_X_SIZE>>>(idx, min(idx+KERN_CHAIN_SIZE, task->rainbowChainLen-1));
			break;
		case rcuda::RHASH_SHA1:
			RTGenSHA1Kernel<<<numBlocks, BLOCK_X_SIZE>>>(idx, min(idx+KERN_CHAIN_SIZE, task->rainbowChainLen-1));
			break;
		case rcuda::RHASH_LM:
			RTGenLMKernel<<<numBlocks, BLOCK_X_SIZE>>>(idx, min(idx+KERN_CHAIN_SIZE, task->rainbowChainLen-1));
			break;
		}
		cuErr = cudaGetLastError();
		if(cuErr == cudaSuccess)
			cuErr = cudaThreadSynchronize();
	}

	if(cuErr == cudaSuccess)
		cudaMemcpy(resultBuff, data, task->idxCount*2*sizeof(uint64), cudaMemcpyDeviceToHost);
	else
		fprintf(stderr, "Error happened: %d (%s)\n", cuErr, cudaGetErrorString(cuErr));	

	cudaFree(cpPos);
	cudaFree(charSet);
	cudaFree(dimVec);
	cudaFree(stPlain);
	cudaFree(data);
	return cuErr==cudaSuccess? task->idxCount : -1;
}

extern "C" int GetChainsBufferSize(int minSize) {
	int grSizeX = (minSize-1)/BLOCK_X_SIZE + 1;
	int grSizeY = (grSizeX-1)/GRID_X_SIZE + 1;
	grSizeX = GRID_X_SIZE;
	return grSizeX*grSizeY*BLOCK_X_SIZE;
}

extern "C" int SetCudaDevice(int device) {
	return cudaSetDevice(device);
}
