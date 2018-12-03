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
 *	Safe version of the C std library stdio.h
 *
 ******************************************************************************/


#ifndef _UTIL_IO_H
#define _UTIL_IO_H


#ifdef __cplusplus
	extern "C" {
#endif


#include <stdio.h>



// disable problematic functions of the standard library
#undef gets
#undef sprintf

#define gets(...)    gets is disabled
#define sprintf(...) sprintf is disabled, use snprintf


#ifdef __cplusplus
	}
#endif

#endif // _UTIL_IO_H


