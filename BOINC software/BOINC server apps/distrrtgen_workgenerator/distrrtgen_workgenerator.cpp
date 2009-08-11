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

// sample_work_generator.C: an example BOINC work generator.
// This work generator has the following properties
// (you may need to change some or all of these):
//
// - Runs as a daemon, and creates an unbounded supply of work.
//   It attempts to maintain a "cushion" of 100 unsent job instances.
//   (your app may not work this way; e.g. you might create work in batches)
// - Creates work for the application "uppercase".
// - Creates a new input file for each job;
//   the file (and the workunit names) contain a timestamp
//   and sequence number, so that they're unique.

#include <unistd.h>
#include <cstdlib>
#include <string>
#include <cstring>

#include "boinc_db.h"
#include "error_numbers.h"
#include "backend_lib.h"
#include "parse.h"
#include "util.h"

#include "sched_config.h"
#include "sched_util.h"
#include "sched_msgs.h"

#define CUSHION 100
#define int64 long long
    // maintain at least this many unsent results
#define REPLICATION_FACTOR 2
// globals
//
char* wu_template;
DB_APP app;
DB_CONN *frt;
int start_time;
int seqno;

using namespace std;
// create one new job
//
int make_job() {
    DB_WORKUNIT wu;
    MYSQL_RES* resp;    
    MYSQL_ROW row;
    char name[256], path[256], query[1024];
    const char* infiles[1];
    int retval;
    string charset;
//    retval = frt->do_query("CALL requestwork(1)");    
    frt->do_query("SELECT TableID, ChainStartPosition FROM generator_tables WHERE ChainsDone < (NumTargetChains/* - (NumTargetChains / 20)*/) AND ChainStartPosition < Keyspace - PartSize AND IsCompleted = 0 LIMIT 1;");
    if(retval) 	return retval;
    resp = mysql_store_result(frt->mysql);
    if (!resp) return ERR_DB_NOT_FOUND;
    row = mysql_fetch_row(resp);
    mysql_free_result(resp);
    if (!row) return ERR_DB_NOT_FOUND;
    int tableid = atoi(row[0]);
    int64 chainstart = atol(row[1]);
    sprintf(query, "INSERT INTO generator_parts SET TableID = %s, ClientID = 1, AssignedTime = NOW(), Status = 1, ChainStart = %s\0", row[0], row[1]);
    log_messages.printf(MSG_DEBUG, "%s\n", query);
    retval = frt->do_query(query);
    if(retval)  return retval;
    int partid = frt->insert_id();

    sprintf(query, "UPDATE generator_tables SET ChainStartPosition = ChainStartPosition + PartSize WHERE TableID = %i\0", tableid);
    log_messages.printf(MSG_DEBUG, "%s\n", query);
    retval = frt->do_query(query);
    if(retval)  return retval;

    
    log_messages.printf(MSG_DEBUG, "SELECTING FROM DB\n");
    sprintf(query, "SELECT %i AS PartID, HashRoutine, Charset, MinLetters, MaxLetters, `Index`, ChainLength, PartSize AS ChainCount, '%lld' AS ChainStart, CheckPoints, Salt  FROM generator_tables WHERE TableID = %i;\0", partid, chainstart, tableid);    
    log_messages.printf(MSG_DEBUG, "%s\n", query);

    retval = frt->do_query(query);
    if(retval)  return retval;
    resp = mysql_store_result(frt->mysql);
    if (!resp) return ERR_DB_NOT_FOUND;
    row = mysql_fetch_row(resp);
    mysql_free_result(resp);
    if (!row) return ERR_DB_NOT_FOUND;
    char command_line[256];
//    char filename[256];
    sprintf(command_line, "%s %s %s %s %s %s %s %s %s", row[1], row[2], row[3], row[4], row[5], row[6], row[7], row[8], row[9]);

    // make a unique name (for the job and its input file)
    //
    sprintf(name, "%s %s_%s#%s-%s_%s_%sx%s - %s", row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7], row[8]);
//    sprintf(filename, "%s.txt", row[0]);
    log_messages.printf(MSG_DEBUG, "%s\n", name);
    
    read_file_malloc("templates/input_template.xml", wu_template);
  //  read_file_string("../download/charset.txt", charset);
 /*   retval = config.download_path(filename, path);
    if (retval) return retval;
    FILE* f = fopen(path, "wb");
    if (!f) return ERR_FOPEN;
    fwrite(charset.c_str(), charset.length(), 1, f);
    fclose(f);
    infiles[0] = filename;*/
    // Fill in the job parameters
    //
    wu.clear();
    wu.appid = app.id;
    strcpy(wu.name, name);
    wu.rsc_fpops_est = 1e13;
    wu.rsc_fpops_bound = 1e16;
    wu.rsc_memory_bound = 1e8;
    wu.rsc_disk_bound = 1e8;
    wu.delay_bound = 4*86400;
    wu.min_quorum = 1;
    wu.target_nresults = 1;
    wu.max_error_results = 8;
    wu.max_total_results = 10;
    wu.max_success_results = 2;
    // Register the job with BOINC
    //
    return create_work(
        wu,
        wu_template,
        "templates/output_template.xml",
        "../templates/output_template.xml",
        NULL,
        0,
        config,
	command_line
    );
}

void main_loop() {
    int retval;

    while (1) {
        check_stop_daemons();
        int n;
        retval = count_unsent_results(n, 0);
        if (n > CUSHION) {
            sleep(60);
        } else {
            int njobs = (CUSHION-n)/REPLICATION_FACTOR;
            log_messages.printf(MSG_DEBUG,
                "Making %d jobs\n", njobs
            );
            for (int i=0; i<njobs; i++) {
                retval = make_job();
                if (retval) {
                    log_messages.printf(MSG_CRITICAL,
                        "can't make job: %d\n", retval
                    );
                    exit(retval);
                }
            }
            // Now sleep for a few seconds to let the transitioner
            // create instances for the jobs we just created.
            // Otherwise we could end up creating an excess of jobs.
            sleep(5);
        }
    }
}

int main(int argc, char** argv) {
    int i, retval;
    frt = new DB_CONN();

    for (i=1; i<argc; i++) {
        if (!strcmp(argv[i], "-d")) {
            log_messages.set_debug_level(atoi(argv[++i]));
        } else {
            log_messages.printf(MSG_CRITICAL,
                "bad cmdline arg: %s", argv[i]
            );
        }
    }

    if (config.parse_file("..")) {
        log_messages.printf(MSG_CRITICAL,
            "can't read config file\n"
        );
        exit(1);
    }
    retval = frt->open("rainbowtables-distrrtgen", config.db_host, config.db_user, config.db_passwd);
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "can't open db rainbowtables-distrrtgen\n");
        exit(1);
    }

    retval = boinc_db.open(
        config.db_name, config.db_host, config.db_user, config.db_passwd
    );
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "can't open db\n");
        exit(1);
    }
    if (app.lookup("where name='distrrtgen'")) {
        log_messages.printf(MSG_CRITICAL, "can't find app\n");
        exit(1);
    }
    if (read_file_malloc("../templates/input_template.xml", wu_template)) {
        log_messages.printf(MSG_CRITICAL, "can't read WU template\n");
        exit(1);
    }

    start_time = time(0);
    seqno = 0;

    log_messages.printf(MSG_NORMAL, "Starting\n");

    main_loop();
}
