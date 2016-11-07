#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"


int main(void){
	char * temp;
	char *first = "FIRST",
		*second = "SECOND",
		*third = "THIRD",
		*fourth = "FORTH";
	
	//DS list    = DS_new(DS_list         , true );
	DS circle  = DS_new(DS_circular_list, true );
	DS dup_bst = DS_new(DS_bst          , true );
	DS ex_bst  = DS_new(DS_bst          , false);
	
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
	
	printf("\nEND CIRCLE TESTS\n\n");
	
	/***************************** BST TESTS **********************************/
	
	if(!DS_isempty(ex_bst)) puts("ERROR: empty BST reports nodes");
	
	if(DS_sort(ex_bst, second, second)) puts("ERROR: failed insert second");
	if(DS_sort(ex_bst, third, third)) puts("ERROR: failed insert third");
	if(DS_sort(ex_bst, first, first)) puts("ERROR: failed insert first");
	if(DS_sort(ex_bst, fourth, fourth)) puts("ERROR: failed insert forth");
	
	if(DS_isempty(ex_bst)) puts("ERROR: BST mis-reports empty");
	if(DS_count(ex_bst) != 4) puts("ERROR: ex_bst miscount");
	
	if(!DS_sort(ex_bst, first, first)) puts("ERROR: inserted dup first");
	if(!DS_sort(ex_bst, second, second)) puts("ERROR: inserted dup second");
	if(!DS_sort(ex_bst, third, third)) puts("ERROR: inserted dup third");
	if(!DS_sort(ex_bst, fourth, fourth)) puts("ERROR: inserted dup fourth");
	
	puts("Dumping ex_bst:");
	DS_dump(ex_bst);
	printf("\nEND EX_BST TESTS\n\n");
	
	if(DS_sort(dup_bst, fourth, fourth)) puts("ERROR: failed insert forth");
	if(DS_sort(dup_bst, second, second)) puts("ERROR: failed insert second");
	if(DS_sort(dup_bst, third, third)) puts("ERROR: failed insert third");
	if(DS_sort(dup_bst, first, first)) puts("ERROR: failed insert first");
	if(DS_sort(dup_bst, first, first)) puts("ERROR: failed duplicate first");
	if(DS_sort(dup_bst, second, second)) puts("ERROR: failed duplicate second");
	if(DS_sort(dup_bst, third, third)) puts("ERROR: failed duplicate third");
	if(DS_sort(dup_bst, fourth, fourth)) puts("ERROR: failed duplicate fourth");
	
	if(DS_count(dup_bst) != 8) puts("ERROR: dup_bst miscount");
	
	// Test in-order traversal
	printf("First in-order node: %s\n", (char*) DS_first(dup_bst));
	while((temp = DS_next(dup_bst)))
		printf("Next: %s\n", temp);
		
	printf("Current is: %s\n", (char*) DS_current(dup_bst));
	
	while((temp = DS_previous(dup_bst)))
		printf("Previous: %s\n", temp);
	printf("Last in-order node: %s\n", (char*) DS_last(dup_bst));
	
	puts("Dumping dup_bst:");
	DS_dump(dup_bst);
	printf("\nEND DUP_BST TESTS\n\n");
	
	/***************************** LIST TESTS *********************************/
	
	printf("\n*** END OF TESTS ***\n\n");
	
	return EXIT_SUCCESS;
}
