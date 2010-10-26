#include <stdio.h>  
#include <cuda.h>  
#include "rcuda.h"

#define   GRID_X_L2     6
#define   GRID_Y_L2     6
#define   BLOCK_X_L2    7
#define   GRID_X_SIZE   (1<<GRID_X_L2)
#define   GRID_Y_SIZE   (1<<GRID_Y_L2)
#define   BLOCK_X_SIZE  (1<<BLOCK_X_L2)
#define   KERN_CHAIN_SIZE   100
#define   SHIDX(x)      ((x)<<4)


__device__ __constant__ unsigned int h[4] = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476 };
__device__ __constant__ unsigned char r[64] = { \
							7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22, \
							5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20, \
							4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23, \
							6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21 };
__device__ __constant__ unsigned char g[64] = { \
							0, 1,  2,  3,   4,  5,  6,  7,   8,  9, 10, 11,  12, 13, 14, 15, \
							1, 6, 11,  0,   5, 10, 15,  4,   9, 14,  3,  8,  13,  2,  7, 12, \
							5, 8, 11, 14,   1,  4,  7, 10,  13,  0,  3,  6,   9, 12, 15,  2, \
							0, 7, 14,  5,  12,  3, 10,  1,   8, 15,  6, 13,   4, 11,  2,  9 };
__device__ __constant__ unsigned int ac[64] = { \
							0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, \
							0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501, \
							0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, \
							0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, \
							\
							0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, \
							0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8, \
							0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, \
							0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, \
							\
							0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, \
							0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, \
							0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, \
							0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, \
							\
							0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, \
							0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1, \
							0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, \
							0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

__device__ __constant__ unsigned int testData[16] = { \
							0x79706d63, 0x6d627667, 0x00000080, 0x00000000, \
							0x00000000, 0x00000000, 0x00000000, 0x00000000, \
							0x00000000, 0x00000000, 0x00000000, 0x00000000, \
							0x00000000, 0x00000000, 0x00000040, 0x00000000 };

__device__ unsigned int FF(unsigned int a, unsigned int b, unsigned int c, unsigned int d, int i, const unsigned int* data) {
	unsigned int ret;
	ret = a + ((b&c)|((~b)&d)) + ac[i] + data[SHIDX(g[i])];
	ret = (ret<<r[i])|(ret>>(32-r[i]));
	ret += b;
	return ret;
}

__device__ unsigned int GG(unsigned int a, unsigned int b, unsigned int c, unsigned int d, int i, const unsigned int* data) {
	unsigned int ret;
	ret = a + ((b&d)|(c&(~d))) + ac[i] + data[SHIDX(g[i])];
	ret = (ret<<r[i])|(ret>>(32-r[i]));
	ret += b;
	return ret;
}

__device__ unsigned int HH(unsigned int a, unsigned int b, unsigned int c, unsigned int d, int i, const unsigned int* data) {
	unsigned int ret;
	ret = a + (b^c^d) + ac[i] + data[SHIDX(g[i])];
	ret = (ret<<r[i])|(ret>>(32-r[i]));
	ret += b;
	return ret;
}

__device__ unsigned int II(unsigned int a, unsigned int b, unsigned int c, unsigned int d, int i, const unsigned int* data) {
	unsigned int ret;
	ret = a + (c^(b|(~d))) + ac[i] + data[SHIDX(g[i])];
	ret = (ret<<r[i])|(ret>>(32-r[i]));
	ret += b;
	return ret;
}

__device__ void MD5(unsigned int* dataHash) {
	unsigned int a = h[0], b = h[1], c = h[2], d = h[3], x;
	int ii;

	// Round 1
	for(ii = 0; ii < 16; ii++) {
		x = b;
		b = FF(a, b, c, d, ii, dataHash);
		a = d; d = c; c = x;
	}

	// Round 2
	for(; ii < 32; ii++) {
		x = b;
		b = GG(a, b, c, d, ii, dataHash);
		a = d; d = c; c = x;
	}
	
	// Round 3
	for(; ii < 48; ii++) {
		x = b;
		b = HH(a, b, c, d, ii, dataHash);
		a = d; d = c; c = x;
	}
	
	// Round 4
	for(; ii < 64; ii++) {
		x = b;
		b = II(a, b, c, d, ii, dataHash);
		a = d; d = c; c = x;
	}

	dataHash[SHIDX(0)] = a + h[0];
	dataHash[SHIDX(1)] = b + h[1];
	dataHash[SHIDX(2)] = c + h[2];
	dataHash[SHIDX(3)] = d + h[3];
}


#define   PLAIN_MAX_SIZE     20

__device__ unsigned __int64 *dataHeap;
__device__ unsigned char *plStart;
__device__ uint3 *plDimVec;
__device__ unsigned char *plChrSet;
__device__ int *plCpPos;
__device__ int plCpPosSize;
__device__ unsigned __int64 reduceOffset;
__device__ unsigned __int64 plainSpaceTotal;
__device__ unsigned __int64 rPlainSpaceTotal;


__global__ void RTGenMD5Kernel(unsigned int chainStart, unsigned int chainStop) {
	unsigned int *hData;
	uint3 dimItem;
	unsigned int uiVal, uiMul, uiDiv, idx;
	unsigned __int64 uiVal64, uiMul64, uiDiv64, idx64;
	unsigned int nPos, size, ii, jj, kk;
	unsigned int cpcheck, checkpoint;
	unsigned int plain;

	__shared__ unsigned int shData[SHIDX(BLOCK_X_SIZE)];

	if(threadIdx.x == 0) {
		nPos = ((((blockIdx.y<<GRID_X_L2) + blockIdx.x)<<BLOCK_X_L2) + threadIdx.x) << 1;
		for(ii = 0; ii < BLOCK_X_SIZE; ii++, nPos+=2) {
			hData = shData + ((ii>>4)<<8)+(ii&15);
			hData[SHIDX(0)] = dataHeap[nPos];
			hData[SHIDX(1)] = dataHeap[nPos]>>32;
			hData[SHIDX(2)] = dataHeap[nPos+1];
		}
	}
	__syncthreads();

	hData = shData + ((threadIdx.x>>4)<<8)+(threadIdx.x&15);
	
	idx64 = hData[SHIDX(1)];
	idx64 = (idx64<<32) | hData[SHIDX(0)];
	cpcheck = hData[SHIDX(2)];
	checkpoint = cpcheck&0x0000ffff;
	cpcheck = cpcheck>>16;

	for(nPos = chainStart; nPos < chainStop; nPos++) {
		// transform to the plain text
		plain = 0x80;
		jj = (PLAIN_MAX_SIZE>>2)+1;
		for(ii = 0; idx64 > 0xfffffff0ull && ii < PLAIN_MAX_SIZE; ii++) {
			uiVal64 = idx64 + plStart[ii];
			uiVal64--;
			dimItem = plDimVec[ii];
			uiMul64 = (unsigned __int64)dimItem.y<<32;
			idx64 = __umul64hi(uiVal64, uiMul64);
			uiDiv64 = uiVal64 - idx64*(unsigned __int64)dimItem.x;
			uiVal = __umulhi((unsigned int)uiDiv64, dimItem.y);
			uiDiv = (unsigned int)uiDiv64 - uiVal * dimItem.x;
			idx64 += uiVal;
			if(uiDiv >= dimItem.x) {
				uiDiv -= dimItem.x;
				idx64++;
			}
			plain = (plain<<8) | plChrSet[dimItem.z + uiDiv];
			if((ii&3) == 2) {
				hData[SHIDX(jj--)] = plain;
				plain = 0;
			}
		}
		for(idx = (unsigned int)idx64; idx != 0 && ii < PLAIN_MAX_SIZE; ii++) {
			uiVal = idx + plStart[ii];
			uiVal--;
			dimItem = plDimVec[ii];
			idx = __umulhi(uiVal, dimItem.y);
			uiDiv = uiVal - idx*dimItem.x;
			if(uiDiv >= dimItem.x) {
				uiDiv -= dimItem.x;
				idx++;
			}
			plain = (plain<<8) | plChrSet[dimItem.z + uiDiv];
			if((ii&3) == 2) {
				hData[SHIDX(jj--)] = plain;
				plain = 0;
			}
		}

		// prepare for MD5
		size = ii;
		ii = ((((3-(ii&3))<<3)-1)&0x1f)+1;
		plain = plain<<ii;
		for(jj++, kk = 0; jj <= (PLAIN_MAX_SIZE>>2)+1; plain = hData[SHIDX(jj++)], kk++)
			hData[SHIDX(kk)] = (plain>>ii)|(hData[SHIDX(jj)]<<(32-ii));
		hData[SHIDX(kk)] = plain>>ii;
		for(kk++; kk < 14; kk++)
			hData[SHIDX(kk)] = 0;
		hData[SHIDX(kk++)] = size<<3;
		hData[SHIDX(kk)] = 0;

		// hash
		MD5(hData);

		idx64 = hData[SHIDX(1)];
		idx64 = (idx64<<32) | hData[SHIDX(0)];
		idx64 += reduceOffset + nPos;
		uiDiv64 = __umul64hi(idx64, rPlainSpaceTotal);
		idx64 -= uiDiv64*plainSpaceTotal;
		if(idx64 >= plainSpaceTotal)
			idx64 -= plainSpaceTotal;
			
		if(cpcheck < plCpPosSize && nPos == plCpPos[cpcheck]) {
			checkpoint |= ((unsigned int)idx64&1) << cpcheck;
			cpcheck++;
		}
	}

	hData[SHIDX(0)] = idx64;
	hData[SHIDX(1)] = idx64>>32;
	hData[SHIDX(2)] = (cpcheck<<16)|(checkpoint&0x0000ffff);
	__syncthreads();
	
	if(threadIdx.x == 0) {
		nPos = ((((blockIdx.y<<GRID_X_L2) + blockIdx.x)<<BLOCK_X_L2) + threadIdx.x) << 1;
		for(ii = 0; ii < BLOCK_X_SIZE; ii++, nPos+=2) {
			hData = shData + ((ii>>4)<<8)+(ii&15);
			dataHeap[nPos] = ((unsigned __int64)hData[SHIDX(1)]<<32)|(unsigned __int64)hData[SHIDX(0)];
			dataHeap[nPos+1] = hData[SHIDX(2)];
		}
	}
	__syncthreads();
}


extern "C" int CalcChainsOnCUDA(const rcuda::RCudaTask* task, unsigned __int64 *resultBuff) {
	cudaError_t cuErr;
	char buff[PLAIN_MAX_SIZE];
	unsigned __int64 *data;
	unsigned char *stPlain;
	uint3 *dimVec;
	unsigned char *charSet;
	int *cpPos;
	unsigned __int64 uiVal64;
	time_t tStart, tEnd;
	if(task->hash != rcuda::RHASH_MD5)
		return 0;

	memset(buff, 0, PLAIN_MAX_SIZE);
	cudaMalloc((void**)&data, task->idxCount*2*sizeof(unsigned __int64));
	cudaMalloc((void**)&stPlain, PLAIN_MAX_SIZE);
	cudaMalloc((void**)&dimVec, task->dimVecSize*sizeof(uint3));
	cudaMalloc((void**)&charSet, task->charSetSize);
	cudaMalloc((void**)&cpPos, task->cpPosSize*sizeof(int));

	cudaMemcpy(data, resultBuff, task->idxCount*2*sizeof(unsigned __int64), cudaMemcpyHostToDevice);
	cudaMemcpy(stPlain, buff, PLAIN_MAX_SIZE, cudaMemcpyHostToDevice);
	cudaMemcpy(stPlain, task->stPlain, min(task->stPlainSize, PLAIN_MAX_SIZE), cudaMemcpyHostToDevice);
	cudaMemcpy(dimVec, task->dimVec, task->dimVecSize*sizeof(uint3), cudaMemcpyHostToDevice);
	cudaMemcpy(charSet, task->charSet, task->charSetSize, cudaMemcpyHostToDevice);
	cudaMemcpy(cpPos, task->cpPositions, task->cpPosSize*sizeof(int), cudaMemcpyHostToDevice);

	cudaMemcpyToSymbol(dataHeap, &data, sizeof(data));
	cudaMemcpyToSymbol(plStart, &stPlain, sizeof(stPlain));
	cudaMemcpyToSymbol(plDimVec, &dimVec, sizeof(dimVec));
	cudaMemcpyToSymbol(plChrSet, &charSet, sizeof(charSet));
	cudaMemcpyToSymbol(plCpPos, &cpPos, sizeof(cpPos));
	cudaMemcpyToSymbol(plCpPosSize, &task->cpPosSize, sizeof(task->cpPosSize));
	cudaMemcpyToSymbol(reduceOffset, &task->reduceOffset, sizeof(task->reduceOffset));
	cudaMemcpyToSymbol(plainSpaceTotal, &task->plainSpaceTotal, sizeof(task->plainSpaceTotal));
	uiVal64 = (unsigned __int64)-1/task->plainSpaceTotal;
	cudaMemcpyToSymbol(rPlainSpaceTotal, &uiVal64, sizeof(uiVal64));

	int grSizeX = (task->idxCount-1)/BLOCK_X_SIZE + 1;
	int grSizeY = (grSizeX-1)/GRID_X_SIZE + 1;
	grSizeX = GRID_X_SIZE;
	dim3 numBlocks(grSizeX, grSizeY);
	cuErr = cudaSuccess;
	tStart = time(NULL);
	for(int idx = 0; idx < task->rainbowChainLen-1 && cuErr == cudaSuccess; idx+=KERN_CHAIN_SIZE) {
		RTGenMD5Kernel<<<numBlocks, BLOCK_X_SIZE>>>(idx, min(idx+KERN_CHAIN_SIZE, task->rainbowChainLen-1));
		cuErr = cudaGetLastError();
		if(cuErr == cudaSuccess)
			cuErr = cudaThreadSynchronize();
		
	}
	tEnd = time(NULL);
	fprintf(stderr, "Kernel run time: %i\n", (tEnd - tStart));

	if(cuErr == cudaSuccess)
		cudaMemcpy(resultBuff, data, task->idxCount*2*sizeof(unsigned __int64), cudaMemcpyDeviceToHost);
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
