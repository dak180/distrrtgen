# freerainbowtables is a project for generating, distributing, and using
# perfect rainbow tables
#
# Copyright 2011 James Nobis <quel@quelrod.net>
# Copyright 2011 Janosch Rux <janosch.rux@web.de>
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
MAKE = /usr/bin/make

OSNAME = $(shell uname -s)

MAKE_Darwin = /usr/bin/make
MAKE_FreeBSD = /usr/bin/make
MAKE_Linux = /usr/bin/make
MAKE_NetBSD = /usr/bin/make
MAKE_OpenBSD = /usr/bin/make
MAKE_SunOS = /usr/bin/gmake

MAKE = ${MAKE_$(OSNAME)}

all: converti2 distrrtgen distrrtgen_cuda rcracki_mt rti2rto \
	rtperfecter0.0 rtperfecti rtperfectp

converti2:
	cd client_applications/converti2 && $(MAKE) $(TARGET)

distrrtgen:
	cd boinc_software/boinc_client_apps/distrrtgen && $(MAKE) $(TARGET)

distrrtgen_cuda:
	cd boinc_software/boinc_client_apps/distrrtgen_cuda && $(MAKE) $(TARGET)

distrrtgen_validator:
	cd boinc_software/boinc_server_apps/distrrtgen_validator && $(MAKE) $(TARGET)

rcracki_mt:
	cd client_applications/rcracki_mt && $(MAKE) $(TARGET)

rti2rto:
	cd client_applications/rti2rto && $(MAKE) $(TARGET)

rtperfecter0.0:
	cd server_applications/rtperfecter0.0 && $(MAKE) $(TARGET)

rtperfecti:
	cd server_applications/rtperfecti && $(MAKE) $(TARGET)

rtperfectp:
	cd server_applications/rtperfectp && $(MAKE) $(TARGET)

tests:
	cd tests && $(MAKE) $(TARGET)

clean: TARGET=clean
clean: converti2 distrrtgen distrrtgen_cuda distrrtgen_validator rcracki_mt \
	rti2rto rtperfecter0.0 rtperfecti rtperfectp

distclean: TARGET=distclean
distclean: distrrtgen distrrtgen_cuda distrrtgen_validator

rebuild: TARGET=clean
rebuild: converti2 distrrtgen distrrtgen_cuda rcracki_mt rti2rto \
	rtperfecter0.0 rtperfecti rtperfectp
