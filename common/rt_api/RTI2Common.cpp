/*
 * freerainbowtables is a project for generating, distributing, and using
 * perfect rainbow tables
 *
 * Copyright 2011 Sc00bz
 * Copyright 2011 James Nobis <frt@quelrod.net>
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
 */

#include "RTI2Common.h"
#include <map>

enum algorithmValue { Custom
	, LM
	, NTLM
	, MD2
	, MD4
	, MD5
	, DoubleMD5
	, DoubleBinaryMD5
	, CiscoPIX
	, SHA1
	, MySQLSHA1
	, SHA256
	, SHA384
	, SHA512
	, RIPEMD160
	, MSCache
	, HalfLMChallenge
	, SecondHalfLMChallenge
	, NTLMChallenge
	, Oracle
};

static std::map<std::string, algorithmValue> mapAlgorithmValue;

static void initializeAlgorithmMap()
{
	mapAlgorithmValue["Custom"] = Custom;
	mapAlgorithmValue["lm"] = LM;
	mapAlgorithmValue["ntlm"] = NTLM;
	mapAlgorithmValue["md2"] = MD2;
	mapAlgorithmValue["md4"] = MD4;
	mapAlgorithmValue["md5"] = MD5;
	mapAlgorithmValue["doublemd5"] = DoubleMD5;
	mapAlgorithmValue["doublebinarymd5"] = DoubleBinaryMD5;
	mapAlgorithmValue["ciscopix"] = CiscoPIX;
	mapAlgorithmValue["sha1"] = SHA1;
	mapAlgorithmValue["mysqlsha1"] = MySQLSHA1;
	mapAlgorithmValue["sha256"] = SHA256;
	mapAlgorithmValue["sha384"] = SHA384;
	mapAlgorithmValue["sha512"] = SHA512;
	mapAlgorithmValue["ripemd160"] = RIPEMD160;
	mapAlgorithmValue["mscache"] = MSCache;
	mapAlgorithmValue["halflmchall"] = HalfLMChallenge;
	mapAlgorithmValue["SecondHalfLMChallenge"] = SecondHalfLMChallenge;
	mapAlgorithmValue["ntlmchall"] = NTLMChallenge;
	mapAlgorithmValue["oracle"] = Oracle;
}

uint8 getAlgorithmId( std::string algorithmName )
{
	initializeAlgorithmMap();

	switch( mapAlgorithmValue[algorithmName] )
	{
		case Custom:
			return 0;
		case LM:
			return 1;
		case NTLM:
			return 2;
		case MD2:
			return 3;
		case MD4:
			return 4;
		case MD5:
			return 5;
		case DoubleMD5:
			return 6;
		case DoubleBinaryMD5:
			return 7;
		case CiscoPIX:
			return 8;
		case SHA1:
			return 9;
		case MySQLSHA1:
			return 10;
		case SHA256:
			return 11;
		case SHA384:
			return 12;
		case SHA512:
			return 13;
		case RIPEMD160:
			return 14;
		case MSCache:
			return 15;
		case HalfLMChallenge:
			return 16;
		case SecondHalfLMChallenge:
			return 17;
		case NTLMChallenge:
			return 18;
		case Oracle:
			return 19;
		default:
			printf( "Hash Algorithm %s is not supported\n", algorithmName.c_str() );
			exit( 1 );
	}
}
