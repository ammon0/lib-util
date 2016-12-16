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
INCDIR:=$(INSTALLDIR)/include

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
	#-Wno-discarded-qualifiers -Wpadded

CFLAGS:= $(CWARNINGS) --std=c11 -g

ALLFILES:= data.c data.h test-data.c input.h input.c test-input.c
CLEANFILES:= *.o *.a test-data test-input


.PHONEY: install debug-data debug-input release

release: CFLAGS += -O2
release: libdata.a libinput.a

debug-data: CFLAGS += -DDEBUG
debug-data: test-data
debug-input: CFLAGS += -DDEBUG
debug-input: test-input

install: libdata.a data.h libinput.a input.h
	install -d $(LIBDIR) $(INCDIR)
	install -C ./libdata.a $(LIBDIR)
	install -C ./libinput.a $(LIBDIR)
	install -C ./data.h $(INCDIR)
	install -C ./input.h $(INCDIR)

test-input: input.h test-input.c input.o
	$(CC) $(CFLAGS) -o $@ test-input.c input.o
	chmod +x $@

test-data: data.h test-data.c data.o
	$(CC) $(CFLAGS) -o $@ test-data.c data.o
	chmod +x $@

libinput.a: input.o
	ar rcs $@ input.o

libdata.a: data.o
	ar rcs $@ data.o

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<


################################## UTILITIES ###################################

.PHONEY: clean todolist docs
docs: Doxyfile data.h input.h README.md
	doxygen Doxyfile

clean:
	rm -f $(CLEANFILES)

todolist:
	-@for file in $(ALLFILES:Makefile=); do fgrep -H -e TODO -e FIXME $$file; done; true

