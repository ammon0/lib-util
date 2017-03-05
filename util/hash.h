/*******************************************************************************
 *
 *	lib-util : A Utility Library
 *
 *	Copyright (c) 2016-2017 Ammon Dodson
 *	You should have received a copy of the license terms with this software. If
 *	not, please visit the project homepage at:
 *	https://github.com/ammon0/lib-util
 *
 ******************************************************************************/

/** @file hash.h
 *
 *	A Collection of hash functions with a common interface. These functions make
 *	no attempt to correct for endianess and therefore may produce different
 *	results on different machines. All of these functions process their input 4
 *	bytes at a time. All require the caller to set the the inital hash value,
 *	which should never be 0.
 *
 *	##Hash Functions
 *	hash.h currently provides 4 hash functions:
 *	* `hash_a`: based on [FVN-1a](http://www.isthe.com/chongo/tech/comp/fnv/)
 *	but with variable initalization, and eats 4 bytes at a time.
 *	* `hash_b`: based on [murmur](https://github.com/aappleby/smhasher)
 *	* `hash_c`: based on murmur3
 *	* `hash_d`: based on burtle's
 *	[newhash](http://burtleburtle.net/bob/hash/evahash.html)
 *
 *	In addition to the actual hash functions, hash.h provides 3 different hasher
 *	functions, one each for array's strings, and files. The actual hash function
 *	used is determined by passing its pointer to the hasher.
 *
 *	I can't make any guarantee's about these hash functions. In my own tests
 *	(`test-hash.c`) I get from 651 to 702 collisions from 3 150 027 entries when
 *	usind a 32-bit hash, and none at 64-bits.
 *
 ******************************************************************************/


#ifndef _HASH_H
#define _HASH_H

#include <util/types.h>
#include <string.h>
#include <stdio.h>

#ifdef __cplusplus
	extern "C" {
#endif

/// 32-bit barrel shift
static inline uint32_t __attribute__((const, always_inline))
rotl32 ( uint32_t x, int8_t r ){
	return (x << r) | (x >> (32 - r));
}

/// 64-bit barrel shift
static inline uint64_t __attribute__((const, always_inline))
rotl64 ( uint64_t x, int8_t r ){
	return (x << r) | (x >> (64 - r));
}


/******************************************************************************/
//                              HASH FUNCTIONS
/******************************************************************************/


/*	The combining step is a reversible mapping of the internal state to the
 	internal state,
 *	The combining step is a one-to-one mapping (every input block value is
 	mapped to a distinct internal state value),
 *	The mixing function is reversible,
 *	The mixing step causes every bit of the internal state to affect every bit
 	of the result.
 *	The mixing step, when run either forwards or in reverse, causes every bit of
 	the internal state to affect at least v bits of the internal state.
 *	For every intermediate point in the mixing step, consider running the mixing step forward to that point from the previous combine, and backward to that point from the next combine. For every set Y of bits in the internal state in the middle, there some set X of bits in the previous input block and Z in the next input block that affect those Y bits. For every Y with less than v bits, |X|+|Z| must be less than or equal to |Y|. (Note that if every bit in the previous and next block affects at least v bits in the center of the mixing step, this requirement is satisfied.)
 */
 
/* Operators:
	& | do not create a one-to-one mapping, nor are they reversable
	* / % are slow
	+ - ^ >> <<
*/

/// A hash function based on [FVN-1a](http://www.isthe.com/chongo/tech/comp/fnv/)
static inline uint64_t __attribute__((const, always_inline))
hash_a(uint64_t hash, uint32_t chunk){
	return (hash ^ chunk) * 0x100000001b3;
}


/// A hash function based on [murmur](https://github.com/aappleby/smhasher)
static inline uint64_t __attribute__((const, always_inline))
hash_b(uint64_t hash, uint32_t chunk){
	hash += chunk;
	hash *= 0x7fd652ad;
	return hash ^ (hash >> 16);
}


/// A hash function based on [murmur3](https://github.com/aappleby/smhasher)
static inline uint64_t __attribute__((const, always_inline))
hash_c(uint64_t hash, uint32_t chunk){
	chunk *= 0xcc9e2d51;
	chunk = rotl32(chunk, 15);
	chunk *= 0x1b873593;
	
	hash ^= chunk;
	hash = rotl64(hash, 37);
	return hash + (hash << 2) + 0xe6546b64;
}


/**	A hash function based on
 *	[newhash](http://burtleburtle.net/bob/hash/evahash.html)
 */
static inline uint64_t __attribute__((const, always_inline))
hash_d(uint64_t hash, uint32_t chunk){
	static uint32_t a, b, c;
	
	a += hash;
	b += (hash>>32);
	c += chunk;
	
	a-=c;  b-=a;  b^=(a<<8);
	c-=a;  c-=b;  c^=(b>>13);
	
	a-=b;  a-=c;  a^=(c>>12);
	b-=c;  b-=a;  b^=(a<<16);
	c-=a;  c-=b;  c^=(b>>5);
	
	a-=b;  a-=c;  a^=(c>>3);
	b-=c;  b-=a;  b^=(a<<10);
	c-=a;  c-=b;  c^=(b>>15);
	
	return ((uint64_t)a<<32 | b) ^ (c<<16);
}


/// The number of hash functions availible. Used in test-hash.c
#define _FUN_CNT 4

/// size of file buffer for file hashing
#define BUF_SZ ((size_t) 1<<10)


/******************************************************************************/
//                                  HASHER
/******************************************************************************/


/** Hash a memory array
 *
 *	@param hash  The inital seed for the hash. Should not be 0.
 *	@param fn    The hash function to use. One of hash_X.
 *	@param array The array of data to be hashed.
 *	@param len   The size of the array in bytes.
 *
 *	@return a 64-bit hash of the input data.
 */
static uint64_t __attribute__((pure)) array_hash(
	uint64_t hash,
	uint64_t (*fn)(uint64_t hash, uint32_t chunk),
	const void * array,
	size_t len
	){
#ifndef _TEST_HASH
	hash += len;
#endif
	
	if(len & 1) hash = fn(hash, ((uint8_t*)array)[--len]);
	len >>= 1;
	
	if(len & 1) hash = fn(hash, ((uint16_t*)array)[--len]);
	len >>= 1;
	
	while(len) hash = fn(hash, ((uint32_t*)array)[--len]);
	
#ifndef _TEST_HASH
	hash = fn(hash, 0xcc9e2d51);
	hash = fn(hash, 0x1b873593);
#endif
	
	return hash;
}


/** Hash a `'\0'` terminated string
 *
 *	@param hash The inital seed for the hash. Should not be 0.
 *	@param fn   The hash function to use. One of hash_X.
 *	@param str  A null terminated string to be hashed.
 *
 *	@return a 64-bit hash of the input data.
 */
static uint64_t __attribute__((pure)) string_hash(
	uint64_t hash,
	uint64_t (*fn)(uint64_t hash, uint32_t chunk),
	const unsigned char * str
){ return array_hash(hash, fn, str, strlen(str)); }


/** Hash a file.
 *
 *	@param hash The inital seed for the hash. Should not be 0.
 *	@param fn   The hash function to use. One of hash_X.
 *	@param fd   The file descriptor to be hashed.
 *
 *	@return a 64-bit hash of the input data.
 */
static uint64_t __attribute__((pure)) file_hash(
	uint64_t hash,
	uint64_t (*fn)(uint64_t hash, uint32_t chunk),
	FILE * fd
){
	size_t cnt;
	uint32_t buffer[BUF_SZ];
	
	if(!fd){
		fprintf(stderr, "no such file");
		return 0;
	}
	
	while(!feof(fd)){
		cnt = fread(buffer, sizeof(uint32_t), BUF_SZ, fd);
		hash = array_hash(hash, fn, (uint8_t*)buffer, cnt*sizeof(uint32_t));
	}
	
	return hash;
}

#ifdef __cplusplus
	}
#endif

#endif // _HASH_H


