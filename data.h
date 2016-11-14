/*******************************************************************************
 *
 *	lib-util : A Utility Library
 *
 *	Copyright (c) 2016 Ammon Dodson
 *	You should have received a copy of the licence terms with this software. If
 *	not, please visit the project homepage at:
 *	https://github.com/ammon0/lib-util
 *
 *******************************************************************************
 *
 *	data.h : A generic data structures library for C development
 *
 *	Provides linked lists and binary search trees
 *	Each data structure keeps track of the current working position, and most of
 *	the function calls operate at, or modify the current position.
 *
 *	Data structure types include:
 *		list: a general list that is also used to implement stacks and queues
 *		circular list
 *		sorted list: not recommended as the BST should be better in every way
 *		binary search tree
 *
 *		The caller's data is stored in a generic fixed length byte array. The
 *	size of the arrays is set by the caller when the data structure is
 *	initialized. All incoming data is copied into the data structure. The caller
 *	does not need to manage data structure memory.
 *		DS_remove() functions return a pointer to the removed data. The caller
 *	must copy that data out as there is no guarantee that it will be preserved
 *	after the next call to the data structure.
 *		Data accesed through any of the various visiting functions may be
 *	directly modified through the returned pointer. Obviously, do not modify the
 *	sort key in a sorted data structure as it will result in unpredictable
 *	behaviour and possible data loss.
 *
 **************** ACCEPTABLE ACTIONS FOR EACH STRUCTURE TYPE *******************
 *
 *	List
 *		DS_count()
 *		DS_isempty()
 *		DS_dump()
 *		DS_insert()
 *		DS_insert_first()
 *		DS_insert_last()
 *		DS_nq()
 *		DS_push()
 *		DS_remove()
 *		DS_remove_first()
 *		DS_remove_last()
 *		DS_pop()
 *		DS_dq()
 *		DS_set_data()
 *		DS_first()
 *		DS_last()
 *		DS_next()
 *		DS_previous()
 *		DS_current()
 *		DS_position()
 *
 *	Circular List
 *		DS_count()
 *		DS_isempty()
 *		DS_dump()
 *		DS_insert()
 *		DS_remove()
 *		DS_set_data()
 *		DS_next()
 *		DS_previous()
 *		DS_current()
 *
 *	Binary Search Tree
 *		DS_count()
 *		DS_isempty()
 *		DS_dump()
 *		DS_sort()
 *		DS_remove()
 *		DS_remove_first()
 *		DS_remove_last()
 *		DS_set_data()
 *		DS_find()
 *		DS_first()
 *		DS_last()
 *		DS_next()
 *		DS_previous()
 *		DS_current()
 *
 *
 *	Individual functions are documented below. All functions report errors on
 *	stderr.
 *
 ******************************************************************************/


#ifndef _DATA_H_INCLUDE
#define _DATA_H_INCLUDE

#include <stdbool.h>

// All data structures are represented in the caller's code as type DS
typedef struct _root* DS;

// These are types of data structures availible
typedef enum {DS_list, DS_circular_list, DS_bst } DS_type;


/******************************************************************************/
//                     COMMANDS FOR ALL DATA STRUCTURES
/******************************************************************************/


DS DS_new(
	DS_type type,
	size_t  data_size,
	bool    duplicates,
	int     (*cmp_data)(const void * left, const void * right),
	int     (*cmp_key )(const void * key , const void * data)
);
/**	CREATE A NEW DATA STRUCTURE
 *
 *	Parameters
 *	type      : REQUIRED
 *		Must be one of the enumerated DS_type
 *	data_size : REQUIRED
 *		The size in bytes of each data unit being stored in this
 *		structure.
 *	duplicates: IGNORED UNLESS BST
 *		Used for sorted structures and indicates whether duplicated data is
 *		allowed.
 *	cmp_data  : IGNORED UNLESS BST
 *		comparator is used in sorted structures. It must return <0 if left is
 *		ordered before right, >0 if left is ordered after right, and 0 if they
 *		are the same.
 *	cmp_key   : IGNORED UNLESS BST
 *
 *	Results
 *		Returns NULL on failure
 */

// Delete the entire contents of a data structure and free its memory
void DS_delete(DS root);

void DS_flush (DS root);
/**	Flushes unecessary cached memory. Recently used memory may be kept allocated
 *	for performance purposes. This command frees that cached memory.
 */


// return the number of nodes in the structure
unsigned int DS_count  (const DS root);
bool         DS_isempty(const DS root); // is the structure empty
bool         DS_isleaf (const DS root); // is the current position a leaf
void         DS_dump   (const DS root);
/**	Dump the entire contents of the data structure to the console
 *	the stored data is not changed. Used for debugging.
 */


/******************************************************************************/
//                           ADD TO DATA STRUCTURE
/******************************************************************************/


// All return 0 on success

#define DS_nq(A,B)   DS_insert_first(A,B)
#define DS_push(A,B) DS_insert_first(A,B)

// TODO: these should return a pointer to the data in the structure

// Inserts a new node at the "current" position
bool DS_insert      (DS root, const void * data);
bool DS_insert_first(DS root, const void * data);
bool DS_insert_last (DS root, const void * data);

bool DS_sort(DS root, const void * data);
/**	insert node in to the structure in sort order
 *	the third value must be a \0 terminated string used for sorting
 *	duplicate indexes are not allowed
 */


/******************************************************************************/
//                       REMOVE FROM DATA STRUCTURE
/******************************************************************************/


// All return NULL on failure

#define DS_pop(A) DS_remove_first(A)
#define DS_dq(A)  DS_remove_last(A)

// Removes the node at the "current" position and returns its data
const void * DS_remove      (DS root);
const void * DS_remove_first(DS root);
const void * DS_remove_last (DS root);


/******************************************************************************/
//                          MODIFY DATA IN STRUCTURE
/******************************************************************************/


// set the data at the current position. returns 0 on success
//bool DS_set_data(const DS, const void * data);


/******************************************************************************/
//                       VISIT RECORD IN DATA STRUCTURE
/******************************************************************************/


// All return NULL on failure

void * DS_find(const DS root, const void * key);
/**	search for a node by index in a sorted link list and return a pointer to
 *	its contents
 *	does not change contents of data structure
 */

void * DS_first    (      DS root); // visit the first node
void * DS_last     (      DS root); // visit the last node
void * DS_next     (      DS root); // visit the next in-order node
void * DS_previous (      DS root); // visit the previous in-order node
void * DS_current  (const DS root); // visit the current node
void * DS_position (const DS root, const unsigned int);
// set the current position to a specific count from the beginning


#endif // _DATA_H_INCLUDE


