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

    
    const char *pos = strchr(wu.name, ' ');
    char partnum[256] = {0};
    strncpy(partnum, wu.name, pos - wu.name);
    int lookupid = atoi(partnum);
    log_messages.printf(MSG_DEBUG, "Handling lookupid %i\n", lookupid);    

    
    if (wu.canonical_resultid) {
	 char resdir[512];
struct passwd *pwd;
struct group *grp;
pwd = getpwnam("rainbow");
grp = getgrnam("rainbow");

        sprintf(query, "UPDATE rainbowcrack_cracker_lookups SET hasindices = 2 WHERE lookupid = %i", lookupid);
//	log_messages.printf(MSG_DEBUG, "%s\n", query);
        frt.do_query(query);
        if(retval)  goto cleanup;
        sprintf(query, "SELECT tableid FROM rainbowcrack_cracker_lookups WHERE lookupid = %i", lookupid);
//	log_messages.printf(MSG_DEBUG, "%s\n", query);
	retval = frt.do_query(query);
    	if(retval) {
        	log_messages.printf(MSG_DEBUG, "Query returned %i\n", retval);
        	goto cleanup;
    	}
    	resp = mysql_store_result(frt.mysql);
    	if (!resp) {retval = ERR_DB_NOT_FOUND; goto cleanup; }
    	row = mysql_fetch_row(resp);
    	mysql_free_result(resp);
    	if (!row) { retval = ERR_DB_NOT_FOUND; goto cleanup; }
	int tableid = atoi(row[0]);
	 sprintf(resdir, "../../indices/%s", row[0]);
//	log_messages.printf(MSG_DEBUG, "Result dir: %s\n", resdir);	
        retval = boinc_mkdir(resdir);
        vector<FILE_INFO> output_files;
        char copy_path[256];
        get_output_file_infos(canonical_result, output_files);
        unsigned int n = output_files.size();
//	log_messages.printf(MSG_DEBUG, "Number of output files: %i\n", n);
        for (i=0; i<n; i++) {
            FILE_INFO& fi = output_files[i];
                if (n==1) {
                     sprintf(copy_path, "../../indices/%i/%i.index", tableid, lookupid);
                } else {
                    sprintf(copy_path, "../../indices/%i/%i_%d.index", tableid, lookupid, i);
                }
//		log_messages.printf(MSG_DEBUG, "Copying from %s to %s\n", fi.path.c_str(), copy_path);
                retval = boinc_copy(fi.path.c_str() , copy_path);
		int fildes = open(copy_path, O_RDWR);
		fchown(fildes, pwd->pw_uid, grp->gr_gid);
                if (retval && !fi.optional) {
		    log_messages.printf(MSG_CRITICAL, "FAILED copy operation of %s to %s\n", fi.path.c_str(), copy_path);
                    sprintf(buf, "couldn't copy file %s\n", fi.path.c_str());
                    write_error(buf);
		    goto cleanup;
                }
        }
    } else {
        sprintf(buf, "%s: 0x%x\n", wu.name, wu.error_mask);
        return write_error(buf);
    }
    retval = 0;
cleanup:
    frt.close();	
    return retval;
}
