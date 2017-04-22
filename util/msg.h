/*******************************************************************************
 *
 *	lib-util : A Utility Library
 *
 *	Copyright (c) 2016-2017 Ammon Dodson
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


#include <util/types.h>


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
typedef uint8_t flag_t; ///< flags for use in the log_descriptor


/******************************************************************************/
//                                CONSTANTS
/******************************************************************************/


/// whether log file should be synced after each entry
#define MF_LOG_SYNC (flag_t)0
#define MF_LOG_DATE (flag_t)1 ///< whether log file should record the date
/// whether log file should record seconds / useconds
#define MF_LOG_USEC (flag_t)2


/******************************************************************************/
//                                PROTOTYPES
/******************************************************************************/

#ifdef __cplusplus
	extern "C" {
#endif

/// Set the minimum log level necessary to print to `stderr`
void msg_set_verbosity(msg_log_lvl);

/// Open a log file
RETURN msg_log_open (log_descriptor log, msg_log_mode mode, const char *path);

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
void msg_print(log_descriptor log, msg_log_lvl lvl, const char * format, ...);

/// set a flag on the log_descriptor
void msg_set_flag(log_descriptor log, flag_t f);
/// unset a flag on the log_descriptor
void msg_unset_flag(log_descriptor log, flag_t f);
/// check if a flag is set on the log_descriptor
bool msg_check_flag(log_descriptor log, flag_t f);

#ifdef __cplusplus
	}
#endif

#endif // _MESSAGE_H


