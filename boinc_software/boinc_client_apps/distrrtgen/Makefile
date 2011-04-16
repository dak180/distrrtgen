# freerainbowtables is a project for generating, distributing, and using
# perfect rainbow tables
#
# Copyright (C) Zhu Shuanglei <shuanglei@hotmail.com>
# Copyright Martin Westergaard Jørgensen <martinwj2005@gmail.com>
# Copyright 2009, 2010 Daniël Niggebrugge <niggebrugge@fox-it.com>
# Copyright 2009, 2010, 2011 James Nobis <frt@quelrod.net>
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
# Debian
BOINC_DIR = /usr/share/boinc-dev
# FreeBSD
#BOINC_DIR = /usr/local/lib
# Linux
BOINC_API_DIR = $(BOINC_DIR)/api
# FreeBSD
#BOINC_API_DIR = /usr/local/lib
# Linux
BOINC_LIB_DIR = $(BOINC_DIR)/lib
# FreeBSD
#BOINC_LIB_DIR = $(BOINC_DIR)
#BOINC_INC_DIR = /usr/local/include/boinc
CC = g++
COMMON_API_PATH = ../../../common/rt_api
COMMON_LIB_DIR = /usr/lib
INCLUDES = -I$(COMMON_API_PATH) -I$(BOINC_DIR) -I$(BOINC_LIB_DIR) -I$(BOINC_API_DIR) -I$(BOINC_INC_DIR)

OPTIMIZATION = -O3
WARNALL = -Wextra -Wunused-macros -Wunsafe-loop-optimizations -Wundef -Woverlength-strings -Wdisabled-optimization -Wformat-extra-args -Wformat-security -Winline

CXXFLAGS = -g \
    -static -static-libgcc \
	 -Wall \
	 -c \
	 -DBOINC \
	 $(INCLUDES) \
	 $(OPTIMIZATION) \
	 $(DEBUG)

LFLAGS = -static -static-libgcc -Wall $(INCLUDES) $(OPTIMIZATION) \
	-L$(BOINC_API_DIR) -L$(BOINC_LIB_DIR) -L /usr/X11R6/lib -L. $(DEBUG)

LIBS = -lboinc_api -lboinc -lpthread -lssl libstdc++.a \
	$(COMMON_LIB_DIR)/libssl.a $(COMMON_LIB_DIR)/libpthread.a

OBJS = ChainWalkContext.o des_enc.o des_setkey.o distrrtgen.o ecb_enc.o \
	HashAlgorithm.o HashRoutine.o md4.o md5.o Public.o

OSNAME = $(shell uname -s)

all: distrrtgen

distrrtgen: libstdc++.a $(BOINC_API_DIR)/libboinc_api.a $(BOINC_LIB_DIR)/libboinc.a $(OBJS)
	$(CXX) $(LFLAGS) $(OBJS) $(LIBS) -o distrrtgen

libstdc++.a:
	ln -s `$(CC) $(DEBUG) -print-file-name=libstdc++.a`

clean:
	/bin/rm -f distrrtgen *.o

debug: DEBUG += -DDEBUG -g
debug: all

debugall: DEBUG += -DDEBUG -g $(WARNALL)
debugall: distrrtgen

distclean:
	/bin/rm -f distrrtgen *.o libstdc++.a stderr.txt boinc_lockfile result \
		init_data.xml boinc_finish_called

rebuild: clean all

ChainWalkContext.o: $(COMMON_API_PATH)/ChainWalkContext.h \
	$(COMMON_API_PATH)/ChainWalkContext.cpp $(COMMON_API_PATH)/HashRoutine.h \
	$(COMMON_API_PATH)/Public.h
	$(CXX) $(CXXFLAGS) $(COMMON_API_PATH)/ChainWalkContext.cpp

des_enc.o: $(COMMON_API_PATH)/des_enc.c $(COMMON_API_PATH)/des_locl.h \
	$(COMMON_API_PATH)/spr.h
	$(CXX) $(CXXFLAGS) $(COMMON_API_PATH)/des_enc.c

des_setkey.o: $(COMMON_API_PATH)/des_setkey.c $(COMMON_API_PATH)/des_locl.h \
	$(COMMON_API_PATH)/podd.h $(COMMON_API_PATH)/sk.h
	$(CXX) $(CXXFLAGS) $(COMMON_API_PATH)/des_setkey.c

distrrtgen.o: distrrtgen.cpp distrrtgen.h $(COMMON_API_PATH)/ChainWalkContext.h $(COMMON_API_PATH)/Public.h
	$(CXX) $(CXXFLAGS) distrrtgen.cpp

ecb_enc.o: $(COMMON_API_PATH)/ecb_enc.c $(COMMON_API_PATH)/des_locl.h \
	$(COMMON_API_PATH)/spr.h
	$(CXX) $(CXXFLAGS) $(COMMON_API_PATH)/ecb_enc.c

HashAlgorithm.o: $(COMMON_API_PATH)/HashAlgorithm.h \
	$(COMMON_API_PATH)/HashAlgorithm.cpp $(COMMON_API_PATH)/Public.h \
	$(COMMON_API_PATH)/md4.h $(COMMON_API_PATH)/md5.h
	$(CXX) $(CXXFLAGS) $(COMMON_API_PATH)/HashAlgorithm.cpp

HashRoutine.o: $(COMMON_API_PATH)/HashRoutine.h \
	$(COMMON_API_PATH)/HashRoutine.cpp $(COMMON_API_PATH)/global.h \
	$(COMMON_API_PATH)/HashAlgorithm.h
	$(CXX) $(CXXFLAGS) $(COMMON_API_PATH)/HashRoutine.cpp

md4.o: $(COMMON_API_PATH)/md4.h $(COMMON_API_PATH)/md4.cpp \
	$(COMMON_API_PATH)/global.h
	$(CXX) $(CXXFLAGS) $(COMMON_API_PATH)/md4.cpp

md5.o: $(COMMON_API_PATH)/md5.h $(COMMON_API_PATH)/md5.cpp \
	$(COMMON_API_PATH)/global.h
	$(CXX) $(CXXFLAGS) $(COMMON_API_PATH)/md5.cpp

Public.o: $(COMMON_API_PATH)/Public.h $(COMMON_API_PATH)/Public.cpp \
	$(COMMON_API_PATH)/global.h
	$(CXX) $(CXXFLAGS) $(COMMON_API_PATH)/Public.cpp
