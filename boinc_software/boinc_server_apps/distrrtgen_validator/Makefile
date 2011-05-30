# distrrtgen_validator is a tool to validate completed BOINC WUs
# from distrrtgen[_cuda] as part of the freerainbowtables project.
#
# Copyright Martin Westergaard Jørgensen <martinwj2005@gmail.com>
# Copyright 2009, 2010 Daniël Niggebrugge <niggebrugge@fox-it.com>
# Copyright 2009, 2010, 2011 James Nobis <quel@quelrod.net>
# Copyright 2011 Andrew Dranse <aramith@aramith.org>
#
# This file is part of distrrtgen_validator.
#
# distrrtgen_validator is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# distrrtgen_validator is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with distrrtgen_validator.  If not, see <http://www.gnu.org/licenses/>.

SHELL = /bin/sh
CXX = g++
CC = gcc

# NetBSD server
BOINC_DIR = /home/frt/src/boinc_core_release_6_12
#Debian GNU/Linux
#BOINC_DIR = /home/quel/download/boinc_core_release_6_12.PB
BOINC_API_DIR = $(BOINC_DIR)/api
BOINC_LIB_DIR = $(BOINC_DIR)/lib
BOINC_SCHED_DIR = $(BOINC_DIR)/sched
BOINC_DB_DIR = $(BOINC_DIR)/db
# NetBSD
MYSQL_DIR = /usr/local/include/mysql
MYSQL_LIB_DIR = /usr/local/lib/mysql
# Debian GNU/Linux
#MYSQL_DIR = /usr/include/mysql
#MYSQL_LIB_DIR = /usr/lib/mysql
# NetBSD
COMMON_LIB_DIR = /usr/local/lib
# Debian GNU/Linux
#COMMON_LIB_DIR = /usr/lib
COMMON_API_PATH = ../../../common/rt_api

# NetBSD doesn't like native
#-mtune=native
OPTIMIZATION = -O3

INCLUDES = -I$(BOINC_DIR) \
	-I$(BOINC_LIB_DIR) \
	-I$(BOINC_API_DIR) \
	-I$(BOINC_SCHED_DIR) \
	-I$(BOINC_DB_DIR) \
	-I$(MYSQL_DIR) \
	-I$(COMMON_API_PATH)

LFLAGS = -L$(MYSQL_LIB_DIR) \
	-L$(BOINC_API_DIR) \
	-L$(BOINC_LIB_DIR) \
	-L$(BOINC_SCHED_DIR) \
	-L$(BOINC_DB_DIR) \
	-L/usr/X11R6/lib \
	-L. \
	-Wall \
	$(OPTIMIZATION)

LIBS = libstdc++.a -lpthread -lboinc_api -lboinc -lcrypto -lssl -lmysqlclient \
	$(BOINC_SCHED_DIR)/libsched.a $(BOINC_LIB_DIR)/libboinc.a

OBJS = ChainWalkContext.o des_enc.o des_setkey.o ecb_enc.o \
	fast_md4.o fast_md5.o HashAlgorithm.o HashRoutine.o MemoryPool.o \
	part_validator.o Public.o validate_util.o validate_util2.o validator.o

CXXFLAGS = -g \
	-DAPP_GRAPHICS \
	-DBOINC \
	$(INCLUDES) \
	-Wall \
	$(OPTIMIZATION)

PROG = distrrtgen_validator

RELEASE_DIR = distrrtgen_validator

all: $(PROG)

distrrtgen_validator: libstdc++.a $(BOINC_API_DIR)/libboinc_api.a \
	$(BOINC_LIB_DIR)/libboinc.a $(BOINC_SCHED_DIR)/libsched.a $(OBJS)
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic $(LFLAGS) $(OBJS) $(LIBS) -o $(PROG)

libstdc++.a:
	ln -s `g++ -print-file-name=libstdc++.a`

clean:
	/bin/rm -f $(PROG) *.o

distclean:
	/bin/rm -f $(PROG) *.o libstdc++.a

rebuild: clean all

release: clean
	mkdir $(RELEASE_DIR)
	cp $(COMMON_API_PATH)/ChainWalkContext.* $(COMMON_API_PATH)/des.h \
		$(COMMON_API_PATH)/des_enc.c $(COMMON_API_PATH)/des_locl.h \
		$(COMMON_API_PATH)/des_setkey.c $(COMMON_API_PATH)/ecb_enc.c \
		$(COMMON_API_PATH)/HashAlgorithm.* $(COMMON_API_PATH)/HashRoutine.* \
		$(COMMON_API_PATH)/MemoryPool.* $(COMMON_API_PATH)/global.h \
		$(COMMON_API_PATH)/fast_md4.* $(COMMON_API_PATH)/fast_md5.* \
		$(COMMON_API_PATH)/podd.h $(COMMON_API_PATH)/sha1.* \
		$(COMMON_API_PATH)/Public.* $(COMMON_API_PATH)/sk.h \
		$(COMMON_API_PATH)/spr.h charset.txt part_validator.* validate_util.* \
		validator.* validate_util2.* ../../../COPYING $(RELEASE_DIR)/
	cat Makefile | sed 's/..\/..\/common\/rt_api/./' > $(RELEASE_DIR)/Makefile

ChainWalkContext.o: $(COMMON_API_PATH)/ChainWalkContext.h \
	ChainWalkContext.cpp $(COMMON_API_PATH)/HashRoutine.h \
	$(COMMON_API_PATH)/Public.h
	$(CXX) $(CXXFLAGS) -c ChainWalkContext.cpp

des_enc.o: $(COMMON_API_PATH)/des_enc.c $(COMMON_API_PATH)/des_locl.h \
	$(COMMON_API_PATH)/spr.h
	$(CXX) $(CXXFLAGS) -c $(COMMON_API_PATH)/des_enc.c

des_setkey.o: $(COMMON_API_PATH)/des_setkey.c $(COMMON_API_PATH)/des_locl.h \
	$(COMMON_API_PATH)/podd.h $(COMMON_API_PATH)/sk.h
	$(CXX) $(CXXFLAGS) -c $(COMMON_API_PATH)/des_setkey.c

ecb_enc.o: $(COMMON_API_PATH)/ecb_enc.c $(COMMON_API_PATH)/des_locl.h \
	$(COMMON_API_PATH)/spr.h
	$(CXX) $(CXXFLAGS) -c $(COMMON_API_PATH)/ecb_enc.c

HashAlgorithm.o: $(COMMON_API_PATH)/HashAlgorithm.h \
	$(COMMON_API_PATH)/HashAlgorithm.cpp $(COMMON_API_PATH)/Public.h \
	$(COMMON_API_PATH)/fast_md4.h $(COMMON_API_PATH)/fast_md5.h
	$(CXX) $(CXXFLAGS) -c $(COMMON_API_PATH)/HashAlgorithm.cpp

HashRoutine.o: $(COMMON_API_PATH)/HashRoutine.h \
	$(COMMON_API_PATH)/HashRoutine.cpp $(COMMON_API_PATH)/global.h \
	$(COMMON_API_PATH)/HashAlgorithm.h
	$(CXX) $(CXXFLAGS) -c $(COMMON_API_PATH)/HashRoutine.cpp

fast_md4.o: $(COMMON_API_PATH)/fast_md4.h $(COMMON_API_PATH)/fast_md4.cpp \
	$(COMMON_API_PATH)/global.h
	$(CXX) $(CXXFLAGS) -c $(COMMON_API_PATH)/fast_md4.cpp

fast_md5.o: $(COMMON_API_PATH)/fast_md5.h $(COMMON_API_PATH)/fast_md5.cpp \
	$(COMMON_API_PATH)/global.h
	$(CXX) $(CXXFLAGS) -c $(COMMON_API_PATH)/fast_md5.cpp

MemoryPool.o: $(COMMON_API_PATH)/MemoryPool.h \
	$(COMMON_API_PATH)/MemoryPool.cpp $(COMMON_API_PATH)/Public.h \
	$(COMMON_API_PATH)/global.h
	$(CXX) $(CXXFLAGS) -c $(COMMON_API_PATH)/MemoryPool.cpp

part_validator.o: part_validator.h $(COMMON_API_PATH)/ChainWalkContext.h \
	$(COMMON_API_PATH)/MemoryPool.h validate_util.h
	$(CXX) $(CXXFLAGS) -c part_validator.cpp

Public.o: $(COMMON_API_PATH)/Public.h $(COMMON_API_PATH)/Public.cpp \
	$(COMMON_API_PATH)/global.h
	$(CXX) $(CXXFLAGS) -c $(COMMON_API_PATH)/Public.cpp

validate_util.o: validate_util.h validator.h
	$(CXX) $(CXXFLAGS) -c validate_util.cpp

validate_util2.o: part_validator.h  validator.h validate_util.h \
	validate_util2.h $(COMMON_API_PATH)/ChainWalkContext.h \
	$(COMMON_API_PATH)/Public.h
	$(CXX) $(CXXFLAGS) -c validate_util2.cpp

validator.o: validator.h $(BOINC_SCHED_DIR)/sched_config.h \
	$(BOINC_DB_DIR)/boinc_db.h $(BOINC_SCHED_DIR)/sched_util.h \
	$(BOINC_SCHED_DIR)/sched_msgs.h validate_util.h validate_util2.h \
	$(COMMON_API_PATH)/Public.h
	$(CXX) $(CXXFLAGS) -c validator.cpp
