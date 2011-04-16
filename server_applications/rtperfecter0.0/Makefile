#	Copyright (C) 2008 Steve Thomas <SMT837784@yahoo.com>
#
#	This file is part of RT Perfecter v0.0.
#
#	RT Perfecter v0.0 is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	RT Perfecter v0.0 is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with RT Perfecter v0.0.  If not, see <http://www.gnu.org/licenses/>.

SHELL = /bin/sh

CXX = g++
COMMON_API_PATH = ../../common/rt_api
INCLUDES = -I$(COMMON_API_PATH)
OPTIMIZATION = -O3

CXXFLAGS = -Wall $(OPTIMIZATION)

all: main

main: 
	$(CXX) $(CXXFLAGS) RTRead.cpp RTWrite.cpp main.cpp -o rtperfecter0
clean:
	rm -f *.o rtperfecter0

