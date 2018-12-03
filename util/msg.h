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
 
/** @file msg.h
 *
 *	Functions for logging to files and printing messages to stderr. Each message
 *	is tagged with the message's importance. There are various options for date
 *	and time tagging.
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


#ifndef _MSG_H
#define _MSG_H


#include <util/types.h>
#include <util/flags.h>

#ifdef __cplusplus
	extern "C" {
#endif


/******************************************************************************/
//                             TYPE DEFINITIONS
/******************************************************************************/


/// The availible log levels
typedef enum {
	V_QUIET, ///< no output is printed to `stderr`
	V_ERROR, ///< print only errors
	V_WARN,  ///< print errors and warnings
	V_NOTE,  ///< print notices (default)
	V_INFO,  ///< print information
	V_DEBUG, ///< print debug output
	V_TRACE, ///< Print program trace information
	V_NUM    ///< the number of log levels, **DO NOT USE**
} msg_log_lvl;

/// Mode to determine if the file will be appended to or replaced
typedef enum{
	lm_append, ///< append to the end of any existing log file
	lm_replace ///< overwrite any existing log file
} msg_log_mode;

typedef struct log_t * log_descriptor; ///< A log stream descriptor

/// Log file options
typedef enum{
	MSG_LOG_SYNC, ///< sync log file after each entry
	MSG_LOG_DATE, ///< record the date in the log file
	MSG_LOG_USEC  ///< record seconds / useconds in the log file
} msg_flags;


/******************************************************************************/
//                                PROTOTYPES
/******************************************************************************/


/// Set the minimum log level necessary to print to `stderr`
void msg_set_verbosity(msg_log_lvl);

/// Open a log file
log_descriptor msg_log_open (msg_log_mode mode, const char *path);

/// Close a log file
void msg_log_close(log_descriptor log);

/**	Print a message
 *
 *	@param log    The log descriptor, if any, that the message should be sent to.
 *	This should be set to NULL if none.
 *	@param lvl    The log level of the message.
 *	@param format A printf type message format followed by any arguments.
 *
 *	@return void.
 */
void __attribute__((format(printf, 3, 4)))
msg_print(log_descriptor log, msg_log_lvl lvl, const char * format, ...);

/// set a flag on the log_descriptor
void msg_set_flag(log_descriptor log, msg_flags f);
/// unset a flag on the log_descriptor
void msg_unset_flag(log_descriptor log, msg_flags f);
/// check if a flag is set on the log_descriptor
bool msg_check_flag(log_descriptor log, msg_flags f);

/**	Optimized program trace
 *	msg_print() is implemented as a macro that expands to nothing when DEBUG is
 *	not defined. This could be useful for speed critical components of a program.
 */
#ifdef DEBUG
	#define msg_trace(log, ...) \
	msg_print(log, V_TRACE, __VA_ARGS__)
#else
	#define msg_trace(log, ...)
#endif

#ifdef __cplusplus
	}
#endif

#endif // _MSG_H


