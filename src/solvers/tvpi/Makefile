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
      tvpi_systemt.cpp \
      tvpi_math.cpp \

OBJ += ../../util/util$(LIBEXT) \
       ../../big-int/big-int$(LIBEXT) \
       ../../analyses/analyses$(LIBEXT) \
       ../../goto-programs/goto-programs$(LIBEXT) \
       ../../langapi/langapi$(LIBEXT) \

INCLUDES= -I ../../

include ../../config.inc
include ../../common

CLEANFILES = tvpi_math$(LIBEXT)

all: tvpi_math$(LIBEXT)

ifneq ($(CUDD),)
  OBJ += $(CUDD)/cudd/.libs/libcudd$(LIBEXT) $(CUDD)/cplusplus/.libs/libobj$(LIBEXT)
endif

###############################################################################

tvpi_math$(LIBEXT): $(OBJ)
	$(LINKLIB)

all: tvpi_math$(EXEEXT)

tvpi_math$(EXEEXT): $(OBJ)
	$(LINKBIN)
