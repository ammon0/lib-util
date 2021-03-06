/*******************************************************************************
 *
 *	lib-util : A Utility Library
 *
 *	Copyright (c) 2016-2018 Ammon Dodson
 *	You should have received a copy of the license terms with this software. If
 *	not, please visit the project homepage at:
 *	https://github.com/ammon0/lib-util
 *
 *
 *	Welcome to pointer HELL >:D
 *
 ******************************************************************************/


/******************************************************************************/
//                                 HEADERS
/******************************************************************************/


#include <util/data.h>
#include <util/types.h>
#include <util/msg.h>
#include <util/string.h>
#include <util/io.h>

#include <stdlib.h>



/******************************************************************************/
//                          PRIVATE TYPE DEFINITIONS
/******************************************************************************/


#define DS_DEFAULT_TABLE_SZ 1023

typedef enum {
	DS_list,
	DS_circular_list,
	DS_bst,
	DS_heap,
	DS_hash
} DS_type;

typedef struct _list_node {
	struct _list_node * next;
	struct _list_node * prev;
	int8_t data[];
} * _lnode_pt;

typedef struct _tree_node {
	struct _tree_node * parent;
	struct _tree_node * left  ;
	struct _tree_node * right ;
	int8_t data[];
} * _tnode_pt;

typedef union {
	_lnode_pt l;
	_tnode_pt t;
} _node_pt;

struct _root {
	_node_pt     head;
	_node_pt     tail;
	_node_pt     current;
	_node_pt     freelist;
	union{
		const void * (*key)(const void * data);
		uint64_t     (*hash)(const void * data);
	} keys;
	imax         (*cmp_keys) (const void * left, const void * right);
	size_t       data_size;
	size_t       key_size;
	size_t       table_size;
	DS_type      type;
	uint         count;  // number of nodes in the structure
	bool         dups;  // duplicate data allowed
};

/********************************* MESSAGES ***********************************/

// ERRORS
const char* _e_invtype="ERROR: Invalid DS type";
const char* _e_mem    ="ERROR: Could not allocate more memory";
const char* _e_nsense ="ERROR: Nonsensical action for given structure type";
const char* _e_null   ="ERROR: the DS pointer is NULL";
const char* _e_nimp   ="ERROR: that feature is not implemented";
const char* _e_over   ="ERROR: Overflow";


/******************************************************************************/
//                       PRIVATE FUNCTION DEFINITIONS
/******************************************************************************/


// report an error
inline static void _error(const char * message){
	msg_print(NULL, V_ERROR, "data.h: %s\n", message);
}

inline static _node_pt _new_node(const DS const root){
	_node_pt new_node; // will return null on failure
	
	switch(root->type){
	case DS_list         :
	case DS_circular_list:
	case DS_hash         :
		if (root->freelist.l){ // check the freelist first
			new_node = root->freelist;
			root->freelist.l = root->freelist.l->prev;
		}
		else {
			new_node.l=(_lnode_pt) malloc(sizeof(struct _list_node)+root->data_size);
			if (!new_node.l) _error(_e_mem);
		}
		
		// Make sure it's clean for the next use
		new_node.l->next = NULL;
		new_node.l->prev = NULL;
		break;
	
	case DS_heap:
	case DS_bst:
		if (root->freelist.t){
			new_node = root->freelist;
			root->freelist.t = root->freelist.t->left;
		}
		else{
			new_node.t=(_tnode_pt) malloc(sizeof(struct _tree_node)+root->data_size);
			if (!new_node.t) _error(_e_mem);
		}
		
		// Make sure it's clean for the next use
		new_node.t->left = NULL;
		new_node.t->right = NULL;
		new_node.t->parent = NULL;
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


/******************************************************************************/
//                       PUBLIC FUNCTION DEFINITIONS
/******************************************************************************/


/****************************** NEW STRUCTURE *********************************/

DS DS_new_list(size_t data_size){
	DS new_structure;
	
	// Allocate space
	new_structure= (DS) calloc(1, sizeof(struct _root));
	if (new_structure == NULL) {
		_error(_e_mem);
		return NULL;
	}
	
	new_structure->type       = DS_list;
	new_structure->data_size  = data_size;
	new_structure->count      = 0        ;
	
	return new_structure;
}


DS DS_new_circular(size_t data_size){
	DS new_structure;
	
	// Allocate space
	new_structure= (DS) calloc(1, sizeof(struct _root));
	if (new_structure == NULL) {
		_error(_e_mem);
		return NULL;
	}
	
	new_structure->type       = DS_circular_list;
	new_structure->data_size  = data_size;
	new_structure->count      = 0        ;
	
	return new_structure;
}


DS DS_new_bst(
	size_t       data_size,
	bool         duplicates_allowed,
	const void * (*key)(const void * data),
	imax         (*cmp_keys)(const void * left , const void * right)
){
	DS new_structure;
	
	if (!key || !cmp_keys){
		_error(_e_nsense);
		return NULL;
	}
	
	// Allocate space
	new_structure= (DS) calloc(1, sizeof(struct _root));
	if (new_structure == NULL) {
		_error(_e_mem);
		return NULL;
	}
	
	new_structure->keys.key = key;
	new_structure->cmp_keys = cmp_keys;
	
	new_structure->type       = DS_bst;
	new_structure->data_size  = data_size;
	new_structure->count      = 0        ;
	new_structure->dups       = duplicates_allowed;
	
	return new_structure;
}

DS DS_new_heap(
	size_t data_size,
	imax    (*cmp_data)(const void * left , const void * right)
){
	DS new_structure;
	
	if (!cmp_data){
		_error(_e_nsense);
		return NULL;
	}
	
	// Allocate space
	new_structure= (DS) calloc(1, sizeof(struct _root));
	if (new_structure == NULL) {
		_error(_e_mem);
		return NULL;
	}
	
	new_structure->type      = DS_heap  ;
	new_structure->data_size = data_size;
	new_structure->count     = 0        ;
	new_structure->cmp_keys  = cmp_data ;
	
	return new_structure;
}



DS DS_new_hash(
	size_t   data_size,
	size_t   table_size,
	bool     duplicates_allowed,
	uint64_t (*hash_func)(const void * data)
){
	DS new_structure;
	
	if (!hash_func){
		_error(_e_nsense);
		return NULL;
	}
	
	// Allocate space
	new_structure= (DS) calloc(1, sizeof(struct _root));
	if (new_structure == NULL) {
		_error(_e_mem);
		return NULL;
	}
	
	new_structure->type      = DS_hash           ;
	new_structure->data_size = data_size         ;
	new_structure->count     = 0                 ;
	new_structure->dups      = duplicates_allowed;
	new_structure->keys.hash = hash_func         ;
	
	if(!table_size) new_structure->table_size = DS_DEFAULT_TABLE_SZ;
	new_structure->table_size = table_size;
	
	return new_structure;
}

/********************** ACTIONS ON WHOLE DATA STRUCTURE ***********************/

inline void DS_delete(DS root){
	DS_empty(root);
	DS_flush(root); // clear the freelist
	free    (root);
}

inline void DS_empty (DS root){
	while (DS_remove(root));
}

inline void DS_flush (DS root){
	_node_pt dead_node;
	
	switch (root->type){
	case DS_list:
	case DS_circular_list:
		while (root->freelist.l) {
		dead_node = root->freelist;
		root->freelist.l = root->freelist.l->prev;
		free(dead_node.l);
		}
		return;
	
	case DS_heap:
	case DS_bst:
		while (root->freelist.t) {
		dead_node = root->freelist;
		root->freelist.t = root->freelist.t->left;
		free(dead_node.t);
		}
		return;
	
	case DS_hash: _error(_e_nimp); return;
	
	default: _error(_e_invtype); return;
	}
}



// Simple tests
inline uint DS_count(const DS root) {
	if (!root){
		_error(_e_null);
		return 0;
	}
	return root->count;
}

inline bool DS_isempty (const DS root) {
	if (!root){
		_error(_e_null);
		return true;
	}
	return !root->count;
}

bool DS_isleaf (const DS root){
	if (!root){
		_error(_e_null);
		return false;
	}

	switch (root->type){
	case DS_bst          : break;
	case DS_heap         :
	case DS_hash         :
	case DS_list         :
	case DS_circular_list: _error(_e_nsense); return false;
	default: _error(_e_invtype); return false;
	}
	
	return (!( root->current.t->left || root->current.t->right ));
}

// Dump the contents of the data structure
void DS_dump (const DS root){
	_node_pt this_node;
	
	if (!root){
		_error(_e_null);
		return;
	}
	
	this_node = root->head;
	
	switch (root->type){
	case DS_list:
		while(this_node.l != NULL) {
			printf("%s\n", (char*) this_node.l->data);
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
	
	case DS_heap:
	case DS_bst :
		if (this_node.t == NULL) break;
		_print_node(root->head.t, 0);
		break;
	
	case DS_hash: _error(_e_nimp); return;
	
	default:
		_error(_e_invtype);
	}
}

/**************************** ADD TO DATA STRUCTURE ***************************/

void * DS_insert (DS root, const void * data){
	_node_pt    new_node;
	_tnode_pt * position;
	imax        result;
	
	if (!root){
		_error(_e_null);
		return NULL;
	}
	
	switch (root->type){
	case DS_list:
		if (root->current.l == root->head.l)
			return DS_insert_first(root, data);
		else if (root->current.l == root->tail.l)
			return DS_insert_last (root, data);
		
		new_node = _new_node(root);
		if (new_node.l == NULL) return NULL;
		
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
		
		root->current=new_node;
		root->count++;
		
		memcpy(new_node.l->data, data, root->data_size);
		return new_node.l->data;
	
	
	case DS_circular_list:
		new_node = _new_node(root);
		if (new_node.l == NULL) return NULL;
		
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
		
		root->current=new_node;
		root->count++;
		
		memcpy(new_node.l->data, data, root->data_size);
		return new_node.l->data;
	
	
	case DS_bst:
		// Find the position
		position = &(root->head.t);
		while (*position){
			root->current.t = *position;
			result=root->cmp_keys(
				root->keys.key(data),
				root->keys.key(root->current.t->data)
			);
			if      (result <0) position = &(root->current.t->left);
			else if (result >0) position = &(root->current.t->right);
			else { // result is 0
				if(root->dups) position = &(root->current.t->right);
				else {
					//_error(_e_repeat);
					return NULL;
				}
			}
		}
		
		// position now points to a null _tnode_pt where the new node will go
		
		// allocate the node
		new_node = _new_node(root);
		if (new_node.t == NULL) return NULL;
		
		// insert the node
		new_node.t->parent = root->current.t;
		*position = new_node.t;
		root->current=new_node;
		root->count++;
		
		// copy data and return it
		memcpy(new_node.t->data, data, root->data_size);
		return new_node.t->data;
	
	case DS_heap:
		// allocate the node
		new_node = _new_node(root);
		if (new_node.t == NULL) return NULL;
		
		// insert node and set root->current
		
		// relocate root->tail
		
		// copy data
		
		// relocate root->current
		
		// the next position is one of the children of root->tail
		//if(!root->tail)
		
		
		
	case DS_hash: _error(_e_nimp); return NULL;
	
	default: _error(_e_invtype); return NULL;
	}
}

void * DS_insert_first(DS root, const void * data){
	_node_pt new_node;
	
	if (!root){
		_error(_e_null);
		return NULL;
	}
	
	switch (root->type){
	case DS_list         : break;
	case DS_hash         :
	case DS_circular_list:
	case DS_heap         :
	case DS_bst          : _error(_e_nsense ); return NULL;
	default              : _error(_e_invtype); return NULL;
	}
	
	// allocate the node
	new_node = _new_node(root);
	if (new_node.l == NULL) return NULL;
	
	// if the structure is empty
	if (root->head.l == NULL) root->tail=new_node;
	else{
		new_node.l->next = root->head.l;
		root->head.l->prev = new_node.l;
	}
	root->current=new_node;
	root->head   =new_node;
	root->count++;
	
	// assign data
	memcpy(new_node.l->data, data, root->data_size);
	
	return new_node.l->data;
}

void * DS_insert_last (DS root, const void * data){
	_node_pt new_node;
	
	if (!root){
		_error(_e_null);
		return NULL;
	}
	
	switch (root->type){
	case DS_list         : break;
	case DS_hash         :
	case DS_circular_list:
	case DS_heap         :
	case DS_bst          : _error(_e_nsense ); return NULL;
	default              : _error(_e_invtype); return NULL;
	}
	
	// allocate the node
	new_node = _new_node(root);
	if (new_node.l == NULL) return NULL;
	
	// if the structure is empty
	if (root->head.l == NULL) root->head=new_node;
	else{
		new_node.l->prev = root->tail.l;
		root->tail.l->next = new_node.l;
	}
	
	root->current=new_node;
	root->tail   =new_node;
	root->count++;
	
	// assign data
	memcpy(new_node.l->data, data, root->data_size);
	
	return new_node.l->data;
}

/*********************** REMOVE FROM DATA STRUCTURE ***************************/

const void * DS_remove(DS root){
	const void * data;
	_tnode_pt swapnode=NULL;
	_tnode_pt * parent_pt;
	
	if (!root){
		_error(_e_null);
		return NULL;
	}
	
	if (root->current.l == NULL) return NULL;
	
	//data = root->current.l->data;
	
	switch (root->type){
	case DS_bst:
		// save the data
		data = root->current.t->data;
		
		// get the parent's pointer pointer
		if(!root->current.t->parent) parent_pt = &root->head.t;
		else if(root->current.t->parent->left == root->current.t)
			parent_pt = &root->current.t->parent->left;
		else parent_pt = &root->current.t->parent->right;
		
		
		// No Right Child
		if(!root->current.t->right){
			*parent_pt = root->current.t->left;
			if(*parent_pt) (*parent_pt)->parent = root->current.t->parent;
		}
		// Only Right Child
		else if(!root->current.t->left){
			*parent_pt = root->current.t->right;
			(*parent_pt)->parent = root->current.t->parent;
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
		root->current.t->left = root->freelist.t;
		root->freelist = root->current;
		
		// Reset current to the head
		root->current = root->head;
		break;
		
	case DS_list:
		// save the data
		data = root->current.l->data;
		
		// Check if we are at the beginning or end
		if (root->current.l == root->head.l)
			return DS_remove_first(root);
		else if (root->current.l == root->tail.l)
			return DS_remove_last (root);
	
	case DS_circular_list:
		// save the data
		data = root->current.l->data;
		
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
	
	case DS_heap:
	case DS_hash: _error(_e_nimp); return NULL;
	
	default: _error(_e_invtype); return NULL;
	}
	
	if (!root->head.l){
		root->current.l = NULL;
		root->tail.l    = NULL;
	}
	
	root->count--;
	return data;
}


const void * DS_remove_first(DS root){
	const void * data;
	_tnode_pt * parent_pt;
	
	if (!root){
		_error(_e_null);
		return NULL;
	}
	
	if (root->current.l == NULL) return NULL;
	
	switch (root->type){
	case DS_list:
		root->current = root->head;
		data = root->current.l->data;
		
		// remove from structure
		root->head.l = root->head.l->next;
		if (root->head.l) root->head.l->prev = NULL;
		
		// move to freelist
		root->current.l->prev = root->freelist.l;
		root->freelist = root->current;
		
		// set current
		root->current = root->head;
		if (!root->head.l) root->tail.l = NULL;
		
		break;
	
	case DS_bst:
		parent_pt = &root->head.t;
		
		while((*parent_pt)->left) parent_pt = &(*parent_pt)->left;
		root->current.t = *parent_pt;
		
		data = root->current.t->data;
		
		// remove from tree
		*parent_pt = root->current.t->right;
		if (*parent_pt) (*parent_pt)->parent = root->current.t->parent;
		
		// move to freelist
		root->current.t->left = root->freelist.t;
		root->freelist = root->current;
		
		// set current to next in-order node
		root->current = root->head;
		if (root->current.t) // may have been the last node
			while (root->current.t->left)
				root->current.t = root->current.t->left;
		
		break;
	
	case DS_heap         :
	case DS_hash         :
	case DS_circular_list: _error(_e_nsense ); return NULL;
	default              : _error(_e_invtype); return NULL;
	}
	
	root->count--;
	return data;
}


const void * DS_remove_last (DS root){
	const void * data;
	_tnode_pt * parent_pt;
	
	if (!root){
		_error(_e_null);
		return NULL;
	}
	
	if (root->current.l == NULL) return NULL;
	
	switch (root->type){
	case DS_list:
		root->current = root->tail;
		data = root->current.l->data;
		
		// remove from structure
		root->tail.l = root->tail.l->prev;
		if (root->tail.l) root->tail.l->next = NULL;
		
		// move to freelist
		root->current.l->prev = root->freelist.l;
		root->freelist = root->current;
		
		// set current
		root->current = root->tail;
		if (!root->tail.l) root->head.l = NULL;
		break;
	
	case DS_bst:
		parent_pt = &root->head.t;
		
		while((*parent_pt)->right) parent_pt = &(*parent_pt)->right;
		root->current.t = *parent_pt;
		
		data = root->current.t->data;
		
		// remove from tree
		*parent_pt = root->current.t->left;
		if (*parent_pt) (*parent_pt)->parent = root->current.t->parent;
		
		// move to freelist
		root->current.t->left = root->freelist.t;
		root->freelist = root->current;
		
		// set current
		root->current = root->head;
		if (root->current.t) // may have been the last node
			while (root->current.t->right)
				root->current.t = root->current.t->right;
		break;
	
	case DS_heap         :
	case DS_hash         :
	case DS_circular_list: _error(_e_nsense ); return NULL;
	default              : _error(_e_invtype); return NULL;
	}
	
	
	root->count--;
	return data;
}

/********************** VIEW RECORD IN DATA STRUCTURE *************************/

void * DS_find(const DS root, const void * key){
	_tnode_pt node;
	imax result;
	
	if (!root){
		_error(_e_null);
		return NULL;
	}
	
	if (root->current.l == NULL) return NULL;
	
	switch (root->type){
	case DS_bst: break;
	case DS_hash: _error(_e_nimp); return NULL;
	
	case DS_heap         :
	case DS_list         :
	case DS_circular_list: _error(_e_nsense); return NULL;
	default: _error(_e_invtype); return NULL;
	}
	
	node = root->head.t;
	while (node != NULL){
		result=root->cmp_keys(key, root->keys.key(node->data));
		
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
void * DS_first(const DS root){ // visit the first node
	if (!root){
		_error(_e_null);
		return NULL;
	}
	
	if (root->current.l == NULL) return NULL;
	
	switch (root->type){
	case DS_bst:
		root->current=root->head;
		if (root->current.t == NULL) return NULL;
		while(root->current.t->left != NULL)
			root->current.t=root->current.t->left;
		return root->current.t->data;
	
	case DS_heap:
	case DS_list:
		root->current=root->head;
		return root->current.l->data;
	
	
	case DS_hash         :
	case DS_circular_list: _error(_e_nsense ); return NULL;
	default              : _error(_e_invtype); return NULL;
	}
}

void * DS_last(const DS root){ // visit the last node
	if (!root){
		_error(_e_null);
		return NULL;
	}
	
	if (root->current.l == NULL) return NULL;
	
	switch (root->type){
	case DS_bst:
		root->current=root->head;
		while(root->current.t->right)
			root->current.t=root->current.t->right;
		return root->current.t->data;
	
	case DS_list:
		root->current=root->tail;
		return root->current.l->data;
	
	case DS_heap         :
	case DS_hash         :
	case DS_circular_list: _error(_e_nsense ); return NULL;
	default              : _error(_e_invtype); return NULL;
	}
}

void * DS_next(const DS root){ // visit the next in-order node
	if (!root){
		_error(_e_null);
		return NULL;
	}
	
	if (root->current.l == NULL) return NULL;
	
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
		if(!root->current.l) {
			root->current=root->tail;
			return NULL;
		}
		else return root->current.l->data;
	
	case DS_heap:
	case DS_hash: _error(_e_nsense); return NULL;
	default     : _error(_e_invtype); return NULL;
	}
}

void * DS_previous(const DS root){ // visit the previous in-order node
	if (!root){
		_error(_e_null);
		return NULL;
	}
	
	if (root->current.l == NULL) return NULL;
	
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
		if(!root->current.l){
			root->current = root->tail;
			return NULL;
		}
		else return root->current.l->data;
	
	case DS_heap:
	case DS_hash: _error(_e_nsense); return NULL;
	default     : _error(_e_invtype); return NULL;
	}
}

void * DS_current (DS root){ // visit the current node
	if (!root){
		_error(_e_null);
		return NULL;
	}
	
	if (root->current.l == NULL) return NULL;
	
	switch (root->type){
	case DS_bst          : return root->current.t->data;
	case DS_hash         :
	case DS_list         :
	case DS_circular_list: return root->current.l->data;
	case DS_heap         : _error(_e_nsense); return NULL;
	default              : _error(_e_invtype); return NULL;
	}
}

// set the current position to a specific count from the beginning
void * DS_position(const DS root, const unsigned int position){
	if (!root){
		_error(_e_null);
		return NULL;
	}
	
	if (root->current.l == NULL) return NULL;
	
	switch (root->type){
	case DS_list         : break;
	case DS_bst          :
	case DS_heap         :
	case DS_hash         :
	case DS_circular_list: _error(_e_nsense); return NULL;
	default              : _error(_e_invtype); return NULL;
	}
	
	root->current=root->head;
	for (uint i=1; i != position; i++) root->current.l = root->current.l->next;
	
	return root->current.l->data;
}


/******************************************************************************/
//                              ARRAY UTILITIES
/******************************************************************************/


#define _at(A) ((uint8_t*)buffer+(A*size))

#define CACHE_SZ ((size_t)1<<15)

void DS_memswap(void *a, void *b, size_t size){
	uint64_t temp;
	
	if(size & 1){
		temp = ((uint8_t*)a)[size-1];
		((uint8_t*)a)[size-1] = ((uint8_t*)b)[size-1];
		((uint8_t*)b)[size-1] = (uint8_t)temp;
	}
	size>>=1;
	
	if(size & 1){
		temp = ((uint16_t*)a)[size-1];
		((uint16_t*)a)[size-1] = ((uint16_t*)b)[size-1];
		((uint16_t*)b)[size-1] = (uint16_t)temp;
	}
	size>>=1;
	
	if(size & 1){
		temp = ((uint32_t*)a)[size-1];
		((uint32_t*)a)[size-1] = ((uint32_t*)b)[size-1];
		((uint32_t*)b)[size-1] = (uint32_t)temp;
	}
	size>>=1;
	
	while(size){
		size--;
		temp = ((uint64_t*)a)[size-1];
		((uint64_t*)a)[size-1] = ((uint64_t*)b)[size-1];
		((uint64_t*)b)[size-1] = temp;
	}
	
}

/*static inline void _merge(*/
/*	void *buffer,*/
/*	size_t idx_l,*/
/*	size_t idx_r,*/
/*	size_t idx_end,*/
/*	size_t size,*/
/*	int (*compare)(const void *left, const void *right)*/
/*){*/
/*	while(){*/
/*		int r=compare(_at(idx_l), _at(idx_r));*/
/*		*/
/*		if(r<=0) idx_l++;*/
/*		else */
/*	}*/
/*}*/

/*static inline void _insertion_sort(*/
/*	void *buffer,*/
/*	size_t count,*/
/*	size_t size,*/
/*	int (*compare)(const void *left, const void *right)*/
/*){*/
/*	size_t i, j;*/
/*	*/
/*	// efficient for small data sizes and nearly sorted lists*/
/*	for(i = 1; i<count; i++){*/
/*		if(compare(_at(i-1), _at(i))>0){*/
/*			j=i-1;*/
/*			do DS_memswap(_at(j), _at(j+1), size);*/
/*			while(j-- && compare(_at(j), _at(j+1))>0);*/
/*		}*/
/*	}*/
/*}*/


/*void DS_sort(*/
/*	void *buffer,*/
/*	size_t count,*/
/*	size_t size,*/
/*	int (*compare)(const void *left, const void *right)*/
/*){*/
/*	size_t total_sz = count*size;*/
/*	size_t run_count = CACHE_SZ / size;*/
/*	size_t full_runs = count / run_count;*/
/*	size_t i;*/
/*	*/
/*	*/
/*	// first sort cache sized runs. I could multithread this part*/
/*	for(i=0; i<full_runs; i++)*/
/*		_insertion_sort(_at(i*run_count), run_count, size, compare);*/
/*	*/
/*	// sort the stragglers*/
/*	_insertion_sort(_at(i*run_count), count-i*run_count, size, compare);*/
/*	*/
/*	while(run_count < count){*/
/*		_merge*/
/*	}*/
/*	*/
/*}*/

/***************************** ARRAY BASED HEAPS ******************************/

#define _heap_left(A) (A*2+1)
#define _heap_right(A) (A*2+2)
#define _heap_parent(A) ((A-1)/2)

inline static void _sift_down(
	const void   *buffer,
	      size_t start, // index of the element to sift down, <stop
	const size_t stop,  // how far to sift down
	const size_t size,
	int    (*compare)(const void *left, const void *right)
){
	size_t smallest, left;
	
	while(_heap_left(start) <= stop){
		left = _heap_left(start);
		smallest = start;
		
		if( compare(_at(smallest),_at(left)) > 0 ) // if left child is smaller
			smallest = left;
		
		if(left+1 <= stop && compare(_at(smallest),_at(left+1))>0)
			smallest=left+1;
		
		if(smallest == start) return; // both children are in order
		else DS_memswap(_at(start), _at(smallest), size);
		start = smallest;
	}
	
}

inline static void _sift_up(
	const void   *buffer,
	const size_t stop,    // how far to sift up, <element
	      size_t element, // index of the element to sift up
	const size_t size,
	int    (*compare)(const void *left, const void *right)
){
	size_t parent;
	
	parent = _heap_parent(element);
	while(element >= stop && compare(_at(parent), _at(element)) >0){
		DS_memswap(_at(parent), _at(element), size);
		element = parent;
		parent = _heap_parent(element);
	}
}


void DS_heapify(
	void   *buffer,
	size_t count,
	size_t size,
	int    (*compare)(const void *left, const void *right)
){
	size_t start = _heap_parent(count-1);
	
	do{
		_sift_down(buffer, start, count-1, size, compare);
	} while(start-- != 0);
	
}


