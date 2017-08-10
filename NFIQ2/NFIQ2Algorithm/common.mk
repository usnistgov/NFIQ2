#
# This software was developed at the National Institute of Standards and
# Technology (NIST) by employees of the Federal Government in the course
# of their official duties. Pursuant to title 17 Section 105 of the
# United States Code, this software is not subject to copyright protection
# and is in the public domain. NIST assumes no responsibility  whatsoever for
# its use by other parties, and makes no guarantees, expressed or implied,
# about its quality, reliability, or any other characteristic.
#


# This set of directories is where the header files, libraries, programs,
# and man pages are to be installed.

PREFIX := /usr/local
INCPATH := $(PREFIX)/include
LIBPATH := $(PREFIX)/lib
BINPATH := $(PREFIX)/bin
MANPATH := $(PREFIX)/man/man1

#
# Files and directories that are created during the build process, that
# are to be removed during 'make clean'.
DISPOSABLEFILES = *.o *.exe .gdb_history *.dll *.dylib *.so *.a
DISPOSABLEDIRS  = *.dSYM

#
# Each package that includes this common file can define these variables:
# COMMONINCOPT : Location of include files from other packages, specified
#                as a compiler option (e.g. -I/usr/local/an2k/include
#
# COMMONLIBOPT : Location of libraries from other packages, specified as a
#                compiler option (e.g. -L/usr/local/an2k/lib)
#
# The next set of variables are set by files that include this file, and
# specify the location of package-only files:
#
# LOCALINC : Location where include files are stored.
# LOCALLIB : Location where the libaries are stored.
# LOCALBIN : Location where the programs are stored.
# LOCALMAN : Locatation where the man pages are stored.
#
CP := cp -f
RM := rm -f
PWD := $(shell pwd)
OS := $(shell uname -s)
ARCH := $(shell uname -m)


ifeq ($(findstring CYGWIN,$(OS)), CYGWIN)
	ROOT = Administrator
else
	ROOT  = root
endif

ifeq ($(NO_CHOWN),)
	CHOWN_TO_ROOT=-o $(ROOT)
endif


#
# Define CFLAGS for clients of the common BIOMDI APIs and libraries.
# If there are any 'non-standard' include or lib directories that need to
# be searched prior to the 'standard' libraries, add the to the CFLAGS
# variable.

OBJ      = $(subst .cpp,.o,$(SRC))
DEP      = $(subst .cpp,.d,$(SRC))

CXXFLAGS = -O2 -D__NBISLE__ -DLINUX -fPIC $(INCLUDE) -I$(PROJECT_ROOT)/../src/include -I$(PROJECT_ROOT)/../libFRFXLL/include -I$(PROJECT_ROOT)/../OpenCV/modules/core/include -I$(PROJECT_ROOT)/../OpenCV/modules/calib3d/include -I$(PROJECT_ROOT)/../OpenCV/modules/contrib/include -I$(PROJECT_ROOT)/../OpenCV/modules/features2d/include -I$(PROJECT_ROOT)/../OpenCV/modules/flann/include -I$(PROJECT_ROOT)/../OpenCV/modules/gpu/include -I$(PROJECT_ROOT)/../OpenCV/modules/highgui/include  -I$(PROJECT_ROOT)/../OpenCV/modules/imgproc/include -I$(PROJECT_ROOT)/../OpenCV/modules/ml/include -I$(PROJECT_ROOT)/../OpenCV/modules/nonfree/include -I$(PROJECT_ROOT)/../OpenCV/modules/objdetect/include -I$(PROJECT_ROOT)/../OpenCV/modules/photo/include -I$(PROJECT_ROOT)/../OpenCV/modules/stitching/include -I$(PROJECT_ROOT)/../OpenCV/modules/ts/include -I$(PROJECT_ROOT)/../OpenCV/modules/video/include -I$(PROJECT_ROOT)/../OpenCV/modules/videostab/include -I$(PROJECT_ROOT)/../OpenCV/include -I$(PROJECT_ROOT)/../OpenCV/modules/legacy/include -I$(PROJECT_ROOT)/../biomdi/common/src/include -I$(PROJECT_ROOT)/../biomdi/fingerminutia/src/include
ALL_LIBS = $(LIB) -L$(PROJECT_ROOT)/../biomdi/common/lib -L$(PROJECT_ROOT)/../biomdi/fingerminutia/lib -lbiomdi -lfmr -L$(PROJECT_ROOT)/../libOpenCV/lib -L$(PROJECT_ROOT)/../libFRFXLL/lib -lFRFXLL -lopencv_core -lopencv_calib3d -lopencv_contrib -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_ts -lopencv_video -lopencv_videostab

LDFLAGS := -Wl,-rpath,$(shell readlink -f $(PROJECT_ROOT)/../biomdi/fingerminutia/lib) -Wl,-rpath,$(shell readlink -f $(PROJECT_ROOT)/../biomdi/common/lib) -Wl,-rpath,$(shell readlink -f $(PROJECT_ROOT)/../libOpenCV/lib)
