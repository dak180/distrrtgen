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

#include "str_util.h"
#include "util.h"
#include "filesys.h"
#include "boinc_api.h"
#include "Public.h"
// Rainbowcrack code
#include "ChainWalkSet.h"
#include "ChainWalkContext.h"
typedef unsigned int uint32;
//typedef unsigned __int64 uint64;


using std::string;

int main(int argc, char **argv) {
    int i;
    int c, nchars = 0, retval, n;
    double fsize, fd;
    char output_path[512], chkpt_path[512];
    FILE* state, *infile;	
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


	if(argc < 8)
	{
		std::cerr << "Not enough parameters";
		return -1;
	}
	string sHashRoutineName, sCharsetName, sHash;
	uint32 nRainbowChainCount, nPlainLenMin, nPlainLenMax, nRainbowTableIndex, nRainbowChainLen;
	uint64 nChainStart;
	sHashRoutineName = argv[1];
	sCharsetName = argv[2];
	nPlainLenMin = atoi(argv[3]);
	nPlainLenMax = atoi(argv[4]);
	nRainbowTableIndex = atoi(argv[5]);
	nRainbowChainLen = atoi(argv[6]);
	sHash = argv[7];
	//std::cout << "Starting ChainGenerator" << std::endl;
	// Setup CChainWalkContext
	//std::cout << "ChainGenerator started." << std::endl;

	if (!CChainWalkContext::SetHashRoutine(sHashRoutineName))
	{
		std::cerr << "hash routine " << sHashRoutineName << " not supported" << std::endl;
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
/*
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
	}*/
	else if(sHashRoutineName == "halflmchall")
	{ // The salt is hardcoded into the hash routine
	//	CChainWalkContext::SetSalt((unsigned char*)&salt, 8);
	}
	/*
	else if(sHashRoutineName == "oracle")
	{
		CChainWalkContext::SetSalt((unsigned char *)sSalt.c_str(), sSalt.length());
	}
	*/
	//std::cout << "Opening chain file" << std::endl;

	
	// Open file
    boinc_resolve_filename("result", output_path, sizeof(output_path));
	FILE *outfile = boinc_fopen(output_path, "wb");
	
	if (outfile == NULL)
	{
		std::cerr << "failed to create " << output_path << std::endl;
		return 4;
	}
	unsigned char TargetHash[255];
	int nHashLen = 0;
	ParseHash(sHash, TargetHash, nHashLen);
	CChainWalkSet m_cws;
	CChainWalkContext cwc;

	bool fNewlyGenerated;
	uint64* pStartPosIndexE = m_cws.RequestWalk(TargetHash,
												nHashLen,
												sHashRoutineName,
												sCharsetName,
												nPlainLenMin,
												nPlainLenMax,
												nRainbowTableIndex,
												nRainbowChainLen,
												fNewlyGenerated);
	int nPos;
	double nTargetChains = pow((double)nRainbowChainLen - 2, 2) / 2 - (nRainbowChainLen - 2);
	if (fNewlyGenerated)
	{
		time_t tStart = time(NULL);
		for (nPos = nRainbowChainLen - 2; nPos >= 0; nPos--)
		{
			if(time(NULL) - tStart > 1)
			{
				time(&tStart);
				double nCurrentChains = pow(((double)nRainbowChainLen - 2 - (double)nPos), 2) / 2 - (nRainbowChainLen - 2);
				double fResult = ((double)((double)(nCurrentChains) / nTargetChains));
				if(fResult < 0)
					fResult = 0;
				boinc_fraction_done(fResult);
			}
			
			cwc.SetHash(TargetHash);
			cwc.HashToIndex(nPos);
			int i;
			for (i = nPos + 1; i <= nRainbowChainLen - 2; i++)
			{
				cwc.IndexToPlain();
				cwc.PlainToHash();
				cwc.HashToIndex(i);
			}
			uint64 index = cwc.GetIndex();
			if(fwrite(&index, 8, 1, outfile) != 1)
			{
				fprintf(stderr, "unable to write to outfile");
				return 9;
			}
				//nChainWalkStep += nRainbowChainLen - 2 - nPos;
		}
//			printf("ok\n");			
		double nCurrentChains = pow(((double)nRainbowChainLen - 2 - (double)nPos), 2) / 2 - (nRainbowChainLen - 2);
		double fResult = ((double)((double)(nCurrentChains) / nTargetChains));
		boinc_fraction_done(fResult);
	}
	
	
	fclose(outfile);
    
	// main loop - read characters, convert to UC, write
    //

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

const char *BOINC_RCSID_33ac47a071 = "$Id: upper_case.C 12135 2007-02-21 20:04:14Z davea $";

