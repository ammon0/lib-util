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

/** @file types.h
 *
 *	Commonly Used Type Definitions
 *	
 *	## Includes
 *	* The integer types: `int8_t`, `int16_t`, `int32_t`, and `int64_t`
 *	* Limits for the integer types
 *	* `bool`
 *	* `NULL`
 *	* size_t
 *	* `uchar`, `uint`, `umax`, and `imax`
 */


#ifndef _TYPES_H
#define _TYPES_H


#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
// size_t NULL
#include <stddef.h>


/// The largest signed integer for a given architecture
typedef long long int imax;

#define IMAX_MAX ((size_t)LLONG_MAX) ///< the maximum value of `imax`
#define IMAX_MIN ((size_t)LLONG_MIN) ///< the minimum value of `imax`

// Unsigned
/// The largest unsigned integer for a given architecture
typedef unsigned long long int umax;
typedef unsigned int  uint; ///< a naturally sized unsigned integer
typedef unsigned char uchar; ///< an unsigned character

#define UMAX_MAX ((size_t)ULLONG_MAX) ///< the maximum value of `umax`
// UINT_MAX is defined in limits.h
// UCHAR_MAX is defined in limits

/// A type to simply indicate success or failure
typedef enum{
	success, ///< indicates the function call was successful
	failure  ///< indicates the function call failed
} return_t;


#endif // _TYPES_H


