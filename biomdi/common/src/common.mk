#
# This software was developed at the National Institute of Standards and
# Technology (NIST) by employees of the Federal Government in the course
# of their official duties. Pursuant to title 17 Section 105 of the
# United States Code, this software is not subject to copyright protection
# and is in the public domain. NIST assumes no responsibility whatsoever for
# its use by other parties, and makes no guarantees, expressed or implied,
# about its quality, reliability, or any other characteristic.
#
LOCALINC := ../include
LOCALLIB := ../../lib
LOCALBIN := ../../bin
LOCALMAN := ../../man

include ../../common.mk
#
# Define CFLAGS for the common BIOMDI library only.

#CFLAGS := -Wall -g -std=c99 -I$(BIOMDIINC) $(COMMONINCOPT) -I$(LOCALINC) -I$(INCPATH) $(COMMONLIBOPT) -L$(BIOMDILIB) -lbiomdi -L$(LOCALLIB) -L$(LIBPATH) $(EXTRACFLAGS)
CFLAGS := -Wall -g -std=c99 $(COMMONINCOPT) -I$(LOCALINC) -I$(INCPATH) $(COMMONLIBOPT) -L$(LOCALLIB) -L$(LIBPATH) $(EXTRACFLAGS)
