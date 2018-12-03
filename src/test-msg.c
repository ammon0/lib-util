// just a test program

#include <util/msg.h>
#include <util/io.h>

log_descriptor testLog;

int main (void){
	
	
	
	
	if(!( testLog=msg_log_open(lm_replace, "test.log") ))
		puts("failed to open log");
	
	msg_set_flag(testLog, MSG_LOG_USEC);
	msg_set_flag(testLog, MSG_LOG_DATE);
	msg_set_flag(testLog, MSG_LOG_SYNC);
	
	
/*	msg_set_verbosity(V_TRACE);*/
/*	msg_print(testLog, V_INFO, "V_QUIET is %i\n", V_QUIET);*/
/*	msg_print(testLog, V_INFO, "V_ERROR is %i\n", V_ERROR);*/
/*	msg_print(testLog, V_INFO, "V_NUM is %i\n", V_NUM);*/
	
	for(msg_log_lvl i=V_QUIET; i<V_NUM; i++){
		msg_set_verbosity(i);
		printf("Verbosity set to %i\n", i);
		
		for(msg_log_lvl j=V_ERROR; j<V_NUM; j++){
			msg_print(testLog, j, "message at level %i\n", j);
		}
		
		puts("");
	}
	
	
	msg_log_close(testLog);
	
	return 0;
}
