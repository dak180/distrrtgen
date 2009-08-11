#include "stdafx.h"
#include "MySQL.h"
#include <sstream>
#include <stdlib.h>
#include <iostream>
MySQL::MySQL(void)
{
}
MySQL::MySQL(std::string Servername, std::string Username, std::string Password, std::string Database, int Port)
{
	m_LastResult = NULL;
#ifdef DEBUG
//	logfile << "Connecting to database";
#endif
	OpenDatabase(Servername, Username, Password, Database, Port);
}
MySQL::~MySQL(void)
{
	if(m_LastResult != NULL)
	{
		mysql_free_result(m_LastResult);

	}
#ifdef DEBUG
//	logfile << "Shutting down database connection";
#endif
	mysql_close(&m_DBHandle);
}

void MySQL::OpenDatabase(std::string Servername, std::string Username, std::string Password, std::string Database, int Port)
{
	mysql_init(&m_DBHandle);
	if(!mysql_real_connect(&m_DBHandle, Servername.c_str(), Username.c_str(), Password.c_str(), Database.c_str(), Port, NULL, 0))
	{
		std::stringstream error;
		error << "Could not connect to server: " << mysql_error(&m_DBHandle);
		throw new MySQLException(error.str(), mysql_errno(&m_DBHandle));
	}
	m_Servername = Servername;
	m_Username = Username;
	m_Password = Password;
	m_Database = Database;
	m_Port = Port;
}

void MySQL::Reconnect()
{
	OpenDatabase(m_Servername, m_Username, m_Password, m_Database, m_Port);
}
void MySQL::Query(std::string SQLQuery)
{
#ifdef DEBUG
//	logfile << SQLQuery;
#endif
	try
	{
		if(mysql_query(&m_DBHandle, SQLQuery.c_str()) != 0)
		{
			std::stringstream error;
			error << "Error querying server: " << mysql_error(&m_DBHandle) << std::endl << "SQL: " << SQLQuery;
			throw new MySQLException(error.str(), mysql_errno(&m_DBHandle));
		}
	}
	catch(DatabaseException *ex)
	{
		if(ex->getErrorCode() == CR_SERVER_LOST)
		{
//			logfile << "Reconnecting to server" << std::endl;
			Reconnect();
			
			Query(SQLQuery);
		}
		else throw ex;
	}
}

uint64 MySQL::Select(std::string SQLQuery)
{
#ifdef DEBUG
	std::cout << "Doing query " << SQLQuery << std::endl;
#endif
	Query(SQLQuery);
	if(m_LastResult != NULL) { 
#ifdef DEBUG
		std::cout << "Freeing up previous result" << std::endl;
#endif
		mysql_free_result(m_LastResult); 
	}

#ifdef DEBUG
		std::cout << "Transferring result from database server" << std::endl;
#endif
		
	m_LastResult = mysql_store_result(&m_DBHandle);
	if(m_LastResult == NULL)
	{
		std::stringstream szError;
		szError << "Error while transferring result from database server: " << mysql_error(&m_DBHandle);
		throw new MySQLException(szError.str(), mysql_errno(&m_DBHandle));
	}
#ifdef DEBUG
	std::cout << "Result transferred OK" << std::endl;
#endif
	return true;
}
int MySQL::SelectUnbuffered(std::string SQLQuery)
{
	Query(SQLQuery);
	if(m_LastResult != NULL) { 
#ifdef DEBUG
	//	logfile << "Freeing up previous result";
#endif
		mysql_free_result(m_LastResult); 
	}
/*
#ifdef DEBUG
		logfile << "Transferring result from database server";
#endif
		*/
	m_LastResult = mysql_use_result(&m_DBHandle);
	if(m_LastResult == NULL)
	{
		std::stringstream szError;
		szError << "Error while transferring result from database server: " << mysql_error(&m_DBHandle);
		throw new MySQLException(szError.str(), mysql_errno(&m_DBHandle));
	}
#ifdef DEBUG
//	logfile << "Result transferred OK";
#endif
	return mysql_num_rows(m_LastResult);
}

int MySQL::Fetch()
{
	if(m_LastResult == NULL)
		return false;
#ifdef DEBUG
	std::cout << "Fetching MySQL row" << std::endl;
#endif
	m_Row = mysql_fetch_row(m_LastResult);
	if(m_Row == NULL)
	{
#ifdef DEBUG
        std::cout << "row == NULL" << std::endl;
#endif

		if(mysql_errno(&m_DBHandle) > 0)
		{
			std::stringstream szError;
			szError << "Error while fetching row from database server: " << mysql_error(&m_DBHandle);
			throw new MySQLException(szError.str(), mysql_errno(&m_DBHandle));		
		}
		return false;	
	}
	return true;
}

const unsigned char* MySQL::GetText(int Column)
{
	return (const unsigned char*)m_Row[Column];
}
int MySQL::GetInt(int Column)
{
	if(m_Row[Column] == NULL)
	{
#ifdef DEBUG
		std::cout << "Returning NULL from MySQL::GetInt()";
#endif
		return 0;
	}
/*
#ifdef DEBUG
	std::stringstream log;
	log << "mysql atoi() " << Column << " " << m_Row[Column];
	logfile.AddLine(log.str());
#endif
	*/
	return atoi(m_Row[Column]);
}
__int64 MySQL::GetInt64(int Column)
{
	
	return _atoi64(m_Row[Column]);
}
__int64 MySQL::GetLastInsertID()
{
	return (__int64)mysql_insert_id(&m_DBHandle);
}
__int64 MySQL::GetAffectedRows()
{
	return (__int64)mysql_affected_rows(&m_DBHandle);
}
void MySQL::Escape(std::string &Data)
{
	int tmpsize = Data.size() * 2 + 1;
	int usedsize;
	char tmp[8096];// = new char[tmpsize];
	memset(tmp, 0x00, sizeof(tmp));
	usedsize = mysql_real_escape_string(&m_DBHandle, tmp, Data.c_str(), Data.size()); 
	Data.assign(tmp, usedsize);
//	delete tmp;
}

