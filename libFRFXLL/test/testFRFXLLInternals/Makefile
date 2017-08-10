#   FingerJetFX OSE -- Fingerprint Feature Extractor, Open Source Edition
#
#   Copyright (c) 2011 by DigitalPersona, Inc. All rights reserved.
#
#   DigitalPersona, FingerJet, and FingerJetFX are registered trademarks 
#   or trademarks of DigitalPersona, Inc. in the United States and other
#   countries.
#
#   FingerJetFX OSE is open source software that you may modify and/or
#   redistribute under the terms of the GNU Lesser General Public License
#   as published by the Free Software Foundation, either version 3 of the 
#   License, or (at your option) any later version, provided that the 
#   conditions specified in the COPYRIGHT.txt file provided with this 
#   software are met.
#
#   For more information, please visit digitalpersona.com/fingerjetfx.

#
#      BINARY: testFRFXLLInternals - Unit Tests for Fingerprint Feature 
#                                    Extractor Internal Function
#
#      ALGORITHM:      Alexander Ivanisov
#                      Yi Chen
#                      Salil Prabhakar
#      IMPLEMENTATION: Alexander Ivanisov
#                      Jacob Kaminsky
#                      Lixin Wei
#      DATE:           11/08/2011
#

ifdef DEBUG
DBGDIR = /debug
CXXFLAGS += -g -O0
else
CXXFLAGS += -O2
endif

OUTDIR = ../../bin$(DBGDIR)
OUTPUT = $(OUTDIR)/testFRFXLLInternals
LIBDIR = ../../lib$(DBGDIR)
LIBS = -lFRFXLLTestVectors
INCDIR = ../../src/algorithm ../TestVectors ../../src/include \
         ../../../ExternalDependencies/STL/STLport-5.2.1/stlport \
         ../../../ExternalDependencies/UnitTestFrameworks/cxxtest-3.10.1-dpbranch
         
VPATH += $(LIBDIR)
CXXFLAGS += $(addprefix -I, $(INCDIR)) $(addprefix -L, $(LIBDIR)) $(LIBS)  -Wno-invalid-offsetof
SRC = runner.cpp
HFILES = $(wildcard $(addsuffix /*.h, $(INCDIR) .))

DUMMY := $(shell test -d $(OUTDIR) || mkdir $(OUTDIR))
#OBJ = $(SRC:%.cpp=%.o)
#$(OUTPUT)($(OBJ)) : $(HFILES)

all : $(OUTPUT)
check : all ; ./$(OUTPUT)

$(OUTDIR) : ; @mkdir $(OUTDIR)
$(OUTPUT) : $(SRC) $(LIBS) $(HFILES) Makefile; g++ $(SRC) $(CXXFLAGS) -o $@

clean : ; @rm -f $(OUTPUT) runner.cpp

PERL=perl

# Where to look for the tests
TESTS            = ./*.h
TEMPLATE         = ../TestVectors/ConsoleRunner.tpl

# Where the CxxTest distribution is unpacked
CXXTESTDIR       = ../../../ExternalDependencies/UnitTestFrameworks/cxxtest-3.10.1-dpbranch
CXXTESTGEN       = $(PERL) "$(CXXTESTDIR)/cxxtestgen.pl"

# The arguments to pass to cxxtestgen
#  - ParenPrinter is the way MSVC likes its compilation errors
#  - --have-eh/--abort-on-fail are nice when you have them
CXXTESTGEN_FLAGS = --template=$(TEMPLATE) --no-static-init --no-eh --no-std

# How to generate the test runner, `runner.cpp'
runner.cpp: $(TESTS) $(TEMPLATE) Makefile ; $(CXXTESTGEN) $(CXXTESTGEN_FLAGS) -o $@ $(TESTS)

debug : ; @$(MAKE) all
debug : export DEBUG=1

.PHONY: all clean check debug
