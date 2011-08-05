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

// Simple validator framework:
// Lets you create a custom validator by supplying three functions.
// See http://boinc.berkeley.edu/trac/wiki/ValidationSimple
//

#include "config.h"
#include <vector>
#include <cstdlib>
#include <string>

#include "boinc_db.h"
#include "error_numbers.h"

#include "sched_config.h"
#include "sched_msgs.h"

#include "validator.h"
#include "validate_util.h"
#include "validate_util2.h"
#include "ChainWalkContext.h"
#include "Public.h"
#include "part_validator.h"

using std::vector;

// Given a set of results, check for a canonical result,
// i.e. a set of at least min_quorum/2+1 results for which
// that are equivalent according to check_pair().
//
// invariants:
// results.size() >= wu.min_quorum
// for each result:
//   result.outcome == SUCCESS
//   result.validate_state == INIT
//
int check_set(
    vector<RESULT>& results, WORKUNIT& wu,
    int& canonicalid, double&, bool& retry
) {
    DB_CONN frt;
    char query[1024];
    MYSQL_RES* resp;
    MYSQL_ROW row;
    int retval = frt.open("rainbowtables-distrrtgen", config.db_host, config.db_user, config.db_passwd);
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "can't open db rainbowtables-distrrtgen\n");
	 return retry = true;
    }
    log_messages.printf(MSG_DEBUG, "Validating WU %i\n", wu.id);
    
    vector<void *> data;
    vector<bool> had_error;
    int i, j, neq = 0, n;
    int min_valid = wu.min_quorum/2+1;

    retry = false;
    n = results.size();
    data.resize(n);
    had_error.resize(n);

    // Initialize results

    for (i=0; i<n; i++) {
//        data[i] = "";
        had_error[i] = false;
    }
    int good_results = 0;
    for (i=0; i<n; i++) {
        retval = init_result(results[i], data[i]);
        if (retval == ERR_OPENDIR) {
            log_messages.printf(MSG_CRITICAL,
                "check_set: init_result([RESULT#%d %s]) transient failure\n",
                results[i].id, results[i].name
            );
            had_error[i] = true;
        } else if (retval) {
            log_messages.printf(MSG_CRITICAL,
                "check_set: init_result([RESULT#%d %s]) failed: %d\n",
                results[i].id, results[i].name, retval
            );
            results[i].outcome = RESULT_OUTCOME_VALIDATE_ERROR;
            results[i].validate_state = VALIDATE_STATE_INVALID;
            had_error[i] = true;
        } else  {
	    log_messages.printf(MSG_DEBUG, "check_set: init_result([RESULTÂ#%d %s]) fie is ok\n", results[i].id, results[i].name);
            good_results++;

        }
    }
    if (good_results < wu.min_quorum) {
	log_messages.printf(MSG_DEBUG, "good_results < wu.min_quorum (%d < %d)", good_results, wu.min_quorum);
	goto cleanup;
	}
    // Compare results

    for (i=0; i<n; i++) {
        if (had_error[i]) continue;
        vector<bool> matches;
        matches.resize(n);
        neq = 0;
        RainbowPartFile *curData = (RainbowPartFile*)data[i];
        const char *pos = strchr(results[i].name, ' ');
        char partnum[256] = {0};
        strncpy(partnum, results[i].name, pos - results[i].name);
        int partid = atoi(partnum);
        sprintf(query, "SELECT t.PartSize, t.HashRoutine, t.Charset, t.MinLetters, t.MaxLetters, t.Index, t.ChainLength, p.ChainStart, t.Keyspace FROM generator_parts p INNER JOIN generator_tables t ON t.TableID = p.TableID WHERE p.PartID = %i", partid);
        retval = frt.do_query(query);
        if(retval)  {
		retry = true;
		log_messages.printf(MSG_CRITICAL, "Error executing query '%s'", query);
 		goto cleanup;
	}
        resp = mysql_store_result(frt.mysql);
        if (!resp) {
		log_messages.printf(MSG_CRITICAL, "Error doing mysql_store_result()");
		retry = true;
		goto cleanup;
	}
        row = mysql_fetch_row(resp);
        if (!row) { 
		log_messages.printf(MSG_CRITICAL, "mysql row not found for query '%s'", query);
		retry = true;
		goto cleanup;
	}
        int PartSize = atoi(row[0]);
	log_messages.printf(MSG_DEBUG, "Starting verification of RESULT#%i...\n", results[i].id);
	log_messages.printf(MSG_DEBUG, "Num chains: %i PartSize: %i...\n", curData->numchains, PartSize);
	 
	 if(curData->numchains != PartSize)
	 {		
		log_messages.printf(MSG_CRITICAL,
                    "[RESULT#%d %s] File size doesn't match. Expectected %i, but got %i chains\n",
                    results[i].id, results[i].name, PartSize, curData->numchains
                );
            results[i].outcome = RESULT_OUTCOME_VALIDATE_ERROR;
            results[i].validate_state = VALIDATE_STATE_INVALID;		
	     continue;
	 }
	log_messages.printf(MSG_DEBUG, "Validating %i %s %s %s %s %s %s %s %s...\n", partid, row[1], row[2], row[3], row[4], row[5], row[6], row[7], row[8]);
	
	 string sHashRoutineName = row[1];
	 string sCharsetName = row[2];
        int nPlainLenMin = atoi(row[3]);
	 int nPlainLenMax = atoi(row[4]);
        int nRainbowTableIndex = atoi(row[5]);
	 int nRainbowChainLen = atoi(row[6]);
    mysql_free_result(resp);

	 uint64 nChainStartPosition = atoll(row[7]);
	 uint64 nKeyspace = atoll(row[8]);
	log_messages.printf(MSG_DEBUG, "Setting hash routine to %s...\n", sHashRoutineName.c_str());
  	 if (!CChainWalkContext::SetHashRoutine(sHashRoutineName))
 	 {
 		log_messages.printf(MSG_CRITICAL,"[RESULT#%d %s]hash routine %s not supported\n", results[i].id, results[i].name, sHashRoutineName.c_str());
              results[i].outcome = RESULT_OUTCOME_VALIDATE_ERROR;
              results[i].validate_state = VALIDATE_STATE_INVALID;					
		break;
	 }
	log_messages.printf(MSG_DEBUG, "Setting charset to %s (%i-%i)...\n", sCharsetName.c_str(), nPlainLenMin, nPlainLenMax);
	 if (!CChainWalkContext::SetPlainCharset(sCharsetName, nPlainLenMin, nPlainLenMax))
	 {
 		log_messages.printf(MSG_CRITICAL,"[RESULT#%d %s]charset %s (%i - %i) not supported\n", results[i].id, results[i].name, sCharsetName.c_str(), nPlainLenMin, nPlainLenMax);
              results[i].outcome = RESULT_OUTCOME_VALIDATE_ERROR;
              results[i].validate_state = VALIDATE_STATE_INVALID;					
		break;
	 }
	log_messages.printf(MSG_DEBUG, "Setting index to %i...\n", nRainbowTableIndex);

	 if (!CChainWalkContext::SetRainbowTableIndex(nRainbowTableIndex))
	 {
 		log_messages.printf(MSG_CRITICAL,"[RESULT#%d %s]invalid rainbow table index %d\n", results[i].id, results[i].name, nRainbowTableIndex);
              results[i].outcome = RESULT_OUTCOME_VALIDATE_ERROR;
              results[i].validate_state = VALIDATE_STATE_INVALID;					
		break;
	 }
	 RainbowChainCP *pChain = curData->pChain;
	 int nRainbowChainCountRead = curData->numchains;
						// Chain length test
	for(j = 1; j <= 25; j++)
	{
		int nIndexToVerify = nRainbowChainCountRead / 25 * j - 1;
		CChainWalkContext cwc;
		log_messages.printf(MSG_DEBUG, "Setting seed to %llu for verification step %i...\n", (nChainStartPosition + nIndexToVerify), j);
//		cwc.Dump();
		cwc.SetIndex(nChainStartPosition + nIndexToVerify);
		int nPos;
		for (nPos = 0; nPos < nRainbowChainLen - 1; nPos++)
		{
			cwc.IndexToPlain();
			cwc.PlainToHash();
			cwc.HashToIndex(nPos);
		}
		
		if (cwc.GetIndex() != pChain[nIndexToVerify].nIndexE)
		{
			log_messages.printf(MSG_CRITICAL,
				"[RESULT#%d %s] Rainbow chain length verification failed at step %i index %i (%llu != %llu)\n", results[i].id, results[i].name, j, nIndexToVerify, cwc.GetIndex(), pChain[nIndexToVerify].nIndexE);
	                results[i].outcome = RESULT_OUTCOME_VALIDATE_ERROR;
                        results[i].validate_state = VALIDATE_STATE_INVALID;		
			break;
		} 
	}
	if(results[i].outcome == RESULT_OUTCOME_VALIDATE_ERROR && results[i].validate_state == VALIDATE_STATE_INVALID) continue;
	log_messages.printf(MSG_DEBUG, "Checking if all %i chains is within bounds...\n", PartSize);
	for(j = 0; j < PartSize; j++)
   {
      if(pChain[j].nIndexE > nKeyspace)
      {
         log_messages.printf(MSG_CRITICAL,
         "[RESULT#%d %s] Endpoint index verification failed at step %i with number %llu > keyspace %llu\n", results[i].id, results[i].name, j, pChain[j].nIndexE, nKeyspace);
         results[i].outcome = RESULT_OUTCOME_VALIDATE_ERROR;
         results[i].validate_state = VALIDATE_STATE_INVALID;
         break;
      }
   }

/* We can't do StartingPoint and sorting tests anymore because we don't recieve the start points and the files should NOT be sorted anymore!
	for(j = 0; j < PartSize; j++)
	{
		if(pChain[j].nIndexS < (nChainStartPosition - 500000) ||  pChain[j].nIndexS > (nChainStartPosition + PartSize))
		{
			log_messages.printf(MSG_CRITICAL,
			"[RESULT#%d %s] Start index verification failed at step %i with number %llu. (< %llu | > %llu)\n", results[i].id, results[i].name, j, pChain[j].nIndexS, nChainStartPosition, (nChainStartPosition + PartSize));
	              results[i].outcome = RESULT_OUTCOME_VALIDATE_ERROR;
                     results[i].validate_state = VALIDATE_STATE_INVALID;		
			break;
		}
		if(j > 0 && pChain[j].nIndexE < pChain[j-1].nIndexE)
		{
			log_messages.printf(MSG_CRITICAL,
			"[RESULT#%d %s] Chain sort test failed at step %i with number %llu < %llu\n", results[i].id, results[i].name, j, pChain[j].nIndexE, pChain[j-1].nIndexE);
	              results[i].outcome = RESULT_OUTCOME_VALIDATE_ERROR;
                     results[i].validate_state = VALIDATE_STATE_INVALID;		
			break;
		}
	}
*/
	if(results[i].outcome == RESULT_OUTCOME_VALIDATE_ERROR && results[i].validate_state == VALIDATE_STATE_INVALID) continue;

	results[i].validate_state = VALIDATE_STATE_VALID;
       canonicalid = results[i].id;		
       log_messages.printf(MSG_DEBUG, "WU %i is OK\n", wu.id);
	retry = false;
	 /*
        for (j=0; j!=n; j++) {
            if (had_error[j]) continue;
            bool match = false;
            if (i == j) {
                ++neq;
                matches[j] = true;
            } else if (compare_results(results[i], data[i], results[j], data[j], match)) {
                log_messages.printf(MSG_CRITICAL,
                    "generic_check_set: check_pair_with_data([RESULT#%d %s], [RESULT#%d %s]) failed\n",
                    results[i].id, results[i].name, results[j].id, results[j].name
                );
            } else if (match) {
                ++neq;
                matches[j] = true;
            }
        }
        if (neq >= min_valid) {

            // set validate state for each result
            //
            for (j=0; j!=n; j++) {
                if (had_error[j]) continue;
                if (max_claimed_credit && results[j].claimed_credit > max_claimed_credit) {
                    results[j].validate_state = VALIDATE_STATE_INVALID;
                } else {
                    results[j].validate_state = matches[j] ? VALIDATE_STATE_VALID : VALIDATE_STATE_INVALID;
                }
            }
            canonicalid = results[i].id;
            credit = compute_granted_credit(wu, results);
            break;
        }*/
    }

cleanup:

    for (i=0; i<n; i++) {
        cleanup_result(results[i], data[i]);
    }
    frt.close();
    log_messages.printf(MSG_DEBUG, "[RESULT#%d %s] --------------TEST IS OVER! ------------------\n", results[i].id, results[i].name);
   
//	exit(0);
    return 0;
}

// r1 is the new result; r2 is canonical result
//

/*
void check_pair(RESULT& r1, RESULT& r2, bool& retry) {
    string data1;
    string data2;
    int retval;
    bool match;

    retry = false;
    retval = init_result(r1, data1);
    if (retval == ERR_OPENDIR) {
        log_messages.printf(MSG_CRITICAL,
            "check_pair: init_result([RESULT#%d %s]) transient failure 1\n",
            r1.id, r1.name
        );
        retry = true;
        return;
    } else if (retval) {
        log_messages.printf(MSG_CRITICAL,
            "check_pair: init_result([RESULT#%d %s]) perm failure 1\n",
            r1.id, r1.name
        );
        r1.outcome = RESULT_OUTCOME_VALIDATE_ERROR;
        r1.validate_state = VALIDATE_STATE_INVALID;
        return;
    }

    retval = init_result(r2, data2);
    if (retval == ERR_OPENDIR) {
        log_messages.printf(MSG_CRITICAL,
            "check_pair: init_result([RESULT#%d %s]) transient failure 2\n",
            r2.id, r2.name
        );
        cleanup_result(r1, data1);
        retry = true;
        return;
    } else if (retval) {
        log_messages.printf(MSG_CRITICAL,
            "check_pair: init_result([RESULT#%d %s]) perm failure2\n",
            r2.id, r2.name
        );
        cleanup_result(r1, data1);
        r1.outcome = RESULT_OUTCOME_VALIDATE_ERROR;
        r1.validate_state = VALIDATE_STATE_INVALID;
        return;
    }

    retval = compare_results(r1, data1, r2, data2, match);
    if (max_claimed_credit && r1.claimed_credit > max_claimed_credit) {
        r1.validate_state = VALIDATE_STATE_INVALID;
    } else {
        r1.validate_state = match?VALIDATE_STATE_VALID:VALIDATE_STATE_INVALID;
    }
    cleanup_result(r1, data1);
    cleanup_result(r2, data2);
}

*/
