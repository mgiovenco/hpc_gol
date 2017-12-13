COMMON  = ../common

DBG      ?=
CXX      ?= g++
CXXFLAGS  = -O3 -I. -std=c++11 -I$(COMMON) $(DBG)

ifeq ($(CXX),icpc)
  CXXFLAGS += -xHost #-no-vec
  CXXFLAGS += -qopt-report=5
  CXXFLAGS += -Wunknown-pragmas # Disable warning about OpenMP pragma no defined.
  CXXFLAGS += -fopenmp
endif

ifeq ($(CXX),g++)
  CXXFLAGS += -mtune=native -march=native
endif

EXEC = gol

all: $(EXEC)

# Load common make options
include $(COMMON)/Makefile.common
LDFLAGS   = $(COMMON_LIBS)

gol.o: gol.cpp
        $(CXX) $(CXXFLAGS) -c gol.cpp

gol: gol.o $(COMMON_OBJS)
        $(CXX) $(CXXFLAGS) -o gol gol.o $(COMMON_OBJS) $(LDFLAGS)

clean: clean_common
        /bin/rm -fv $(EXEC) *.o *.optrpt
