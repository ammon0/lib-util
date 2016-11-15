# lib-util : A Utility Library

Includes
* input.h
* data.h

##Licence
Copyright (c) 2016 Ammon Dodson

You should have received a copy of the licence terms with this software. If not, please visit the project homepage at: [https://github.com/ammon0/lib-util](https://github.com/ammon0/lib-util)

## input.h : File Input Functions
Safe replacements for some Standard C Library functions. *grabline()* in particular is a safe replacement for fgets().

Functions include:
* grabword() : get the next whitespace delimited word from the input stream
* grabline() : get the next line of text from the input stream
* grabfield() : get the next __tab__ delimited field from the input stream

Each function returns a dynamically sized string that must be freed by the caller. All will return __NULL__ on an error or the end of input.

## data.h : Quick Data Structures
A C library implementing of some of the most common data structures with an easy-to-use interface. It works with any data type because data is stored as ()void*). This will require you to recast some pointers returned by data.h to the correct data type.

Each data structure keeps track of the "current working position", and most of the function calls operate at, or modify the current position. The _data.h_ header contains details for the use of each function.

Data structure types include:
*	List: a general list that is also used to implement stacks and queues
*	Circular List
*	Binary Search Tree

Future Plans include:
*	Splay Trees
*	k Trees
*	General Trees
*	heaps
*	Hash Tables

### Acceptable Actions for Each Structure Type
 
#### List
*	`DS_count()`
*	`DS_isempty()`
*	`DS_dump()` : For debugging.
*	`DS_insert()` : Insert at _current_ position
*	`DS_insert_first()`
*	`DS_insert_last()`
*	`DS_nq()` : enqueue
*	`DS_push()`
*	`DS_remove()` : Remove node at _current_ position
*	`DS_remove_first()`
*	`DS_remove_last()`
*	`DS_pop()`
*	`DS_dq()` : dequeue
*	`DS_set_data()` : Modify the data at the _current_ position
*	`DS_first()`
*	`DS_last()`
*	`DS_next()`
*	`DS_previous()`
*	`DS_current()` : return the data at the _current_ position
*	`DS_position()` : set _current_ to the nth node in the list

#### Circular List
*	`DS_count()`
*	`DS_isempty()`
*	`DS_dump()`
*	`DS_insert()` : Insert at _current_ position
*	`DS_remove()` : Remove node at _current_ position
*	`DS_set_data()`
*	`DS_next()`
*	`DS_previous()`
*	`DS_current()`

#### Binary Search Tree
*	`DS_count()`
*	`DS_isempty()`
*	`DS_dump()`
*	`DS_sort()`
*	`DS_remove()`
*	`DS_remove_first()`
*	`DS_remove_last()`
*	`DS_set_data()`
*	`DS_find()`
*	`DS_first()`
*	`DS_last()`
*	`DS_next()`
*	`DS_previous()`
*	`DS_current()`


