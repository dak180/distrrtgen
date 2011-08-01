/*
 * freerainbowtables is a project for generating, distributing, and using
 * perfect rainbow tables
 *
 * Copyright 2011 Janosch Rux <janosch.rux@web.de>
 * Copyright 2011 James Nobis <quel@quelrod.net>
 *
 * This file is part of freerainbowtables.
 *
 * freerainbowtables is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * freerainbowtables is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with freerainbowtables.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Changes: not using OpenSSL routines the slow way anymore, as suggested by jci.
 */
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>

#include "HashRoutine.h"
#include "Public.h"

#include <string.h>
#include <stdio.h>

using namespace std;

int main(int argc, char *argv[]) {

	int hashlen;
	uint32 i, num;
	char buffer[250];
	CHashRoutine hr;
	string plaintext, hash, testhash, plainfilename;
	fstream plainfile, hashfile;
	static HASHROUTINE HashRoutine;
        unsigned char m_Hash[MAX_HASH_LEN];
	char m_HexHash[3];

	//XXX see todo in HashRoutine.cpp
	vector<string> hashlist = hr.vHashRoutineName;
	std::vector<int> plainmaxlen = hr.vMaxPlainLen;

        if(argc < 2) {
		cout << "usage: HashTest plaintextfile\n";
		exit(-1);
	}
	plainfilename = argv[1];

	cout << "Available Routines: ";
	for(i=0; i < hashlist.size(); i++) {
 		cout << hashlist[i] << " ";
	}
	cout << endl;

	//Test each Algo
	for(i=0; i < hashlist.size(); i++) {
		//see notes in bug #20
		if ( hashlist[i] != "halflmchall" && hashlist[i] != "lmchall" && hashlist[i] != "ntlmchall" ) {
			cout << "Testing " << hashlist[i] << "... ";
			hr.GetHashRoutine(hashlist[i], HashRoutine, hashlen);
			plainfile.open(plainfilename.c_str());
			string hashfilename = "check_lists/" +  hashlist[i] + "_hash.txt";
			hashfile.open(hashfilename.c_str());

			if(!plainfile) { cerr << "Can't find " << plainfilename << endl; exit(-1);}
			if(!hashfile) { cerr << "Can't find " << hashfilename << endl; exit(-1);}

			num = 0;
			while( plainfile.getline(buffer, sizeof(buffer)) )  {

				plaintext = buffer;
				plaintext.erase(plaintext.find_last_not_of(" \n\r\t")+1);

			  if (plainmaxlen[i] == -1 || plaintext.size() < (unsigned int)plainmaxlen[i]){

				if(hashlist[i] == "lm")
					transform(plaintext.begin(), plaintext.end(), plaintext.begin(), ::toupper);

				//plaintext = "AAAAAAAAAAAAAAAA";
				hashfile.getline(buffer, sizeof(buffer));
				hash = buffer;
				hash.erase(hash.find_last_not_of(" \n\r\t")+1);
				HashRoutine((unsigned char*)plaintext.c_str(), plaintext.size(), m_Hash);

				testhash.clear();

				for (int ii = 0; ii < hashlen; ii++)  {
					 sprintf(m_HexHash,"%02x", m_Hash[ii]);
					 testhash +=  m_HexHash;
				}

				if (hash.compare(0, testhash.size(),testhash) != 0) {
					cout << " failed,  Plaintext Nr." << num+1 << ": \"" << plaintext << "\" should be " << hash << " but returns " << testhash;
					break;
				}
				num++;
			 } else {
				break;
			 }//if
			}//while
			cout << endl;
			plainfile.close();
			hashfile.close();
		}//if
	}//for

	return 0;
}
