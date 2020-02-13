
#PLATFORM = intelfpga
PLATFORM = intelgpu

ifeq ($(PLATFORM),intelfpga)
CXX=g++
CFLAGS = -Wall -O2 -g -Wno-unknown-pragmas
CFLAGS += $(shell aocl compile-config)
CXXFLAGS = $(CFLAGS) -std=c++11
LDFLAGS = $(shell aocl link-config)
endif

ifeq ($(PLATFORM),intelgpu)
CXX=g++
CXXFLAGS = -Wall -O2 -g -std=gnu++0x
LDFLAGS = -lOpenCL
endif

ifneq (,$(SDK))
CXXFLAGS += -I$(SDK)/include

ifneq (,$(ICDHACK))
LDFLAGS += -L$(ICDHACK)/.libs
else
LDFLAGS += -L$(SDK)/lib64
endif

endif

#LDFLAGS = $(shell aocl link-config) -lnalla_pcie_mmd
#LIBC214 = -L/opt/glibc-2.14/lib
#LDFLAGS = $(shell aocl link-config) $(LIBC214)

INSTALL_PATH ?= /usr/local

all: demohost daxpyhost slowpihost

demohost : demohost.cpp clwrap.hpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

daxpyhost : daxpyhost.cpp clwrap.hpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

slowpihost : slowpihost.cpp clwrap.hpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

demokernel.aocx : demokernel.cl
	aoc -march=emulator -DEMULATOR $<

install: clwrap.hpp
	mkdir -p $(INSTALL_PATH)/include
	install -t $(INSTALL_PATH)/include clwrap.hpp

clean:
	rm -f demohost *.o

distclean: clean
	rm -f *~
	rm -rf *.aocx *.aoco
