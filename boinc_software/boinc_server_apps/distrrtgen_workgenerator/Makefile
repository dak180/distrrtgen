# This should work on Linux.  Modify as needed for other platforms.

BOINC_DIR = /home/frt/server_stable
BOINC_API_DIR = $(BOINC_DIR)/api
BOINC_LIB_DIR = $(BOINC_DIR)/lib
BOINC_SCHED_DIR = $(BOINC_DIR)/sched
BOINC_DB_DIR = $(BOINC_DIR)/db
BOINC_TOOLS_DIR = $(BOINC_DIR)/tools
MYSQL_DIR = /usr/local/include/mysql

CXXFLAGS = -g \
    -DAPP_GRAPHICS \
    -I$(BOINC_DIR) \
    -I$(BOINC_LIB_DIR) \
    -I$(BOINC_API_DIR) \
    -I$(BOINC_SCHED_DIR) \
    -I$(BOINC_DB_DIR) \
    -I$(MYSQL_DIR) \
    -I$(BOINC_TOOLS_DIR) \
    -L$(BOINC_API_DIR) \
    -L$(BOINC_LIB_DIR) \
    -L$(BOINC_SCHED_DIR) \
    -L/usr/X11R6/lib \
    -L/usr/local/lib/mysql \
    -L.

# the following should be freeglut; use nm to check
# you may have to change the paths for your system

PROGS = distrrtgen \
# uc2_graphics
# make this optional so compile doesn't break on systems without OpenGL

all: $(PROGS)

libstdc++.a:
	ln -s `g++ -print-file-name=libstdc++.a`

clean:
	/bin/rm -f $(PROGS) *.o

distclean:
	/bin/rm -f $(PROGS) *.o libstdc++.a

distrrtgen: distrrtgen_workgenerator.o libstdc++.a $(BOINC_API_DIR)/libboinc_api.a $(BOINC_LIB_DIR)/libboinc.a $(BOINC_SCHED_DIR)/libsched.a 
	$(CXX) distrrtgen_workgenerator.cpp $(CXXFLAGS) -o distrrtgen_workgenerator libstdc++.a -pthread -lboinc_api -lsched -lboinc -lmysqlclient -lssl -O3 
