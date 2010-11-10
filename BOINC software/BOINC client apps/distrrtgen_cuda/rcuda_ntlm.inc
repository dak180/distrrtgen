//============================================================================
// Name        : rcuda_ntlm.inc
// Author      : Jan Kyska
// Version     : 1.00
// Description : NTLM hash kernel for Generator of FreeRainbowTables
//============================================================================ 

namespace RC_NTLM {

__device__ __constant__ unsigned int h[4] = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476 };
__device__ __constant__ unsigned char r[48] = { \
							3,  7, 11, 19,  3,  7, 11, 19,  3,  7, 11, 19,  3,  7, 11, 19, \
							3,  5,  9, 13,  3,  5,  9, 13,  3,  5,  9, 13,  3,  5,  9, 13, \
							3,  9, 11, 15,  3,  9, 11, 15,  3,  9, 11, 15,  3,  9, 11, 15 };
__device__ __constant__ unsigned char g[48] = { \
							0, 1,  2,  3,   4,  5,  6,  7,   8,  9, 10, 11,  12, 13, 14, 15, \
							0, 4,  8, 12,   1,  5,  9, 13,   2,  6, 10, 14,   3,  7, 11, 15, \
							0, 8,  4, 12,   2, 10,  6, 14,   1,  9,  5, 13,   3, 11,  7, 15 };

__device__ unsigned int FF(unsigned int a, unsigned int b, unsigned int c, unsigned int d, int i, const unsigned int* data) {
	unsigned int ret;
	ret = a + ((b&c)|((~b)&d)) + data[SHIDX(g[i])];
	ret = (ret<<r[i])|(ret>>(32-r[i]));
	return ret;
}

__device__ unsigned int GG(unsigned int a, unsigned int b, unsigned int c, unsigned int d, int i, const unsigned int* data) {
	unsigned int ret;
	ret = a + ((b&c)|(b&d)|(c&d)) + data[SHIDX(g[i])] + 0x5a827999u;
	ret = (ret<<r[i])|(ret>>(32-r[i]));
	return ret;
}

__device__ unsigned int HH(unsigned int a, unsigned int b, unsigned int c, unsigned int d, int i, const unsigned int* data) {
	unsigned int ret;
	ret = a + (b^c^d) + data[SHIDX(g[i])] + 0x6ed9eba1u;
	ret = (ret<<r[i])|(ret>>(32-r[i]));
	return ret;
}

__device__ void MD4(unsigned int* dataHash) {
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
	
	dataHash[SHIDX(0)] = a + h[0];
	dataHash[SHIDX(1)] = b + h[1];
	dataHash[SHIDX(2)] = c + h[2];
	dataHash[SHIDX(3)] = d + h[3];
}

}

__global__ void RTGenNTLMKernel(unsigned int chainStart, unsigned int chainStop) {
	uint3 dimItem;
	uint64 uiDiv64, uiVal64, uiMul64;
	unsigned int uiVal, uiDiv;
	unsigned int size, jj, kk;
	unsigned int plain;

	RTGEN_PROLOGUE;
	
	// transform to the plain text
	plain = 0x80;
	jj = (PLAIN_MAX_SIZE>>1)+1;

	for(ii = kk = 0; idx64 > 0xfffffff0ull && ii < PLAIN_MAX_SIZE; ii++) {
		uiVal64 = idx64 + cplStart[ii];
		uiVal64--;
		dimItem = cplDimVec[ii];

		uiMul64 = (uint64)dimItem.y<<32;
		idx64 = __umul64hi(uiVal64, uiMul64);
		uiDiv64 = uiVal64 - idx64*(uint64)dimItem.x;
		uiVal = __umulhi((unsigned int)uiDiv64, dimItem.y);
		uiDiv = (unsigned int)uiDiv64 - uiVal * dimItem.x;
		idx64 += uiVal;
		if(uiDiv >= dimItem.x) {
			uiDiv -= dimItem.x;
			idx64++;
		}
		plain = (plain<<8);
		if((kk++)&1) {
			hData[SHIDX(jj--)] = plain;
			plain = 0;
		}
		plain = (plain<<8) | cplChrSet[dimItem.z + uiDiv];
	}

	for(idx = (unsigned int)idx64; idx != 0 && ii < PLAIN_MAX_SIZE; ii++) {
		uiVal = idx + cplStart[ii];
		uiVal--;
		dimItem = cplDimVec[ii];

		idx = __umulhi(uiVal, dimItem.y);
		uiDiv = uiVal - idx*dimItem.x;
		if(uiDiv >= dimItem.x) {
			uiDiv -= dimItem.x;
			idx++;
		}
		plain = (plain<<8);
		if((kk++)&1) {
			hData[SHIDX(jj--)] = plain;
			plain = 0;
		}
		plain = (plain<<8) | cplChrSet[dimItem.z + uiDiv];
	}

	// prepare for MD4
	size = (ii<<1);
	ii = (((kk^1)&1)<<4)+8;
	plain = plain<<ii;
	for(jj++, idx = 0; jj <= (PLAIN_MAX_SIZE>>1)+1; plain = hData[SHIDX(jj++)], idx++)
		hData[SHIDX(idx)] = (plain>>ii)|(hData[SHIDX(jj)]<<(32-ii));
	hData[SHIDX(idx)] = plain>>ii;
	for(idx++; idx < 14; idx++)
		hData[SHIDX(idx)] = 0;
	hData[SHIDX(idx++)] = size<<3;
	hData[SHIDX(idx)] = 0;

	// hash
	RC_NTLM::MD4(hData);

	RTGEN_EPILOGUE
}
