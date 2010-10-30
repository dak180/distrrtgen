#ifndef MD4_H
#define MD4_H

#include "global.h"


#define MD4_DIGEST_LENGTH 16

//Main function
void MD4_NEW( unsigned char * buf, int len, unsigned char * pDigest);

#endif /* !MD4_H */
