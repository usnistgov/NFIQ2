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
#
# Two variables are used within the common.mk file from the common package:
#     COMMONINCOPT can be appended to add include directories, such as 
#                  /opt/local/include
#     COMMONLIBOPT can be appeneded to add library directories.
#
# Examples are below:
#
#COMMONINCOPT := -I/opt/local/include
#COMMONLIBOPT := -L/opt/local/lib

include ../../../common/common.mk
