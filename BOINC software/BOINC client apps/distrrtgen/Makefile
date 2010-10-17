# freerainbowtables is a project for generating, distributing, and using
# perfect rainbow tables
#
# Copyright (C) Zhu Shuanglei <shuanglei@hotmail.com>
# Copyright Martin Westergaard Jørgensen <martinwj2005@gmail.com>
# Copyright 2009, 2010 Daniël Niggebrugge <niggebrugge@fox-it.com>
# Copyright 2009, 2010 James Nobis <frt@quelrod.net>
#
# This file is part of freerainbowtables.
#
# freerainbowtables is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# freerainbowtables is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with freerainbowtables.  If not, see <http://www.gnu.org/licenses/>.

SHELL = /bin/sh

#BOINC_DIR = /home/ubuntu/boinc
BOINC_DIR = /usr/share/boinc-dev
BOINC_API_DIR = $(BOINC_DIR)/api
BOINC_LIB_DIR = $(BOINC_DIR)/lib
COMMON_LIB_DIR = /usr/lib

OPTIMIZATION = -O3

CXXFLAGS = -g \
    -static -static-libgcc \
    -I$(BOINC_DIR) \
    -I$(BOINC_LIB_DIR) \
    -I$(BOINC_API_DIR) \
	 -Wall \
	 -c \
	 -DBOINC \
	 $(OPTIMIZATION)

LFLAGS = -static -static-libgcc -Wall $(OPTIMIZATION) -L$(BOINC_API_DIR) \
	-L$(BOINC_LIB_DIR) -L /usr/X11R6/lib -L.

LIBS = -lboinc_api -lboinc -lpthread -lssl libstdc++.a $(COMMON_LIB_DIR)/libssl.a $(COMMON_LIB_DIR)/libpthread.a
OBJS = ChainWalkContext.o des_enc.o des_setkey.o distrrtgen.o ecb_enc.o HashAlgorithm.o HashRoutine.o md4.o md5.o Public.o

OSNAME = $(shell uname -s)

all: distrrtgen

distrrtgen: libstdc++.a $(BOINC_API_DIR)/libboinc_api.a $(BOINC_LIB_DIR)/libboinc.a $(OBJS)
	$(CXX) $(LFLAGS) $(OBJS) $(LIBS) -o distrrtgen

libstdc++.a:
	ln -s `g++ -print-file-name=libstdc++.a`

clean:
	/bin/rm -f distrrtgen *.o

distclean:
	/bin/rm -f distrrtgen *.o libstdc++.a

rebuild: clean all

ChainWalkContext.o: ChainWalkContext.h ChainWalkContext.cpp HashRoutine.h Public.h
	$(CXX) $(CXXFLAGS) ChainWalkContext.cpp

des_enc.o: des_enc.c des_locl.h spr.h
	$(CXX) $(CXXFLAGS) des_enc.c

des_setkey.o: des_setkey.c des_locl.h podd.h sk.h
	$(CXX) $(CXXFLAGS) des_setkey.c

distrrtgen.o: distrrtgen.cpp distrrtgen.h ChainWalkContext.h Public.h
	$(CXX) $(CXXFLAGS) distrrtgen.cpp

ecb_enc.o: ecb_enc.c des_locl.h spr.h
	$(CXX) $(CXXFLAGS) ecb_enc.c

HashAlgorithm.o: HashAlgorithm.h HashAlgorithm.cpp Public.h md4.h md5.h
	$(CXX) $(CXXFLAGS) HashAlgorithm.cpp

HashRoutine.o: HashRoutine.h HashRoutine.cpp global.h HashAlgorithm.h
	$(CXX) $(CXXFLAGS) HashRoutine.cpp

md4.o: md4.h md4.cpp global.h
	$(CXX) $(CXXFLAGS) md4.cpp

md5.o: md5.h md5.cpp global.h
	$(CXX) $(CXXFLAGS) md5.cpp

Public.o: Public.h Public.cpp global.h
	$(CXX) $(CXXFLAGS) Public.cpp
