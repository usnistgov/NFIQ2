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
#      LIBRARY: FRFXLL - Fingerprint Feature Extractor - LowLevel API
#
#      ALGORITHM:      Alexander Ivanisov
#                      Yi Chen
#                      Salil Prabhakar
#      IMPLEMENTATION: Alexander Ivanisov
#                      Jacob Kaminsky
#                      Lixin Wei
#      DATE:           11/08/2011
#

# don't use make -j, ar has problem working in parallel on the same archive

ifdef DEBUG
DBGDIR = /debug
CXXFLAGS += -g -O0
else
CXXFLAGS += -O2
endif

OUTDIR = ../../lib$(DBGDIR)
OUTPUT = $(OUTDIR)/libFRFXLL.a
all : $(OUTPUT)

include ../../../version.mak
INCDIR = ../algorithm ../../include ../../src/include ../../../ExternalDependencies/STL/STLport-5.2.1/stlport 
CXXFLAGS += $(addprefix -I, $(INCDIR)) -fno-exceptions -fno-rtti -fno-stack-protector -fPIC
CXXFLAGS += -DFRFXLL_MAJOR=$(MAJOR) -DFRFXLL_MINOR=$(MINOR) -DFRFXLL_REVISION=$(REVISION) -DFRFXLL_BUILD=$(BUILD)
SRC = $(wildcard *.cpp)
HFILES = $(wildcard $(addsuffix /*.h, $(INCDIR) .))
OBJ = $(SRC:%.cpp=%.o)
DUMMY := $(shell test -d $(OUTDIR) || mkdir $(OUTDIR))


$(OUTPUT)($(OBJ)) : $(HFILES) Makefile

$(OUTPUT) : $(OUTPUT)($(OBJ))

clean : ; @rm -f $(OUTPUT) *.o
check : all

#TESTS = EngineTests dpfrTestRunner
#check : $(TESTS)
#clean : $(TESTS)
#$(TESTS) : $(OUTPUT) ; $(MAKE) -C ../../test/$@ $(MAKECMDGOALS)

debug : ; @$(MAKE) all
debug : export DEBUG=1

.PHONY: all clean check debug
#$(TESTS)
