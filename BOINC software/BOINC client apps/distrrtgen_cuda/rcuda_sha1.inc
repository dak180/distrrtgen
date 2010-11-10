//============================================================================
// Name        : rcuda_sha1.inc
// Author      : Jan Kyska
// Version     : 1.00
// Description : SHA1 hash kernel for Generator of FreeRainbowTables
//============================================================================ 

namespace RC_SHA1 {

__device__ __constant__ unsigned int h[5] = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };

__device__ unsigned int SwapEndian(unsigned int n) {
	return (n<<24)|((n&0x0000ff00)<<8)|((n>>8)&0x0000ff00)|(n>>24);
}

__device__ unsigned int FF(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int e, int i, unsigned int* data) {
	return ((a<<5)|(a>>27)) + ((b&c)|((~b)&d)) + e + 0x5A827999 + data[SHIDX(i)];
}

__device__ unsigned int FF2(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int e, int i, unsigned int* data) {
	unsigned int dt;
	dt = data[SHIDX(i&15)]^data[SHIDX((i-3)&15)]^data[SHIDX((i-8)&15)]^data[SHIDX((i-14)&15)];
	data[SHIDX(i&15)] = dt = ((dt<<1)|(dt>>31));
	return ((a<<5)|(a>>27)) + ((b&c)|((~b)&d)) + e + 0x5A827999 + dt;
}

__device__ unsigned int GG(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int e, int i, unsigned int* data) {
	unsigned int dt;
	dt = data[SHIDX(i&15)]^data[SHIDX((i-3)&15)]^data[SHIDX((i-8)&15)]^data[SHIDX((i-14)&15)];
	data[SHIDX(i&15)] = dt = ((dt<<1)|(dt>>31));
	return ((a<<5)|(a>>27)) + (b^c^d) + e + 0x6ED9EBA1 + dt;
}

__device__ unsigned int HH(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int e, int i, unsigned int* data) {
	unsigned int dt;
	dt = data[SHIDX(i&15)]^data[SHIDX((i-3)&15)]^data[SHIDX((i-8)&15)]^data[SHIDX((i-14)&15)];
	data[SHIDX(i&15)] = dt = ((dt<<1)|(dt>>31));
	return ((a<<5)|(a>>27)) + ((b&c)|(b&d)|(c&d)) + e + 0x8F1BBCDC + dt;
}

__device__ unsigned int II(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int e, int i, unsigned int* data) {
	unsigned int dt;
	dt = data[SHIDX(i&15)]^data[SHIDX((i-3)&15)]^data[SHIDX((i-8)&15)]^data[SHIDX((i-14)&15)];
	data[SHIDX(i&15)] = dt = ((dt<<1)|(dt>>31));
	return ((a<<5)|(a>>27)) + (b^c^d) + e + 0xCA62C1D6 + dt;
}

__device__ void SHA1(unsigned int* dataHash) {
	unsigned int a = h[0], b = h[1], c = h[2], d = h[3], e = h[4], x;
	int ii;

	// Round 1
	for(ii = 0; ii < 16; ii++) {
		x = FF(a, b, c, d, e, ii, dataHash);
		e = d; d = c; c = ((b<<30)|(b>>2)); b = a; a = x;
	}
	for(; ii < 20; ii++) {
		x = FF2(a, b, c, d, e, ii, dataHash);
		e = d; d = c; c = ((b<<30)|(b>>2)); b = a; a = x;
	}
	
	// Round 2
	for(; ii < 40; ii++) {
		x = GG(a, b, c, d, e, ii, dataHash);
		e = d; d = c; c = ((b<<30)|(b>>2)); b = a; a = x;
	}

	// Round 3
	for(; ii < 60; ii++) {
		x = HH(a, b, c, d, e, ii, dataHash);
		e = d; d = c; c = ((b<<30)|(b>>2)); b = a; a = x;
	}
	
	// Round 4
	for(; ii < 80; ii++) {
		x = II(a, b, c, d, e, ii, dataHash);
		e = d; d = c; c = ((b<<30)|(b>>2)); b = a; a = x;
	}

	dataHash[SHIDX(0)] = a + h[0];
	dataHash[SHIDX(1)] = b + h[1];
	dataHash[SHIDX(2)] = c + h[2];
	dataHash[SHIDX(3)] = d + h[3];
	dataHash[SHIDX(4)] = e + h[4];
}

}

__global__ void RTGenSHA1Kernel(unsigned int chainStart, unsigned int chainStop) {
	uint3 dimItem;
	uint64 uiDiv64, uiVal64, uiMul64;
	unsigned int uiVal, uiDiv;
	unsigned int size, jj;
	unsigned int plain;

	RTGEN_PROLOGUE;
	
	// transform to the plain text
	plain = 0x80;
	jj = (PLAIN_MAX_SIZE>>2)+1;

	for(ii = 0; idx64 > 0xfffffff0ull && ii < PLAIN_MAX_SIZE; ii++) {
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
		plain = (plain<<8) | cplChrSet[dimItem.z + uiDiv];
		if((ii&3) == 2) {
			hData[SHIDX(jj--)] = plain;
			plain = 0;
		}
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
		plain = (plain<<8) | cplChrSet[dimItem.z + uiDiv];
		if((ii&3) == 2) {
			hData[SHIDX(jj--)] = plain;
			plain = 0;
		}
	}

	// prepare for SHA1
	size = ii;
	ii = ((((3-(ii&3))<<3)-1)&0x1f)+1;
	plain = plain<<ii;
	for(jj++, idx = 0; jj <= (PLAIN_MAX_SIZE>>2)+1; plain = hData[SHIDX(jj++)], idx++)
		hData[SHIDX(idx)] = RC_SHA1::SwapEndian((plain>>ii)|(hData[SHIDX(jj)]<<(32-ii)));
	hData[SHIDX(idx)] = RC_SHA1::SwapEndian(plain>>ii);
	for(idx++; idx < 14; idx++)
		hData[SHIDX(idx)] = 0;
	hData[SHIDX(idx++)] = 0;
	hData[SHIDX(idx)] = size<<3;

	// hash
	RC_SHA1::SHA1(hData);
	
	hData[SHIDX(0)] = RC_SHA1::SwapEndian(hData[SHIDX(0)]);
	hData[SHIDX(1)] = RC_SHA1::SwapEndian(hData[SHIDX(1)]);
	RTGEN_EPILOGUE
}
