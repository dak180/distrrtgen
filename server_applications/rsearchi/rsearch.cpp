// distrrtgen_assembler.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "MySQL.h"
#include <string>
#include <sstream>
#include <list>
#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>
#include <map>
#include <sys/types.h>
#include "dirent.h"
#include <errno.h>
#include <vector>

#include "ChainWalkContext.h"
#include "CrackEngine.h"
//#include "ServerConnector.h"
#ifdef WIN32
	#include <direct.h>
#endif
typedef struct
{
	std::string sHost;
	std::string sUsername;
	std::string sPassword;
	std::string sDatabase;
} DB_SETTINGS;
using std::map;
using namespace std;
#define DIR_INDICES "/home/boincadm/indices/"
#define DIR_CHAINS "/home/boincadm/chains/"
//typedef __int64 uint64;
//typedef int uint32;

DB_SETTINGS dbSettings;

void GetCharset(std::string sCharsetName, char **pCharset);

void RunCracker(int nTableID, CHashSet &hs, vector<string> &vPathName)
{
//	vector<string> vPathName;
//	vPathName.push_back("md5_mixalpha-numeric-all-space#1-6_0_10000x9434153_0.rt");

}


int main(int argc, char* argv[])
{
	
#ifdef WIN32
	std::string sRTPath = "/rainbowtables_i";
#else
	std::string sRTPath = "/home/rainbow/tables/indexed";
nice(19);
#endif
	while(1)
	{
		try
		{
	//		chdir(sRTPath.c_str());

			dbSettings.sDatabase.assign(MYSQL_DATABASE);
			dbSettings.sHost.assign(MYSQL_HOST);
			dbSettings.sPassword.assign(MYSQL_PASSWORD);
			dbSettings.sUsername.assign(MYSQL_USERNAME);

			map<int, vector<string> > mTableSets; 
			std::cout << "Connecting to database" << std::endl;
			CDatabase *pDatabase = (CDatabase*)new MySQL(dbSettings.sHost, dbSettings.sUsername, dbSettings.sPassword, dbSettings.sDatabase);
			std::cout << "Connected!" << std::endl;
			while(1)
			{
				vector<string> vFiles;
/*
				pDatabase->Select("SELECT rct.tableid, rctf.filename, rcts.hashroutine, rcts.charset, rcts.minletters, rcts.maxletters, rct.`index` FROM rainbowcrack_cracker_tables rct INNER JOIN rainbowcrack_cracker_tablesets rcts ON rcts.tablesetid = rct.tablesetid INNER JOIN rainbowcrack_cracker_tsfiles rctf ON rctf.tableid = rct.tableid");
				int nPrevSet = 0;
				while(pDatabase->Fetch() == true)
				{
					if(pDatabase->GetInt(0) != nPrevSet)
					{
						if(nPrevSet != 0)
						{
							mTableSets[nPrevSet] = vFiles;
							vFiles.erase(vFiles.begin(), vFiles.end());
						}
						nPrevSet = pDatabase->GetInt(0);
					}
					vFiles.push_back((const char *)pDatabase->GetText(1));				
				}
				if(nPrevSet > 0)
					mTableSets[nPrevSet] = vFiles;
*/
				DIR *hr, *files, *tables;//, *letters, *index, ;
				struct dirent *hrp, *filesp, *tablesp;//, *charsetp, *lettersp, *indexp;
				cout << "Opening dir " << sRTPath << endl;
				if((hr  = opendir(sRTPath.c_str())) == NULL) {
					cout << "Error(" << errno << ") opening dir " << sRTPath << endl;
					return errno;
				}
				while ((hrp = readdir(hr)) != NULL) 
				{

					string hrs = string(hrp->d_name);
					if(hrs != "." && hrs != "..")
					{
						string sDirTables = string(sRTPath + string("/") + hrs);
						cout << "Opening dir " << sDirTables << endl;
						if((tables = opendir(sDirTables.c_str())) == NULL) {
							cout << "Error(" << errno << ") opening dir " << sDirTables << endl;
							continue;
						}
						while ((tablesp = readdir(tables)) != NULL) 
						{
							string tablename = string(tablesp->d_name);
							if(tablename != "." && tablename != ".." && tablename.substr(tablename.length()-8) != ".torrent")
							{
								string hashroutine;
								string charsets;
								string sminletters;
								string smaxletters;
								string indexs;

								vector<string> vPart;
								if(tablename.find("hybrid") != string::npos)
								{// Special file handling of hybrid files
									if (!SeperateString(tablename, "_()#-_", vPart))
									{
										printf("filename %s not identified\n", tablename.c_str());
										continue;
									}
									
									hashroutine = vPart[0];
									charsets = string(vPart[1] + string("(") + vPart[2] + ")");
									sminletters = vPart[4];
									smaxletters = vPart[5];
									indexs = vPart[6];
									
								}
								else
								{
									if (!SeperateString(tablename, "_#-_", vPart))
									{
										printf("filename %s not identified\n", tablename.c_str());
										continue;
									}
									hashroutine = vPart[0];
									charsets = vPart[1];
									sminletters = vPart[2];
									smaxletters = vPart[3];
									indexs = vPart[4];
								}

								string sDirFiles = string(sRTPath + string("/") + hrs + string("/") + tablename);
								if((files  = opendir(sDirFiles.c_str())) == NULL) {
									cout << "Error(" << errno << ") opening " << sDirFiles << endl;
									continue;
								}
								vFiles.erase(vFiles.begin(), vFiles.end());												
								while ((filesp = readdir(files)) != NULL) 
								{
									string filess = string(filesp->d_name);
									if(filess != "." && filess != "..")
									{
										string extension = filess.substr(filess.length() - 4, 4);
										if(extension.compare(".rti") == 0)
											vFiles.push_back(filess);
									}
								}
								closedir(files);											
								if(vFiles.size() > 0)
								{
									std::stringstream sSQL;
									sSQL << "SELECT t.tableid FROM rainbowcrack_cracker_tables t INNER JOIN rainbowcrack_cracker_tablesets ts ON ts.tablesetid = t.tablesetid WHERE ts.hashroutine = '" << hashroutine << "' AND ts.charset = '" << charsets << "' AND ts.minletters = " << sminletters<< " AND ts.maxletters = " << smaxletters << " AND t.`index` = " << indexs;
									pDatabase->Select(sSQL.str());
									if(pDatabase->Fetch() == true)
									{
										int nTableID = pDatabase->GetInt(0);
										mTableSets[nTableID] = vFiles;
									}
								}									
							}
						}
					}											
				}
				closedir(hr);

				CCrackEngine ce;
				
				for(map<int, vector<string> >::iterator cur = mTableSets.begin(); cur != mTableSets.end(); cur++)
				{

					ostringstream sSQL;
					vector<int> vRequestID;

	//				sSQL << "SELECT workid, hash, cl.salt, indicelist, ts.charset, ts.chainlength FROM rainbowcrack_cracker_hashlist cl INNER JOIN rainbowcrack_cracker_tablesets ts ON (ts.tableid = cl.tableid) WHERE processed = 1 AND indicelist IS NOT NULL AND ts.tableid = " << cur->first;
	// 				sSQL << "SELECT cl.lookupid, ch.hash, ts.salt, cl.indices, ts.charset, ts.chainlength FROM rainbowcrack_cracker_hashlist ch INNER JOIN rainbowcrack_cracker_lookups cl ON (ch.hashid = cl.hashid) INNER JOIN rainbowcrack_cracker_tables ct ON (ct.tableid = cl.tableid) INNER JOIN rainbowcrack_cracker_tablesets ts ON (ts.tablesetid = ct.tablesetid) WHERE cl.indices IS NOT NULL AND ch.password IS NULL AND status = 0 AND ct.tableid = " << cur->first << " LIMIT 100";

					cout << "Querying for " << cur->first << std::endl;
					sSQL << "SELECT cl.lookupid, ch.hash, ts.hashroutine, ts.charset, ts.minletters, ts.maxletters, ct.index, ts.salt, ts.chainlength FROM rainbowcrack_cracker_hashlist ch INNER JOIN rainbowcrack_cracker_lookups cl ON (ch.hashid = cl.hashid) INNER JOIN rainbowcrack_cracker_tables ct ON (ct.tableid = cl.tableid) INNER JOIN rainbowcrack_cracker_tablesets ts ON (ts.tablesetid = ct.tablesetid) WHERE cl.hasindices = 2 AND ch.password IS NULL AND (status = 0 OR (status = 1 AND DATE_SUB(NOW(), INTERVAL 1 HOUR) > time_serverassignment)) AND ct.tableid = " << cur->first << " LIMIT 1000";
					if(pDatabase->Select(sSQL.str()) > 0)
					{
						std::string sHashRoutine;							
						CHashSet *hs = new CHashSet();
						std::ostringstream sToLook;
						sToLook << "UPDATE rainbowcrack_cracker_lookups SET status = 1, time_serverassignment = NOW() WHERE lookupid IN (";
						int nRow = 0;						
						while(pDatabase->Fetch() == true)
						{
							if(nRow > 0)
							{
								sToLook << ",";
							}
							else
							{
								printf("Fetching indices");
							}
							nRow++;
							printf(".");
							sHashRoutine.assign((char *)pDatabase->GetText(2));
							sToLook << pDatabase->GetInt(0);
							ostringstream sPath;
							sPath << sRTPath << "/" << pDatabase->GetText(2) << "/" << pDatabase->GetText(2) << "_" << pDatabase->GetText(3) << "#" << pDatabase->GetInt(4) << "-" << pDatabase->GetInt(5) << "_" << pDatabase->GetInt(6);
	#ifdef WIN32
							_chdir(sPath.str().c_str());				
	#else
							chdir(sPath.str().c_str());				
	#endif
							vRequestID.push_back(pDatabase->GetInt(0));
							



							// New code, loads from the local disk
							std::string sIndexDir = DIR_INDICES;
							std::stringstream indiceFilename;
							indiceFilename << sIndexDir.c_str() << pDatabase->GetText(0) << ".index";
							FILE *fIndex = fopen(indiceFilename.str().c_str(), "rb");						
							if(fIndex == NULL)
							{
								printf("Could not open file %s", indiceFilename.str().c_str());
								continue;
							}


							uint64 *indices = new uint64[pDatabase->GetInt(8)];
							int i = 0;
							std::string sInt;
							if(fread(indices, 8, pDatabase->GetInt(8), fIndex) != pDatabase->GetInt(8))
							{
								printf("Error reading index file %s\n", indiceFilename.str().c_str()); 
								fclose(fIndex);
								continue;
							}
							fclose(fIndex);
							hs->AddHash((const char *)pDatabase->GetText(1), indices, pDatabase->GetInt(0));
						}
						
						sToLook << ")";
						if(nRow > 0)
						{
							pDatabase->Query(sToLook.str());
							printf("ok!\n");
						}
						ce.Run(cur->second, *hs);
						map<string, vector<FoundRainbowChain> > mHashes;
						hs->GetFoundChains(mHashes);
//							ServerConnector *sc = new ServerConnector();
						for(map<string, vector<FoundRainbowChain> >::iterator curHash = mHashes.begin(); curHash != mHashes.end(); curHash++)
						{
							std::stringstream sChains;
							// Old code - Upload the chains through HTTP
							// New code. Write it directly to the file and update the DB
							std::stringstream sChainFile;
							sChainFile << DIR_CHAINS << hs->GetLookupID(curHash->first) << ".chains";
							FILE *fChains = fopen(sChainFile.str().c_str(), "wb");
							if(fChains == NULL)
							{
								printf("Unable to open file %s", sChainFile.str().c_str());
								continue;
							}
							for(int i = 0; i < curHash->second.size(); i++)
							{
								if(fwrite(&curHash->second[i].nIndexS, sizeof(curHash->second[i].nIndexS), 1, fChains) != 1)
								{
									printf("Unable to write data to %s", sChainFile.str().c_str());
									continue;
								}
								if(fwrite(&curHash->second[i].nGuessedPos, sizeof(curHash->second[i].nGuessedPos), 1, fChains) != 1)
								{
									printf("Unable to write data to %s", sChainFile.str().c_str());
									continue;
								}
							}							
							fclose(fChains);							
							sSQL.str("");
							sSQL << "UPDATE rainbowcrack_cracker_lookups SET status = 4, hasindices = 3 WHERE lookupid = " << hs->GetLookupID(curHash->first);
							try
							{
								pDatabase->Query(sSQL.str());
								sSQL.str("");
								sSQL << "INSERT IGNORE INTO rainbowcrack_cracker_verificationqueue (lookupid, hash, hashroutine, tablesetid, charset, minletters, maxletters, chainlength, salt, `index`) ";
	    						sSQL << "SELECT l.lookupid, h.hash, h.hashroutine, t.tablesetid, ts.charset, ts.minletters, ts.maxletters, ts.chainlength, ts.salt, t.`index` ";
	    						sSQL << " FROM rainbowcrack_cracker_lookups l ";
	    						sSQL << " INNER JOIN rainbowcrack_cracker_hashlist h ON h.hashid = l.hashid ";
	    						sSQL << " INNER JOIN rainbowcrack_cracker_tables t ON t.tableid = l.tableid ";
	    						sSQL << " INNER JOIN rainbowcrack_cracker_tablesets ts ON ts.tablesetid = t.tablesetid ";
	   							sSQL << " WHERE l.lookupid = " << hs->GetLookupID(curHash->first);
								pDatabase->Query(sSQL.str());

							}
							catch(DatabaseException *ex)
							{
								if(ex->getErrorCode() == CR_SERVER_GONE_ERROR || ex->getErrorCode() == CR_SERVER_LOST)
								{
									std::cout << "Server has disconnected";
								}
								else throw ex;
							}
						}
//						delete sc;
						delete hs;
					}
					sSQL.str("");
					for(int i = 0; i < vRequestID.size(); i++)
					{
						stringstream sFile;
						sFile << DIR_INDICES << vRequestID[i] << ".index";
						unlink(sFile.str().c_str());
					}					
		/*			if(vRequestID.size() > 0)
					{
						stringstream sList;
						for(int i = 0; i < vRequestID.size(); i++)
						{
							if(i != 0)
								sList << ",";					
							sList << vRequestID[i];
						}
						sSQL << "UPDATE rainbowcrack_cracker_lookups SET status = 2, hasindices = 2 WHERE lookupid IN (" << sList.str() << ")";
						pDatabase->Query(sSQL.str());
					}*/
				}
				Sleep(1000);
			}
		}
		catch(DatabaseException *ex)
		{
			std::cout << "Database exception " << " ( " << ex->getErrorCode() << " ): " << ex->GetErrorMessage()  << std::endl;
			delete ex;
			
		}
	}
	return 0;
}

