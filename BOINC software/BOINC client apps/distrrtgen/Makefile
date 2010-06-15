# This should work on Linux.  Modify as needed for other platforms.

BOINC_DIR = /home/ubuntu/boinc
BOINC_API_DIR = $(BOINC_DIR)/api
BOINC_LIB_DIR = $(BOINC_DIR)/lib
COMMON_LIB_DIR = /usr/lib

CXXFLAGS = -g \
    -static -static-libgcc \
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
	$(CXX) *.c *.cpp $(CXXFLAGS) -o distrrtgen distrrtgen.o libstdc++.a $(COMMON_LIB_DIR)/libssl.a $(COMMON_LIB_DIR)/libpthread.a -lboinc_api -lboinc -lpthread -lssl -O3
