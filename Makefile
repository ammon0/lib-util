################################################################################
#
#	lib-util : A Utility Library
#
#	Copyright (c) 2016 Ammon Dodson
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
	-Wpadded #-Wno-discarded-qualifiers 

CFLAGS:= $(CWARNINGS) --std=c11 -I./ -O3 -g

headers:= util/data.h util/input.h util/msg.h util/hash.h util/types.h
libraries:= libdata.a libinput.a libmsg.a

ALLFILES:= $(headers) data.c test-data.c input.c test-input.c msg.c
CLEANFILES:= *.o *.a test-data test-input test-hash


.PHONEY: install all

all: $(libraries)

install: $(libraries) $(headers)
	install -d $(LIBDIR) $(INCDIR)
	for f in $(libraries); do install -C $$f $(LIBDIR); done
	
	for f in $(headers); do install -C $$f $(INCDIR); done

test-hash: util/hash.h test-hash.c data.o input.o
	$(CC) $(CFLAGS) -Wno-conversion -Wno-pointer-sign -o $@ test-hash.c data.o input.o -lm
	chmod +x $@

test-input: util/input.h test-input.c input.o
	$(CC) $(CFLAGS) -o $@ test-input.c input.o
	chmod +x $@

test-data: util/data.h test-data.c data.o
	$(CC) $(CFLAGS) -o $@ test-data.c data.o
	chmod +x $@

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

