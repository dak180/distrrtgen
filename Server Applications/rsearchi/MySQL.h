#ifndef __MYSQL__H_DB__
#define __MYSQL__H_DB__
#ifdef WIN32
	#include <WinSock2.h>
#else 
	#include <sys/socket.h>
#define _atoi64 atoll

#endif
#include <mysql.h>
#include <errmsg.h>
#include <string>
#include "Exception.h"
#include "Database.h"


class MySQLException : public DatabaseException
{
public:
	MySQLException(std::string szErrorMessage, unsigned int nErrorCode) : DatabaseException(szErrorMessage, nErrorCode)	{ }
};

class MySQL : public CDatabase
{
private:
	MYSQL m_DBHandle;
	MYSQL_RES *m_LastResult;
	MYSQL_ROW m_Row;

	std::string m_Servername, m_Username, m_Password, m_Database;
	int m_Port;
public:
	MySQL(void);
	MySQL(std::string Servername, std::string Username, std::string Password, std::string Database, int Port = 0);
	~MySQL(void);

public:
	void Reconnect();
	void Query(std::string SQLQuery);
	uint64 Select(std::string SQLQuery);
	int SelectUnbuffered(std::string SQLQuery);
	int Fetch(); // TRUE is more data is available
	void OpenDatabase(std::string Servername, std::string Username, std::string Password, std::string Database, int Port = 0);
	int GetInt(int Column);
	__int64 GetInt64(int Column);
	__int64 GetLastInsertID();
	__int64 GetAffectedRows();
	const unsigned char* GetText(int Column);
	void Escape(std::string&);
};
#endif


