/*******************************************************************************
 *
 *	Welcome to pointer HELL >:D
 *
 ******************************************************************************/

/******************************************************************************/
//                                 HEADERS
/******************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "data.h"


/******************************************************************************/
//                          PRIVATE TYPE DEFINITIONS
/******************************************************************************/


typedef unsigned int uint;

typedef struct _list_node {
	const void        * data ;
	struct _list_node * next ;  // previous pointer for linked lists
	struct _list_node * prev ; // next     pointer for linked lists
} * _lnode_pt;

typedef struct _tree_node {
	const void        * data ;
	const char        * index;
	struct _tree_node * parent;
	struct _tree_node * left;
	struct _tree_node * right;
} * _tnode_pt;

typedef union {
	_lnode_pt l;
	_tnode_pt t;
} _node_pt;

struct _root {
	_node_pt head;
	_node_pt tail;
	_node_pt current;
	_node_pt freelist;
	DS_type  type;
	bool     duplicates_allowed;
	uint     count;  // number of nodes in the structure
};

/********************************* MESSAGES ***********************************/

// ERRORS
	// internal error
const char* _e_noimp  ="Feature Not yet Implemented";
const char* _e_invtype="Invalid DS type";
	// os error
const char* _e_mem    ="Could not allocate more memory";
	// caller error
const char* _e_repeat ="Attempt to insert duplicate data in a structure that does not allow duplicates";
const char* _e_nsense ="Nonsensical action for given structure type";
const char* _e_empty  ="The data structure is empty";


/******************************************************************************/
//                           PRIVATE PROTOTYPES
/******************************************************************************/


/******************************************************************************/
//                       PRIVATE FUNCTION DEFINITIONS
/******************************************************************************/


// report an error
inline static void _error(const char * message){
	fprintf(stderr, "data.h: ERROR: %s\n", message);
}

inline static _node_pt _new_node(const DS const root){
	_node_pt new_node; // will return null on failure
	
	switch(root->type){
	case DS_list:
	case DS_circular_list:
	
		if (root->freelist.l){ // check the freelist first
			new_node = root->freelist;
			root->freelist.l = root->freelist.l->next;
		}
		else {
			new_node.l = calloc(1, sizeof(struct _list_node));
			if (!new_node.l) _error(_e_mem);
		}
	
	break;
	case DS_bst:
	
		if (root->freelist.t){
			new_node = root->freelist;
			root->freelist.t = root->freelist.t->right;
		}
		else{
			new_node.t = calloc(1, sizeof(struct _tree_node));
			if (!new_node.t) _error(_e_mem);
		}
	
	break;
	default:
		_error(_e_invtype);
		new_node.l = NULL;
	}
	
	return new_node;
}

inline static void _print_node(_tnode_pt node, uint lvl){
	for (uint i=0; i<lvl; i++)
		printf("   ");
	puts(node->index);
	if (node->left != NULL)
		_print_node(node->left, lvl+1);
	else {
		for (uint i=0; i<lvl; i++)
			printf("   ");
		puts("left: NULL");
	}
	if (node->right != NULL)
		_print_node(node->right, lvl+1);
	else {
		for (uint i=0; i<lvl; i++)
			printf("   ");
		puts("right: NULL");
	}
}

inline static _tnode_pt _find_least_in_tree(_tnode_pt node){
	while(node->left) node=node->left;
	return node;
}


/******************************************************************************/
//                       PUBLIC FUNCTION DEFINITIONS
/******************************************************************************/


/********************** ACTIONS ON WHOLE DATA STRUCTURE ***********************/

// Make a new data structure
DS DS_new(DS_type type, bool duplicates){
	DS new_structure;
	
	// Checks if any
	switch (type){
	case DS_list         :
	case DS_circular_list:
	case DS_bst          : break;
	default:
		puts(_e_invtype);
		return NULL;
	}
	
	// Allocate space
	new_structure=calloc(1, sizeof(struct _root));
	if (new_structure == NULL) {
		puts(_e_mem);
		return NULL;
	}
	
	// Initialize
	new_structure->type               = type      ;
	new_structure->duplicates_allowed = duplicates;
	
	return new_structure;
}

// Simple tests
inline uint DS_count   (const DS const root) { return root->count   ; }
inline bool DS_isempty (const DS const root) { return !(root->count); }

bool DS_isleaf (const DS const root){
	switch (root->type){
	case DS_bst          : break;
	case DS_list         :
	case DS_circular_list: _error(_e_nsense); return false;
	default: _error(_e_invtype); return false;
	}
	
	return (!( root->current.t->left || root->current.t->right ));
}

// Dump the contents of the data structure
void DS_dump (const DS const root){
	_node_pt this_node = root->head;
	
	switch (root->type){
	case DS_list:
	
		while(this_node.l != NULL) {
			printf("%p\n", this_node.l->data);
			this_node.l = this_node.l->next;
		}
	
	break;
	case DS_circular_list:
	
		if (this_node.l == NULL) break;
		do {
			printf("%s\n", (char*) this_node.l->data);
			this_node.l = this_node.l->next;
		} while (this_node.l != root->head.l);
	
	break;
	case DS_bst:
	
		if (this_node.t == NULL) break;
		_print_node(root->head.t, 0);
	
	break;
	default:
		puts(_e_invtype);
	}
}

/**************************** ADD TO DATA STRUCTURE ***************************/

#define DS_nq(A,B)   DS_insert_first(A,B)
#define DS_push(A,B) DS_insert_first(A,B)

bool DS_insert (const DS const root, void const * const data){
	_node_pt new_node;
	
	switch (root->type){
	case DS_list:
		if (root->current.l == root->head.l)
			return DS_insert_first(root, data);
		else if (root->current.l == root->tail.l)
			return DS_insert_last (root, data);
		
		new_node = _new_node(root);
		if (new_node.l == NULL) return EXIT_FAILURE;
		
		if (root->head.l == NULL){
			root->head = new_node;
			root->tail = new_node;
		}
		else {
			new_node.l->next=root->current.l;
			new_node.l->prev=root->current.l->prev;
			root->current.l->prev->next = new_node.l;
			root->current.l->prev       = new_node.l;
		}
		break;
		
		
	case DS_circular_list:
		new_node = _new_node(root);
		if (new_node.l == NULL) return EXIT_FAILURE;
		
		if (root->head.l == NULL){
			root->head = new_node;
			new_node.l->next = new_node.l;
			new_node.l->prev = new_node.l;
		}
		else {
			new_node.l->next=root->current.l;
			new_node.l->prev=root->current.l->prev;
			root->current.l->prev->next = new_node.l;
			root->current.l->prev       = new_node.l;
		}
		break;
		
		
	case DS_bst: _error(_e_nsense); return EXIT_FAILURE;
	default    : _error(_e_invtype); return EXIT_FAILURE;
	}
	
	root->count++;
	new_node.l->data=data;
	root->current=new_node;
	
	return EXIT_SUCCESS;
}

bool DS_insert_first(const DS const root, void const * const data){
	_node_pt new_node;
	
	switch (root->type){
	case DS_list         : break;
	case DS_circular_list:
	case DS_bst          : _error(_e_nsense ); return EXIT_FAILURE;
	default              : _error(_e_invtype); return EXIT_FAILURE;
	}
	
	// allocate the node
	new_node = _new_node(root);
	if (new_node.l == NULL) return EXIT_FAILURE;
	
	// if the structure is empty
	if (root->head.l == NULL) root->tail=new_node;
	else{
		new_node.l->next = root->head.l;
		root->head.l->prev = new_node.l;
	}
	
	root->count++;
	
	// assign data
	new_node.l->data=data;
	root->current=new_node;
	root->head   =new_node;
	return EXIT_SUCCESS;
}

bool DS_insert_last (const DS const root, void const * const data){
	_node_pt new_node;
	
	switch (root->type){
	case DS_list         : break;
	case DS_circular_list:
	case DS_bst          : _error(_e_nsense ); return EXIT_FAILURE;
	default              : _error(_e_invtype); return EXIT_FAILURE;
	}
	
	// allocate the node
	new_node = _new_node(root);
	if (new_node.l == NULL) return EXIT_FAILURE;
	
	// if the structure is empty
	if (root->head.l == NULL) root->head=new_node;
	else{
		new_node.l->prev = root->tail.l;
		root->tail.l->next = new_node.l;
	}
	
	root->count++;
	
	// assign data
	new_node.l->data=data;
	root->current=new_node;
	root->tail   =new_node;
	return EXIT_SUCCESS;
}

bool DS_sort(
	const DS     const root,
	const void * const data,
	const char * const index
){
	_node_pt new_node;
	_tnode_pt * position;
	int result;
	
	
	switch (root->type){
	case DS_bst          : break;
	case DS_list         :
	case DS_circular_list: _error(_e_nsense ); return EXIT_FAILURE;
	default              : _error(_e_invtype); return EXIT_FAILURE;
	}
	
	// Find the position
	position = &(root->head.t);
	while (*position){
		root->current.t = *position;
		result=strcmp(index, root->current.t->index);
		if      (result <0) position = &(root->current.t->left);
		else if (result >0) position = &(root->current.t->right);
		else { // result is 0
			if(root->duplicates_allowed) position = &((*position)->right);
			else {
				puts(_e_repeat);
				return EXIT_FAILURE;
			}
		}
	}
	
	// position now points to a null _tnode_pt where the new node will go
	
	// allocate the node
	new_node = _new_node(root);
	if (new_node.t == NULL) return EXIT_FAILURE;
	
	root->count++;
	new_node.t->data   = data;
	new_node.t->index  = index;
	new_node.t->parent = root->current.t;
	root->current=new_node;
	
	*position = new_node.t;
	return EXIT_SUCCESS;
}

/*********************** REMOVE FROM DATA STRUCTURE ***************************/

#define DS_pop(A) DS_remove_first(A)
#define DS_dq(A)  DS_remove_last(A)

//void * DS_remove      (DS const root){}
//void * DS_remove_first(DS const root){}
//void * DS_remove_last (DS const root){}

/************************* CHANGE DATA IN STRUCTURE ***************************/

// set the data at the current position
void DS_set_data(const DS const root, void * data){
		if (root->current.l == NULL){
		_error(_e_empty);
		return;
	}
	
	switch (root->type){
	case DS_bst:
		root->current.t->data = data;
		break;
		
	case DS_list:
	case DS_circular_list:
		root->current.l->data = data;
		
	}
}

/********************** VIEW RECORD IN DATA STRUCTURE *************************/

//void* DS_find(const DS root, const char * const){
//	if (root->current.l == NULL){
//		_error(_e_empty);
//		return NULL;
//	}
//	
//}


// VISITING
void * DS_first(const DS const root){ // visit the first node
	if (root->current.l == NULL){
		_error(_e_empty);
		return NULL;
	}
	
	switch (root->type){
	case DS_bst:
		root->current=root->head;
		if (root->current.t == NULL) return NULL;
		while(root->current.t->left != NULL)
			root->current.t=root->current.t->left;
		return root->current.t->data;
		
	case DS_list:
		root->current=root->head;
		return root->current.l->data;
		
	case DS_circular_list: _error(_e_nsense ); return NULL;
	default              : _error(_e_invtype); return NULL;
	}
}

void * DS_last(const DS const root){ // visit the last node
	if (root->current.l == NULL){
		_error(_e_empty);
		return NULL;
	}
	
	switch (root->type){
	case DS_bst:
		root->current=root->head;
		if (root->current.t == NULL) return NULL;
		while(root->current.t->right != NULL)
			root->current.t=root->current.t->right;
		return root->current.t->data;
		
	case DS_list:
		root->current=root->tail;
		return root->current.l->data;
		
	case DS_circular_list: _error(_e_nsense ); return NULL;
	default              : _error(_e_invtype); return NULL;
	}
}

void * DS_next(const DS const root){ // visit the next in-order node
	if (root->current.l == NULL){
		_error(_e_empty);
		return NULL;
	}

	switch (root->type){
	case DS_bst:
		if (root->current.t == NULL) return NULL;
		
		// We can assume the left children have already been visited
		if (root->current.t->right)
			root->current.t = _find_least_in_tree(root->current.t->right);
		
		/* If there are no more right children then we must go up.
		To prevent duplicate visits, we must go up until we approch a parent
		from the left child
		*/
		else{
			while(root->current.t->parent && root->current.t->parent->right == root->current.t)
				root->current.t = root->current.t->parent;
			
			if(!root->current.t->parent){ // we're at the last node
				// leave current somewhere reasonable
				while(root->current.t->right != NULL)
					root->current.t=root->current.t->right;
				return NULL;
			}
			root->current.t = root->current.t->parent;
		}
		
		return root->current.t->data;
		
	case DS_list:
	case DS_circular_list:
		root->current.l=root->current.l->next;
		return root->current.l->data;
		
	default: _error(_e_invtype); return NULL;
	}
}

void * DS_previous(const DS const root){ // visit the previous in-order node
	if (root->current.l == NULL){
		_error(_e_empty);
		return NULL;
	}

	switch (root->type){
	case DS_bst:
		if (root->current.t == NULL) return NULL;
		
		// We can assume the right children have already been visited
		if (root->current.t->left){
			root->current.t=root->current.t->left;
			while(root->current.t->right != NULL)
				root->current.t=root->current.t->right;
		}
		/* If there are no more right children then we must go up.
		To prevent duplicate visits, we must go up until we approch a parent
		from the right child
		*/
		else{
			while(root->current.t->parent && root->current.t->parent->left == root->current.t)
				root->current.t = root->current.t->parent;
			if(!root->current.t->parent){
				// leave current somewhere reasonable
				while(root->current.t->left != NULL)
					root->current.t=root->current.t->left;
				return NULL; // we're at the last node
			}
			root->current.t = root->current.t->parent;
		}
		
		return root->current.t->data;
		
	case DS_list:
	case DS_circular_list:
		root->current.l=root->current.l->prev;
		return root->current.l->data;
		
	default: _error(_e_invtype); return NULL;
	}
}

void * DS_current (const DS const root){ // visit the current node
		if (root->current.l == NULL){
		_error(_e_empty);
		return NULL;
	}
	
	switch (root->type){
	case DS_bst          : return root->current.t->data;
	case DS_list         :
	case DS_circular_list: return root->current.l->data;
	default              : _error(_e_invtype); return NULL;
	}
}

// set the current position to a specific count from the beginning
void * DS_position(const DS const root, const unsigned int position){
		if (root->current.l == NULL){
		_error(_e_empty);
		return NULL;
	}
	
	switch (root->type){
	case DS_list         : break;
	case DS_bst          :
	case DS_circular_list: _error(_e_nsense); return NULL;
	default              : _error(_e_invtype); return NULL;
	}
	
	root->current=root->head;
	for (uint i=1; i != position; i++) root->current.l = root->current.l->next;
	
	return root->current.l->data;
}











/*********************** REMOVE FROM DATA STRUCTURE ***************************/

///**	Pop the top record from a linked list
// */
//void* pop(const DS root) {
//	void* data;
//	struct _node* temp;
//	
//	if (root->head==NULL)
//		return NULL;
//	
//	if(root->view == root->head)
//		root->view= NULL;
//	
//	data=root->head->data;
//	temp=root->head;
//	
//	switch (root->type){
//	case _LL:
//	
//		if (root->head->right == NULL){ // if this is the last node
//			root->head=NULL;
//			root->tail=NULL;
//		}else{
//			root->head->right->previous=NULL;
//			root->head=root->head->right;
//		}
//	
//	break;
//	case _CLL:
//	
//		if(root->head->right == root->head){ // if this is the last node
//			root->head=NULL;
//			root->tail=NULL;
//		}else{
//			root->tail->right			=root->head->right;
//			root->head->right->previous	=root->tail;
//			root->head					=root->head->right;
//		}
//	
//	break;
//	case _BST:
//	case 'b':
//		puts(_e_nsense);
//		return NULL;
//	default:
//		puts(_e_invtype);
//		return NULL;
//	}
//	
//	free(temp);
//	root->size--;
//	return data;
//}
///**	de-queue the bottom record from a linked list
// */
//void* dq(DS root) {
//	void* data;
//	struct _node* temp;
//	
//	if (root->tail==NULL)
//		return NULL;
//	
//	if(root->view == root->tail)
//		root->view= NULL;
//	
//	data=root->tail->data;
//	temp=root->tail;
//	
//	switch (root->type){
//	case _LL:
//	
//		if (root->tail->previous == NULL){ // if this is the last node
//			root->head=NULL;
//			root->tail=NULL;
//		}else{
//			root->tail->previous->right=NULL;
//			root->tail=root->tail->previous;
//		}
//	
//	break;
//	case _CLL:
//	
//		if(root->tail->right == root->tail){ // if this is the last node
//			root->head=NULL;
//			root->tail=NULL;
//		}else{
//			root->head->previous		=root->tail->previous;
//			root->tail->previous->right	=root->head;
//			root->tail					=root->tail->previous;
//		}
//	
//	break;
//	case _BST:
//	case 'b':
//		puts(_e_nsense);
//		return NULL;
//	default:
//		puts(_e_invtype);
//		return NULL;
//	}
//	free(temp);
//	root->size--;
//	return data;
//}

///**	remove a record from a linked list
// *	record is searched by ID
// */
//int iremove(DS root, char* index) {
//	_isearch(root, index);
//	if (root->view == NULL){
//		printf("iremove(): ERROR: %s not found.\n", index);
//		return EXIT_FAILURE;
//	}
//	_delete_node(root);
//	return EXIT_SUCCESS;
//}

///**	remove a node by position
// *	positive counts from the head, negative from the tail
// */
////int premove(DS root, int position) {
////	return EXIT_FAILURE;
////}

///********************** VIEW RECORD IN DATA STRUCTURE *************************/

///**	search for a node by index in a sorted link list and return its contents
// */
//void* iview(const DS root, const char* index){
//	_isearch(root, index);
//	if (root->view == NULL)
//		return NULL;
//	return root->view->data;
//}

///**	search for a node by position and return its contents
// *	positive counts from the head, negative from the tail
// */
//void* pview(const DS root, int position){
//	switch (root->type){
//	case _LL:
//		if (position != 0){
//			puts(_e_noimp);
//			// FIXME
//			return NULL;
//		}else{
//			root->view=NULL;
//			return root->view;
//		}
//	
//	case _CLL:
//		puts(_e_noimp);
//		return NULL;
//	
//	case _BST:
//	case 'b':
//		puts(_e_nsense);
//		return NULL;
//	
//	default:
//		puts(_e_invtype);
//		return NULL;
//	}
//}

///**	View each member of a linked list in order
// *	each call to this function returns the next member of the ll
// *	return to the first member with pview(root, 1);
// */
//void* view_next(DS root) {
//	if (root->head == NULL){ // empty data structure
//		//puts(_n_empty);
//		return NULL;
//	}
//	
//	if (root->view == NULL){
//		root->view = root->head;
//		return root->view->data;
//	}
//	
//	if (root->view->right == NULL) {
//		//puts(_n_end);
//		root->view = root->head;
//		return NULL;
//	}
//	
//	root->view = root->view->right;
//	return root->view->data;
//}

///***************************** PRIVATE FUNCTIONS ******************************/



//static void _isearch(const DS root, const char* index){
//	int result;
//	
//	if (root->head == NULL){
//		root->view=NULL;
//		return;
//	}
//	
//	// FIXME this function should check that the index pointer is set. 
//	// is this a sorted list?
//	
//	root->view=root->head;
//	
//	switch (root->type){
//	case _CLL:
//	
//		while (root->view != root->tail){
//			result=strcmp(index,root->view->index);
//			
//			if (result>0)
//				root->view=root->view->right;
//			else if (result<0)
//				root->view=NULL;
//			else return; //strcmp returns 0
//		}
//	
//	break;
//	case _LL:
//	
//		while (root->view != NULL){
//			result=strcmp(index,root->view->index);
//			
//			if (result>0)
//				root->view=root->view->right;
//			else if (result<0)
//				root->view=NULL;
//			else return; //strcmp returns 0
//		}
//	
//	break; // this will probably be the same for trees
//	case _BST:
//	
//		while (root->view != NULL){
//			result=strcmp(index,root->view->index);
//			
//			if (result>0)
//				root->view=root->view->right;
//			else if (result<0)
//				root->view=root->view->left;
//			else return; //strcmp returns 0
//		}
//	
//	break;
//	case 'b':
//	
//		puts(_e_noimp);
//	
//	break;
//	default:
//		puts(_e_invtype);
//	}
//}

//static void _delete_node(const DS root){
//	if (root->view == NULL) return;
//	
//	switch (root->type){
//	case _LL:
//	case _CLL:
//	
//		if (root->view == root->head){
//			(void) pop(root);
//			return;
//		}
//		if (root->view == root->tail){
//			(void) dq(root);
//			return;
//		}
//		
//		// It we're in the middle of the list
//		root->view->previous->right=root->view->right;
//		root->view->right->previous=root->view->previous;
//		root->size--;
//	
//	break;
//	case _BST:
//	case 'b':
//	
//		puts(_e_noimp);
//	
//	break;
//	default:
//		puts(_e_invtype);
//	}
//	
//	free(root->view);
//	root->view = NULL;
//	return;
//}



///**	add new data to a link list data structure in sort order
// *	data must be a structure with field int* ID that will be used for sorting
// */
//static int _sortll(DS root, void* data_pt, const char* index) {
//	struct _node* new_node;
//	struct _node* current_position;
//	
//	// allocate a new node
//	new_node=malloc(sizeof(struct _node));
//	if (new_node == NULL){
//		puts(_e_mem);
//		return EXIT_FAILURE;
//	}
//	
//	// setup the new node
//	new_node->data=data_pt;
//	new_node->index=index;
//	new_node->left=NULL;
//	root->view=new_node;
//	
//	root->size++;
//	
//	if (root->head == NULL) { // if the data structure is empty
//		_add_first_node(root, new_node);
//		return EXIT_SUCCESS;
//	}
//	
//	current_position=root->head;
//	while (( strcmp(index,current_position->index) )>0) {
//		if (current_position->right == NULL) {
//			// we are at the end of the linked list,
//			// and the new data must go last
//			
//			current_position->right=new_node;
//			
//			new_node->previous=current_position;
//			new_node->right=NULL;
//			
//			root->tail=new_node;
//			
//			return EXIT_SUCCESS;
//		}
//		
//		current_position=current_position->right;
//	}
//	
//	if (!strcmp(index,current_position->index)) {
//		puts(_e_repeat);
//		free(new_node);
//		return EXIT_FAILURE;
//	}
//	
//	if (current_position == root->head){
//		// we are at the head of the linked list and the new data must go before
//		
//		current_position->previous=new_node;
//		
//		new_node->previous=NULL;
//		new_node->right=current_position;
//		
//		root->head=new_node;
//		
//		return EXIT_SUCCESS;
//	}
//	
//	// If we are still running, then there is a node before and after us,
//	// and the new node must go before the current node
//	
//	current_position->previous->right=new_node;
//	
//	new_node->previous=current_position->previous;
//	new_node->right=current_position;
//	
//	current_position->previous=new_node;
//	
//	return EXIT_SUCCESS;
//}



