#
# This software was developed at the National Institute of Standards and
# Technology (NIST) by employees of the Federal Government in the course
# of their official duties. Pursuant to title 17 Section 105 of the
# United States Code, this software is not subject to copyright protection
# and is in the public domain. NIST assumes no responsibility  whatsoever for
# its use by other parties, and makes no guarantees, expressed or implied,
# about its quality, reliability, or any other characteristic.
#

.DEFAULT_GOAL = all

# This set of directories is where the header files, libraries, programs,
# and man pages are to be installed.

BIOMDIINC := ../../../common/src/include
BIOMDILIB := ../../../common/lib
PREFIX := /usr/local
INCPATH := $(PREFIX)/include
LIBPATH := $(PREFIX)/lib
BINPATH := $(PREFIX)/bin
MANPATH := $(PREFIX)/man/man1

#
# Files and directories that are created during the build process, that
# are to be removed during 'make clean'.
DISPOSABLEFILES = *.o *.exe .gdb_history *.dll *.dylib *.so *.a
DISPOSABLEDIRS  = *.dSYM

#
# Each package that includes this common file can define these variables:
# COMMONINCOPT : Location of include files from other packages, specified
#                as a compiler option (e.g. -I/usr/local/an2k/include
#
# COMMONLIBOPT : Location of libraries from other packages, specified as a
#                compiler option (e.g. -L/usr/local/an2k/lib)
#
# The next set of variables are set by files that include this file, and
# specify the location of package-only files:
#
# LOCALINC : Location where include files are stored.
# LOCALLIB : Location where the libaries are stored.
# LOCALBIN : Location where the programs are stored.
# LOCALMAN : Locatation where the man pages are stored.
#
CP := cp -f
RM := rm -f
PWD := $(shell pwd)
OS := $(shell uname -s)
ARCH := $(shell uname -m)

#
# Set the GCC version, but right now all we care about is version 4
#
_GCCV := $(shell gcc --version)
ifeq ($(findstring gcc-4, $(_GCCV)), gcc-4)
GCCV := 4
else
GCCV := Unknown
endif

ifeq ($(findstring CYGWIN,$(OS)), CYGWIN)
	ROOT = Administrator
else
	ROOT  = root
endif

ifeq ($(NO_CHOWN),)
	CHOWN_TO_ROOT=-o $(ROOT)
endif

ifeq ($(findstring amd64, $(ARCH)), amd64)
	EXTRACFLAGS = -fPIC
else
	ifeq ($(findstring x86_64, $(ARCH)), x86_64)
		EXTRACFLAGS = -fPIC
	endif
endif

installpaths: $(INCPATH) $(LIBPATH) $(BINPATH) $(MANPATH)
$(INCPATH):
		@echo "$(INCPATH) does not exist";
		exit 2
$(LIBPATH):
		@echo "$(LIBPATH) does not exist";
		exit 2
$(BINPATH):
		@echo "$(BINPATH) does not exist";
		exit 2
$(MANPATH):
		@echo "$(MANPATH) does not exist";
		exit 2

#
# Define CFLAGS for clients of the common BIOMDI APIs and libraries.
# If there are any 'non-standard' include or lib directories that need to
# be searched prior to the 'standard' libraries, add the to the CFLAGS
# variable.

CFLAGS := -Wall -g -std=c99 -I$(BIOMDIINC) $(COMMONINCOPT) -I$(LOCALINC) -I$(INCPATH) $(COMMONLIBOPT) -L$(BIOMDILIB) -lbiomdi -L$(LOCALLIB) -L$(LIBPATH) $(EXTRACFLAGS)
