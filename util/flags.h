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

/** @file flags.h
 *
 * A bit flag implementation
 *
 ******************************************************************************/

typedef uint8_t  flag8;  ///< an 8-bit flag field
typedef uint16_t flag16; ///< an 16-bit flag field
typedef uint32_t flag32; ///< an 32-bit flag field
typedef uint64_t flag64; ///< an 64-bit flag field

/// Check the state of a bit flag
#define flag_status(flags,pos) (!!( flags &   ((uint64_t)1<<pos) ))

/// Flip a bit flag
#define flag_flip(  flags,pos)    ( flags ^=  ((uint64_t)1<<pos) )

/// Set a bit flag to 1 even if it alreay is set
#define flag_set(   flags,pos)    ( flags |=  ((uint64_t)1<<pos) )

/// Clear a bit flag to 0 whether is was set or not
#define flag_clear( flags,pos)    ( flags &= ~((uint64_t)1<<pos) )


