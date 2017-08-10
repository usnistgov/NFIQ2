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
# Top-level make file to build all of the M1 source code (library and binaries).
#

# The 'core' library and programs, that always build
CORE := libfmr prfmr mkfmr fmrv fmroverlap fmrprune fmrsort fmr2fmr fmrmod fmrisocompact minexv test

#
# Programs dependent on NBIS; see http://fingerprint.nist.gov/NBIS/index.html
# Requires that the NBIS include and library paths be added to the
# COMMONINCOPT and COMMONLIBOPT directories, respectively, in the common.mk
# file in this project.
#
NBIS := fmr2an2k an2k2fmr

#
# Programs dependent on the GD graphics library
# Requires that the GD include and library paths be added to the
# COMMONINCOPT and COMMONLIBOPT directories, respectively, in the common.mk
# file in this project.
#
GD := fmrplot

SUBDIRS := $(CORE)

#SUBDIRS := $(CORE) $(GD)

# If you have the NIST NBIS distribution, use this SUBDIRS line to build
# the NIST/ANSI 2000 minutiae convertor programs.

#SUBDIRS := $(CORE) $(NBIS)

# If you have the NIST NBIS distribution, and have installed the GD Graphics
# Library used by the minutiae plotter, use this SUBDIRS line. See the comments
# in src/fmrplot/fmrplot.c for information on the graphics library.

#SUBDIRS := $(CORE) $(NBIS) $(GD)

all:
	@for subdir in $(SUBDIRS); do \
		(cd $$subdir && $(MAKE) all) || exit 1; \
	done

clean:
	@for subdir in $(SUBDIRS); do \
		(cd $$subdir && $(MAKE) clean) || exit 1; \
	done
	rm -f .gdb_history
