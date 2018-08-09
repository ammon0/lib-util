// just a test program

#include <util/msg.h>
#include <util/io.h>

int main (void){
	
	for(msg_log_lvl i=V_QUIET; i<V_NUM; i++){
		msg_set_verbosity(i);
		printf("Verbosity is %d\n", i);
		
		for(msg_log_lvl j=V_ERROR; j<V_NUM; j++){
			msg_print(NULL, j, "message at level %d\n", j);
		}
		
		puts("");
	}
	
	return 0;
}
