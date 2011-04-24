# rti2rto is a tool to convert from RTI and RTI2 to RT
#
# Copyright 2009, 2010, 2011 Martin Westergaard Jørgensen <martinwj2005@gmail.com>
# Copyright 2010, 2011 James Nobis <quel@quelrod.net>
# Copyright 2011 Andrew Dranse <aramith@aramith.org>
#
# This file is part of rti2rto.
#
# rti2rto is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# rti2rto is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with rti2rto.  If not, see <http://www.gnu.org/licenses/>.

SHELL = /bin/sh
CC = g++
OPTIMIZATION = -O3
INCLUDES = -I../../common/rt_api
CFLAGS = -Wall -ansi $(INCLUDES) $(OPTIMIZATION) -c $(DEBUG)
LFLAGS = -Wall -ansi $(INCLUDES) $(OPTIMIZATION) $(DEBUG)
LIBS = 
OBJS = BaseRTReader.o BaseRTWriter.o MemoryPool.o Public.o RTI2Common.o \
	RTI2Reader.o RTIReader.o
COMMON_API_PATH = ../../common/rt_api

all: rti2rto

rti2rto: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) rti2rto.cpp -o rti2rto

clean:
	rm -f *.o rti2rto core

debug: DEBUG += -DDEBUG -g -O0
debug: all

m32: DEBUG += -m32
m32: rti2rto

rebuild: clean all

static: DEBUG += -static -static-libgcc
static: all

static32: DEBUG += -m32 -static -static-libgcc
static32: all

win32: CC=i586-mingw32msvc-g++
win32: LIBS += -L/usr/lib/gcc/i586-mingw32msvc/4.2.1-sjlj/lib
win32: all

BaseRTReader.o: $(COMMON_API_PATH)/Public.h $(COMMON_API_PATH)/BaseRTReader.h
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/BaseRTReader.cpp

BaseRTWriter.o: $(COMMON_API_PATH)/Public.h $(COMMON_API_PATH)/BaseRTWriter.h
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/BaseRTWriter.cpp

MemoryPool.o: $(COMMON_API_PATH)/MemoryPool.h $(COMMON_API_PATH)/MemoryPool.cpp $(COMMON_API_PATH)/Public.h
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/MemoryPool.cpp

Public.o: $(COMMON_API_PATH)/Public.h $(COMMON_API_PATH)/Public.cpp
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/Public.cpp

RTI2Common.o: $(COMMON_API_PATH)/RTI2Common.h
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/RTI2Common.cpp

RTI2Reader.o: $(COMMON_API_PATH)/RTI2Reader.h $(COMMON_API_PATH)/RTI2Reader.cpp
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/RTI2Reader.cpp

RTIReader.o: $(COMMON_API_PATH)/RTIReader.h $(COMMON_API_PATH)/RTIReader.cpp
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/RTIReader.cpp
