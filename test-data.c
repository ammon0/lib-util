#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"


static inline int cmp(const void * left, const void * right){
	return strcmp((char*) left, (char*) right);
}

int main(void){
	char * temp;
	const char *first = "AAAA",
		*second = "BBBB",
		*third  = "CCCC",
		*fourth = "DDDD";
	
	//DS list    = DS_new(DS_list         , true );
	DS circle  = DS_new(DS_circular_list, sizeof(char*), true , NULL, NULL);
	DS dup_bst = DS_new(DS_bst          , sizeof(char*), true , &cmp, &cmp);
	DS ex_bst  = DS_new(DS_bst          , sizeof(char*), false, &cmp, &cmp);
	
	puts("");
	
	/*************************** CIRCULAR TESTS *******************************/
	
	if(!DS_isempty(circle)) puts("ERROR: empty circle reports nodes");
	
	if(DS_insert(circle, first)) puts("ERROR: failed insert first");
	if(DS_insert(circle, second)) puts("ERROR: failed insert second");
	if(DS_insert(circle, third)) puts("ERROR: failed insert third");
	if(DS_insert(circle, fourth)) puts("ERROR: failed insert fourth");
	
	if(DS_isempty(circle)) puts("ERROR: circle misreports empty");
	if(DS_count(circle) != 4) puts("ERROR: circle insert miscount");
	
	if(strcmp(DS_next(circle), third)) puts("ERROR: not back at the beginning");
	if(strcmp(DS_previous(circle), fourth))
		puts("ERROR: not back at the beginning");
	
	puts("Dumping circle:");
	DS_dump(circle);
	
	for(int i=0; i<4; i++){
		printf("Count is: %d, ", DS_count(circle));
		DS_remove(circle);
	}
	
	if(!DS_isempty(circle)) puts("ERROR: empty circle is not empty");
	
	DS_delete(circle);
	
	printf("\nEND CIRCLE TESTS\n\n");
	
	/***************************** BST TESTS **********************************/
	
	if(!DS_isempty(ex_bst)) puts("ERROR: empty BST reports nodes");
	
	if(DS_sort(ex_bst, second)) puts("ERROR: failed insert second");
	if(DS_sort(ex_bst, third )) puts("ERROR: failed insert third");
	if(DS_sort(ex_bst, first )) puts("ERROR: failed insert first");
	if(DS_sort(ex_bst, fourth)) puts("ERROR: failed insert forth");
	
	if(DS_isempty(ex_bst)) puts("ERROR: BST mis-reports empty");
	if(DS_count(ex_bst) != 4) puts("ERROR: ex_bst miscount");
	
	if(!DS_sort(ex_bst, first )) puts("ERROR: inserted dup first");
	if(!DS_sort(ex_bst, second)) puts("ERROR: inserted dup second");
	if(!DS_sort(ex_bst, third )) puts("ERROR: inserted dup third");
	if(!DS_sort(ex_bst, fourth)) puts("ERROR: inserted dup fourth");
	
	temp=DS_find(ex_bst, second);
	if(!temp || strcmp(second, temp)) puts("ERROR: find failed");
	
	puts("Dumping ex_bst:");
	DS_dump(ex_bst);
	
	for(int i=0; i<4; i++){
		printf("Current is: %s\n", (char*) DS_current(ex_bst));
		DS_remove(ex_bst);
		DS_dump(ex_bst);
	}
	
	DS_dump(ex_bst);
	
	printf("\nEND EX_BST TESTS\n\n");
	
	if(DS_sort(dup_bst, fourth)) puts("ERROR: failed insert forth");
	if(DS_sort(dup_bst, second)) puts("ERROR: failed insert second");
	if(DS_sort(dup_bst, third )) puts("ERROR: failed insert third");
	if(DS_sort(dup_bst, first )) puts("ERROR: failed insert first");
	if(DS_sort(dup_bst, first )) puts("ERROR: failed duplicate first");
	if(DS_sort(dup_bst, second)) puts("ERROR: failed duplicate second");
	if(DS_sort(dup_bst, third )) puts("ERROR: failed duplicate third");
	if(DS_sort(dup_bst, fourth)) puts("ERROR: failed duplicate fourth");
	
	if(DS_count(dup_bst) != 8) puts("ERROR: dup_bst miscount");
	
	// Test in-order traversal
	printf("First in-order node: %s\n", (char*) DS_first(dup_bst));
	while((temp = DS_next(dup_bst)))
		printf("Next: %s\n", temp);
		
	printf("Current is: %s\n", (char*) DS_current(dup_bst));
	
	while((temp = DS_previous(dup_bst)))
		printf("Previous: %s\n", temp);
	printf("Last in-order node: %s\n", (char*) DS_last(dup_bst));
	
	printf("Current is: %s\n", (char*) DS_current(dup_bst));
	
	temp=DS_find(dup_bst, second);
	if(!temp || strcmp(second, temp)) puts("ERROR: find failed");
	temp=DS_find(dup_bst, third);
	if(!temp || strcmp(third, temp)) puts("ERROR: find failed");
	
	puts("Dumping dup_bst:");
	DS_dump(dup_bst);
	
	while(! DS_isempty(dup_bst)){
		DS_remove_first(dup_bst);
		puts("Dumping dup_bst:");
		DS_dump(dup_bst);
		DS_remove_last(dup_bst);
		puts("Dumping dup_bst:");
		DS_dump(dup_bst);
	}
	
	DS_sort(dup_bst, second);
	DS_flush(dup_bst);
	
	DS_sort(ex_bst, second);
	
	DS_delete(ex_bst);
	DS_delete(dup_bst);
	
	printf("\nEND DUP_BST TESTS\n\n");
	
	/***************************** LIST TESTS *********************************/
	
	printf("\n*** END OF TESTS ***\n\n");
	
	return EXIT_SUCCESS;
}
