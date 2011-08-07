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

#include "boinc_api.h"

#define CALC_BUFFER_SIZE_ADJ 0x2000
#define EXIT_CODE_TEMP_SLEEP -20

struct UC_SHMEM
{
	double update_time;
	double fraction_done;
	double cpu_time;
	BOINC_STATUS status;
	int countdown;
		// graphics app sets this to 5 repeatedly,
		// main program decrements it once/sec.
		// If it's zero, don't bother updating shmem
};

void boinc_temporary_exitHack()
{
	//XXX this call doesn't work on linux
	// fixed in upstream source 2010-09-16
	// http://bolt.berkeley.edu/trac/changeset/22382
	
	#ifdef _WIN32
		boinc_temporary_exit(60);
	#else
		sleep(60);
		exit(EXIT_CODE_TEMP_SLEEP);
	#endif
}
