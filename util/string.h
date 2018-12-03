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


/*

size_t strlcpy (char *dst, const char *src, size_t size);
size_t strlcat (char *dst, const char *src, size_t size);

Both strlcpy and strlcat take the full size of the destination buffer as a parameter (not the maximum number of characters to be copied) and guarantee to NIL-terminate the result (as long as size is larger than 0). Remember that you should include a byte for NIL in the size.

The strlcpy function copies up to size-1 characters from the NUL-terminated string src to dst, NIL-terminating the result. The strlcat function appends the NIL-terminated string src to the end of dst. It will append at most size - strlen(dst) - 1 bytes, NIL-terminating the result.

One minor disadvantage of strlcpy(3) and strlcat(3) is that they are not, by default, installed in most Unix-like systems. In OpenBSD, they are part of <string.h>. This is not that difficult a problem; since they are small functions, you can even include them in your own program’s source (at least as an option), and create a small separate package to load them. You can even use autoconf to handle this case automatically. If more programs use these functions, it won’t be long before these are standard parts of Linux distributions and other Unix-like systems. Also, these functions have been recently added to the “glib” library (I submitted the patch to do this), so using recent versions of glib makes them available. In glib these functions are named g_strlcpy and g_strlcat (not strlcpy or strlcat) to be consistent with the glib library naming conventions.

Also, strlcat(3) has slightly varying semantics when the provided size is 0 or if there are no NIL characters in the destination string dst (inside the given number of characters). In OpenBSD, if the size is 0, then the destination string’s length is considered 0. Also, if size is nonzero, but there are no NIL characters in the destination string (in the size number of characters), then the length of the destination is considered equal to the size. These rules make handling strings without embedded NILs consistent. Unfortunately, at least Solaris doesn’t (at this time) obey these rules, because they weren’t specified in the original documentation. I’ve talked to Todd Miller, and he and I agree that the OpenBSD semantics are the correct ones (and that Solaris is incorrect). The reasoning is simple: under no condition should strlcat or strlcpy ever examine characters in the destination outside of the range of size; such access might cause core dumps (from accessing out-of-range memory) and even hardware interactions (through memory-mapped I/O). Thus, given:

  a = strlcat ("Y", "123", 0);

The correct answer is 3 (0+3=3), but Solaris will claim the answer is 4 because it incorrectly looks at characters beyond the "size" length in the destination. For now, I suggest avoiding cases where the size is 0 or the destination has no NIL characters. Future versions of glib will hide this difference and always use the OpenBSD semantics.

https://www.dwheeler.com/secure-programs/Secure-Programs-HOWTO/library-c.html

*/


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


