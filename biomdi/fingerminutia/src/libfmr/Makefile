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
SOURCES = fmr.c fvmr.c fmd.c fedb.c polar.c random.c xy.c angle.c quality.c ansi2iso.c iso2ansi.c validate.c
OBJECTS = fmr.o fvmr.o fmd.o fedb.o polar.o random.o xy.o angle.o quality.o ansi2iso.o iso2ansi.o validate.o

#
# On OS-X, when using gcc 4.0 or later, the SystemStubs library must be
# linked in. This may be true on other OS as well.
#
ifeq ($(OS), Darwin)
#ifeq ($(findstring gcc-4, $(GCCV)), gcc-4)
ifeq ($(GCCV), 4)
EXTRALIBS := -lSystemStubs
endif
endif

all: $(SOURCES)
ifeq ($(OS), Darwin)
	$(CC) -c $(CFLAGS) $(SOURCES)
	libtool -dynamic -o libfmr.dylib -lbiomdi -lc -L$(BIOMDILIB) $(OBJECTS) $(EXTRALIBS)
	$(CP) libfmr.dylib $(LOCALLIB)
else
ifeq ($(findstring CYGWIN,$(OS)), CYGWIN)
	$(CC) $(CFLAGS) -c $(SOURCES)
	ar rs libfmr.a $(OBJECTS)
	ranlib libfmr.a
	$(CC) $(CFLAGS) -shared -o libfmr.dll -Wl,--out-implib=libfmr.dll.a -Wl,--export-all-symbols -Wl,--enable-auto-import -Wl,--whole-archive libfmr.a -Wl,--no-whole-archive -lbiomdi
	$(CP) libfmr.a $(LOCALLIB)
	$(CP) libfmr.dll.a $(LOCALLIB)
	$(CP) libfmr.dll $(LOCALLIB)
else
	$(CC) $(CFLAGS) -shared $(SOURCES) -lm -o libfmr.so -lbiomdi
	$(CP) libfmr.so $(LOCALLIB)
endif
endif

clean:
	$(RM) $(DISPOSABLEFILES)
	$(RM) -r $(DISPOSABLEDIRS)
