#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__
#include <string>


class Exception
{
protected:
	std::string m_Message;

public:
	Exception(std::string szErrorMessage);
	const char* GetErrorMessage();
public:
	~Exception(void);
};

class DatabaseException : public Exception
{
protected:
	unsigned int nErrorCode;	
	
public:
	DatabaseException(std::string szError, unsigned int nErrorNum) : Exception(szError) { nErrorCode = nErrorNum; };
	unsigned int getErrorCode() { return nErrorCode; }
};

#endif

