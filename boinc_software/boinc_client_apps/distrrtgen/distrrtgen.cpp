// This file is part of BOINC.
// http://boinc.berkeley.edu
// Copyright (C) 2008 University of California
//
// BOINC is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// BOINC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with BOINC.  If not, see <http://www.gnu.org/licenses/>.

// This program serves as both
// - An example BOINC application, illustrating the use of the BOINC API
// - A program for testing various features of BOINC
//
// NOTE: this file exists as both
// boinc/apps/upper_case.C
// and
// boinc_samples/example_app/uc2.C
// If you update one, please update the other!

// The program converts a mixed-case file to upper case:
// read "in", convert to upper case, write to "out"
//
// command line options
// -run_slow: sleep 1 second after each character
// -cpu_time N: use about N CPU seconds after copying files
// -early_exit: exit(10) after 30 chars
// -early_crash: crash after 30 chars
//

#ifdef _WIN32
#include "boinc_win.h"
#else
#include "config.h"
#include <cstdio>
#include <cctype>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <csignal>
#include <unistd.h>
#endif

#include <string>
#include <fstream>
#include <iostream>
#include "str_util.h"
#include "util.h"
#include "filesys.h"
#include "boinc_api.h"
#include "Public.h"
// Rainbowcrack code
#include "ChainWalkContext.h"

/*
bool early_exit = false;
bool early_crash = false;
bool early_sleep = false;
double cpu_time = 20, comp_result;
*/

int main(int argc, char **argv) {    
    int retval;
    double fd;
    char output_path[512]; //, chkpt_path[512];
    //FILE* state;	
    retval = boinc_init();
    if (retval) {
        fprintf(stderr, "boinc_init returned %d\n", retval);
        exit(retval);
    }
	

    // get size of input file (used to compute fraction done)
    //
    //file_size(input_path, fsize);

    // See if there's a valid checkpoint file.
    // If so seek input file and truncate output file
    //


	if(argc < 10)
	{
		fprintf(stderr, "Not enough parameters");
		return -1;
	}
	std::string sHashRoutineName, sCharsetName, sSalt, sCheckPoints;
	uint32 nRainbowChainCount, nPlainLenMin, nPlainLenMax, nRainbowTableIndex, nRainbowChainLen;
	uint64 nChainStart;
	sHashRoutineName = argv[1];
	sCharsetName = argv[2];
	nPlainLenMin = atoi(argv[3]);
	nPlainLenMax = atoi(argv[4]);
	nRainbowTableIndex = atoi(argv[5]);
	nRainbowChainLen = atoi(argv[6]);
	nRainbowChainCount = atoi(argv[7]);
#ifdef _WIN32

	nChainStart = _atoi64(argv[8]);

#else
	nChainStart = atoll(argv[8]);
#endif
	sCheckPoints = argv[9];
	std::vector<uint32> vCPPositions;
	char *cp = strtok((char *)sCheckPoints.c_str(), ",");
	while(cp != NULL)
	{
		vCPPositions.push_back(atoi(cp));
		cp = strtok(NULL, ",");
	}
	if(argc == 11)
	{
		sSalt = argv[10];
	}
	//std::cout << "Starting ChainGenerator" << std::endl;
	// Setup CChainWalkContext
	//std::cout << "ChainGenerator started." << std::endl;

	if (!CChainWalkContext::SetHashRoutine(sHashRoutineName))
	{
		fprintf(stderr, "hash routine %s not supported\n", sHashRoutineName.c_str());
		return 1;
	}
	//std::cout << "Hash routine validated" << std::endl;

	if (!CChainWalkContext::SetPlainCharset(sCharsetName, nPlainLenMin, nPlainLenMax))
	{	
		std::cerr << "charset " << sCharsetName << " not supported" << std::endl;
		return 2;
	}
	//std::cout << "Plain charset validated" << std::endl;

	if (!CChainWalkContext::SetRainbowTableIndex(nRainbowTableIndex))
	{
		std::cerr << "invalid rainbow table index " << nRainbowTableIndex << std::endl;
		return 3;
	}
	//std::cout << "Rainbowtable index validated" << std::endl;

	if(sHashRoutineName == "mscache")// || sHashRoutineName == "lmchall" || sHashRoutineName == "halflmchall")
	{
		// Convert username to unicode
		const char *szSalt = sSalt.c_str();
		int salt_length = strlen(szSalt);
		unsigned char cur_salt[256];
		for (int i=0; i<salt_length; i++)
		{
			cur_salt[i*2] = szSalt[i];
			cur_salt[i*2+1] = 0x00;
		}
		CChainWalkContext::SetSalt(cur_salt, salt_length*2);
	}
	else if(sHashRoutineName == "halflmchall")
	{ // The salt is hardcoded into the hash routine
	//	CChainWalkContext::SetSalt((unsigned char*)&salt, 8);
	}
	else if(sHashRoutineName == "oracle")
	{
		CChainWalkContext::SetSalt((unsigned char *)sSalt.c_str(), sSalt.length());
	}
	//std::cout << "Opening chain file" << std::endl;

	
	// Open file
    boinc_resolve_filename("result", output_path, sizeof(output_path));
	fclose(boinc_fopen(output_path, "a"));
	FILE *outfile = boinc_fopen(output_path, "r+b");
	
	if (outfile == NULL)
	{
		std::cerr << "failed to create " << output_path << std::endl;
		return 4;
	}
	
	// Check existing chains
	unsigned int nDataLen = (unsigned int)GetFileLen(outfile);
	
	// Round to boundary
	nDataLen = nDataLen / 10 * 10;
	if (nDataLen == nRainbowChainCount * 10)
	{		
		std::cerr << "precomputation of this rainbow table already finished" << std::endl;
		fclose(outfile);
		return 0;
	}
	nChainStart += (nDataLen / 10);
	fseek(outfile, nDataLen, SEEK_SET);
	//XXX size_t isn't 32/64 clean
	size_t nReturn;
	CChainWalkContext cwc;
	uint64 nIndex[2];
	//time_t tStart = time(NULL);

//	std::cout << "Starting to generate chains" << std::endl;
	for(uint32 nCurrentCalculatedChains = nDataLen / 10; nCurrentCalculatedChains < nRainbowChainCount; nCurrentCalculatedChains++)
	{		
		uint32 cpcheck = 0;
		unsigned short checkpoint = 0;
		fd = (double)nCurrentCalculatedChains / (double)nRainbowChainCount;
		boinc_fraction_done(fd);
		cwc.SetIndex(nChainStart++); // use a given index now!
		nIndex[0] = cwc.GetIndex();
		
		for (uint32 nPos = 0; nPos < nRainbowChainLen - 1; nPos++)
		{
		//	std::cout << "IndexToPlain()" << std::endl;
			cwc.IndexToPlain();
		//	std::cout << "PlainToHash()" << std::endl;
			cwc.PlainToHash();
		//	std::cout << "HashToIndex()" << std::endl;
			cwc.HashToIndex(nPos);
			if(cpcheck < vCPPositions.size() && nPos == vCPPositions[cpcheck])
			{
				
				checkpoint |= (1 << cpcheck) & (unsigned short)cwc.GetIndex() << cpcheck;
				cpcheck++;
			}
		}
		//std::cout << "GetIndex()" << std::endl;

		nIndex[1] = cwc.GetIndex();
		// Write chain to disk
		if ((nReturn = fwrite(&nIndex[1], 1, 8, outfile)) != 8)
		{
			std::cerr << "disk write fail" << std::endl;
			fclose(outfile);
			return 9;
		}
		if((nReturn = fwrite(&checkpoint, 1, 2, outfile)) != 2)
		{
			std::cerr << "disk write fail" << std::endl;
			fclose(outfile);
			return 9;
		}
	}
	//std::cout << "Generation completed" << std::endl;
	fclose(outfile);
    
	boinc_fraction_done(1);
	boinc_finish(0);
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR Args, int WinMode) {
    LPSTR command_line;
    char* argv[100];
    int argc;

    command_line = GetCommandLine();
    argc = parse_command_line( command_line, argv );
    return main(argc, argv);
}
#endif
