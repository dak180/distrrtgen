# freerainbowtables is a project for generating, distributing, and using
# perfect rainbow tables
#
# Copyright (C) Zhu Shuanglei <shuanglei@hotmail.com>
# Copyright 2010 Martin Westergaard Jørgensen <martinwj2005@gmail.com>
# Copyright 2009, 2010 Daniël Niggebrugge <niggebrugge@fox-it.com>
# Copyright 2009, 2010, 2011 James Nobis <quel@quelrod.net>
# Copyright 2011 Andrew Dranse <aramith@aramith.org>
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
CC = g++
COMMON_LIB_DIR = /usr/lib
CUDA_PATH = /usr/local/cuda
CUDA_LIB = libcudart.so.2
#CUDA_LIB = libcudart.so.3
INCLUDES = -I../../../common/rt_api -I$(BOINC_DIR) -I$(BOINC_LIB_DIR) \
	-I$(BOINC_API_DIR) -I$(CUDA_PATH)/include
COMMON_API_PATH = ../../../common/rt_api

OPTIMIZATION = -O3
WARNALL = -Wextra -Wunused-macros -Wunsafe-loop-optimizations -Wundef \
	-Woverlength-strings -Wdisabled-optimization -Wformat-extra-args \
	-Wformat-security -Winline

CXXFLAGS = -Wall -ansi -c -DBOINC $(INCLUDES) $(OPTIMIZATION) $(DEBUG)
NVCCFLAGS = --ptxas-options=-v

LFLAGS = -Wall -ansi $(INCLUDES) $(OPTIMIZATION) \
	-L$(BOINC_API_DIR) -L$(BOINC_LIB_DIR) -L/usr/X11R6/lib -L/usr/lib \
	-L$(CUDA_PATH)/lib64 -L$(CUDA_PATH)/lib -L. -Xlinker -rpath . \
	$(DEBUG)

LIBS = -lboinc_api -lboinc -lpthread -lcudart $(CUDA_LIB) libgcc_s.so.1 \
	libstdc++.so.6 $(COMMON_LIB_DIR)/libpthread.a
OBJS = ChainWalkContext.o des_enc.o des_setkey.o distrrtgen.o ecb_enc.o \
	HashAlgorithm.o HashRoutine.o fast_md4.o fast_md5.o Public.o rcuda_ext.o \
	rcuda.o

OSNAME = $(shell uname -s)

all: distrrtgen

distrrtgen: $(CUDA_LIB) libgcc_s.so.1 libstdc++.so.6 $(BOINC_API_DIR)/libboinc_api.a $(BOINC_LIB_DIR)/libboinc.a $(OBJS)
	$(CXX) $(LFLAGS) $(OBJS) $(LIBS) -o distrrtgen

$(CUDA_LIB):
	if [ ! -f $(CUDA_LIB) ]; then ln -s $(CUDA_PATH)/lib64/$(CUDA_LIB) $(CUDA_LIB); fi

libstdc++.so.6:
	if [ ! -f libstdc++.so.6 ]; then ln -s `$(CC) $(CXXFLAGS) -print-file-name=libstdc++.so.6`; fi

libgcc_s.so.1:
	if [ ! -f libgcc_s.so.1 ]; then ln -s `$(CC) $(CXXFLAGS) -print-file-name=libgcc_s.so.1`; fi

clean:
	/bin/rm -f distrrtgen *.o

debug: DEBUG += -DDEBUG -g
debug: NVCCDEBUG += -DDEBUG -g
debug: all

debugall: DEBUG += -DDEBUG -g $(WARNALL)
debugall: NVCCDEBUG += -DDEBUG -g
debugall: all

distclean: clean
	/bin/rm -f $(CUDA_LIB) libgcc_s.so.1 libstdc++.so.6 \
		boinc_lockfile stderr.txt result init_data.xml boinc_finish_called

rebuild: clean all

ChainWalkContext.o: $(COMMON_API_PATH)/ChainWalkContext.h $(COMMON_API_PATH)/ChainWalkContext.cpp $(COMMON_API_PATH)/HashRoutine.h $(COMMON_API_PATH)/Public.h
	$(CXX) $(CXXFLAGS) $(COMMON_API_PATH)/ChainWalkContext.cpp

des_enc.o: $(COMMON_API_PATH)/des_enc.c $(COMMON_API_PATH)/des_locl.h $(COMMON_API_PATH)/spr.h
	$(CXX) $(CXXFLAGS) $(COMMON_API_PATH)/des_enc.c

des_setkey.o: $(COMMON_API_PATH)/des_setkey.c $(COMMON_API_PATH)/des_locl.h $(COMMON_API_PATH)/podd.h $(COMMON_API_PATH)/sk.h
	$(CXX) $(CXXFLAGS) $(COMMON_API_PATH)/des_setkey.c

distrrtgen.o: distrrtgen.cpp distrrtgen.h $(COMMON_API_PATH)/ChainWalkContext.h $(COMMON_API_PATH)/Public.h
	$(CXX) $(CXXFLAGS) distrrtgen.cpp

ecb_enc.o: $(COMMON_API_PATH)/ecb_enc.c $(COMMON_API_PATH)/des_locl.h $(COMMON_API_PATH)/spr.h
	$(CXX) $(CXXFLAGS) $(COMMON_API_PATH)/ecb_enc.c

HashAlgorithm.o: $(COMMON_API_PATH)/HashAlgorithm.h $(COMMON_API_PATH)/HashAlgorithm.cpp $(COMMON_API_PATH)/Public.h $(COMMON_API_PATH)/fast_md4.h 
	$(CXX) $(CXXFLAGS) $(COMMON_API_PATH)/HashAlgorithm.cpp

HashRoutine.o: $(COMMON_API_PATH)/HashRoutine.h $(COMMON_API_PATH)/HashRoutine.cpp $(COMMON_API_PATH)/global.h $(COMMON_API_PATH)/HashAlgorithm.h
	$(CXX) $(CXXFLAGS) $(COMMON_API_PATH)/HashRoutine.cpp

fast_md4.o: $(COMMON_API_PATH)/fast_md4.h $(COMMON_API_PATH)/fast_md4.cpp \
	$(COMMON_API_PATH)/global.h
	$(CXX) $(CXXFLAGS) $(COMMON_API_PATH)/fast_md4.cpp

fast_md5.o: $(COMMON_API_PATH)/fast_md5.h $(COMMON_API_PATH)/fast_md5.cpp $(COMMON_API_PATH)/global.h
	$(CXX) $(CXXFLAGS) $(COMMON_API_PATH)/fast_md5.cpp

Public.o: $(COMMON_API_PATH)/Public.h $(COMMON_API_PATH)/Public.cpp $(COMMON_API_PATH)/global.h
	$(CXX) $(CXXFLAGS) $(COMMON_API_PATH)/Public.cpp

rcuda_ext.o: rcuda_ext.h rcuda_ext.cpp $(COMMON_API_PATH)/ChainWalkContext.h
	$(CXX) $(CXXFLAGS) rcuda_ext.cpp

rcuda.o: rcuda.h rcuda.cu $(COMMON_API_PATH)/Public.h $(COMMON_API_PATH)/global.h
	$(CUDA_PATH)/bin/nvcc --compiler-bindir=/usr/bin/gcc-4.3 $(OPTIMIZATION) \
		$(NVCCDEBUG) $(NVCCFLAGS) --compiler-options="$(DEBUG)" \
		--compiler-options="$(CXXFLAGS)" -I../../../common/rt_api \
		--compile rcuda.cu
# debug needs a -G but:
# /usr/local/cuda/bin/../include/common_functions.h(82): Error: memcpy intrinsic
# needs a constant size (non-inlined intrinsics not supported)

#	$(CUDA_PATH)/bin/nvcc --compiler-bindir=/usr/bin/gcc-4.3 $(OPTIMIZATION) \
#		$(NVCCDEBUG) $(NVCCFLAGS) --compiler-options="$(DEBUG)" --machine 32 \
#		--compiler-options="$(CXXFLAGS)" -I../../../common/rt_api \
#		--compile rcuda.cu
