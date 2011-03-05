// freerainbowtables is a project for generating, distributing, and using
// perfect rainbow tables
//
// Copyright 2010 Jan Kyska
// Copyright 2010 Martin Westergaard Jørgensen <martinwj2005@gmail.com>
// Copyright 2010, 2011 James Nobis <frt@quelrod.net>
//
// This file is part of freerainbowtables.
//
// freerainbowtables is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// freerainbowtables is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with freerainbowtables.  If not, see <http://www.gnu.org/licenses/>.

#include "rcuda_ext.h"
#include <algorithm>
#include <string>
#include <time.h>
#include <fstream>
#include <iostream>

CudaCWCExtender::CudaCWCExtender(CChainWalkContext *cwc) { 
	this->cwc = cwc; 
	hash = rcuda::RHASH_UNDEF;
}

// sets rcuda kernel hash name
// sets basic charset lengths and keyspaces per position of the plaintext
// plainDimVec represents the primary data passed to CUDA
void CudaCWCExtender::Init(void) { 
	std::string hashName;
	int ii, jj;

	plainDimVec.clear();
	plainCharSet.clear();

	hashName = CChainWalkContext::m_sHashRoutineName;
	std::transform(hashName.begin(), hashName.end(), hashName.begin(), ::tolower);
	if(hashName.compare("lm") == 0)
		hash = rcuda::RHASH_LM;
	else if(hashName.compare("md4") == 0)
		hash = rcuda::RHASH_MD4;
	else if(hashName.compare("md5") == 0)
		hash = rcuda::RHASH_MD5;
	else if(hashName.compare("sha1") == 0)
		hash = rcuda::RHASH_SHA1;
	else if(hashName.compare("ntlm") == 0)
		hash = rcuda::RHASH_NTLM;
	else
		hash = rcuda::RHASH_UNDEF;

	for(ii = 0; ii < (int)CChainWalkContext::m_vCharset.size(); ii++) {
		stCharset &chs = CChainWalkContext::m_vCharset[ii];
		int chSetOffset = plainCharSet.size();
		plainCharSet.append((char*)chs.m_PlainCharset, chs.m_nPlainCharsetLen);
		for(jj = 0; jj < chs.m_nPlainLenMax; jj++) {
			plainDimVec.push_back((unsigned int)chs.m_nPlainCharsetLen);
			plainDimVec.push_back((unsigned int)-1/(unsigned int)chs.m_nPlainCharsetLen);
			plainDimVec.push_back((unsigned int)chSetOffset);
		}
	}
}

int CudaCWCExtender::IndexToStartPlain(const uint64 nIndex, std::vector<unsigned char>& stPlain) {
	int nPlainLen, nCharsetLen;
	int ii, jj;

	stPlain.clear();
	stPlain.reserve(0x20);
	nPlainLen = 0;

	for(ii = CChainWalkContext::m_nPlainLenMaxTotal - 1; ii >= CChainWalkContext::m_nPlainLenMinTotal - 1; ii--) {
		if(nIndex >= CChainWalkContext::m_nPlainSpaceUpToX[ii]) {
			nPlainLen = ii + 1;
			break;
		}
	}
	if(nPlainLen == 0)
		nPlainLen = CChainWalkContext::m_nPlainLenMinTotal;
	uint64 nIndexOfX = nIndex - CChainWalkContext::m_nPlainSpaceUpToX[nPlainLen - 1];

	// Slow version, but never mind
	for(ii = nPlainLen - 1; ii >= 0; ii--) {
		nCharsetLen = 0;
		for(jj = 0; jj < (int)CChainWalkContext::m_vCharset.size(); jj++) {
			stCharset &chs = CChainWalkContext::m_vCharset[jj];
			nCharsetLen += chs.m_nPlainLenMax;
			if(ii < nCharsetLen) { // We found the correct charset
				//XXX from md5 only cuda stPlain.push_back(nIndexOfX % chs.m_nPlainCharsetLen + 1);
				stPlain.push_back((unsigned char)(nIndexOfX % (uint64)chs.m_nPlainCharsetLen + 1));
				nIndexOfX /= chs.m_nPlainCharsetLen;
			}
		}
	}

	return stPlain.size();
}
