/*
	Copyright (C) 2008 Steve Thomas <SMT837784@yahoo.com>

	This file is part of RT Perfecter.

	RT Perfecter is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	RT Perfecter is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with RT Perfecter.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RTCOMMON_H
#define RTCOMMON_H

#include <stdio.h>

#ifdef _WIN32
	#define uint64 unsigned __int64
#else
	#define uint64 unsigned long long
#endif

struct RTChain
{
	uint64 startpt;
	uint64 endpt;
	short checkpt;
};

#endif

