# freerainbowtables is a project for generating, distributing, and using
# perfect rainbow tables
#
# Copyright 2011 Janosch Rux <janosch.rux@web.de>
# Copyright 2011 James Nobis <quel@quelrod.net>
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
#
SHELL = /bin/sh
BIN = $(DESTDIR)/usr/bin
CC = g++
# -mtune=native doesn't work on NetBSD 5.1 with gcc 4.1.3 and make (fine with gmake)
#  -mtune=native doesn't work on OSX either
#  add -mtune=native to OPTIMIZATION for better performance (if it works)
OPTIMIZATION = -O3
INCLUDES = -I../common/rt_api
CFLAGS = -Wall -ansi $(INCLUDES) $(OPTIMIZATION) -c $(DEBUG)
LFLAGS = -Wall -ansi  $(INCLUDES) $(OPTIMIZATION) $(DEBUG)
LIBS = -lcrypto -lpthread
OBJS = fast_md5.o HashAlgorithm.o HashRoutine.o HashSet.o lm2ntlm.o fast_md4.o Public.o 
COMMON_API_PATH = ../common/rt_api
RCRACKI_PATH = ../client_applications/rcracki_mt

BLAWARNALL = -Wextra -Wunused-macros -Wunsafe-loop-optimizations -Wundef -Woverlength-strings -Wdisabled-optimization -Wformat-extra-args -Wformat-security -Winline

OSNAME = $(shell uname -s)
# apparently --string-debug works on Linux, OpenBSD, NetBSD, and FreeBSD
# but not on OSX so to the short name -S
STRIP = $(shell which strip) -S

#LIBS_Darwin = -lcrypto
LIBS_NetBSD = -ldes
#LIBS_OpenBSD = -lcrypto

LIBS += ${LIBS_$(OSNAME)}


all: HashTest

HashTest: $(OBJS)
	$(CC) $(LFLAGS) -o HashTest HashTest.cpp  $(OBJS) $(LIBS) 

clean:
	rm -f HashTest *.o

fast_md5.o: $(COMMON_API_PATH)/fast_md5.h $(COMMON_API_PATH)/fast_md5.cpp \
	$(COMMON_API_PATH)/global.h
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/fast_md5.cpp

Public.o: $(COMMON_API_PATH)/Public.h $(COMMON_API_PATH)/Public.cpp \
	$(COMMON_API_PATH)/global.h
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/Public.cpp

HashAlgorithm.o: $(COMMON_API_PATH)/HashAlgorithm.h $(RCRACKI_PATH)/HashAlgorithm.cpp \
	$(COMMON_API_PATH)/Public.h $(COMMON_API_PATH)/fast_md5.h \
	$(COMMON_API_PATH)/fast_md4.h
	$(CC) $(CFLAGS)  $(RCRACKI_PATH)/HashAlgorithm.cpp

HashRoutine.o: $(COMMON_API_PATH)/HashRoutine.h $(RCRACKI_PATH)/HashRoutine.cpp \
	$(COMMON_API_PATH)/global.h $(COMMON_API_PATH)/HashAlgorithm.h
	$(CC) $(CFLAGS) $(RCRACKI_PATH)/HashRoutine.cpp

HashSet.o: $(RCRACKI_PATH)/HashSet.h $(RCRACKI_PATH)/HashSet.cpp $(COMMON_API_PATH)/Public.h
	$(CC) $(CFLAGS) $(RCRACKI_PATH)/HashSet.cpp

lm2ntlm.o: $(RCRACKI_PATH)/lm2ntlm.h $(RCRACKI_PATH)/lm2ntlm.cpp $(COMMON_API_PATH)/Public.h \
	$(COMMON_API_PATH)/fast_md4.h
	$(CC) $(CFLAGS) $(RCRACKI_PATH)/lm2ntlm.cpp

fast_md4.o: $(COMMON_API_PATH)/fast_md4.h $(COMMON_API_PATH)/fast_md4.cpp \
	$(COMMON_API_PATH)/global.h
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/fast_md4.cpp

