/*
	Copyright (C) 2008 Steve Thomas <SMT837784@yahoo.com>
	Copyright (C) 2011 James Nobis <quel@quelrod.net>

	This file is part of RT Perfecter.

	RT Perfecter is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	RT Perfecter is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with RT Perfecter.  If not, see <http://www.gnu.org/licenses/>.
*/

// Current version "RT Perfecter v0.1"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctime>
#include "RTWrite.h"
#include "RTRead.h"

#ifdef _WIN32
	#include <conio.h>
	#define DIRECTORY_SEPERATOR '\\'
#else
	#include <fcntl.h>
	#define DIRECTORY_SEPERATOR '/'
#endif

#define DISPLAY_STATUS_MIN_LOOPS 2000000

struct RTChain2
{
	uint64 startpt;
	unsigned int endpt32_1;
	unsigned int endpt32_2;
};

union RTChainU
{
	RTChain c;
	RTChain2 c2;
};

void usage(char *runStr);

int main(int argc, char **argv)
{
#ifdef _WIN32
	uint64 maxIndex = 0xffffffffffffffff;
#else
	uint64 maxIndex = 0xffffffffffffffffll;
#endif
	int argi = 1, i, argsUsed = 0;
	unsigned int maxChainsPerFile = 67108864;
	freopen("rtperfecti.error.txt", "w", stderr);
	// Get arguments
	if (argc > 3 && argc < 6)
	{
		for (; argi < argc - 2; argi++)
		{
			if (strcmp(argv[argi], "-v") == 0 && (argsUsed & 1) == 0)
			{
				// Enable verbose mode
				argsUsed |= 1;
			}
			else if (strncmp(argv[argi], "-s=", 3) == 0 && (argsUsed & 2) == 0)
			{
				// Max file size in MiB
				argsUsed |= 2;
				maxChainsPerFile = 0;
				for (i = 3; argv[argi][i] >= '0' && argv[argi][i] <= '9'; i++)
				{
					maxChainsPerFile *= 10;
					maxChainsPerFile += ((int) argv[argi][i]) - 0x30;
				}
				if (argv[argi][i] != '\0')
				{
					printf("Error: Invalid number.\n\n");
					usage(argv[0]);
					return 1;
				}
				if (i > 8 || maxChainsPerFile > 65535) // i - 3 > 5
				{
					printf("Error: Number is to large.\n\n");
					usage(argv[0]);
					return 1;
				}
				maxChainsPerFile <<= 16; // maxChainsPerFile *= 1024 * 1024 / 16
			}
			else if (strncmp(argv[argi], "-i=", 3) == 0 && (argsUsed & 4) == 0)
			{
				// Maximum index for chains
				argsUsed |= 4;
				maxIndex = 0;
				for (i = 3; argv[argi][i] >= '0' && argv[argi][i] <= '9'; i++)
				{
					maxIndex *= 10;
					maxIndex += ((int) argv[argi][i]) - 0x30;
				}
				if (argv[argi][i] != '\0')
				{
					printf("Error: Invalid number.\n\n");
					usage(argv[0]);
					return 1;
				}
				if (i > 23) // i - 3 > 20
				{
					printf("Error: Number is to large.\n\n");
					usage(argv[0]);
					return 1;
				}
			}
			else
			{
				printf("Error: Unknown argument.\n\n");
				usage(argv[0]);
				return 1;
			}
		}
	}
	else if (argc != 3)
	{
		if (argc == 1 || argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0))
		{
			usage(argv[0]);
			return 0;
		}
		printf("Error: Wrong number of arguments.\n\n");
		usage(argv[0]);
		return 1;
	}

	// Init
	RTChainU chain, prevChain;
	uint64 uniqueChains = 0;
	RTRead inRt(argv[argi], maxIndex, argsUsed & 1);
	RTWrite outRt(argv[argi + 1], maxChainsPerFile);

	prevChain.c2.endpt32_1 = 0xffffffff;
	prevChain.c2.endpt32_2 = 0xffffffff;
#ifndef _WIN32
	// Set stdin to non-blocking
	fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) | O_NDELAY);
#endif
	clock_t t = clock();
	int loops = 0;
	for (int ret = inRt.readChain(&(chain.c)); ret == 0; ret = inRt.readChain(&(chain.c)))
	{
		// Show status
		loops++;
		if (loops >= DISPLAY_STATUS_MIN_LOOPS)
		{
#ifdef _WIN32
			// Pressing any key will show the status
			if (kbhit())
			{
				inRt.printStatus();
			}
#else
			// Pressing enter will show the status
			int ch;
			do
			{
				ch = getchar();
			} while (ch != (int)'\n' && ch != EOF);
			if (ch == (int)'\n')
			{
				inRt.printStatus();
			}
#endif
			loops = 0;
		}

		// Check the least significate 32 bits first
		if (chain.c2.endpt32_1 != prevChain.c2.endpt32_1 || chain.c2.endpt32_2 != prevChain.c2.endpt32_2)
		{
			outRt.writeChain(&(chain.c));
			uniqueChains++;
		}
		prevChain = chain;
	}
#ifdef _WIN32
	printf("Unique Chains: %I64u\nTotal time:    %1.2f\n", uniqueChains, (clock() - t) / (double)CLOCKS_PER_SEC);
#else
	printf("Unique Chains: %llu\nTotal time:    %1.2f\n", uniqueChains, (clock() - t) / (double)CLOCKS_PER_SEC);
#endif
	return 0;
}

void usage(char *runStr)
{
	printf("\n                          **** RT Perfecter v0.1 ****\n\n");
	printf("Converts sorted rt files in a directory to a perfect rainbow table. All rt files\n");
	printf("in the directory must all be from the same rainbow table.\n\n");
	printf("%s [Options] input-directory output-file-pattern\n\n", runStr);
	printf("Options:\n");
	printf("  -i=number\n");
	printf("    Maximum index for chains.\n\n");
	printf("  -s=number\n");
	printf("    Maximum size for output files in MiB [default is 512, max is 65535].\n\n");
	printf("  -v\n");
	printf("    Verbose mode, displays extra info.\n\n");
	printf("output-file-pattern:\n");
	printf("  This will be passed into sprintf() with the number of chains in the file and\n");
	printf("  the current file number starting at 0. This could look like:\n");
	printf("  'some-folder%csome-name%%u-%%03u.rt'\n", DIRECTORY_SEPERATOR);
	printf("  Where '%%u' would be the number of chains in the file and '%%03u' would be the\n");
	printf("  file number formated like 000, 001, 002, and so on.\n\n");
	printf("Run-time:\n");
	printf("  Press [Enter] to display a status message.\n\n");
	printf("Copyright (c) 2008 Steve Thomas <SMT837784@yahoo.com>\n");
	printf("  This is free software: you can redistribute it and/or modify it under the\n");
	printf("  terms of the GNU General Public License as published by the Free Software\n");
	printf("  Foundation, either version 3 of the License, or (at your option) any later\n");
	printf("  version. There is NO WARRANTY, to the extent permitted by law.\n");
}

