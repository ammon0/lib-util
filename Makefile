################################################################################
#
#	lib-util : A Utility Library
#
#	Copyright (c) 2016-2018 Ammon Dodson
#	You should have received a copy of the licence terms with this software. If
#	not, please visit the project homepage at:
#	https://github.com/ammon0/lib-util
#
################################################################################


# Change these variables to point to the appropriate installation directories
WORKDIR   :=./work
INSTALLDIR:=$(HOME)/prg
LIBDIR:=$(INSTALLDIR)/lib/util
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

CFLAGS:= $(CWARNINGS) --std=c11 -g -O3 -I./


############################### FILES AND FOLDERS ##############################

# Don't change these
include version.mk

srcdir    :=./src
headerdir :=./util

headers   :=$(wildcard $(headerdir)/*.h)
sources   :=$(wildcard $(srcdir)/*.c)
allfiles  := $(headers) $(sources)

libraries:=libdata libinput libmsg libstring
objects  :=data.o input.o msg.o string.o
tests    :=test-hash test-input test-data test-msg test-string

links    :=$(libraries)
libraries:=$(addprefix $(WORKDIR)/, $(libraries) )
objects  :=$(addprefix $(WORKDIR)/, $(objects) )
tests    :=$(addprefix $(WORKDIR)/, $(tests) )
test_links:=$(libraries)

test_links:=$(addsuffix .$(MAJOR), $(test_links))
libraries:=$(addsuffix .so.$(MAJOR).$(MINOR), $(libraries))

#################################### PHONEY ####################################

.PHONEY: libs all tests install
all: libs tests
libs: $(libraries)
tests: $(tests)

################################### LIBRARIES ##################################

$(libraries): $(WORKDIR)/lib%.so.$(MAJOR).$(MINOR): $(WORKDIR)/%.o | $(WORKDIR)
	$(CC) -shared -Wl,-soname,lib$*.$(MAJOR) -o $@ $^

$(objects): $(WORKDIR)/%.o: $(srcdir)/%.c $(headerdir)/%.h | $(WORKDIR)
	$(CC) $(CFLAGS) -c -fPIC -o $@ $<

##################################### TESTS ####################################

$(tests): $(WORKDIR)/%: $(srcdir)/%.c | $(libraries) $(test_links)
	$(CC) $(CFLAGS) -Wno-c++-compat -o $@ $< -Wl,-rpath=$(WORKDIR) -L$(WORKDIR) -linput -ldata -lmsg -lstring
	chmod +x $@

$(test_links):$(WORKDIR)/lib%.$(MAJOR): $(WORKDIR)/lib%.so.$(MAJOR).$(MINOR) | $(WORKDIR)
	ln -s lib$*.so.$(MAJOR).$(MINOR) $@
	ln -s lib$*.so.$(MAJOR).$(MINOR) $(WORKDIR)/lib$*.so

################################### INSTALL ####################################

install: $(libraries) $(headers)
	install -d $(LIBDIR) $(INCDIR)
	for f in $(headers)  ; do install -m0640 -C $$f $(INCDIR); done
	for f in $(libraries); do install -m0750 -C $$f $(LIBDIR); done
	for f in $(links)    ; do \
		ln -fs $$f.so.$(MAJOR).$(MINOR) $(LIBDIR)/$$f.so; \
	done
	ldconfig -nv $(LIBDIR)

################################## UTILITIES ###################################

# working directory
$(WORKDIR):
	mkdir -p $@

.PHONEY: clean todolist docs clean-docs count

docs: Doxyfile $(headers) README.md
	doxygen Doxyfile

clean:
	rm -fr $(WORKDIR)

clean-docs:
	rm -r ./docs

todolist:
	-@for file in $(allfiles:Makefile=); do fgrep -H -e TODO -e FIXME $$file; done; true

count:
	cat $(allfiles) |line_count


