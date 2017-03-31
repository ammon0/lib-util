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

typedef uint8_t  flag8;
typedef uint16_t flag16;
typedef uint32_t flag32;
typedef uint64_t flag64;


#define flag_status(flags,pos) (!!( flags &   ((uint64_t)1<<pos) ))
#define flag_flip(  flags,pos)    ( flags ^=  ((uint64_t)1<<pos) )
#define flag_set(   flags,pos)    ( flags |=  ((uint64_t)1<<pos) )
#define flag_unset( flags,pos)    ( flags &= ~((uint64_t)1<<pos) )


