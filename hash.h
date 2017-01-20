/*******************************************************************************
 *
 *	lib-util : A Utility Library
 *
 *	Copyright (c) 2016 Ammon Dodson
 *	You should have received a copy of the license terms with this software. If
 *	not, please visit the project homepage at:
 *	https://github.com/ammon0/lib-util
 *
 ******************************************************************************/

#ifndef _HASH_H
#define _HASH_H

#include <types.h>
#include <string.h>
#include <stdio.h>

inline uint32_t rotl32 ( uint32_t x, int8_t r ){
	return (x << r) | (x >> (32 - r));
}

inline uint64_t rotl64 ( uint64_t x, int8_t r ){
	return (x << r) | (x >> (64 - r));
}


/******************************************************************************/
//                              HASH FUNCTIONS
/******************************************************************************/

/*
 *	The combining step is a reversible mapping of the internal state to the internal state,
 *	The combining step is a one-to-one mapping (every input block value is mapped to a distinct internal state value),
 *	The mixing function is reversible,
 *	The mixing step causes every bit of the internal state to affect every bit of the result, and
 *	The mixing step, when run either forwards or in reverse, causes every bit of the internal state to affect at least v bits of the internal state.
 *	For every intermediate point in the mixing step, consider running the mixing step forward to that point from the previous combine, and backward to that point from the next combine. For every set Y of bits in the internal state in the middle, there some set X of bits in the previous input block and Z in the next input block that affect those Y bits. For every Y with less than v bits, |X|+|Z| must be less than or equal to |Y|. (Note that if every bit in the previous and next block affects at least v bits in the center of the mixing step, this requirement is satisfied.)
 */
 
/* Operators:
	& | do not create a one-to-one mapping, nor are they reversable
	* / % are slow
	+ - ^ >> <<
*/

static inline uint64_t hash_a(uint64_t hash, uint32_t chunk){ // like FVN-1a
	return (hash ^ chunk) * 0x100000001b3;
}

static inline uint64_t hash_b(uint64_t hash, uint32_t chunk){ // like murmur
	hash += chunk;
	hash *= 0x7fd652ad;
	return hash ^ (hash >> 16);
}

static inline uint64_t hash_c(uint64_t hash, uint32_t chunk){ // like murmur3
	chunk *= 0xcc9e2d51;
	chunk = rotl32(chunk, 15);
	chunk *= 0x1b873593;
	
	hash ^= chunk;
	hash = rotl64(hash, 37);
	return hash + (hash << 2) + 0xe6546b64;
}

static inline uint64_t hash_d(uint64_t hash, uint32_t chunk){
	static uint32_t a, b, c;
	
	a += hash;
	b += (hash>>32);
	c += chunk;
	
	a=a-b;  a=a-c;  a=a^(c>>13);
	b=b-c;  b=b-a;  b=b^(a<<8);
	c=c-a;  c=c-b;  c=c^(b>>13);
	
	a=a-b;  a=a-c;  a=a^(c>>12);
	b=b-c;  b=b-a;  b=b^(a<<16);
	c=c-a;  c=c-b;  c=c^(b>>5);
	
	a=a-b;  a=a-c;  a=a^(c>>3);
	b=b-c;  b=b-a;  b=b^(a<<10);
	c=c-a;  c=c-b;  c=c^(b>>15);
	
	return (uint64_t)b<<32 | c;
}

#define _FUN_CNT 4


/******************************************************************************/
//                           HASH ARRAYS AND STRINGS
/******************************************************************************/


/// Hash an array
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


// Hash a `'\0'` terminated string
static uint64_t __attribute__((pure)) string_hash(
	uint64_t hash,
	uint64_t (*fn)(uint64_t hash, uint32_t chunk),
	const unsigned char * str
){
	
	return array_hash(hash, fn, str, strlen(str));
	
}


// Hash a file
static uint64_t __attribute__((pure)) file_hash(
	uint64_t hash,
	uint64_t (*fn)(uint64_t hash, uint32_t chunk),
	FILE * fd
){
	#define BUF_SZ ((size_t) 1<<10)
	
	size_t cnt;
	uint32_t buffer[BUF_SZ];
	
	while(!feof(fd)){
		cnt = fread(buffer, sizeof(uint32_t), BUF_SZ, fd);
		hash = array_hash(hash, fn, (uint8_t*)buffer, cnt*sizeof(uint32_t));
	}
	
	return hash;
	#undef BUF_SZ
}




/* The whole new hash function */
//uint32_t hash(uint8_t * k, size_t length, uint32_t initval){
//	uint32_t a,b,c;  /* the internal state */
//	uint32_t len;    /* how many key bytes still need mixing */

//	/* Set up the internal state */
//	len = length;
//	a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
//	c = initval;         /* variable initialization of internal state */

//	/*---------------------------------------- handle most of the key */
//	while (len >= 12){
//		a=a+(k[0]+((uint32_t)k[1]<<8)+((uint32_t)k[2]<<16)+((uint32_t)k[3]<<24));
//		b=b+(k[4]+((uint32_t)k[5]<<8)+((uint32_t)k[6]<<16)+((uint32_t)k[7]<<24));
//		c=c+(k[8]+((uint32_t)k[9]<<8)+((uint32_t)k[10]<<16)+((uint32_t)k[11]<<24));
//		mix(a,b,c);
//		k = k+12; len = len-12;
//	}

//	/*------------------------------------- handle the last 11 bytes */
//	c = c+length;
//	switch(len)              /* all the case statements fall through */
//	{
//	case 11: c=c+((uint32_t)k[10]<<24);
//	case 10: c=c+((uint32_t)k[9]<<16);
//	case 9 : c=c+((uint32_t)k[8]<<8);
//	  /* the first byte of c is reserved for the length */
//	case 8 : b=b+((uint32_t)k[7]<<24);
//	case 7 : b=b+((uint32_t)k[6]<<16);
//	case 6 : b=b+((uint32_t)k[5]<<8);
//	case 5 : b=b+k[4];
//	case 4 : a=a+((uint32_t)k[3]<<24);
//	case 3 : a=a+((uint32_t)k[2]<<16);
//	case 2 : a=a+((uint32_t)k[1]<<8);
//	case 1 : a=a+k[0];
//	 /* case 0: nothing left to add */
//	}
//	mix(a,b,c);
//	/*-------------------------------------------- report the result */
//	return c;
//}



#endif // _HASH_H
