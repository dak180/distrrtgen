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

// A sample assimilator that:
// 1) if success, copy the output file(s) to a directory
// 2) if failure, append a message to an error log

#include <vector>
#include <string>
#include <cstdlib>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "lib/util.h"
#include "boinc_db.h"
#include "error_numbers.h"
#include "filesys.h"
#include "sched_msgs.h"
#include "validate_util.h"
#include "sched_config.h"
using std::vector;
using std::string;

int write_error(char* p) {
    static FILE* f = 0;
    if (!f) {
        f = fopen("../../results/errors", "a");
        if (!f) return ERR_FOPEN;
    }
    fprintf(f, "%s", p);
    fflush(f);
    return 0;
}

int GetNumFilesInDir(char *dir)
{
	DIR *partsdir;
	struct dirent *part;
	if((partsdir  = opendir(dir)) == NULL) {
		return 0;
	}
	int numfiles = 0;
	while ((part = readdir(partsdir)) != NULL) 
	{
		char *partname = part->d_name;
		if(partname != "." && partname != "..")
		{
			numfiles++;
		}
	}
	return numfiles;

}
int assimilate_handler(
    WORKUNIT& wu, vector<RESULT>& /*results*/, RESULT& canonical_result
) {
    DB_CONN frt;
    int retval;
    char buf[1024];
    char query[1024];
    unsigned int i;
    int tableid;
    MYSQL_RES* resp;
    MYSQL_ROW row;
    retval = frt.open("rainbowtables-distrrtgen", config.db_host, config.db_user, config.db_passwd);
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "can't open db rainbowtables-distrrtgen\n");
        exit(1);
    }

    
    const char *pos = strchr(wu.name, '_');
    char partnum[256] = {0};
    strncpy(partnum, wu.name, pos - wu.name);
    int lookupid = atoi(partnum);
    log_messages.printf(MSG_DEBUG, "Handling lookupid %i\n", lookupid);    

    
    if (wu.canonical_resultid) {
        sprintf(query, "DELETE FROM rainbowcrack_cracker_verificationqueue WHERE lookupid = %i", lookupid);
        frt.do_query(query);
        if(retval)  goto cleanup;

	 
        vector<FILE_INFO> output_files;
        char copy_path[256];
        get_output_file_infos(canonical_result, output_files);
        unsigned int n = output_files.size();
	 if(n < 1)
	 {
		log_messages.printf(MSG_CRITICAL, "No files found!\n");
		retval = -3;
		goto cleanup;
	 }
        FILE_INFO& fi = output_files[0];
	 string filedata;
	 read_file_string(fi.path.c_str(), filedata);
	 if(strcmp(filedata.c_str(),"0x00") != 0)
	 {
		sprintf(query, "UPDATE rainbowcrack_cracker_hashlist SET password = '%s' WHERE lookupid = %i", filedata.c_str(), lookupid);
		log_messages.printf(MSG_CRITICAL, "BINGO! Password %s is belonging to %i\n", filedata.c_str(), lookupid);
		log_messages.printf(MSG_CRITICAL, "%s\n", query);
	 }
	 else log_messages.printf(MSG_DEBUG, "Ew. Password NOT found in %i (%s)\n", lookupid, filedata.c_str());
  	 sprintf(query, "DELETE FROM rainbowcrack_cracker_verificationqueue WHERE lookupid = %i", lookupid);	
	 frt.do_query(query);
        if(retval)  goto cleanup;  	 
	 sprintf(query, "UPDATE rainbowcrack_cracker_lookups SET status = 2 WHERE lookupid = %i", lookupid);	
	 frt.do_query(query);
        if(retval)  goto cleanup;

    } else {
        sprintf(buf, "%s: 0x%x\n", wu.name, wu.error_mask);
        return write_error(buf);
    }
    retval = 0;
cleanup:
    frt.close();	
    return retval;
}
