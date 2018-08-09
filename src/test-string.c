

#include <util/string.h>
#include <util/msg.h>
#include <util/io.h>

#define DEST_SZ 100

int main(void){
	char dest_string[DEST_SZ] = "canary";
	char beg_string[] = "Beginning "; // 10
	char end_string[] = "ending..."; // 9
	size_t retval;
	
	
	msg_set_verbosity(V_TRACE);
	
/*	strcpy(dest_string, beg_string);*/
/*	strcat(dest_string, beg_string);*/
	strcmp(dest_string, beg_string); //< statement with no effect
/*	strncat(dest_string, beg_string);*/
/*	strncpy(dest_string, beg_string);*/
	
	
	//strlcat(dest_string, src_string, DEST_SZ);
	
	retval = strlcpy(dest_string, beg_string, 0);
	if( (int)strncmp(dest_string, "canary", DEST_SZ) != 0)
		msg_print(NULL, V_ERROR, "canary missing\n");
	
	if(retval != strlen(beg_string))
		msg_print(NULL, V_ERROR, "retval %u\n", retval);
	
	
	retval = strlcpy(dest_string, beg_string, 1);
	if(strncmp(dest_string, "", DEST_SZ) != 0)
		msg_print(NULL, V_ERROR, "1 dest_string: '%s'\n", dest_string);
	
	if(retval != strlen(beg_string))
		msg_print(NULL, V_ERROR, "retval %u\n", retval);
	
	
	retval = strlcpy(dest_string, beg_string, 2);
	if(strncmp(dest_string, "B", DEST_SZ) != 0)
		msg_print(NULL, V_ERROR, "2 dest_string: '%s'\n", dest_string);
	
	if(retval != strlen(beg_string))
		msg_print(NULL, V_ERROR, "retval %u\n", retval);
	
	
	retval = strlcpy(dest_string, beg_string, DEST_SZ);
	if(strncmp(dest_string, beg_string, DEST_SZ))
		msg_print(NULL, V_ERROR, "DEST_SZ\n");
	
	if(retval != strlen(beg_string))
		msg_print(NULL, V_ERROR, "retval %u\n", retval);
	
	
	/** strlcat **/
	
	retval = strlcat(dest_string, end_string, DEST_SZ);
	msg_print(NULL, V_NOTE, "%s\n", dest_string);
	
	if(retval != strlen(beg_string) + strlen(end_string))
		msg_print(NULL, V_ERROR, "retval %u\n", retval);
	
}


