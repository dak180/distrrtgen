/*
 * rcracki_mt is a multithreaded implementation and fork of the original 
 * RainbowCrack
 *
 * Copyright 2009, 2010 Daniël Niggebrugge <niggebrugge@fox-it.com>
 * Copyright 2009, 2010 James Nobis <frt@quelrod.net>
 *
 * This file is part of racrcki_mt.
 *
 * rcracki_mt is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * rcracki_mt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with rcracki_mt.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef _WIN32
	#pragma once
#endif

#include "ChainWalkContext.h"
#include "Public.h"
#include "HashSet.h"
//#include <process.h>
#include <pthread.h>

class rcrackiThread
{
private:
	unsigned char* t_TargetHash;
	int t_nPos;
	int t_nRainbowChainLen;
	CChainWalkContext t_cwc;
	vector<uint64> t_vStartPosIndexE;
	int t_ID;
	int t_count;
	uint64* t_pStartPosIndexE;
	int t_nChainWalkStep;
	bool falseAlarmChecker;
	bool falseAlarmCheckerO;
	vector<RainbowChain *> t_pChainsFound;
	vector<RainbowChainO *> t_pChainsFoundO;
	vector<int> t_nGuessedPoss;
	unsigned char* t_pHash;
	bool foundHash;
	int t_nChainWalkStepDueToFalseAlarm;
	int t_nFalseAlarm;
	string t_Hash;
	string t_Plain;
	string t_Binary;

public:
	rcrackiThread(unsigned char* TargetHash, int thread_id, int nRainbowChainLen, int thread_count, uint64* pStartPosIndexE);
	rcrackiThread(unsigned char* pHash, bool oldFormat = false);
	rcrackiThread(void);
	~rcrackiThread(void);

	//void SetWork(unsigned char* TargetHash, int nPos, int nRainbowChainLen);
	//static unsigned __stdcall rcrackiThread::rcrackiThreadStaticEntryPoint(void * pThis);
	static void * rcrackiThreadStaticEntryPointPthread(void * pThis);
	int GetIndexCount();
	int GetChainWalkStep();
	uint64 GetIndex(int nPos);
	bool FoundHash();
	void AddAlarmCheck(RainbowChain* pChain, int nGuessedPos);
	void AddAlarmCheckO(RainbowChainO* pChain, int nGuessedPos);
	int GetChainWalkStepDueToFalseAlarm();
	int GetnFalseAlarm();
	string GetHash();
	string GetPlain();
	string GetBinary();

private:
	void rcrackiThreadEntryPoint();
	void PreCalculate();
	void CheckAlarm();
	void CheckAlarmO();
};
