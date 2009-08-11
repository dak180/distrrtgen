# This should work on Linux.  Modify as needed for other platforms.

BOINC_DIR = /home/frt/server_stable
BOINC_API_DIR = $(BOINC_DIR)/api
BOINC_LIB_DIR = $(BOINC_DIR)/lib
BOINC_SCHED_DIR = $(BOINC_DIR)/sched
BOINC_DB_DIR = $(BOINC_DIR)/db
MYSQL_DIR = /usr/local/include/mysql
MYSQL_LIB_DIR = /usr/local/lib/mysql
CXXFLAGS = -g \
    -DAPP_GRAPHICS \
    -I$(BOINC_DIR) \
    -I$(BOINC_LIB_DIR) \
    -I$(BOINC_API_DIR) \
    -I$(BOINC_SCHED_DIR) \
    -I$(BOINC_DB_DIR) \
    -I$(MYSQL_DIR) \
    -L$(MYSQL_LIB_DIR) \
    -L$(BOINC_API_DIR) \
    -L$(BOINC_LIB_DIR) \
    -L$(BOINC_SCHED_DIR) \
    -L/usr/X11R6/lib \
    -L/usr/local/lib/mysql \
    -L.

# the following should be freeglut; use nm to check
# you may have to change the paths for your system

PROGS = distrrtgen \

all: $(PROGS)

libstdc++.a:
	ln -s `g++ -print-file-name=libstdc++.a`

clean:
	/bin/rm -f $(PROGS) distrrtgen_validator *.o

distclean:
	/bin/rm -f $(PROGS) *.o libstdc++.a

distrrtgen: part_validator.o libstdc++.a $(BOINC_API_DIR)/libboinc_api.a $(BOINC_LIB_DIR)/libboinc.a $(BOINC_SCHED_DIR)/libsched.a
	$(CXX) Public.cpp *.c md4.cpp ChainWalkContext.cpp HashAlgorithm.cpp HashRoutine.cpp md5.cpp MemoryPool.cpp validate_util.cpp validate_util2.cpp validator.cpp $(CXXFLAGS) -o distrrtgen_validator part_validator.o libstdc++.a -pthread -lboinc_api $(BOINC_SCHED_DIR)/libsched.a -lboinc $(BOINC_LIB_DIR)/libboinc.a -lcrypto -lssl -lmysqlclient -O3 
