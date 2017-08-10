#
# This software was developed at the National Institute of Standards and
# Technology (NIST) by employees of the Federal Government in the course
# of their official duties. Pursuant to title 17 Section 105 of the
# United States Code, this software is not subject to copyright protection
# and is in the public domain. NIST assumes no responsibility  whatsoever for
# its use by other parties, and makes no guarantees, expressed or implied,
# about its quality, reliability, or any other characteristic.
#
#
# Top-level make file to build all of the INCITS 378 source code 
# (library and binaries).
#
include ../common/common.mk
SUBDIRS := src

LOCALINC := $(PWD)/src/include
LOCALLIB := $(PWD)/lib
LOCALBIN := $(PWD)/bin
LOCALMAN := $(PWD)/man

all:
	test -d $(LOCALLIB) || mkdir $(LOCALLIB)
	test -d $(LOCALBIN) || mkdir $(LOCALBIN)
	test -d $(LOCALMAN) || mkdir $(LOCALMAN)
	@for subdir in $(SUBDIRS); do \
		(cd $$subdir && $(MAKE) all) || exit 1; \
	done

install: installpaths
	install -m 644 $(CHOWN_TO_ROOT) $(LOCALINC)/*.h $(INCPATH)
	install -m 755 $(CHOWN_TO_ROOT) $(LOCALLIB)/* $(LIBPATH)
	install -m 755 $(CHOWN_TO_ROOT) $(LOCALBIN)/* $(BINPATH)
	install -m 755 $(CHOWN_TO_ROOT) $(LOCALMAN)/* $(MANPATH)
	ln -sf $(MANPATH)/minexv.1 $(MANPATH)/minexov.1
	ln -sf $(MANPATH)/minexv.1 $(MANPATH)/minex2v.1

clean:
	@for subdir in $(SUBDIRS); do \
		(cd $$subdir && $(MAKE) clean) || exit 1; \
	done
	rm -rf $(LOCALLIB)
	rm -rf $(LOCALBIN)
	rm -rf $(LOCALMAN)
	rm -f .gdb_history
