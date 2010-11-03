SHELL = /bin/sh
CC = g++
OPTIMIZATION = -O3
INCLUDES = -I../../Common/rt\ api
# XXX todo currently only 32-bit targets work
CFLAGS = -Wall -m32 -ansi $(INCLUDES) $(OPTIMIZATION) -c
LFLAGS = -Wall -m32 -ansi $(INCLUDES) $(OPTIMIZATION)
LIBS = 
OBJS = MemoryPool.o Public.o
COMMON_API_PATH = ../../Common/rt\ api

all: converti2

converti2: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) converti2.cpp -o converti2

clean:
	rm -f *.o converti2

rebuild: clean all

MemoryPool.o: $(COMMON_API_PATH)/MemoryPool.h $(COMMON_API_PATH)/MemoryPool.cpp $(COMMON_API_PATH)/Public.h
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/MemoryPool.cpp

Public.o: $(COMMON_API_PATH)/Public.h $(COMMON_API_PATH)/Public.cpp
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/Public.cpp
