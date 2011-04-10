#ifndef __DATABASE_H__
#define __DATABASE_H__
typedef __int64 uint64;
class CDatabase
{
public:
	virtual void Query(std::string szQuery) = 0;
	virtual uint64 Select(std::string szQuery) = 0;
	virtual int SelectUnbuffered(std::string szQuery) = 0;
	virtual int Fetch() = 0;
	virtual void Escape(std::string& szQuery) = 0;
	virtual int GetInt(int nColumn) = 0;
	virtual const unsigned char* GetText(int nColumn) = 0;
	virtual __int64 GetInt64(int Column) = 0;
	virtual __int64 GetLastInsertID() = 0; 
	virtual __int64 GetAffectedRows() = 0;
};

#endif

