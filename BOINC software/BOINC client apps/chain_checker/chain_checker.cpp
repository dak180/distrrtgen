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

// Modified by Martin Westergaard for the chain check usage
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
#include "ChainWalkContext.h"
#include "chain_checker.h"

typedef unsigned int uint32;


using std::string;

int main(int argc, char **argv) {
    int retval;
    char output_path[512], chkpt_path[512];
	string sHashRoutineName, sCharsetName, sHash;
	uint32 nPlainLenMin, nPlainLenMax, nRainbowTableIndex;
    FILE* state, *infile, *outfile;	
    retval = boinc_init();
    if (retval) {
        fprintf(stderr, "boinc_init returned %d\n", retval);
        exit(retval);
    }
	
	if(argc < 7)
	{
		std::cerr << "Not enough parameters";
		return -1;
	}

	// Initialize the args
	sHashRoutineName = argv[1];
	sCharsetName = argv[2];
	nPlainLenMin = atoi(argv[3]);
	nPlainLenMax = atoi(argv[4]);
	nRainbowTableIndex = atoi(argv[5]);
	sHash = argv[6];


	// Setup the ChainWalkContext
	if (!CChainWalkContext::SetHashRoutine(sHashRoutineName))
	{
		std::cerr << "hash routine " << sHashRoutineName << " not supported" << std::endl;
		return 1;
	}

	if (!CChainWalkContext::SetPlainCharset(sCharsetName, nPlainLenMin, nPlainLenMax))
	{	
		std::cerr << "charset " << sCharsetName << " not supported" << std::endl;
		return 2;
	}
	
	if (!CChainWalkContext::SetRainbowTableIndex(nRainbowTableIndex))
	{
		std::cerr << "invalid rainbow table index " << nRainbowTableIndex << std::endl;
		return 3;
	}

	
	// Open the file containing the chains we have to check.
    boinc_resolve_filename("chains", output_path, sizeof(output_path));
	infile = boinc_fopen(output_path, "rb");    
	if (infile == NULL)
	{
		std::cerr << "failed to open " << output_path << std::endl;
		return 4;
	}

	// Open the resulting file. This will *maybe* contain the password (if found)
	boinc_resolve_filename("result", output_path, sizeof(output_path));
	outfile = boinc_fopen(output_path, "wb");	
	if (outfile == NULL)
	{
		std::cerr << "failed to create " << output_path << std::endl;
		return 5;
	}

	// Read the chains from the input file
	int len = GetFileLen(infile); 
	int numchains = len / 12;  // Each chain is 12 bytes. 8 bytes startpoint and 4 bytes for the guessed position
	fseek(infile, 0, SEEK_SET);
	ChainCheckChain *pChain = new ChainCheckChain[numchains];
	int totalread = 0, read;
	for(int i = 0; i < numchains; i++)
	{
		int read = fread(&pChain[i].nIndexS, sizeof(pChain[i].nIndexS), 1, infile);
		if(read != 1)
		{
			std::cerr << "Error reading chaincheck file" << std::endl;
			return 6;
		}
		read = fread(&pChain[i].nGuessedPos, sizeof(pChain[i].nGuessedPos), 1, infile);
		if(read != 1)
		{
			std::cerr << "Error reading chaincheck file" << std::endl;
			return 7;
		}

	}
	fclose(infile);


	// Start checking the chains for false alarms
	CChainWalkContext cwc;	
	int found = 0;
	for(int i = 0; i < numchains; i++)
	{
		cwc.SetIndex(pChain[i].nIndexS);	
		int nPos;
		for (nPos = 0; nPos < pChain[i].nGuessedPos; nPos++)
		{
			cwc.IndexToPlain();
			cwc.PlainToHash();
			cwc.HashToIndex(nPos);
		}
		cwc.IndexToPlain();
		cwc.PlainToHash();
		std::string sHashme = cwc.GetHash();
		// Check if this is a matching chain
		if(sHashme.compare(sHash) == 0)
		{
			fwrite(cwc.GetPlain().c_str(), 1, cwc.GetPlain().length(), outfile);
			std::cout << "Password is " << cwc.GetPlain() << std::endl;
			found = 1;
			break; // Password is found. Lets break out.
		}
		// This chain didn't contain the password, so we update the progress
		boinc_fraction_done((double)i / (double)numchains); 
		
	}
	if(found == 0)
		fwrite("0x00", 4, 1, outfile);
	// Last step: Clean up
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

const char *BOINC_RCSID_33ac47a071 = "$Id: chain_checker.cpp 2008-11-28 Martin Westergaard $";

