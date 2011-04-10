// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef __STDAFX_H__

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <stdio.h>
#include <tchar.h>
#define MYSQL_HOST "freerainbowtables.com"
#else
#define MYSQL_HOST "localhost"

#endif

#ifndef BUILD_ATEA
#define MYSQL_DATABASE "rainbowtables-distrrtgen"
#define MYSQL_USERNAME "rainbow"
#define MYSQL_PASSWORD "QQrQaQQ123$"
#endif

#define STATUS_TABLE_NOTSTARTED 0
#define STATUS_TABLE_INPROGRESS 1
#define STATUS_TABLE_COMPLETED 2

#ifndef _WIN32
	#define Sleep(num) sleep(num / 1000) 
	#define __int64 long long
#endif

#endif
// TODO: reference additional headers your program requires here
