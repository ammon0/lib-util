#include "input.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define IN	1 /* inside a word */
#define OUT	2 /* outside a word */
#define ARRAY_SIZE 50 // this is just a starting size
#define SP_NW_FIELD 3 // number of spaces for a new field in grabfield()


/******************************************************************************/
//                            PRIVATE FUNCTIONS
/******************************************************************************/

static inline bool line(char c){
	return ( c == '\n' || c == '\r' || c == EOF );
}
static inline bool word(char c){
	return ( c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == EOF );
}

static inline char* finishup(char* store, unsigned int i){
	if (*store == '\0') { // if nothing was read
		free(store);
		return NULL;
	}
	
	// get rid of any trailing spaces or tabs we might have captured
	while(store[i-1] == ' ' || store[i-1] == '\t'){
		store[i-1]='\0';
		i--;
	}
	
	// create a correctly sized array to output the result
	if ( (store=realloc(store, i+1)) == NULL ) {
		puts("ERROR: realloc() failed.");
		free(store);
		return NULL;
	}
	return store;
}

// since grabword and grabline were nearly identical I combined their code
static inline char* grab (bool (test) (char c), FILE* source){
	unsigned int i    = 0;          // position in the array
	unsigned int size = ARRAY_SIZE; // current size of the array
	int          state= OUT;        // assume that the pointer is not in a word
	char       * store;             // an array to temporarily hold the word
	char         c;                 // temporary character
	
	if (source == NULL)
		return NULL;
	
	// allocate
	if ( (store=malloc(ARRAY_SIZE)) == NULL ) {
		puts("ERROR: malloc() failed.");
		return NULL;
	}
	*store='\0';
	
	// while c is not whitespace read it into store
	do {
		c = (char) fgetc(source);
		if (test (c)){
			if (state == IN)
				break; // stop when we find whitespace at the end of the word
		} else {
			state = IN;
			if (i == size-1) { // if store isn't big enough we double it.
				if ( (store=realloc(store, size *=2)) == NULL ) {
					puts("ERROR: realloc() failed.");
					free(store);
					return NULL;
				}
			}
			// store c in the array and increment i
			store[i]=c;
			i++;
			store[i]='\0';
		}
	} while (!feof(source));
	
	return finishup(store, i);
}


/******************************************************************************/
//                             PUBLIC FUNCTIONS
/******************************************************************************/


char* grabword(FILE* source){
	return grab(&word, source);
}

char* grabline(FILE* source){
	return grab(&line, source);
}

char* grabfield(FILE* source){
	unsigned int i    = 0;          // position in the array
	unsigned int size = ARRAY_SIZE; // current size of the array
	int          state= OUT;        // assume that the pointer is not in a field
	char*        store;             // an array to temporarily hold the field
	char         c;                 // temporary character
	int          space_count=0;     // track number of contiguous spaces
	
	if (source == NULL)
		return NULL;
	
	// allocate
	if ( (store=calloc(1, ARRAY_SIZE)) == NULL ) {
		puts("ERROR: malloc() failed.");
		return NULL;
	}
	
	// while c is not EOF read it into store
	do {
		c = (char) fgetc(source);
		if      (c == ' ' && state == OUT); // do nothing
		else if( (c>='\t' && c<='\r') || c == EOF || space_count >=SP_NW_FIELD){
			if (state == IN)
				break; // stop when we find whitespace at the end of the word
		
		} else {
			state = IN;
			if (i == size-1) { // if store isn't big enough we double it.
				if ( (store=realloc(store, size *=2)) == NULL ) {
					puts("ERROR: realloc() failed.");
					free(store);
					return NULL;
				}
			}
			// store c in the array and increment i
			store[i]=c;
			i++;
			store[i]='\0';
			
			if (c == ' ') space_count++;
			else space_count=0;
		}
	} while (!feof(source));
	
	return finishup(store, i);
}


