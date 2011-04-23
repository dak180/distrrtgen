# freerainbowtables is a project for generating, distributing, and using
# perfect rainbow tables
#
# Copyright 2011 James Nobis <quel@quelrod.net>
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

all: converti2 distrrtgen distrrtgen_cuda rcracki_mt rti2rto \
	rtperfecter0.0 rtperfecti rtperfectp

converti2:
	cd client_applications/converti2 && make $(TARGET)

distrrtgen:
	cd boinc_software/boinc_client_apps/distrrtgen && make $(TARGET)

distrrtgen_cuda:
	cd boinc_software/boinc_client_apps/distrrtgen_cuda && make $(TARGET)

rcracki_mt:
	cd client_applications/rcracki_mt && make $(TARGET)

rti2rto:
	cd client_applications/rti2rto && make $(TARGET)

rtperfecter0.0:
	cd server_applications/rtperfecter0.0 && make $(TARGET)

rtperfecti:
	cd server_applications/rtperfecti && make $(TARGET)

rtperfectp:
	cd server_applications/rtperfectp && make $(TARGET)

clean: TARGET=clean
clean: converti2 distrrtgen distrrtgen_cuda rcracki_mt rti2rto \
	rtperfecter0.0 rtperfecti rtperfectp

distclean: TARGET=distclean
distclean: distrrtgen distrrtgen_cuda

rebuild: TARGET=clean
rebuild: converti2 distrrtgen distrrtgen_cuda rcracki_mt rti2rto \
	rtperfecter0.0 rtperfecti rtperfectp
