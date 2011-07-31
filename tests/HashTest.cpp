#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>

#include "HashRoutine.h"
#include "Public.h"

#include <string.h>
#include <stdio.h>

using namespace std;

int main() {

	uint32 i;
	char buffer[250];
	CHashRoutine hr;
	string plaintext, hash, testhash;
	string plainfilename = "plaintext.txt";
	fstream plainfile, hashfile;
	static HASHROUTINE HashRoutine;
        unsigned char m_Hash[MAX_HASH_LEN];
	char m_HexHash[3];

	//XXX see todo in HashRoutine.cpp
	vector<string> hashlist = hr.vHashRoutineName;
	std::vector<int> hashlen = hr.vHashLen;
	std::vector<int> plainmaxlen = hr.vMaxPlainLen;

	cout << "Available Routines: ";

	//Display Algos
	for(i=0; i < hashlist.size(); i++) {
 		cout << hashlist[i] << " ";
	}
	cout << endl;

	//Test each Algo
	for(i=0; i < hashlist.size(); i++) {
		//until we have a lists for those
		if ( hashlist[i] != "halflmchall" && hashlist[i] != "lmchall" && hashlist[i] != "ntlmchall" ) {
			cout << "Testing " << hashlist[i] << "... ";
			signed int mhm = 16;
			hr.GetHashRoutine(hashlist[i], HashRoutine, mhm);
			plainfile.open(plainfilename.c_str());
			string hashfilename = hashlist[i] + "_hash.txt";
			hashfile.open(hashfilename.c_str());

			if(!plainfile) { cerr << "Can't find " << plainfilename << endl; exit(-1);}
			if(!hashfile) { cerr << "Can't find " << hashlist[i]+"_hash.txt" << endl; exit(-1);}

			while( plainfile.getline(buffer, sizeof(buffer)) )  {

				plaintext = buffer;
				plaintext.erase(plaintext.find_last_not_of(" \n\r\t")+1);

			  if (plainmaxlen[i] == -1 || plaintext.size() < plainmaxlen[i]){
				hashfile.getline(buffer, sizeof(buffer));
				hash = buffer;
				hash.erase(hash.find_last_not_of(" \n\r\t")+1);
				HashRoutine((unsigned char*)plaintext.c_str(), plaintext.size(), m_Hash);

				testhash.clear();

				for (int ii = 0; ii < hashlen[i]; ii++)  {
					 sprintf(m_HexHash,"%02x", m_Hash[ii]);
					 testhash +=  m_HexHash;
				}

				if (testhash != hash) {
					cout << " failed,  Plaintext: \"" << plaintext << "\" should be " << hash << " but returns " << testhash;
					break;
					//exit(-1);
				}
			 } else {
				cout << "Skipping... ";
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
