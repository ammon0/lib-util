/*******************************************************************************
 *
 *	lib-util : A Utility Library
 *
 *	Copyright (c) 2016 Ammon Dodson
 *	You should have received a copy of the licence terms with this software. If
 *	not, please visit the project homepage at:
 *	https://github.com/ammon0/lib-util
 *
 *******************************************************************************
 *
 *	input.h : Safe input functions
 *
 ******************************************************************************/

#ifndef _INPUT_H_INCLUDE
#define _INPUT_H_INCLUDE

#include <stdio.h>

char* grabline(FILE*);
/**	Get the next newline terminated line of text and store it in an array.
 *	The newline is discarded and the array is \0 teriminated.
 *	Discards leading NL, CL, and trailing spaces and tabs
 *	the caller must free the array provided by grabline()
 *	returns NULL on failure
 */

char* grabword(FILE*);
/**	Get the next alphanumeric word from the input stream
 *	Discards leading whitespace, and trailing spaces and tabs
 *	whitespace is NL, CR, spaces, and tabs
 *	the caller must free the array provided by grabword()
 *	returns NULL on failure
 */

char* grabfield(FILE*);
/**	Get the next tab or newline terminated string from the input stream
 *	Discards leading and terminating whitespace
 *	whitespace is newlines, tabs, and spaces
 *	the caller must free the array provided by grabfield()
 *	returns NULL on failure
 */

#endif // _INPUT_H_INCLUDE


