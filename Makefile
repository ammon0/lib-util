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
INCDIR:=$(INSTALLDIR)/util/include

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

ALLFILES:= data.c data.h test-data.c input.h input.c test-input.c my_types.h
CLEANFILES:= *.o *.a test-data test-input test-hash


.PHONEY: install all

all: libdata.a libinput.a libmsg.a

install: libdata.a data.h libinput.a input.h libmsg.a msg.h hash.h types.h
	install -d $(LIBDIR) $(INCDIR)
	install -C ./libdata.a  $(LIBDIR)
	install -C ./libinput.a $(LIBDIR)
	install -C ./libmsg.a   $(LIBDIR)
	install -C ./data.h  $(INCDIR)
	install -C ./input.h $(INCDIR)
	install -C ./msg.h   $(INCDIR)
	install -C ./types.h $(INCDIR)
	install -C ./hash.h  $(INCDIR)

test-hash: hash.h test-hash.c data.o input.o
	$(CC) $(CFLAGS) -Wno-conversion -Wno-pointer-sign -o $@ test-hash.c data.o input.o -lm
	chmod +x $@

test-input: input.h test-input.c input.o
	$(CC) $(CFLAGS) -o $@ test-input.c input.o
	chmod +x $@

test-data: data.h test-data.c data.o
	$(CC) $(CFLAGS) -o $@ test-data.c data.o
	chmod +x $@

lib%.a: %.o
	ar rcs $@ $<

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<


################################## UTILITIES ###################################

.PHONEY: clean todolist docs very-clean
docs: Doxyfile data.h input.h msg.h types.h hash.h README.md
	doxygen Doxyfile

clean:
	rm -f $(CLEANFILES)

very-clean: clean
	rm -r ./docs

todolist:
	-@for file in $(ALLFILES:Makefile=); do fgrep -H -e TODO -e FIXME $$file; done; true

