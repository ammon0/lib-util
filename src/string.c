/*******************************************************************************
 *
 *	lib-util : A Utility Library
 *
 *	Copyright (c) 2016-2018 Ammon Dodson
 *	You should have received a copy of the license terms with this software. If
 *	not, please visit the project homepage at:
 *	https://github.com/ammon0/lib-util
 *
 ******************************************************************************/

#include <util/string.h>
#include <util/types.h>

/*
dstsize is 0: do nothing, return 0
src terminates eary: fill remainder with '\0'
src terminates at dstsize-1
src does not terminate: keep reading until it does


*/


inline size_t
strlcpy(char * restrict dst, const char * restrict src, size_t dstsize){
	const char * s = src;
	size_t       n = dstsize;
	
	if(n != 0)
		while(--n != 0) // executes dstsize-1 times
			if((*dst++ = *s++) == '\0')
				goto finish;
	
	// dst is now pointing to the last location
	if(dstsize != 0) *dst = '\0';
	
	while (*s++ != '\0') // find the end of src
		;
	
finish:
	return (size_t)(s-src-1);
}


inline size_t
strlcat(char * restrict dst, const char * restrict src, size_t dstsize){
	size_t n;
	char * d = dst;
	
	while(*d++ != '\0') // find the end of the dst string
		;
	
	d--; // point at the '\0'
	
	n = (size_t)(d - dst); // the length of string in dst
	
	// n > dstsize - 1
	if(n+1 > dstsize){
		while(*src++ != '\0') // find the end of src
			n++;
		return n;
	}
	
	return strlcpy(d, src, dstsize-n) + n;
}



