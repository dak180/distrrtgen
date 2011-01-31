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

// A sample validator that grants credit if the majority of results are
// bitwise identical.
// This is useful only if either
// 1) your application does no floating-point math, or
// 2) you use homogeneous redundancy


#include "config.h"
#include "util.h"
#include "sched_util.h"
#include "sched_msgs.h"
#include "validate_util.h"
#include "ChainWalkContext.h"
#include "MemoryPool.h"
#include "filesys.h"
#include "error_numbers.h"
#include "part_validator.h"
using std::string;
using std::vector;


int read_file_binary(const char *path, RainbowPartFile *data, int &isize)
{
    int retval;
    double size;

    retval = file_size(path, size);
    if (retval) return retval;
    FILE *f = fopen(path, "rb");
    if (!f) return ERR_FOPEN;
    isize = (int) size;
        log_messages.printf(MSG_DEBUG,
                    "malloc %lu bytes. size: %lu\n",
                    (isize / 18 * sizeof(RainbowPartFile)), sizeof(RainbowPartFile)
                );	     
    data->pChain = (RainbowChainCP*)malloc(isize / 18 * sizeof(RainbowChainCP));
    data->numchains = isize / 18;
    for(int i = 0; i < data->numchains; i++)
    {	
        size_t n = fread(&data->pChain[i].nIndexS, 1, 8, f);
        n = fread(&data->pChain[i].nIndexE, 1, 8, f);
        n = fread(&data->pChain[i].nCheckPoint, 1, 2, f);
    }
    fclose(f);
    return 0;	
}
int init_result(RESULT& result, void*& data) {
    int retval;
    vector<FILE_INFO> files;

    retval = get_output_file_infos(result, files);
    if (retval) {
        log_messages.printf(MSG_CRITICAL,
            "[RESULT#%d %s] check_set: can't get output filenames\n",
            result.id, result.name
        );
        return retval;
    }

    RainbowPartFile *filedata = new RainbowPartFile();
    int filelen;
    for (unsigned int i=0; i<files.size(); i++) {
        FILE_INFO& fi = files[i];
        if (fi.no_validate) continue;
        retval = read_file_binary(fi.path.c_str(), filedata, filelen);
        if (retval) {
            if (fi.optional) {
                filedata = NULL;
            } else {
                log_messages.printf(MSG_CRITICAL,
                    "[RESULT#%d %s] Couldn't open %s\n",
                    result.id, result.name, fi.path.c_str()
                );
                return retval;
            }
	     
        }
        log_messages.printf(MSG_DEBUG,
                    "[RESULT#%d %s] Size of file %s is %u\n",
                    result.id, result.name, fi.path.c_str(), filelen
                );	     
        data = (void*)filedata;
    }
    return 0;
}

int cleanup_result(RESULT const& /*result*/, void *data) {
    if(data)
    {
    delete ((RainbowPartFile*)data)->pChain;
	delete ((RainbowPartFile*)data);
    }
    return 0;
}

const char *BOINC_RCSID_7ab2b7189c = "$Id: sample_bitwise_validator.cpp 16069 2008-09-26 18:20:24Z davea $";
