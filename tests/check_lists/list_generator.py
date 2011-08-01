#!/usr/bin/env python
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
