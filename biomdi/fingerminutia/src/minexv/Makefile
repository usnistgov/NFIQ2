
# This software was developed at the National Institute of Standards and
# Technology (NIST) by employees of the Federal Government in the course
# of their official duties. Pursuant to title 17 Section 105 of the
# United States Code, this software is not subject to copyright protection
# and is in the public domain. NIST assumes no responsibility  whatsoever for
# its use by other parties, and makes no guarantees, expressed or implied,
# about its quality, reliability, or any other characteristic.
#
#
include ../common.mk
LIBFMRSRC = ../libfmr/fedb.c ../libfmr/fmd.c ../libfmr/fmr.c ../libfmr/fvmr.c ../libfmr/validate.c


all:	minexv minexov minex2v

minexv: minexv.c $(LIBFMRSRC)
	$(CC) -DMINEX $(CFLAGS) $(LIBFMRSRC) -DMINEX04 minexv.c -o minexv -lm -lbiomdi
	$(CP) minexv.1 $(LOCALMAN)
	$(CP) minexv $(LOCALBIN)

minexov: minexv.c $(LIBFMRSRC)
	$(CC) -DMINEX $(CFLAGS) $(LIBFMRSRC) -DOMINEX minexv.c -o minexov -lm -lbiomdi
	$(CP) minexov $(LOCALBIN)

minex2v: minexv.c $(LIBFMR)
	$(CC) $(CFLAGS) -DMINEX2 minexv.c -lfmr -o minex2v -lm -lbiomdi
	$(CP) minex2v $(LOCALBIN)

clean:
	$(RM) minexv minexov minex2v $(DISPOSABLEFILES)
	$(RM) -r $(DISPOSABLEDIRS)
