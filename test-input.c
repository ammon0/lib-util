// just a test program

#include <stdlib.h>
#include "input.h"

void output (char* string);

void output (char* string){
	printf("'%s'\n", string);
}

int main (void){
	char* string;
	FILE* fp;
	
	fp=fopen("testtext", "r");
	if (fp == NULL){
		puts("could not find file.");
		return EXIT_FAILURE;
	}
	
	puts("WORDS:");
	for(int i=0; i<3; i++){
		string=grabword(fp);
		output(string);
		free(string);
	}
	
	puts("LINES:");
	for(int i=0; i<3; i++){
		string=grabline(fp);
		output(string);
		free(string);
	}
	
	puts("FIELDS:");
	while((string=grabfield(fp)) != NULL){
		output(string);
		free(string);
	}
	
	return EXIT_SUCCESS;
}
