SRC = program.cpp \

OBJ += ../src/util/util$(LIBEXT) \
       ../src/big-int/big-int$(LIBEXT) \
       ../src/solvers/tvpi/tvpi$(LIBEXT) \
       ../src/goto-programs/goto-programs$(LIBEXT) \
      ../src/langapi/langapi$(LIBEXT) \


INCLUDES= -I ../src

LIBS =

include ../src/config.inc
include ../src/common

ifneq ($(CUDD),)
  OBJ += $(CUDD)/cudd/.libs/libcudd$(LIBEXT) $(CUDD)/cplusplus/.libs/libobj$(LIBEXT)
endif

###############################################################################

all: ./tvpi_tools$(EXEEXT)

tvpi_tools$(EXEEXT): $(OBJ)
	$(LINKBIN)
