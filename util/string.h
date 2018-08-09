/*******************************************************************************
 *
 *	lib-util : A Utility Library
 *
 *	Copyright (c) 2016-2018 Ammon Dodson
 *	You should have received a copy of the licence terms with this software. If
 *	not, please visit the project homepage at:
 *	https://github.com/ammon0/lib-util
 *
 ******************************************************************************/
 
/** @file string.h
 *
 *	Safe version of the C std library string.h
 *
 ******************************************************************************/


#ifndef _UTIL_STRING_H
#define _UTIL_STRING_H


#ifdef __cplusplus
	extern "C" {
#endif


#include <string.h>


// disable problematic functions of the standard library
#undef strcpy
#undef strcat
#undef strncpy
#undef strncat

#define strcpy(...)  strcpy is disabled ///< disable standard library strcpy
#define strcat(...)  strcat is disabled ///< disable standard library strcat
#define strncpy(...) strncpy is disabled ///< disable standard library strncpy
#define strncat(...) strncat is disabled ///< disable standard library strncat


/**	Copy a string in src to dest
 *	Copies up to dstsize - 1 characters from the string src to dst,
 *	NUL-terminating the result if dstsize is not 0.
 *	*	guarantees that dst will be '\0' terminated
 *	*	If the src and dst strings overlap, the behavior is undefined.
 *	@param dst the region of memory the string is to be copied to
 *	@param src the null terminated string to be copied
 *	@param dstsize the size in bytes of dst
 *	@return the total length of the string in src. If return value >= dstsize then
 *	the src string has been truncated.
 */
static inline size_t
strlcpy(char * dst, const char * src, size_t dstsize){
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


/**	Appends the string in src to the end of dest
 *	It will append at most dstsize - strlen(dst) - 1 bytes. It will then 
 *	NUL-terminate, unless dstsize is 0 or the original dst string was longer than
 *	dstsize.
 *	*	guarantees that dst will be '\0' terminated
 *	*	If the src and dst strings overlap, the behavior is undefined.
 *	@param dst the region of memory the string is to be copied to
 *	@param src the null terminated string to be copied
 *	@param dstsize the size in bytes of dst
 *	@return the initial length of dst plus the length of src. If return value >=
 *	dstsize then the src string has been truncated.
 */
static inline size_t
strlcat(char * dst, const char * src, size_t dstsize){
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



#ifdef __cplusplus
	}
#endif

#endif // _UTIL_STRING_H


