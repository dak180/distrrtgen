#!/usr/bin/env python

# freerainbowtables is a project for generating, distributing, and using
# perfect rainbow tables
#
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

import hashlib, smbpasswd, sys
from passlib.hash import mysql323

def addHashtoFile(prefix, hash):
 f = open(prefix+"_hash.txt","a")
 f.write(hash+"\n")


if (len(sys.argv) < 2):
 print "usage: list_generator.py file"
 exit(0)

f = open(sys.argv[1],"r")
line = f.readline()
while line:
    #lm
    addHashtoFile("lm", smbpasswd.lmhash(line.rstrip('\n\r')).lower())
    #ntlm
    addHashtoFile("ntlm", hashlib.new('md4', line.rstrip('\n\r').encode('utf-16le')).hexdigest())
    #mysql323
    addHashtoFile("mysql323", mysql323.encrypt( line.rstrip('\n\r')))
    #mysqlsha1
    addHashtoFile("mysqlsha1",  hashlib.sha1( hashlib.sha1(line.rstrip('\n\r')).digest()).hexdigest())
    #md4
    addHashtoFile("md4", hashlib.new('md4', line.rstrip('\n\r')).hexdigest())
    #md5    
    addHashtoFile("md5", hashlib.md5(line.rstrip('\n\r')).hexdigest())
    #doublemd5
    addHashtoFile("doublemd5", hashlib.md5(hashlib.md5(line.rstrip('\n\r')).digest()).hexdigest())
    #sha1
    addHashtoFile("sha1", hashlib.sha1(line.rstrip('\n\r')).hexdigest())

    line = f.readline()
f.close()


#def halfmlchall(hash):
# lmhash = smbpasswd.lmhash(hash)
# key1 = lmhash[:13] 
# key2 = lmhash[14:25]
# key3 = lmhash[26:]
# key3 += "000000000000"
# challenge = int("1122334455667788", 16)
# response
