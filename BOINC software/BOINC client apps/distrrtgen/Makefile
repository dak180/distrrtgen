# This should work on Linux.  Modify as needed for other platforms.

BOINC_DIR = /home/frt/server_stable
BOINC_API_DIR = $(BOINC_DIR)/api
BOINC_LIB_DIR = $(BOINC_DIR)/lib

CXXFLAGS = -g \
    -DAPP_GRAPHICS \
    -I$(BOINC_DIR) \
    -I$(BOINC_LIB_DIR) \
    -I$(BOINC_API_DIR) \
    -L$(BOINC_API_DIR) \
    -L$(BOINC_LIB_DIR) \
    -L /usr/X11R6/lib \
    -L.


PROGS = distrrtgen \

all: $(PROGS)

libstdc++.a:
	ln -s `g++ -print-file-name=libstdc++.a`

clean:
	/bin/rm -f $(PROGS) *.o

distclean:
	/bin/rm -f $(PROGS) *.o libstdc++.a

distrrtgen: distrrtgen.o libstdc++.a $(BOINC_API_DIR)/libboinc_api.a $(BOINC_LIB_DIR)/libboinc.a
	$(CXX) Public.cpp ChainWalkContext.cpp HashAlgorithm.cpp HashRoutine.cpp md5.cpp $(CXXFLAGS) -o distrrtgen distrrtgen.o libstdc++.a -pthread -lboinc_api -lboinc -lssl -O3
