/*******************************************************************************
 *
 *	lib-util : A Utility Library
 *
 *	Copyright (c) 2016 Ammon Dodson
 *	You should have received a copy of the licence terms with this software. If
 *	not, please visit the project homepage at:
 *	https://github.com/ammon0/lib-util
 *
 ******************************************************************************/
 
/** @file msg.h
 *
 *	Functions for logging and printing messages to stderr.
 *	
 *	##Log Levels
 *	The log level setting determines what messages will be printed to `stderr`.
 *	All messages are printed to the given log stream with a tag indicating the
 *	level of the message.
 *
 *	##Log Descriptors
 *	The caller may create as many log descriptors as needed, each corrosponding
 *	to a different log file.
 *
 ******************************************************************************/


#ifndef _MESSAGE_H
#define _MESSAGE_H


#include <types.h>


/******************************************************************************/
//                             TYPE DEFINITIONS
/******************************************************************************/


/// The availible log levels
typedef enum {
	V_QUIET, ///< no output is printed to `stderr`
	V_ERROR,
	V_WARN,
	V_NOTE,
	V_INFO,
	V_DEBUG,
	V_TRACE,
	V_NUM
} msg_log_lvl;

/// Mode to determine if the file will be appended to or replaced
typedef enum{
	lm_append,
	lm_replace
} msg_log_mode;

typedef struct log_t * log_descriptor; ///< A log stream descriptor


/******************************************************************************/
//                                PROTOTYPES
/******************************************************************************/


/// Set the minimum log level necessary to print to `stderr`
void     msg_set_verbosity(msg_log_lvl verbosity);

/// Open a log file
return_t msg_log_open (log_descriptor log, const char *path, msg_log_mode mode);

/// Close a log file
void     msg_log_close(log_descriptor log                                 );

/**	Print a message
 *
 *	
 */
void msg_print(log_descriptor log, msg_log_lvl, const char * format, ...);


#endif // _MESSAGE_H


