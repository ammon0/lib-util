

#include <util/types.h>
#include <util/data.h>
#include <util/msg.h>
#include <util/string.h>
#include <util/io.h>

#include <stdlib.h>

static inline imax cmp(const void * left, const void * right){
	return strcmp((char*) left, (char*) right);
}

// the data is just a character string, nothing to extract
static inline const void * key(const void * data) {
	return data;
}

int main(void){
	char * temp;
	const char
		*first  = "AAAA",
		*second = "BBBB",
		*third  = "CCCC",
		*fourth = "DDDD";
	
	DS list    = DS_new_list    (sizeof(char*)                   );
	DS circle  = DS_new_circular(sizeof(char*)                   );
	DS dup_bst = DS_new_bst     (sizeof(char*), true , &key, &cmp);
	DS ex_bst  = DS_new_bst     (sizeof(char*), false, &key, &cmp);
	DS heap    = DS_new_heap    (sizeof(char*),              &cmp);
	
	typedef enum {DS_list, DS_circular_list, DS_bst, DS_hash} DS_type;
	
	msg_set_verbosity(V_TRACE);
	
	printf("Size of (void*)       : %lu\n", sizeof(void*       ));
	printf("Size of (size_t)      : %lu\n", sizeof(size_t      ));
	printf("Size of (int)         : %lu\n", sizeof(int         ));
	printf("Size of (unsigned int): %lu\n", sizeof(unsigned int));
	printf("Size of (DS_type)     : %lu\n", sizeof(DS_type     ));
	printf("Size of (bool)        : %lu\n", sizeof(bool        ));
	
	puts("");
	
	/*************************** CIRCULAR TESTS *******************************/
	
	if(!DS_isempty(circle)) puts("ERROR: empty circle reports nodes");
	
	if(!DS_insert(circle, first)) puts("ERROR: failed insert first");
	if(!DS_insert(circle, second)) puts("ERROR: failed insert second");
	if(!DS_insert(circle, third)) puts("ERROR: failed insert third");
	if(!DS_insert(circle, fourth)) puts("ERROR: failed insert fourth");
	
	if(DS_isempty(circle)) puts("ERROR: circle misreports empty");
	if(DS_count(circle) != 4) puts("ERROR: circle insert miscount");
	
	if(strcmp((char*)DS_next(circle), third))
		puts("ERROR: not back at the beginning");
	if(strcmp((char*)DS_previous(circle), fourth))
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
	
	if(!DS_insert(ex_bst, second)) puts("ERROR: failed insert second");
	if(!DS_insert(ex_bst, third )) puts("ERROR: failed insert third");
	if(!DS_insert(ex_bst, first )) puts("ERROR: failed insert first");
	if(!DS_insert(ex_bst, fourth)) puts("ERROR: failed insert forth");
	
	if(DS_isempty(ex_bst)) puts("ERROR: BST mis-reports empty");
	if(DS_count(ex_bst) != 4) puts("ERROR: ex_bst miscount");
	
	if(DS_insert(ex_bst, first )) puts("ERROR: inserted dup first");
	if(DS_insert(ex_bst, second)) puts("ERROR: inserted dup second");
	if(DS_insert(ex_bst, third )) puts("ERROR: inserted dup third");
	if(DS_insert(ex_bst, fourth)) puts("ERROR: inserted dup fourth");
	
	temp=(char*) DS_find(ex_bst, second);
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
	
	if(!DS_insert(dup_bst, fourth)) puts("ERROR: failed insert forth");
	if(!DS_insert(dup_bst, second)) puts("ERROR: failed insert second");
	if(!DS_insert(dup_bst, third )) puts("ERROR: failed insert third");
	if(!DS_insert(dup_bst, first )) puts("ERROR: failed insert first");
	if(!DS_insert(dup_bst, first )) puts("ERROR: failed duplicate first");
	if(!DS_insert(dup_bst, second)) puts("ERROR: failed duplicate second");
	if(!DS_insert(dup_bst, third )) puts("ERROR: failed duplicate third");
	if(!DS_insert(dup_bst, fourth)) puts("ERROR: failed duplicate fourth");
	
	if(DS_count(dup_bst) != 8) puts("ERROR: dup_bst miscount");
	
	// Test in-order traversal
	printf("First in-order node: %s\n", (char*) DS_first(dup_bst));
	while((temp = (char*) DS_next(dup_bst)))
		printf("Next: %s\n", temp);
		
	printf("Current is: %s\n", (char*) DS_current(dup_bst));
	
	while((temp =(char*) DS_previous(dup_bst)))
		printf("Previous: %s\n", temp);
	printf("Last in-order node: %s\n", (char*) DS_last(dup_bst));
	
	printf("Current is: %s\n", (char*) DS_current(dup_bst));
	
	temp= (char*) DS_find(dup_bst, second);
	if(!temp || strcmp(second, temp)) puts("ERROR: find failed");
	temp= (char*) DS_find(dup_bst, third);
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
	
	DS_insert(dup_bst, second);
	DS_flush(dup_bst);
	
	DS_insert(ex_bst, second);
	
	DS_delete(ex_bst);
	DS_delete(dup_bst);
	
	printf("\nEND DUP_BST TESTS\n\n");
	
	/***************************** LIST TESTS *********************************/
	
	if(!DS_push(list, first)) puts("ERROR: failed push first");
	if(!DS_push(list, second)) puts("ERROR: failed push second");
	if(!DS_push(list, third)) puts("ERROR: failed push third");
	if(!DS_push(list, fourth)) puts("ERROR: failed push fourth");
	
	puts("dumping list");
	DS_dump(list);
	
	DS_delete(list);
	list = DS_new_list(sizeof(char*));
	
	if(!DS_nq(list, first)) puts("ERROR: failed push first");
	if(!DS_nq(list, second)) puts("ERROR: failed push second");
	if(!DS_nq(list, third)) puts("ERROR: failed push third");
	if(!DS_nq(list, fourth)) puts("ERROR: failed push fourth");
	
	puts("dumping list");
	DS_dump(list);
	
	while(DS_count(list)) DS_remove_last(list);
	
	DS_delete(list);
	
	printf("\nEND LIST TESTS\n\n");
	
	/***************************** HEAP TESTS *********************************/
	
	msg_print(NULL, V_NOTE, "START HEAP TESTS\n\n");
	
	if(!DS_isempty(heap))
		msg_print(NULL, V_ERROR, "empty heap reports nodes.\n");
	
	if(!DS_insert(heap, first))
		msg_print(NULL, V_ERROR, "failed insert first\n");
	if(!DS_insert(heap, second))
		msg_print(NULL, V_ERROR, "failed insert second\n");
	if(!DS_insert(heap, third))
		msg_print(NULL, V_ERROR, "failed insert third\n");
	if(!DS_insert(heap, fourth))
		msg_print(NULL, V_ERROR, "failed insert fourth\n");
	
	if(DS_isempty(heap)) msg_print(NULL, V_ERROR, "heap misreports empty\n");
	if(DS_count(heap) != 4) msg_print(NULL, V_ERROR, "heap insert miscount\n");
	
	msg_print(NULL, V_NOTE, "Dumping heap:\n");
	DS_dump(heap);
	
	for(int i=0; i<4; i++){
		msg_print(NULL, V_INFO, "Current is: %s\n", (char*)DS_current(heap));
		msg_print(NULL, V_INFO, "Count is: %d\n", DS_count(heap));
		DS_remove(heap);
	}
	
	if(!DS_isempty(heap)) msg_print(NULL, V_ERROR, "empty heap is not empty\n");
	
	DS_delete(heap);
	
	
	msg_print(NULL, V_NOTE, "END HEAP TESTS\n\n");
	
	msg_print(NULL, V_NOTE,"\t*** END OF TESTS ***\n\n");
	
	return EXIT_SUCCESS;
}
