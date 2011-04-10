#ifndef MD5_H
#define MD5_H

#include "global.h"

#define MD5_DIGEST_LENGTH 16

//Main function
void MD5_NEW( unsigned char * buf, int len, unsigned char * pDigest);

#endif /* !MD5_H */
