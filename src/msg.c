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


#include <util/msg.h>
//#include <util/flags.h>
#include <util/io.h>

#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h> // unix dependant


/******************************************************************************/
//                             TYPE DEFINITIONS
/******************************************************************************/


struct log_t{
	FILE * fd;
	flag8 mode;
};

const char * lvl_str[V_NUM] = {
	"       ",
	"ERROR  ",
	"WARNING",
	"NOTICE ",
	"INFO   ",
	"\tDEBUG",
	"\t\tTRACE"
};

static msg_log_lvl _lvl = V_NOTE;


/******************************************************************************/
//                           PRIVATE FUNCTIONS
/******************************************************************************/


static inline void __attribute__((format(printf, 3, 0)))
_log_to_file(
	FILE * fd,
	msg_log_lvl lvl,
	const char * format,
	va_list ap
){
	struct timeval now;
	time_t seconds;
	struct tm * tm_pt;
	const char * months[12] ={
		"JAN",
		"FEB",
		"MAR",
		"APR",
		"MAY",
		"JUN",
		"JUL",
		"AUG",
		"SEP",
		"OCT",
		"NOV",
		"DEC"
	};
	
	gettimeofday(&now, NULL);
	seconds = time(NULL);
	tm_pt = gmtime(&seconds);
	
	fprintf(
		fd,
		"%s.%02d %02d.%02d.%02d [%ld.%06ld] %s: ",
		months[tm_pt->tm_mon],
		tm_pt->tm_mday,
		
		tm_pt->tm_hour,
		tm_pt->tm_min,
		tm_pt->tm_sec,
		
		now.tv_sec,
		now.tv_usec,
		
		lvl_str[lvl]
	);
	
	if(ap) vfprintf(fd, format, ap);
	else   fprintf(fd, "%s\n", format);
}


/******************************************************************************/
//                           PUBLIC FUNCTIONS
/******************************************************************************/


void msg_set_verbosity(msg_log_lvl verbosity){ _lvl = verbosity; }

log_descriptor msg_log_open(msg_log_mode mode, const char *path){
	log_descriptor log;
	
	log = (log_descriptor) malloc(sizeof(struct log_t));
	if(!log){
		msg_print(NULL, V_ERROR, "msg_log_open(): could not allocate memory\n");
		return NULL;
	}
	
	if(mode == lm_append) log->fd = fopen(path, "a");
	else                  log->fd = fopen(path, "w");
	
	if(!log->fd){
		msg_print(NULL, V_ERROR, "msg_log_open(): could not open file\n");
		free(log);
		return NULL;
	}
	
	_log_to_file(log->fd, V_INFO, "*** OPENING LOG ***", NULL);
	return log;
}

void msg_log_close(log_descriptor log){
	_log_to_file(log->fd, V_INFO, "*** CLOSING LOG ***", NULL);
	fclose(log->fd);
	free(log);
}


void //__attribute__((format(printf, 3, 4)))
msg_print(log_descriptor log, msg_log_lvl lvl, const char * format, ...){
	va_list ap;
	
	va_start(ap, format);
	
	if(log){
		_log_to_file(log->fd, lvl, format, ap);
		fflush(log->fd);
	}
	
	if(_lvl >= lvl){
		fprintf(stderr, "%s: ", lvl_str[lvl]);
		vfprintf(stderr, format, ap);
	}
	
	va_end(ap);
}

void msg_set_flag(log_descriptor log, msg_flags f){
	flag_set(log->mode, f,flag8);
}

void msg_unset_flag(log_descriptor log, msg_flags f){
	flag_unset(log->mode, f,flag8);
}

bool __attribute__((const))
msg_check_flag(log_descriptor log, msg_flags f){
	return flag_status(log->mode, f, flag8);
}


