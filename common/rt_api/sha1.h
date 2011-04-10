/**************************** sha.h ****************************/
/******************* See RFC 4634 for details ******************/
#ifndef _SHA_H_
#define _SHA_H_

/*
 *  Description:
 *      This file implements the Secure Hash Signature Standard
 *      algorithms as defined in the National Institute of Standards
 *      and Technology Federal Information Processing Standards
 *      Publication (FIPS PUB) 180-1 published on April 17, 1995, 180-2
 *      published on August 1, 2002, and the FIPS PUB 180-2 Change
 *      Notice published on February 28, 2004.
 *
 *      A combined document showing all algorithms is available at
 *              http://csrc.nist.gov/publications/fips/
 *              fips180-2/fips180-2withchangenotice.pdf
 *
 *      The five hashes are defined in these sizes:
 *              SHA-1           20 byte / 160 bit
 *              SHA-224         28 byte / 224 bit
 *              SHA-256         32 byte / 256 bit
 *              SHA-384         48 byte / 384 bit
 *              SHA-512         64 byte / 512 bit
 */

#include <stdint.h>
/*
 * If you do not have the ISO standard stdint.h header file, then you
 * must typedef the following:
 *    name              meaning
 *  uint64_t         unsigned 64 bit integer
 *  uint32_t         unsigned 32 bit integer
 *  uint8_t          unsigned 8 bit integer (i.e., unsigned char)
 *  int_least16_t    integer of >= 16 bits
 *
 */

#define SHA1_DIGEST_LENGTH 20

#ifndef _SHA_enum_
#define _SHA_enum_
/*
 *  All SHA functions return one of these values.
 */
enum {
	shaSuccess = 0,
	shaNull,            /* Null pointer parameter */
	shaInputTooLong,    /* input data too long */
	shaStateError,      /* called Input after FinalBits or Result */
	shaBadParam         /* passed a bad parameter */
};
#endif /* _SHA_enum_ */

/*
 *  These constants hold size information for each of the SHA
 *  hashing operations
 */
enum {
    SHA1_Message_Block_Size = 64
	, SHA1HashSize = 20
	, SHA1HashSizeBits = 160
};

/*
 *  These constants are used in the USHA (unified sha) functions.
 */
typedef enum SHAversion {
	SHA1
} SHAversion;

/*
 *  This structure will hold context information for the SHA-1
 *  hashing operation.
 */
typedef struct SHA1Context {
	uint32_t Intermediate_Hash[SHA1HashSize/4]; /* Message Digest */

	uint32_t Length_Low;                /* Message length in bits */
	uint32_t Length_High;               /* Message length in bits */

	int_least16_t Message_Block_Index;  /* Message_Block array index */
				 /* 512-bit message blocks */
	uint8_t Message_Block[SHA1_Message_Block_Size];

	int Computed;                       /* Is the digest computed? */
	int Corrupted;                      /* Is the digest corrupted? */
} SHA1Context;

/*
 *  Function Prototypes
 */

/* SHA-1 */
extern int SHA1Reset(SHA1Context *);
extern int SHA1Input(SHA1Context *, const uint8_t *bytes,
                     unsigned int bytecount);
extern int SHA1FinalBits(SHA1Context *, const uint8_t bits,
                         unsigned int bitcount);
extern int SHA1Result(SHA1Context *,
                      uint8_t Message_Digest[SHA1HashSize]);

/*
 * These definitions are defined in FIPS-180-2, section 4.1.
 * Ch() and Maj() are defined identically in sections 4.1.1,
 * 4.1.2 and 4.1.3.
 *
 * The definitions used in FIPS-180-2 are as follows:
 */

/*
 * The following definitions are equivalent and potentially faster.
 */

#define SHA_Ch(x, y, z)      (((x) & ((y) ^ (z))) ^ (z))
#define SHA_Maj(x, y, z)     (((x) & ((y) | (z))) | ((y) & (z)))

#define SHA_Parity(x, y, z)  ((x) ^ (y) ^ (z))

#endif /* _SHA_H_ */
