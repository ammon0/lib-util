################################################################################
#
#	lib-util : A Utility Library
#
#	Copyright (c) 2016-2017 Ammon Dodson
#	You should have received a copy of the licence terms with this software. If
#	not, please visit the project homepage at:
#	https://github.com/ammon0/lib-util
#
################################################################################

# Change these variables to point to the appropriate installation directories
INSTALLDIR:=$(HOME)/prg
LIBDIR:=$(INSTALLDIR)/lib
INCDIR:=$(INSTALLDIR)/include/util

# My code builds without warnings--ALWAYS
CWARNINGS:=	-Wall -Wextra -pedantic \
	-Wmissing-prototypes -Wstrict-prototypes -Wmissing-declarations \
	-Wredundant-decls -Werror=implicit-function-declaration -Wnested-externs \
	-Wshadow -Wbad-function-cast \
	-Wcast-align \
	-Wdeclaration-after-statement -Werror=uninitialized \
	-Winline \
	-Wswitch-default -Wswitch-enum \
	-Wsuggest-attribute=pure -Wsuggest-attribute=const \
	-Wsuggest-attribute=noreturn -Wsuggest-attribute=format \
	-Wtrampolines -Wstack-protector \
	-Wwrite-strings \
	-Wc++-compat \
	-Wconversion -Wdisabled-optimization \
	-Wpadded

CXXWARNINGS:=-Wall -Wextra -pedantic \
	-Wmissing-declarations -Werror=implicit-function-declaration \
	-Wredundant-decls -Wshadow \
	-Wpointer-arith -Wcast-align \
	-Wuninitialized -Wmaybe-uninitialized -Werror=uninitialized \
	-Winline -Wno-long-long \
	-Wswitch \
	-Wsuggest-attribute=pure -Wsuggest-attribute=const \
	-Wsuggest-attribute=noreturn -Wsuggest-attribute=format \
	-Wtrampolines -Wstack-protector \
	-Wwrite-strings \
	-Wconversion -Wdisabled-optimization

CFLAGS  := $(CWARNINGS)   --std=c11   -g -O3 -I./
CXXFLAGS:= $(CXXWARNINGS) --std=c++14 -g -O3 -I./

headers:= \
util/data.h util/input.h util/msg.h \
util/hash.h util/types.h util/flags.h \
util/string_array.hpp

libraries:= libdata.a libinput.a libmsg.a libstring_array.a
tests    := test-hash test-input test-data test-msg

ALLFILES:= $(headers) data.c test-data.c input.c test-input.c msg.c string_array.cpp
CLEANFILES:= *.o *.a test-data test-input test-hash test-msg


.PHONEY: install all

all: $(libraries)

install: $(libraries) $(headers)
	install -d $(LIBDIR) $(INCDIR)
	for f in $(libraries); do install -C $$f $(LIBDIR); done
	for f in $(headers)  ; do install -C $$f $(INCDIR); done

test-hash: util/hash.h test-hash.c data.o input.o msg.o
	$(CC) $(CFLAGS) -Wno-conversion -Wno-pointer-sign -o $@ test-hash.c data.o input.o msg.o -lm
	chmod +x $@

test-input: util/input.h test-input.c libinput.a
	$(CC) $(CFLAGS) -o $@ -L./ test-input.c -linput
	chmod +x $@

test-data: util/data.h test-data.c libdata.a libmsg.a
	$(CC) $(CFLAGS) -o $@ -L./ test-data.c -ldata -lmsg
	chmod +x $@

test-msg: util/msg.h test-msg.c libmsg.a
	$(CC) $(CFLAGS) -o $@ -L./ test-msg.c -lmsg
	chmod +x $@

msg.o: msg.c util/msg.h util/flags.h
	$(CC) $(CFLAGS) -Wno-conversion -c $<

lib%.a: %.o
	ar rcs $@ $<

%.o: %.c util/%.h
	$(CC) $(CFLAGS) -c $<


################################## UTILITIES ###################################

.PHONEY: clean todolist docs very-clean
docs: Doxyfile $(headers) README.md
	doxygen Doxyfile

clean:
	rm -f $(CLEANFILES)

very-clean: clean
	rm -r ./docs

todolist:
	-@for file in $(ALLFILES:Makefile=); do fgrep -H -e TODO -e FIXME $$file; done; true

