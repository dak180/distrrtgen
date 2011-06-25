# converti2 is a tool to convert from RT and RTI to RTI2
#
# Copyright 2009, 2010, 2011 Martin Westergaard Jørgensen <martinwj2005@gmail.com>
# Copyright 2010, 2011 James Nobis <quel@quelrod.net>
# Copyright 2011 Andrew Dranse <aramith@aramith.org>
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
	RTIReader.o RTReader.o RTI2Writer.o
BINARY_NAME=converti2
COMMON_API_PATH = ../../common/rt_api
WARNALL = -Wextra -Wunused-macros -Wunsafe-loop-optimizations -Wundef \
	-Woverlength-strings -Wdisabled-optimization -Wformat-extra-args \
	-Wformat-security -Winline

RELEASE_DIR = converti2_0.3_beta3_src

all: converti2

converti2: $(OBJS)
	$(CXX) $(LFLAGS) $(OBJS) $(LIBS) converti2.cpp -o $(BINARY_NAME)

clean:
	rm -f *.o converti2 converti2.exe core

debug: DEBUG += -DDEBUG -g
debug: all

debugall: DEBUG += -DDEBUG -g $(WARNALL)
debugall: all

#m32: DEBUG += -m32
#m32: all

rebuild: clean all

release: clean
	mkdir $(RELEASE_DIR)
	cp $(COMMON_API_PATH)/BaseRTReader.* $(COMMON_API_PATH)/BaseRTWriter.* \
		$(COMMON_API_PATH)/global.h $(COMMON_API_PATH)/MemoryPool.* \
		$(COMMON_API_PATH)/Public.* $(COMMON_API_PATH)/RTI2Common.* \
		$(COMMON_API_PATH)/RTReader.* $(COMMON_API_PATH)/RTIReader.* \
		$(COMMON_API_PATH)/RTI2Writer.* charset.txt converti2.cpp \
		converti2.h ../../COPYING $(RELEASE_DIR)/
	cat Makefile | sed 's/..\/..\/common\/rt_api/./' > $(RELEASE_DIR)/Makefile

static: DEBUG += -static -static-libgcc
static: all

win32: CXX=i586-mingw32msvc-g++
win32: CC=i586-mingw32msvc-gcc
win32: BINARY_NAME=converti2.exe
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

RTReader.o: $(COMMON_API_PATH)/RTReader.h $(COMMON_API_PATH)/RTReader.cpp
	$(CXX) $(CFLAGS) $(COMMON_API_PATH)/RTReader.cpp

RTIReader.o: $(COMMON_API_PATH)/RTIReader.h $(COMMON_API_PATH)/RTIReader.cpp
	$(CXX) $(CFLAGS) $(COMMON_API_PATH)/RTIReader.cpp

RTI2Writer.o: $(COMMON_API_PATH)/RTI2Writer.h \
	$(COMMON_API_PATH)/RTI2Writer.cpp $(COMMON_API_PATH)/RTI2Common.h \
	$(COMMON_API_PATH)/RTI2Common.cpp
	$(CXX) $(CFLAGS) $(COMMON_API_PATH)/RTI2Writer.cpp
