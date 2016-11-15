# lib-util : A Utility Library

A collection of utilities for development in C. Including some basic data structures. and headache free IO functions to replace some of the functionality of `stdio.h`.

## Project Pages
*	[Latest Release](https://github.com/ammon0/lib-util/releases/latest)
*	[API Documentation](https://ammon0.github.io/lib-util/)
*	[github](https://github.com/ammon0/lib-util)

## Headers Provided

### input.h : File Input Functions
Safe replacements for some Standard C Library functions. *grabline()* in particular is a safe replacement for fgets() / gets(). Grabword can often be used effectively in place of getchar() / getc() / fgetc().

Each function automatically handles differnt types of line endings (__CR__ / __LF__) in a sane way. Never have to worry about eating the newline.

Functions include:
* grabword() : get the next whitespace delimited word from the input stream
* grabline() : get the next line of text from the input stream
* grabfield() : get the next __TAB__ delimited field from the input stream

Each function returns a dynamically sized string that must be freed by the caller. All will return __NULL__ on an error or the end of input.

### data.h : Quick Data Structures
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

## Version Numbering
This project is using [semantic versioning](http://semver.org/). As such, the API as documented in the header files and the project website will not change in an incompatible way unless there is a new major version number. Compatible changes are indicated by the minor version number.

## Dependencies
*	Standard C Library
*	Lib-Util is built to the C11 standard. It may not build with lower versions.
*	make

##MIT License

Copyright (c) 2016 Ammon Dodson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
