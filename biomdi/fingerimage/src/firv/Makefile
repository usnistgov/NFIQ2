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
all: firv.c
	$(CC) $(CFLAGS) firv.c -lfir -o firv
	$(CP) firv $(LOCALBIN)
	$(CP) firv.1 $(LOCALMAN)
clean:
	$(RM) firv $(DISPOSABLEFILES)
	$(RM) -r $(DISPOSABLEDIRS)
