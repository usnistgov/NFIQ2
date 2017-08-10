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
#      BINARY: FRFXLLSample - Sample Code for Fingerprint Feature Extractor - LowLevel API
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
  CFLAGS += -g -O0
endif

OUTDIR = ../../bin$(DBGDIR)
OUTPUT = $(OUTDIR)/FRFXLLSample
LIBDIR = ../../lib$(DBGDIR)
LIBS = -lFRFXLL
all : $(OUTPUT)
check : all ; ./$(OUTPUT)

INCDIR = ../../include
VPATH += $(LIBDIR)
CFLAGS += $(addprefix -I, $(INCDIR)) -L$(LIBDIR) $(LIBS) 
SRC = $(wildcard *.c)
HFILES = $(wildcard $(addsuffix /*.h, $(INCDIR) .))
#OBJ = $(SRC:%.cpp=%.o)
#$(OUTPUT)($(OBJ)) : $(HFILES)
$(shell test -d $(OUTDIR) || mkdir $(OUTDIR))

$(OUTPUT) : $(SRC) $(LIBS) $(HFILES) Makefile; $(CC) $(SRC) $(CFLAGS) -o $@

clean : ; @rm -f $(OUTPUT)

debug : ; @$(MAKE) all
debug : export DEBUG=1

.PHONY: all clean check debug
