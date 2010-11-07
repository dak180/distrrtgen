SHELL = /bin/sh
CC = g++
OPTIMIZATION = -O3
INCLUDES = -I../../Common/rt\ api
# XXX todo currently only 32-bit targets work
CFLAGS = -Wall -m32 -ansi $(INCLUDES) $(OPTIMIZATION) -c $(DEBUG)
LFLAGS = -Wall -m32 -ansi $(INCLUDES) $(OPTIMIZATION) $(DEBUG)
LIBS = 
OBJS = MemoryPool.o Public.o RTI2Reader.o RTIReader.o RTReader.o
COMMON_API_PATH = ../../Common/rt\ api

all: converti2

converti2: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) converti2.cpp -o converti2

clean:
	rm -f *.o converti2

debug: DEBUG += -DDEBUG -g
debug: all

#m32: DEBUG += -m32
#m32: converti2

rebuild: clean all

MemoryPool.o: $(COMMON_API_PATH)/MemoryPool.h $(COMMON_API_PATH)/MemoryPool.cpp $(COMMON_API_PATH)/Public.h
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/MemoryPool.cpp

Public.o: $(COMMON_API_PATH)/Public.h $(COMMON_API_PATH)/Public.cpp
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/Public.cpp

RTI2Reader.o: $(COMMON_API_PATH)/RTI2Reader.h $(COMMON_API_PATH)/RTI2Reader.cpp
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/RTI2Reader.cpp

RTIReader.o: $(COMMON_API_PATH)/RTIReader.h $(COMMON_API_PATH)/RTIReader.cpp
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/RTIReader.cpp

RTReader.o: $(COMMON_API_PATH)/RTReader.h $(COMMON_API_PATH)/RTReader.cpp
	$(CC) $(CFLAGS) $(COMMON_API_PATH)/RTReader.cpp
