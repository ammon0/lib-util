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
 *	Safe  version of the C std library string.h
 *
 ******************************************************************************/


#ifndef _STRING_H_INCLUDE
#define _STRING_H_INCLUDE


#ifdef __cplusplus
	extern "C" {
#endif


#include <string.h>


#undef strcpy
#undef strcat
#undef strncpy
#undef strncat

// disable problematic functions of the standard library
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
size_t
strlcpy(char * dst, const char * src, size_t dstsize);


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
size_t
strlcat(char * dst, const char * src, size_t dstsize);



#ifdef __cplusplus
	}
#endif

#endif // _STRING_H_INCLUDE


