SRC     = $(shell ls src/*.cpp && ls src/wsq/*.cpp && ls src/features/*.cpp && ls src/prediction/*.cpp)
TARGET  = NFIQ2
LIBTARGET = libnfiq2
INCLUDE = -Iinclude -I. -Iinclude/wsq -Iinclude/features -Iinclude/prediction
PROJECT_ROOT = ..
LIBFRFXLL = ../../libFRFXLL/lib/libFRFXLL.a

include common.mk
LOCALINC := $(PWD)/include
LOCALLIB := $(PWD)/lib
LOCALBIN  := $(PWD)/bin

ifeq ($(OS), Darwin)
SHFLAGS = -dynamiclib -o $(LIBTARGET).dylib -Wl,-macosx_version_min,$(shell sw_vers -productVersion | cut -d. -f1).$(shell sw_vers -productVersion | cut -d. -f2)
else
SHFLAGS = -shared -o $(LIBTARGET).so
endif

all: $(LIBTARGET) $(TARGET)

$(LIBTARGET): $(OBJ)
	$(AR) $(ARFLAGS) $@.a $(filter-out src/NFIQ2.o,$(OBJ))
	dir=`mktemp -d $(LIBTARGET).XXXXX`; cd $$dir; ar x ../$(LIBFRFXLL); ar q ../$(LIBTARGET).a; rm *; cd ..; rmdir $$dir
	ranlib $(LIBTARGET).a

	$(CXX) $(SHFLAGS) $(filter-out src/NFIQ2.o,$(OBJ)) $(ALL_LIBS)

	mkdir -p lib
	mv $(LIBTARGET).* lib

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ src/NFIQ2.o  $(ALL_LIBS) -Llib -lnfiq2
	mkdir -p bin
	mv $(TARGET) bin

%.d: %.cpp
	$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\).o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@ ; \
	rm -f $@.$$$$

install:
	mkdir -p $(INCPATH)/NFIQ2
	mkdir -p $(INCPATH)/NFIQ2/prediction
	mkdir -p $(INCPATH)/NFIQ2/features
	mkdir -p $(INCPATH)/NFIQ2/wsq
	install -m 644 $(CHOWN_TO_ROOT) $(LOCALINC)/*.h* $(INCPATH)/NFIQ2
	install -m 644 $(CHOWN_TO_ROOT) $(LOCALINC)/prediction/* $(INCPATH)/NFIQ2/prediction
	install -m 644 $(CHOWN_TO_ROOT) $(LOCALINC)/features/* $(INCPATH)/NFIQ2/features
	install -m 644 $(CHOWN_TO_ROOT) $(LOCALINC)/wsq/* $(INCPATH)/NFIQ2/wsq
	install -m 755 $(CHOWN_TO_ROOT) $(LOCALLIB)/* $(LIBPATH)
	install -m 777 $(CHOWN_TO_ROOT) $(LOCALBIN)/* $(BINPATH)

clean:
	rm -f $(OBJ) $(TARGET) $(LIBTARGET) $(DEP)
	rm -rf bin
	rm -rf lib

-include $(DEP)

.PHONY: all clean

