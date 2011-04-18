# converti2 is a tool to convert from RT and RTI to RTI2
#
# Copyright 2009, 2010, 2011 Martin Westergaard Jørgensen <martinwj2005@gmail.com>
# Copyright 2010, 2011 James Nobis <frt@quelrod.net>
#
# This file is part of converti2.
#
# converti2 is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# converti2 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with converti2.  If not, see <http://www.gnu.org/licenses/>.

SHELL = /bin/sh
CXX = g++
CC = gcc
OPTIMIZATION = -O3
INCLUDES = -I../../common/rt_api
CFLAGS = -Wall -ansi $(INCLUDES) $(OPTIMIZATION) -c $(DEBUG)
LFLAGS = -Wall -ansi $(INCLUDES) $(OPTIMIZATION) $(DEBUG)
LIBS = 
OBJS = BaseRTReader.o BaseRTWriter.o MemoryPool.o Public.o RTI2Common.o \
	RTI2Reader.o RTIReader.o RTReader.o RTI2Writer.o
COMMON_API_PATH = ../../common/rt_api

all: converti2

converti2: $(OBJS)
	$(CXX) $(LFLAGS) $(OBJS) $(LIBS) converti2.cpp -o converti2

clean:
	rm -f *.o converti2 core

debug: DEBUG += -DDEBUG -g
debug: all

#m32: DEBUG += -m32
#m32: all

rebuild: clean all

static: DEBUG += -static -static-libgcc
static: all

win32: CXX=i586-mingw32msvc-g++
win32: CC=i586-mingw32msvc-gcc
win32: LIBS += -L/usr/lib/gcc/i586-mingw32msvc/4.2.1-sjlj/lib
win32: all

BaseRTReader.o: $(COMMON_API_PATH)/Public.h $(COMMON_API_PATH)/BaseRTReader.h
	$(CXX) $(CFLAGS) $(COMMON_API_PATH)/BaseRTReader.cpp

BaseRTWriter.o: $(COMMON_API_PATH)/Public.h $(COMMON_API_PATH)/BaseRTWriter.h
	$(CXX) $(CFLAGS) $(COMMON_API_PATH)/BaseRTWriter.cpp

MemoryPool.o: $(COMMON_API_PATH)/MemoryPool.h $(COMMON_API_PATH)/MemoryPool.cpp $(COMMON_API_PATH)/Public.h
	$(CXX) $(CFLAGS) $(COMMON_API_PATH)/MemoryPool.cpp

Public.o: $(COMMON_API_PATH)/Public.h $(COMMON_API_PATH)/Public.cpp
	$(CXX) $(CFLAGS) $(COMMON_API_PATH)/Public.cpp

RTI2Common.o: $(COMMON_API_PATH)/RTI2Common.h
	$(CXX) $(CFLAGS) $(COMMON_API_PATH)/RTI2Common.cpp

RTI2Reader.o: $(COMMON_API_PATH)/RTI2Reader.h $(COMMON_API_PATH)/RTI2Reader.cpp
	$(CXX) $(CFLAGS) $(COMMON_API_PATH)/RTI2Reader.cpp

RTIReader.o: $(COMMON_API_PATH)/RTIReader.h $(COMMON_API_PATH)/RTIReader.cpp
	$(CXX) $(CFLAGS) $(COMMON_API_PATH)/RTIReader.cpp

RTReader.o: $(COMMON_API_PATH)/RTReader.h $(COMMON_API_PATH)/RTReader.cpp
	$(CXX) $(CFLAGS) $(COMMON_API_PATH)/RTReader.cpp

RTI2Writer.o: $(COMMON_API_PATH)/RTI2Writer.h \
	$(COMMON_API_PATH)/RTI2Writer.cpp $(COMMON_API_PATH)/RTI2Common.h \
	$(COMMON_API_PATH)/RTI2Common.cpp
	$(CXX) $(CFLAGS) $(COMMON_API_PATH)/RTI2Writer.cpp
