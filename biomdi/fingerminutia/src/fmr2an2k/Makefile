#
# This software was developed at the National Institute of Standards and
# Technology (NIST) by employees of the Federal Government in the course
# of their official duties. Pursuant to title 17 Section 105 of the
# United States Code, this software is not subject to copyright protection
# and is in the public domain. NIST assumes no responsibility  whatsoever for
# its use by other parties, and makes no guarantees, expressed or implied,
# about its quality, reliability, or any other characteristic.
#
include ../common.mk
LIBS:=-lc -lfmr -lan2k -lioutil -lutil
all: fmr2an2k.c

#
# OS-X has a libutil.dylib which clashes with NBIS' libutil.a, so the linker
# is passed an option to search for static and dynamic libraries in each
# directory; by defauly, dynlibs are search across all directories first.
#
ifeq ($(OS), Darwin)
	$(CC) -Wl,-search_paths_first $(CFLAGS) fmr2an2k.c $(LIBS) -o fmr2an2k
else
	$(CC) $(CFLAGS) fmr2an2k.c $(LIBS) -o fmr2an2k
endif
	$(CP) fmr2an2k $(LOCALBIN)
	$(CP) fmr2an2k.1 $(LOCALMAN)

clean:
	$(RM) fmr2an2k $(DISPOSABLEFILES)
	$(RM) -r $(DISPOSABLEDIRS)
