SHELL = /bin/sh
CC = g++
OPTIMIZATION = -O3
INCLUDES = -I../../Common/rt\ api
CFLAGS = -Wall -ansi $(INCLUDES) $(OPTIMIZATION) -c $(DEBUG)
LFLAGS = -Wall -ansi $(INCLUDES) $(OPTIMIZATION) $(DEBUG)
LIBS = 
OBJS = MemoryPool.o Public.o RTI2Reader.o RTIReader.o
COMMON_API_PATH = ../../Common/rt\ api

all: rti2rto

rti2rto: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) rti2rto.cpp -o rti2rto

clean:
	rm -f *.o rti2rto

debug: DEBUG += -DDEBUG -g
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

MemoryPool.o: $(COMMON_API_PATH)/MemoryPool.h $(COMMON_API_PATH)/MemoryPool.cpp $(COMMON_API_PATH)/Public.h
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/MemoryPool.cpp

Public.o: $(COMMON_API_PATH)/Public.h $(COMMON_API_PATH)/Public.cpp
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/Public.cpp

RTI2Reader.o: $(COMMON_API_PATH)/RTI2Reader.h $(COMMON_API_PATH)/RTI2Reader.cpp
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/RTI2Reader.cpp

RTIReader.o: $(COMMON_API_PATH)/RTIReader.h $(COMMON_API_PATH)/RTIReader.cpp
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/RTIReader.cpp
