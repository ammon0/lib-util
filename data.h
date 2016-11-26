/*******************************************************************************
 *
 *	lib-util : A Utility Library
 *
 *	Copyright (c) 2016 Ammon Dodson
 *	You should have received a copy of the licence terms with this software. If
 *	not, please visit the project homepage at:
 *	https://github.com/ammon0/lib-util
 *
 ******************************************************************************/

/** @file data.h
 *
 *	A generic data structures library for C development
 *
 *	##The Current Position
 *	An important concept for each of these structures is the **current
 *	position**. When data is added to the structure the current position will be
 *	set to that new data. The current position can be changed with the various
 *	traversal functions. Functions such as DS_remove() and DS_insert()
 *	implicitly act on the current position.
 *
 *	##Structure Types Availible
 *	Data structure types include:
 *	*	list: a general list that is also used to implement stacks and queues
 *		circular list
 *	*	sorted list: not recommended as the BST should be better in every way
 *	*	binary search tree
 *
 *	###Stacks and Queues
 *	Stacks and queues use the DS_list type structure. The caller must limit
 *	themselves to only using DS_push() & DS_pop() for stacks, and DS_nq() &
 *	DS_dq() for queues.
 *
 *	## Data Storage Method
 *	The caller's data is stored in a generic fixed length byte array. The
 *	size of the arrays is set by the caller in DS_new() when the data structure
 *	is initialized. All incoming data is copied into the data structure from the
 *	pointer location passed by the insert functions.
 *
 *	If the structure's data needs to be variable length then the caller will
 *	have to store pointers in the data structure and manage the memory for the
 *	variable data.
 *
 *	## Removal
 *	DS_remove() functions return a pointer to the removed data. The caller
 *	must copy that data out as there is no guarantee that it will be preserved
 *	after the next call to the data structure.
 *
 *	## Visiting & Modifications
 *	Data accesed through any of the various visiting functions may be
 *	directly modified through the returned pointer. Obviously, do not modify the
 *	sort key in a sorted data structure as it will result in unpredictable
 *	behaviour and possible data loss.
 *
 *	## Errors
 *	Errors are reported on stderr.
 *
 *	## Acceptable Actions For Each Structure Type ##
 *
 *	### All Structures
 *	*	DS_new()
 *	*	DS_flush()
 *	*	DS_delete()
 *	*	DS_count()
 *	*	DS_isempty()
 *	*	DS_dump()
 *
 *	### Lists
 *	*	DS_insert()
 *	*	DS_insert_first()
 *	*	DS_insert_last()
 *	*	DS_remove()
 *	*	DS_remove_first()
 *	*	DS_remove_last()
 *	*	DS_first()
 *	*	DS_last()
 *	*	DS_next()
 *	*	DS_previous()
 *	*	DS_current()
 *	*	DS_position()
 *	#### Stacks Only
 *	*	DS_push()
 *	*	DS_pop()
 *	#### Queues Only
 *	*	DS_nq()
 *	*	DS_dq()
 *
 *	### Circular Lists
 *	*	DS_insert()
 *	*	DS_remove()
 *	*	DS_next()
 *	*	DS_previous()
 *	*	DS_current()
 *
 *	### Binary Search Trees
 *	*	DS_isleaf()
 *	*	DS_insert()
 *	*	DS_remove()
 *	*	DS_remove_first()
 *	*	DS_remove_last()
 *	*	DS_find()
 *	*	DS_first()
 *	*	DS_last()
 *	*	DS_next()
 *	*	DS_previous()
 *	*	DS_current()
 *
 ******************************************************************************/


#ifndef _DATA_H_INCLUDE
#define _DATA_H_INCLUDE

#include <stdbool.h>

/// All data structures are represented in the caller's code as type DS
typedef struct _root* DS;

/// These are types of data structures availible
typedef enum {DS_list, DS_circular_list, DS_bst } DS_type;


/******************************************************************************/
//                     COMMANDS FOR ALL DATA STRUCTURES
/******************************************************************************/


/**	CREATE A NEW DATA STRUCTURE
 *
 *	Not all parameters are meaningful for all structure types. `duplicates`,
 *	`cmp_data`, and `cmp_key` are only used by the BST type. They
 *	are ignored for other types.
 *
 *	##Parameters
 *	### Requried
 *	* type: Must be one of the enumerated DS_type
 *	* data_size: The size in bytes of the data being stored in this structure.
 *		If you need to store variable length data you should store pointers in
 *		the data structure.
 *
 *	###Sorted Structures Only: BST
 *	* duplicates: Indicates whether duplicate  keys are allowed.
 *	* cmp_data: Is used when inserting data. data.h will pass two data entries
 *		to this function. It must return <0 if left is ordered before right, >0
 *		if left is ordered after right, and 0 if they are the same.
 *	* cmp_key: Similar to cmp_data() except that the first entry will be just a
 *		key.
 *
 *	##Results:
 *	Returns NULL on failure
 */
DS DS_new(
	DS_type type,
	size_t  data_size,
	bool    duplicates,
	int     (*cmp_data)(const void * left, const void * right),
	int     (*cmp_key )(const void * key , const void * data)
);


/** Delete the entire contents of a data structure and free its memory.
 */
void DS_delete(DS root);

/**	Flushes cached memory.
 *	Removing nodes from the structure does not immediately release the occupied
 *	memory. This memory is cached for quick reuse. This command frees that
 *	cached memory.
 */
void DS_flush (DS root);


/** Return the number of nodes in the structure.
 */
unsigned int DS_count  (const DS root);
bool         DS_isempty(const DS root); ///< is the structure empty
bool         DS_isleaf (const DS root); ///< is the current position a leaf

/**	Dump the entire contents of the data structure to the console.
 *	the stored data is not changed. Used for debugging.
 */
void         DS_dump   (const DS root);


/******************************************************************************/
//                           ADD TO DATA STRUCTURE
/******************************************************************************/


// All return 0 on success

/** Enqueue data.
 *	*	Used on a DS_list to implement a queue.
 *	*	If your data structure is a queue use only DS_nq() and DS_dq()
 */
#define DS_nq(A,B)   DS_insert_last(A,B)

/** Push data onto a stack.
 *	*	Used on a DS_list to implement a stack.
 *	*	If your data structure is a stack use only DS_push() and DS_pop()
 */
#define DS_push(A,B) DS_insert_first(A,B)

/**	Insert data into the structure
 *	
 *	For sorted structures DS_insert() will insert the data in sort order. For
 *	other structures the data will be inserted at the *current position*.
 *	
 *	##Results
 *	Returns NULL on failure. Otherwise it returns a pointer to the data in its
 *	new location. This call will fail on a sorted structure if a duplicate key
 *	is found and duplicates are not allowed.
 */
void * DS_insert      (DS root, const void * data);

/// Insert data at the beginning of a DS_list
void * DS_insert_first(DS root, const void * data);

/// Insert data at the end of a DS_list
void * DS_insert_last (DS root, const void * data);


/******************************************************************************/
//                       REMOVE FROM DATA STRUCTURE
/******************************************************************************/


// All return NULL on failure

/** Pop data off a stack.
 *	*	Used on a DS_list to implement a stack.
 *	*	If your data structure is a stack use only DS_push() and DS_pop()
 */
#define DS_pop(A) DS_remove_first(A)

/** Dequeue data.
 *	*	Used on a DS_list to implement a queue.
 *	*	If your data structure is a queue use only DS_nq() and DS_dq()
 */
#define DS_dq(A)  DS_remove_first(A)

/** Removes the node at the *current position* and returns its data
 */
const void * DS_remove      (DS root);

/// Remove the first node of a DS_list or DS_bst
const void * DS_remove_first(DS root);

/// Remove the last node of a DS_list or DS_bst
const void * DS_remove_last (DS root);


/******************************************************************************/
//                       VISIT RECORD IN DATA STRUCTURE
/******************************************************************************/


/**	search for data by its key.
 *	does not change contents of data structure
 *
 *	##Results
 *	Returns NULL on failure
 */
void * DS_find(const DS root, const void * key);


void * DS_first    (      DS root); ///< visit the first node
void * DS_last     (      DS root); ///< visit the last node
void * DS_next     (      DS root); ///< visit the next in-order node
void * DS_previous (      DS root); ///< visit the previous in-order node
void * DS_current  (const DS root); ///< visit the current node
void * DS_position (const DS root, const unsigned int);
///< set the current position to a specific count from the beginning


#endif // _DATA_H_INCLUDE


