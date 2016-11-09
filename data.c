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
	const void        * data;
	struct _list_node * next;
	struct _list_node * prev;
} * _lnode_pt;

typedef struct _tree_node {
	const void        * data  ;
	struct _tree_node * parent;
	struct _tree_node * left  ;
	struct _tree_node * right ;
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
	int (*comparator)(const void* left, const void* right);
	bool     duplicates_allowed;
	uint     count;  // number of nodes in the structure
};

/********************************* MESSAGES ***********************************/

// ERRORS
	// internal error
const char* _e_invtype="Invalid DS type";
	// os error
const char* _e_mem    ="Could not allocate more memory";
	// caller error
const char* _e_repeat ="Attempt to insert duplicate data in a structure that does not allow duplicates";
const char* _e_nsense ="Nonsensical action for given structure type";
const char* _e_empty  ="The data structure is empty";


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
			root->freelist.l = root->freelist.l->prev;
			
			// Make sure it's clean for the next use
			new_node.l->next = NULL;
			new_node.l->prev = NULL;
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
			
			// Make sure it's clean for the next use
			new_node.t->left = NULL;
			new_node.t->right = NULL;
			new_node.t->parent = NULL;
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
	puts((char*) node->data);
	
	if (node->right != NULL)
		_print_node(node->right, lvl+1);
	else {
		for (uint i=0; i<lvl; i++)
			printf("   ");
		puts("right: NULL");
	}
	
	if (node->left != NULL)
		_print_node(node->left, lvl+1);
	else {
		for (uint i=0; i<lvl; i++)
			printf("   ");
		puts("left: NULL");
	}
}

inline static _tnode_pt _remove_least_in_tree(_tnode_pt * parent_pt){
	_tnode_pt node;
	
	while((*parent_pt)->left) parent_pt = &(*parent_pt)->left;
	
	// we know there is no left child, but there may be a right
	node = *parent_pt;
	*parent_pt = node->right;
	
	return node;
}

//inline static _tnode_pt * _find_greatest_in_tree(_tnode_pt * parent_pt){
//	while((*parent_pt)->right) parent_pt = &(*parent_pt)->right;
//	return parent_pt;
//}


/******************************************************************************/
//                       PUBLIC FUNCTION DEFINITIONS
/******************************************************************************/


/********************** ACTIONS ON WHOLE DATA STRUCTURE ***********************/

// Make a new data structure
DS DS_new(
	DS_type type,
	bool duplicates,
	int (*comparator)(const void* left, const void* right)
){
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
	new_structure->comparator         = comparator;
	
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
	const void * const data
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
		result=root->comparator(data, root->current.t->data);
		if      (result <0) position = &(root->current.t->left);
		else if (result >0) position = &(root->current.t->right);
		else { // result is 0
			if(root->duplicates_allowed) position = &(root->current.t->right);
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
	new_node.t->parent = root->current.t;
	root->current=new_node;
	
	*position = new_node.t;
	return EXIT_SUCCESS;
}

/*********************** REMOVE FROM DATA STRUCTURE ***************************/

#define DS_pop(A) DS_remove_first(A)
#define DS_dq(A)  DS_remove_last(A)

void * DS_remove(DS const root){
	void * data;
	_tnode_pt swapnode;
	_tnode_pt * parent_pt;
	
	if (root->current.l == NULL){
		_error(_e_empty);
		return NULL;
	}
	
	data = root->current.l->data;
	
	switch (root->type){
	case DS_bst:
		// get the parent's pointer pointer
		if(!root->current.t->parent) parent_pt = &root->head.t;
		else if(root->current.t->parent->left == root->current.t)
			parent_pt = &root->current.t->parent->left;
		else parent_pt = &root->current.t->parent->right;
		
		// No left Child
		if(!root->current.t->left){
			*parent_pt = root->current.t->right;
			if(*parent_pt) (*parent_pt)->parent = root->current.t->parent;
		}
		// No right Child
		else if(!root->current.t->right){
			*parent_pt = root->current.t->left;
			if(*parent_pt) (*parent_pt)->parent = root->current.t->parent;
		} 
		else{ //Two Children
			swapnode = _remove_least_in_tree(&root->current.t->right);
			
			// set the swapnode
			swapnode->left  = root->current.t->left;
			swapnode->right = root->current.t->right;
			swapnode->parent = root->current.t->parent;
			
			// insert the swapnode
			*parent_pt = swapnode;
			if(swapnode->right) swapnode->right->parent = swapnode;
			swapnode->left->parent = swapnode;
		}
		
		// return current to freelist
		root->current.t->right = root->freelist.t;
		root->freelist = root->current;
		
		// Set current to something
		root->current = root->head;
		break;
		
	case DS_list:
		// Check if we are at the beginning or end
		if (root->current.l == root->head.l)
			return DS_remove_first(root);
		else if (root->current.l == root->tail.l)
			return DS_remove_last (root);
	
	case DS_circular_list:
		if (root->current.l->next == root->current.l){
			// last node in a circular list
			root->head.l = NULL;
		}
		else{
			// Close the gap
			root->current.l->prev->next = root->current.l->next;
			root->current.l->next->prev = root->current.l->prev;
		}
		
		// move to freelist
		root->current.l->prev = root->freelist.l;
		root->freelist = root->current;
		
		root->current.l = root->current.l->next;
		break;
		
	default: _error(_e_invtype); return NULL;
	}
	
	if (!root->head.l){
		root->current.l = NULL;
		root->tail.l    = NULL;
	}
	
	root->count--;
	return data;
}


void * DS_remove_first(DS const root){
	void * data;
	_tnode_pt * parent_pt;
	
	if (root->current.l == NULL){
		_error(_e_empty);
		return NULL;
	}
	
	switch (root->type){
	case DS_list:
		root->current = root->head;
		data = root->current.l->data;
		
		// remove from structure
		root->head.l = root->head.l->next;
		root->head.l->prev = NULL;
		
		// move to freelist
		root->current.l->prev = root->freelist.l;
		root->freelist = root->current;
		
		// set current
		root->current = root->head;
		if (!root->head.l) root->tail.l = NULL;
		
		break;
		
	case DS_bst:
		parent_pt = &root->head.t;
		
		while((*parent_pt)->left){
			parent_pt = &(*parent_pt)->left;
		}
		root->current.t = *parent_pt;
		
		// remove from tree
		*parent_pt = root->current.t->right;
		if (root->current.t->right)
			root->current.t->right->parent = root->current.t->parent;
		
		data = root->current.t->data;
		
		// move to freelist
		root->current.t->right = root->freelist.t;
		root->freelist = root->current;
		
		// set current
		root->current = root->head;
		
		break;
		
	case DS_circular_list: _error(_e_nsense ); return NULL;
	default              : _error(_e_invtype); return NULL;
	}
	
	
	root->count--;
	return data;
}


void * DS_remove_last (DS const root){
	void * data;
	_tnode_pt * parent_pt;
	
	if (root->current.l == NULL){
		_error(_e_empty);
		return NULL;
	}
	
	switch (root->type){
	case DS_list:
		root->current = root->tail;
		data = root->current.l->data;
		
		// remove from structure
		root->tail.l = root->tail.l->prev;
		root->tail.l->next = NULL;
		
		// move to freelist
		root->current.l->prev = root->freelist.l;
		root->freelist = root->current;
		
		// set current
		root->current = root->tail;
		if (!root->tail.l) root->head.l = NULL;
		break;
	
	case DS_bst:
		parent_pt = &root->head.t;
		
		while((*parent_pt)->right){
			parent_pt = &(*parent_pt)->right;
		}
		root->current.t = *parent_pt;
		
		// remove from tree
		*parent_pt = root->current.t->left;
		if (root->current.t->left)
			root->current.t->left->parent = root->current.t->parent;
		
		data = root->current.t->data;
		
		// move to freelist
		root->current.t->right = root->freelist.t;
		root->freelist = root->current;
		
		// set current
		root->current = root->head;
		
		break;
	case DS_circular_list: _error(_e_nsense ); return NULL;
	default              : _error(_e_invtype); return NULL;
	}
	
	
	root->count--;
	return data;
}

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
		break;
		
	default: _error(_e_invtype); return;
	}
}

/********************** VIEW RECORD IN DATA STRUCTURE *************************/

void* DS_find(const DS root, const void * const key){
	_tnode_pt node;
	int result;
	
	if (root->current.l == NULL){
		_error(_e_empty);
		return NULL;
	}
	
	switch (root->type){
	case DS_bst: break;
	case DS_list:
	case DS_circular_list: _error(_e_nsense); return NULL;
	default: _error(_e_invtype); return NULL;
	}
	
	node = root->head.t;
	while (node != NULL){
		result=root->comparator(key, node->data);
		
		if      (result>0) node=node->right;
		else if (result<0) node=node->left;
		else { //strcmp returns 0
			root->current.t=node;
			return root->current.t->data;
		}
	}
	
	return NULL;
}


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
		while(root->current.t->right)
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
	case DS_bst: // this is an in-order traversal
		
		// We can assume the left children have already been visited
		if (root->current.t->right){
			root->current.t = root->current.t->right;
			while(root->current.t->left)
				root->current.t = root->current.t->left;
		}
		
		/* If there are no more right children then we must go up.
		To prevent duplicate visits, we must go up until we approach a parent
		from the left child
		*/
		else{
			while(root->current.t->parent && root->current.t->parent->right == root->current.t)
				root->current.t = root->current.t->parent;
			if(!root->current.t->parent){ // we're at the last node
				// leave current somewhere reasonable
				while(root->current.t->right)
					root->current.t=root->current.t->right;
				return NULL;
			}
			else root->current.t = root->current.t->parent;
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
		// We can assume the right children have already been visited
		if (root->current.t->left){
			root->current.t=root->current.t->left;
			while(root->current.t->right)
				root->current.t=root->current.t->right;
		}
		/* If there are no more right children then we must go up.
		To prevent duplicate visits, we must go up until we approach a parent
		from the right child
		*/
		else{
			while(root->current.t->parent && root->current.t->parent->left == root->current.t)
				root->current.t = root->current.t->parent;
			if(!root->current.t->parent){
				// leave current somewhere reasonable
				while(root->current.t->left != NULL)
					root->current.t=root->current.t->left;
				return NULL; // we're at the first node
			}
			else root->current.t = root->current.t->parent;
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


