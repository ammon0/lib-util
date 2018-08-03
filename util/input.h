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
 
/** @file input.h
 *
 *	Safe input functions
 *
 ******************************************************************************/

#ifndef _INPUT_H_INCLUDE
#define _INPUT_H_INCLUDE

#include <stdio.h>

#ifdef __cplusplus
	extern "C" {
#endif

/**	Get the next **newline** terminated line of text and store it in an array.
 *	*	The newline is discarded and the array is \0 teriminated.
 *	*	Discards all leading whitespace and control characters
 *	*	the caller must free the array provided by grabline()
 *	@param source The source file
 *	@return A character string in the heap on success or `NULL` on failure
 */
char * grabline(FILE * source);

/**	Get the next **space** terminated word and store it in an array.
 *	*	The space is discarded and the arrya is \0 terminated.
 *	*	Discards all leading whitespace and control characters
 *	*	the caller must free the array provided by grabword()
 *	@param source The source file
 *	@return A character string in the heap on success or `NULL` on failure
 */
char * grabword(FILE * source);

/**	Get the next **tab** or 3x **newline** terminated string and store it in an
 *	array.
 *	*	Discards leading and terminating whitespace
 *	*	whitespace is newlines, tabs, and spaces
 *	*	the caller must free the array provided by grabfield()
 *	@param source The source file
 *	@return A character string in the heap on success or `NULL` on failure
 */
char * grabfield(FILE * source);

#ifdef __cplusplus
	}
#endif

#endif // _INPUT_H_INCLUDE


