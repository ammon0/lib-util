/*******************************************************************************
 *
 *	lib-util : A Utility Library
 *
 *	Copyright (c) 2016 Ammon Dodson
 *	You should have received a copy of the license terms with this software. If
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
 *	implicitly act on the current position. When inserting at the current
 *	position, what was previously at the current position will now be at the
 *	next from current position.
 *
 *	##Structure Types Available
 *	Data structure types include:
 *	*	list: a general list that is also used to implement stacks and queues
 *	*	circular list
 *	*	binary search tree
 *
 *	###Stacks and Queues
 *	Stacks and queues use the DS_list type structure. The caller must limit
 *	themselves to only using DS_push() & DS_pop() for stacks, and DS_nq() &
 *	DS_dq() for queues. For stacks DS_first() may be used to view the member on
 *	the top of the stack without changing the stack. For queues DS_first() may
 *	be used to view the next-out member, and DS_last() may be used to view the
 *	last-out member.
 *
 *	## Data Storage Method
 *	The caller's data is stored in a generic fixed length byte array. The
 *	size of the arrays is set by the caller in DS_new_XXX() when the data
 *	structure is first initialized. All incoming data is copied into the data
 *	structure from the pointer location passed by the insert functions.
 *
 *	If the structure's data needs to be variable length then the caller will
 *	have to store pointers in the data structure and manage the memory for the
 *	variable data. Keep in mind that the insert functions expect a pointer to
 *	the stored data in this case `**data`. The traversal and removal functions
 *	also return a pointer to the stored data, so again `**data`.
 *
 *	## Removal
 *	DS_remove() functions return a pointer to the removed data. The caller
 *	must copy that data out as there is no guarantee that it will be preserved
 *	after the next call to the data structure.
 *
 *	## Traversal & Modifications
 *	Data accessed through any of the various traversal functions may be
 *	directly modified through the returned pointer. Obviously, do not modify the
 *	sort key in a sorted data structure as it will result in unpredictable
 *	behavior and possible data loss.
 *
 *	## Errors
 *	Errors and messages are reported on `stderr`.
 *
 *	## Acceptable Actions For Each Structure Type ##
 *
 *	### All Structures
 *	*	DS_new()
 *	*	DS_flush()
 *	*	DS_delete()
 *	*	DS_empty()
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
 *	### General Trees
 *	*	DS_isleaf()
 *	*	DS_insert_sibling()
 *	*	DS_insert_first_child()
 *	*	DS_insert_last_child()
 *	*	DS_insert_nth_child()
 *	*	DS_remove()
 *	*	DS_next_sibling()
 *	*	DS_previous_sibling()
 *	*	DS_parent()
 *	*	DS_first_child()
 *	*	DS_last_child()
 *	*	DS_nth_child()
 *	*	DS_current()
 *
 *	### Hash Tables
 *	*	DS_insert()
 *	*	DS_remove()
 *	*	DS_find()
 *	*	DS_current()
 *
 ******************************************************************************/


#ifndef _DATA_H_INCLUDE
#define _DATA_H_INCLUDE

#include <stdbool.h>
#include <stddef.h>

/// All data structures are represented in the caller's code as type DS
typedef struct _root* DS;

#ifdef __cplusplus
	extern "C" {
#endif


/******************************************************************************/
//                          CREATE A NEW STRUCTURE
/******************************************************************************/


/**	Create a new straight list.
 *
 *	##Parameters
 *	### data_size
 *	* The size in bytes of the data being stored in this structure.
 *	* If you need to store variable length data you should store pointers in the
 *		data structure.
 *
 *	##Results:
 *	Returns NULL on failure
 */
DS DS_new_list(size_t data_size);

/**	Create a new circular list.
 *
 *	##Parameters
 *	### data_size
 *	* The size in bytes of the data being stored in this structure.
 *	* If you need to store variable length data you should store pointers in the
 *		data structure.
 *
 *	##Results:
 *	Returns NULL on failure
 */
DS DS_new_circular(size_t data_size);


/**	Create a new binary search tree.
 *
 *	##Parameters
 *	### data_size
 *	* The size in bytes of the data being stored in this structure.
 *	* If you need to store variable length data you should store pointers in the
 *		data structure.
 *
 *	### duplicates_allowed
 *	Non-zero if duplicate keys are allowed, zero otherwise.
 *
 *	### key(const void * data)
 *	Is used to extract a sort key from the data passed into the structure.
 *
 *	### cmp_key(const void * left , const void * right)
 *	* A function to compare keys extracted by key()
 *	* It must return <0 if left is ordered before right, >0 if left is
 *		ordered after right, and 0 if they are the same.
 *
 *	##Results:
 *	Returns NULL on failure
 */
DS DS_new_bst(
	size_t       data_size,
	bool         duplicates_allowed,
	const void * (*key)(const void * data),
	int          (*cmp_keys)(const void * left , const void * right)
);


/*	Create a new hash table.
 *
 *	##Parameters
 *	### data_size
 *	* The size in bytes of the data being stored in this structure.
 *	* If you need to store variable length data you should store pointers in the
 *		data structure.
 *
 *	### key_size
 *	*	0 indicates that the key is null terminated
 *	*	Otherwise indicates the size of the key.
 *
 *	### table_size
 *	*	0 indicates the default size
 *	*	Otherwise indicates the size of the hash table
 *
 *	### duplicates_allowed
 *	Non-zero if duplicate keys are allowed, zero otherwise.
 *
 *	### key(const void * data)
 *	Is used to extract a sort key from the data passed into the structure.
 *
 *	### cmp_key(const void * left , const void * right)
 *	* A function to compare keys extracted by key()
 *	* It must return <0 if left is ordered before right, >0 if left is
 *		ordered after right, and 0 if they are the same.
 *
 *	##Results:
 *	Returns NULL on failure
 */
//DS DS_new_hash(
//	size_t       data_size,
//	size_t       key_size,
//	size_t       table_size,
//	bool         duplicates_allowed,
//	const void * (*key)(const void * data),
//	int          (*cmp_keys)(const void * left , const void * right)
//);


//DS DS_new_tree(
//	unsigned int children,
//	size_t       data_size
//);


/******************************************************************************/
//                     COMMANDS FOR ALL DATA STRUCTURES
/******************************************************************************/


/** Delete the entire contents of a data structure and free its memory.
 */
void DS_delete(DS root);

/**	Flushes cached memory.
 *	Removing nodes from the structure does not immediately release the occupied
 *	memory. This memory is cached for quick reuse. This command frees that
 *	cached memory.
 */
void DS_flush (DS root);

/// Remove all items from a data structure
void DS_empty (DS root);

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


#ifdef __cplusplus
	};
#endif


#endif // _DATA_H_INCLUDE


