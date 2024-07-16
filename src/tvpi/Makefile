SRC = coordinate.cpp \
      inequality.cpp \
      constant_inequality.cpp \
      unary_inequality.cpp \
	    dyadic_inequality.cpp \
      planar_comparator.cpp \
      inequality_factory.cpp \
      smt_printer.cpp \
      tikz_printer.cpp \
      complete.cpp \
      parser.cpp \
      vertex.cpp \
      join.cpp \
      tvpi.cpp \

OBJ += ../util/util$(LIBEXT) \
       ../big-int/big-int$(LIBEXT) \
       ../analyses/analyses$(LIBEXT) \

INCLUDES= -I ..

LIBS =

include ../config.inc
include ../common

CLEANFILES = tvpi$(LIBEXT)

all: tvpi$(LIBEXT)

ifneq ($(CUDD),)
  OBJ += $(CUDD)/cudd/.libs/libcudd$(LIBEXT) $(CUDD)/cplusplus/.libs/libobj$(LIBEXT)
endif

###############################################################################

tvpi$(LIBEXT): $(OBJ)
	$(LINKLIB)

#add binary to

#####################################################################