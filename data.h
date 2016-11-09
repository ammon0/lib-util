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
 *	Individual functions are documented below.
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
	bool duplicates,
	int (*comparator)(const void* left, const void* right)
);
/**	create a new data structure
 *
 *	the duplicate parameter is used for sorted structures and indicates whether
 *	duplicated data is allowed
 *
 *	comparator is used in sorted structures. It must return <0 if left is
 *	ordered before right, >0 if left is ordered after right, and 0 if they are
 *	the same.
 *
 *	Returns NULL on failure
 */

// return the number of nodes in the structure
unsigned int DS_count  (const DS const root); 
bool         DS_isempty(const DS const root); // is the structure empty
bool         DS_isleaf (const DS const root); // is the current position a leaf
void         DS_dump   (const DS const root);
/**	Dump the entire contents of the data structure to the console
 *	the stored data is not changed
 */


/******************************************************************************/
//                           ADD TO DATA STRUCTURE
/******************************************************************************/


// Inserts a new node at the "current" position
bool DS_insert      (const DS const root, void const * const data);
bool DS_insert_first(const DS const root, void const * const data);
bool DS_insert_last (const DS const root, void const * const data);

#define DS_nq(A,B)   DS_insert_first(A,B)
#define DS_push(A,B) DS_insert_first(A,B)

bool DS_sort(
	const DS     const root,
	const void * const data
);
/**	insert node in to the structure in sort order
 *	the third value must be a \0 terminated string used for sorting
 *	duplicate indexes are not allowed
 */


/******************************************************************************/
//                       REMOVE FROM DATA STRUCTURE
/******************************************************************************/


// Removes the node at the "current" position and returns its data
void * DS_remove      (DS const root);
void * DS_remove_first(DS const root);
void * DS_remove_last (DS const root);

#define DS_pop(A) DS_remove_first(A)
#define DS_dq(A)  DS_remove_last(A)


/******************************************************************************/
//                          MODIFY DATA IN STRUCTURE
/******************************************************************************/


// set the data at the current position
void DS_set_data(DS, void * data);


/******************************************************************************/
//                       VIEW RECORD IN DATA STRUCTURE
/******************************************************************************/


void* DS_find(const DS root, const void * const key);
/**	search for a node by index in a sorted link list and return a pointer to
 *	its contents
 *	does not change contents of data structure
 */

// VISITING
void * DS_first    (      DS const root); // visit the first node
void * DS_last     (      DS const root); // visit the last node
void * DS_next     (      DS const root); // visit the next in-order node
void * DS_previous (      DS const root); // visit the previous in-order node
void * DS_current  (const DS const root); // visit the current node
void * DS_position (const DS const root, const unsigned int);
// set the current position to a specific count from the beginning


#endif // _DATA_H_INCLUDE


